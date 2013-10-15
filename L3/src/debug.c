/*
 * CSC372 - RTOS
 *
 * Debugging helper routines
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "kernel.h"

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
