/*
 * CSC372 - RTOS
 *
 * Stack
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#ifndef _STACK_H_
#define _STACK_H_

// Stack type
typedef struct {
    int size;
    int stride;
    int count;
    char* container;
} sstack_t;

void stack_init(sstack_t* stack, void* container, int size, int stride);
int stack_push(sstack_t* stack, const void* element);
int stack_pop(sstack_t* stack, void* element);
int stack_remaining(sstack_t* stack);
int stack_empty(sstack_t* stack);

#endif
