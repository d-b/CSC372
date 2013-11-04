/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#ifndef _USER_H_
#define _USER_H_

// Project includes
#include "defines.h"
#include "debug.h"
#include "ringbuffer.h"
#include "stack.h"

// Lab 4 includes
#include "audio.h"
#include "fft.h"
#include "wave.h"
#include "notes.h"

// Platform includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

RC SysCall( SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2);
void ThreadExit(void);
RC U_VirtualSysCall(SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2);
void mymain(void);

// Assert macro
#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT(expr) user_assert((expr),#expr,__FILE__,__LINE__)

// User assert routine
inline void user_assert(int success, const char* expr, const char* file, int line) {
    if(!success) {
        printk("Assertion '%s' failed at line %d of file %s!\n", expr, line, file);
        SysCall(SYS_DESTROY, 0, 0, 0);
    }
}

#endif
