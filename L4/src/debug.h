/*
 * CSC372 - RTOS
 *
 * Debugging helper routines
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#define KASSERT(x) assert(x)
void printk(const char* format, ...);
void panic(const char* format, ...);

#endif
