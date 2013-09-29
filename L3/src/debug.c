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

#ifndef NATIVE
#include <stdio.h>
#endif /* NATIVE */

#ifdef NATIVE

void printk(const char* format, ...)
{
  
  while(*format != '\0') {
    
    if((*JTAG_UART_CONTROL)&0xffff0000 ) {
      
      *JTAG_UART_DATA = (*format++);
      
    }
  }
}

#else /* NATIVE */

void printk(const char* format, ...)
{
  va_list args;
  va_start (args, format);
  vprintf (format, args);
  va_end (args);
}

#endif /* NATIVE */
