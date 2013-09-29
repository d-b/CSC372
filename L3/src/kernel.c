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

//an always increasing thread id variable
static ThreadId currtid = 1;

IRQL InterruptLevel;
TD *Active, Kernel;
Stack KernelStack;

LL * ReadyQ;
LL * BlockedQ;

void
InitKernel(void) {
  irq_init();
  
  //Init queues
  ReadyQ = malloC(sizeof(LL));
  BlockedQ = malloC(sizeof(LL));
  
  InitList(L_PRIORITY, ReadyQ);
  InitList(L_LIFO, BlockedQ);
  
  Active = CreateTD(getTid());
  InitTD(Active, 0, 0, 1);  //Will be set with proper return registers on context switch
#ifdef NATIVE
  InitTD(&Kernel, (uval32) SysCallHandler, (uval32) &(KernelStack.stack[STACKSIZE]), 0);
  Kernel.regs.sr = DEFAULT_KERNEL_SR;
#endif /* NATIVE */
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
    returnCode = SuspendThread(arg0);
    break;

    case SYS_RESUME:
    returnCode = ResumeThread(arg0);
    break;

    case SYS_CHANGEPRIORITY:
    returnCode = ChangeThreadPriority(arg0);
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

//Return and then increment the Thread ID (a stupid way to ensure IDs are unique)
ThreadId getTid() {
  ThreadId ret = currtid;
  currtid++;
  return(ThreadId);
}

RC CreateThread(uval32 pc, uval32 sp, uval32 priority) { 
  TD * td = CreateTD(getTid());
  if (td == NULL)
    return RC_FAILED;
  InitTD(td, pc, sp, priority);
  ReadyEnqueue(td);
  RC sysReturn = RC_SUCCESS;
  return sysReturn;
}

RC Suspend() {
  if (BlockedEnqueue(Active) != RC_SUCCESS)
    return RC_FAILED;
  
  Active = ReadyDequeue();
  if (Active == NULL)
    return RC_FAILED;
    
  return RC_SUCCESS;
}

RC ResumeThread(ThreadId tid) {
  TD * td = BlockedGet(tid);
  if (td == NULL)
    return RC_FAILED;
  if (ReadyEnqueue(td) != RC_SUCCESS);
    return RC_FAILED;
  if (Yield() != RC_SUCCESS)
    return RC_FAILED;
  return RC_SUCCESS;
}

RC ChangeThreadPriority( ThreadId tid, uval32 priority) {
  TD * td;
  if ((td = Active)->tid == tid || tid == 0) {
    td->priority = priority;
    
  } else if ((td = ReadyGet(tid)) != NULL) {
    td->priority = priority;
    PriorityEnqueue(td);
    
  } else if ((td = BlockedGet(tid)) != NULL) {
    td->priority = priority;
    BlockedEnqueue(td);
  } else 
      return RC_FAILED;
  return RC_SUCCESS;
}

RC Yield(){
  if (ReadyEnqueue(Active) != RC_SUCCESS) 
    return RC_FAILED;
  Active = ReadyDequeue();
  return RC_SUCCESS;
}

RC DestroyThread(ThreadId tid) {
  TD * td;
  if ((td = Active)->tid == tid || tid == 0) {
    Active = ReadyDequeue();
  } else if ((td = ReadyGet(tid)) != NULL) {
    
  } else if ((td = BlockedGet(tid)) != NULL) {
  
  } else 
      return RC_FAILED;
      
  if (DestroyTD(td) != RC_SUCCESS)
    return RC_FAILED;
    
  return RC_SUCCESS;
}

//Ready queue operations
RC ReadyEnqueue(TD * td) {
  return(PriorityEnqueue(td, ReadyQ));
}

TD * ReadyDequeue() {
  return(DequeueHead(ReadyQ));
}

TD * ReadyGet(ThreadId tid) {
  TD * td = FindTD(tid, ReadyQ);
  if (td == NULL)
    return NULL;

  if (DequeueTD(td) == -1)
    return NULL;
  return(td);
}

//Blocked queue operations
RC BlockedEnqueue(TD * td) {
  return(EnqueueAtHead(td, BlockedQ));
}

TD * BlockedGet(ThreadId tid) {
  TD * td = FindTD(tid, BlockedQ);
  if (td == NULL)
    return NULL;

  if (DequeueTD(td) == -1)
    return NULL;
  return(td);
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
