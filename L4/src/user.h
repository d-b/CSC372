/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#ifndef _USER_H_
#define _USER_H_

// Platform includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Project includes
#include "defines.h"
#include "debug.h"
#include "ringbuffer.h"

// Lab 4 includes
#include "audio.h"

RC SysCall( SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2);
void ThreadExit(void);
RC U_VirtualSysCall(SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2);

void mymain(void);

#endif
