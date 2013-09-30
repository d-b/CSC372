#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "defines.h"
#include "list.h"
#include "irq.h"

typedef struct type_STACK Stack;

struct type_STACK 
{ 
  uval8 stack[STACKSIZE]; 
};

extern TD *Active, Kernel;
extern IRQL InterruptLevel;

RC CreateThread(uval32 pc, uval32 sp, uval32 priority);
RC SuspendThread(uval32 tid);
RC ResumeThread(uval32 tid);
RC ChangeThreadPriority(uval32 priority);
RC Yield();
RC DestroyThread(uval32 tid);

void InitKernel(void);  
void Idle(void);
void K_SysCall( SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2);
extern void SysCallHandler(SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2);
#endif
