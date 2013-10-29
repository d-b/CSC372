/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#ifndef _KERNEL_H_
#define _KERNEL_H_

// Platform includes
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

// Project includes
#include "defines.h"
#include "list.h"
#include "irq.h"
#include "debug.h"
#include "main.h"

typedef struct type_STACK Stack;

struct type_STACK 
{ 
  uval8 stack[STACKSIZE]; 
};

extern TD *Active, Kernel;
extern IRQL InterruptLevel;

RC CreateThread(uval32 pc, uval32 sp, uval32 priority);
RC Suspend();
RC ResumeThread(uval32 tid);
RC ChangeThreadPriority(uval32 tid, uval32 priority);
RC Yield();
RC DestroyThread(uval32 tid);

void InitKernel(void);  
void Idle(void);
void K_SysCall( SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2);
void SysCallHandler(SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2);
void U_ThreadExit(void);

#ifndef NATIVE
RC U_VirtualSysCall(SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2);
#endif

#endif
