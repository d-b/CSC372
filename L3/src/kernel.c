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

IRQL InterruptLevel; // Current interrupt servicing level
TD *Active, Kernel;  // Active & kernel tasks
Stack KernelStack;   // Kernel mode stack
LL Ready, Blocked;   // Thread lists

void
InitKernel(void) {
  // Initialize interrupt handling
  irq_init();

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
      panic("Invalid thread state change requested!");
      break;

    // Place thread on ready queue
    case S_READY: {
      status = PriorityEnqueue(Active, &Ready);
      if(!_SUCCESS(status))
        panic("Could not place current thread on ready queue!");
      break;
    }

    // Place thread on blocking queue
    case S_SLEEP: {
      status = EnqueueAtHead(Active, &Blocked);
      if(!_SUCCESS(status))
        panic("Could not place current thread on blocked queue!");
      break;
    }
  }

  // Set new state for current thread
  Active->state = newstate;

  // Get next ready thread
  Active = DequeueHead(&Ready);
  if(!Active) panic("Could not find next ready thread!");

  IRQL_LOWER;
  /////////////////////////////
  // Critical section ends
  /////////////////////////////
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
  // Operation status
  RC status;

  /////////////////////////////
  // Critical section starts
  /////////////////////////////
  IRQL_RAISE_TO_HIGH;

  // Create a new thread descriptor
  TD* td = CreateTD(GetTid());
  if(!td) { IRQL_LOWER; return RC_FAILED; }

  // Initialize it with the user passed program counter, stack pointer & priority
  InitTD(td, pc, sp, priority);

  // Setup non-native context
  #ifndef NATIVE
    getcontext(&td->context);
    td->context.uc_stack.ss_sp = malloc(STACKSIZE);
    td->context.uc_stack.ss_size = STACKSIZE;
    makecontext(&td->context, (void (*)()) pc, 0);
  #endif

  // Add it to the ready queue
  status = PriorityEnqueue(td, &Ready);
  if(!_SUCCESS(status)) { IRQL_LOWER; return status; }

  /// See if we need to yield
  int yield = (td->priority < Active->priority);

  IRQL_LOWER;
  /////////////////////////////
  // Critical section ends 
  /////////////////////////////
  
  // Yield the active process if it has lower priority
  if(yield) Yield();

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

  // Attempt to find thread by id on blocked queue
  TD* td = FindTD(tid, &Blocked);
  if(!td) { IRQL_LOWER; return RC_FAILED; }

  // Remove TD from blocked queue
  RC status = DequeueTD(td);
  if(!_SUCCESS(status)) { IRQL_LOWER; return RC_FAILED; }

  /// See if we need to yield
  int yield = (td->priority < Active->priority);

  IRQL_LOWER;
  /////////////////////////////
  // Critical section ends 
  /////////////////////////////

  // Yield the active process if it has lower priority
  if(yield) Yield();  

  // All operations completed successfully
  return RC_SUCCESS;  
}

RC ChangeThreadPriority(uval32 tid, uval32 priority) {
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
  } else td = Active;

  // Abort if no TD found
  if(!td) { IRQL_LOWER; return RC_FAILED; }
  

  // Cleanup and switch threads if necessary
  DestroyTD(td);
  if(td == Active) {
    Active = DequeueHead(&Ready);
    if(!Active) panic("Destroyed last ready thread!");    
  }

  IRQL_LOWER;
  /////////////////////////////
  // Critical section ends 
  /////////////////////////////

  // All operations completed successfully
  return RC_SUCCESS;  
}

void 
Idle() 
{ 
  /*
  int i; 
  while( 1 ) 
    { 
      printk( "CPU is idle\n" ); 
      for( i = 0; i < MAX_THREADS; i++ ) 
	{ 
	} 
      Yield(); 
    } 
  */
}


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

#endif
