/*
 * CSC372 - Lab 3
 *
 * Kernel code
 */

#include "defines.h"
#include "debug.h"
#include "list.h"
#include "kernel.h"
#include "irq.h"
#include "main.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

IRQL InterruptLevel; // Current interrupt servicing level
TD *Active, Kernel;  // Active & kernel tasks
Stack KernelStack;   // Kernel mode stack
LL Ready, Blocked;   // Thread lists

void
InitKernel(void) {
  // Initialize interrupt handling
  irq_init();

  // Initialize list subsystem
  ListInit();

  // Initialize thread lists
  InitList(L_PRIORITY, &Ready);
  InitList(L_LIFO, &Blocked);

  // Initialize initial task & exception task
  Active = CreateTD(GetTid());
  InitTD(Active, 0, 0, 1);  //Will be set with proper return registers on context switch
  Active->state = S_ACTIVE;
#ifdef NATIVE
  InitTD(&Kernel, (uval32) SysCallHandler, (uval32) &(KernelStack.stack[STACKSIZE]), 0);
  Kernel.regs.sr = DEFAULT_KERNEL_SR;
#endif /* NATIVE */

  // Add idle task
  CreateThread((uval32) &Idle, (uval32) malloc(STACKSIZE), 0x7FFFFFFF);
}

static void SwapThread(ThreadState newstate) {
  /////////////////////////////
  // Critical section starts
  /////////////////////////////
  IRQL_RAISE_TO_HIGH;

  // Operation status
  RC status;

  switch(newstate) {
    // Should never occur
    case S_ACTIVE:
      panic("Invalid thread state change requested!\n");
      break;

    // Place thread on ready queue
    case S_READY: {
      status = PriorityEnqueue(Active, &Ready);
      if(!_SUCCESS(status))
        panic("Could not place current thread on ready queue!\n");
      break;
    }

    // Place thread on blocking queue
    case S_SLEEP: {
      status = EnqueueAtHead(Active, &Blocked);
      if(!_SUCCESS(status))
        panic("Could not place current thread on blocked queue!\n");
      break;
    }
  }

  // Set new state for current thread
  Active->state = newstate;

  // Get next ready thread
  Active = DequeueHead(&Ready);
  if(!Active) panic("Could not find next ready thread!\n");
  Active->state = S_ACTIVE;

  IRQL_LOWER;
  /////////////////////////////
  // Critical section ends
  /////////////////////////////
}

static RC ReadyThread(TD* td) {
  /////////////////////////////
  // Critical section starts
  /////////////////////////////
  IRQL_RAISE_TO_HIGH;

  RC status = PriorityEnqueue(td, &Ready);
  if(!_SUCCESS(status)) panic("Could not place thread on ready queue!\n");
  td->state = S_READY;

  IRQL_LOWER;
  /////////////////////////////
  // Critical section ends
  /////////////////////////////

  // Return the result
  return status;
}

void K_SysCall(SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2)
{ 
#ifdef NATIVE
  asm(".align 4; .global SysCallHandler; SysCallHandler:");
  uval32 sysMode = SYS_EXIT;
#endif

  RC returnCode; 
  switch( type ) {
    case SYS_CREATE: 
    returnCode = CreateThread(arg0, arg1, arg2) ; 
    break;

    case SYS_SUSPEND:
    returnCode = Suspend();
    break;

    case SYS_RESUME:
    returnCode = ResumeThread(arg0);
    break;

    case SYS_CHANGEPRIORITY:
    returnCode = ChangeThreadPriority(arg0, arg1);
    break;

    case SYS_YIELD:
    returnCode = Yield();
    break;

    case SYS_DESTROY:
    returnCode = DestroyThread(arg0);
    break;

  default:
    printk("Invalid SysCall type\n");
    returnCode = RC_FAILED;
    break;
  } 
  
#ifdef NATIVE
  asm volatile("ldw r8, %0" : : "m" (sysMode): "r8");
  asm( "trap" );
#endif /* NATIVE */
}

RC CreateThread(uval32 pc, uval32 sp, uval32 priority) {
  /////////////////////////////
  // Critical section starts
  /////////////////////////////
  IRQL_RAISE_TO_HIGH;

  // Operation status
  RC status;

  // Create a new thread descriptor
  TD* td = CreateTD(GetTid());
  if(!td) { IRQL_LOWER; return RC_FAILED; }

  // Initialize it with the user passed program counter, stack pointer & priority
  InitTD(td, pc, sp, priority);

  
  #ifdef NATIVE
    // Setup native context
    TrapFrame* tf = (TrapFrame*) (sp + 4);
    memset(tf, 0, sizeof(TrapFrame));
    asm volatile("stw r26, %0" : "=m"(tf->gp));
    tf->sr = DEFAULT_THREAD_SR;
    tf->ea = pc;
  #else
    // Setup non-native context
    getcontext(&td->context_outer);
    td->context_outer.uc_stack.ss_sp = malloc(STACKSIZE);
    td->context_outer.uc_stack.ss_size = STACKSIZE;
    makecontext(&td->context_outer, (void (*)()) U_ThreadExit, 0);
    getcontext(&td->context);
    td->context.uc_link = &td->context_outer;
    td->context.uc_stack.ss_sp = malloc(STACKSIZE);
    td->context.uc_stack.ss_size = STACKSIZE;
    makecontext(&td->context, (void (*)()) pc, 0);
  #endif

  // Add it to the ready queue
  status = ReadyThread(td);
  if(!_SUCCESS(status)) { IRQL_LOWER; return status; }

  // See if we need to yield
  if(td->priority < Active->priority) Yield();

  IRQL_LOWER;
  /////////////////////////////
  // Critical section ends 
  /////////////////////////////

  // All operations completed successfully
  return RC_SUCCESS;
}

RC Yield(){
  SwapThread(S_READY);
  return RC_SUCCESS;
}

RC Suspend() {
  SwapThread(S_SLEEP);
  return RC_SUCCESS;
}

RC ResumeThread(uval32 tid) {
  /////////////////////////////
  // Critical section starts
  /////////////////////////////
  IRQL_RAISE_TO_HIGH;

  // Operation status
  RC status;

  // Attempt to find thread by id on blocked queue
  TD* td = FindTD(tid, &Blocked);
  if(!td) { IRQL_LOWER; return RC_FAILED; }

  // Remove TD from blocked queue
  status = DequeueTD(td);
  if(!_SUCCESS(status)) { IRQL_LOWER; return status; }

  // Make thread ready
  status = ReadyThread(td);
  if(!_SUCCESS(status)) { IRQL_LOWER; return status; }

  // See if we need to yield
  if(td->priority < Active->priority) Yield();

  IRQL_LOWER;
  /////////////////////////////
  // Critical section ends 
  /////////////////////////////

  // All operations completed successfully
  return RC_SUCCESS;  
}

RC ChangeThreadPriority(uval32 tid, uval32 priority) {
  /////////////////////////////
  // Critical section starts
  /////////////////////////////
  IRQL_RAISE_TO_HIGH;

  // Make target thread ready again
  int makeready = 0;

  // Find target TD
  TD* td = NULL;
  if(tid == 0 || Active->tid == tid)
    td = Active;
  else {
    // If thread is in ready queue
    td = FindTD(tid, &Ready);
    if(td) { DequeueTD(td); makeready = TRUE; }
    
    // Check for thread in blocked queue
    if(!td) td = FindTD(tid, &Blocked);
  }

  // Abort if no TD found
  if(!td) { IRQL_LOWER; return RC_FAILED; }

  // Change priority
  td->priority = priority;

  // Ready thread if necessary
  if(makeready) ReadyThread(td);  

  // See if we need to yield
  if(td->priority < Active->priority) Yield();

  IRQL_LOWER;
  /////////////////////////////
  // Critical section ends 
  /////////////////////////////

  // All operations completed successfully
  return RC_SUCCESS;
}

RC DestroyThread(uval32 tid) {
  /////////////////////////////
  // Critical section starts
  /////////////////////////////
  IRQL_RAISE_TO_HIGH;

  // Find target TD
  TD* td = NULL;
  if(tid != 0) {
    td = FindTD(tid, &Ready);
    if(!td) td = FindTD(tid, &Blocked);
    if(Active->tid == tid) td = Active;
  } else td = Active;

  // Abort if no TD found
  if(!td) { IRQL_LOWER; return RC_FAILED; }

  // Cleanup and switch threads if necessary
  DequeueTD(td);
  DestroyTD(td);
  if(td == Active) {
    Active = DequeueHead(&Ready);
    if(!Active) panic("Destroyed last ready thread!\n");
    Active->state = S_ACTIVE;
  }

  IRQL_LOWER;
  /////////////////////////////
  // Critical section ends 
  /////////////////////////////

  // All operations completed successfully
  return RC_SUCCESS;  
}

// Idle thread
void Idle() { while(1) Yield(); }

//
// Non-native context switching support
//

#ifndef NATIVE

enum EJumpStatus { JUMP_FallThrough, JUMP_Exit };

void U_VirtualSysCall(SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2) {
  // Jump status
  volatile int status = JUMP_FallThrough;

  // Save the context
  getcontext(&Active->context);
  if(status != JUMP_FallThrough) return;
  status = JUMP_Exit;

  // Call K_SysCall
  K_SysCall(type, arg0, arg1, arg2);

  // Switch to active thread
  setcontext(&Active->context);
}

void U_ThreadExit(void) {
  U_VirtualSysCall(SYS_DESTROY, 0, 0, 0);
}

#endif
