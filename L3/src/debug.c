/*
 * CSC372 - Lab 3
 *
 * Debugging helper routines
 */

#include "defines.h"
#include "list.h"
#include "user.h"
#include "kernel.h"
#include "main.h"

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

void printk(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void panic(const char* format, ...) {
    va_list args;
    va_start(args, format);
    printf("Panic: ");
    vprintf(format, args);
    va_end(args);
    exit(-1);
}
