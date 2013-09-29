/*
 * CSC372 - Lab 3
 *
 * Debugging helper routines
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <assert.h>

#define KASSERT(x) assert(x)
void printk(const char* format, ...);

#endif
