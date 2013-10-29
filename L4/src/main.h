/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#ifndef _MAIN_H_
#define _MAIN_H_

#include "defines.h"

int main(void);
void printHex(uval32 num);


#ifdef NATIVE

void interrupt_handler(int interrupt_mask);
void pushbutton_isr(void);
void timer_isr(void);
void check_exception(void);
#endif /* NATIVE */

#endif 

