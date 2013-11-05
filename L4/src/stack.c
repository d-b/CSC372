/*
 * CSC372 - RTOS
 *
 * Stack
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "user.h"

/*
 * stack_init
 *
 * Initialize a stack object
 */
void stack_init(sstack_t* stack, void* container, int size, int stride) {
    stack->size = size;
    stack->stride = stride;
    stack->count = 0;
    stack->container = (char*) container;
}

/*
 * stack_push
 *
 * Push an element onto the stack
 */
int stack_push(sstack_t* stack, const void* element) {
    if(stack->size - stack->count == 0) return -1;
    memcpy(stack->container + stack->count * stack->stride, element, stack->stride);
    stack->count++; return 0;
}

/*
 * stack_pop
 *
 * Pop an element off the stack
 */
int stack_pop(sstack_t* stack, void* element) {
    if(stack->count == 0) return -1;
    memcpy(element, stack->container + (stack->count - 1) * stack->stride, stack->stride);
    stack->count--; return 0;
}

/*
 * stack_pop
 *
 * Return how much space is left on the stack
 */
int stack_remaining(sstack_t* stack) {
    return stack->size - stack->count;
}

/*
 * stack_empty
 *
 * Return whether or not the stack is empty
 */
int stack_empty(sstack_t* stack) {
    return stack->count == 0;
}
