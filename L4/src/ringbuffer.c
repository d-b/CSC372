/*
 * CSC372 - RTOS
 *
 * Ring buffer
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "user.h"

/*
 * ringbuffer_init
 *
 * Initialize a ring buffer object
 */
void ringbuffer_init(ringbuffer_t* rb, void* buffer, int size, int stride) {
    rb->size = size;
    rb->stride = stride;
    rb->start = 0;
    rb->count = 0;
    rb->buffer = (char*) buffer;
}

/*
 * ringbuffer_write
 *
 * Write a chunk of data to the wring buffer
 */
int ringbuffer_write(ringbuffer_t* rb, const void* src, int elements) {
    // Sanity check on number of elements to write
    if(elements > rb->size) elements = rb->size;

    // Write data to the buffer
    int base = (rb->start + rb->count) % rb->size;
    int count = rb->size - base;
    if(elements <= count)
        // Perform unwrapping write to the buffer
        memcpy(rb->buffer + (base * rb->stride), src, elements * rb->stride);
    else {
        // Perform wrapping write to the buffer
        memcpy(rb->buffer + (base * rb->stride), src, count * rb->stride);
        count = elements - count;
        memcpy(rb->buffer, src, count * rb->stride);
    }
    
    // Update buffer state information
    int rem = rb->size - rb->count;
    if(elements <= rem)
        rb->count += elements;
    else {
        // Make corrections to start since we've overwritten data
        rb->start = (rb->start + (elements - rem)) % rb->size;
        rb->count = rb->size;
    }

    // Return elements written
    return elements;
}

/*
 * ringbuffer_read
 *
 * Read from the ring buffer
 */
int ringbuffer_read(ringbuffer_t* rb, void* dest, int elements) {
    // Sanity check on number of elements to read
    if(elements > rb->count) elements = rb->count;

    // Read data from the buffer
    int count = rb->size - rb->start;
    if(elements <= count)
        // Perform unwrapping read from the buffer
        memcpy(dest, rb->buffer + (rb->start * rb->stride), elements * rb->stride);
    else {
        // Perform wrapping read from the buffer
        memcpy(dest, rb->buffer + (rb->start * rb->stride), count * rb->stride);
        count = elements - count;
        memcpy(dest, rb->buffer, count * rb->stride);
    }

    // Update buffer state information
    rb->start = (rb->start + elements) % rb->size;
    rb->count -= elements;

    // Return elements read
    return elements;
}

/*
 * ringbuffer_remaining
 *
 * Free space in the ring buffer
 */
int ringbuffer_remaining(ringbuffer_t* rb) {
    return rb->size - rb->count;
}

/*
 * ringbuffer_empty
 *
 * Whether or not the ring buffer is empty
 */
int ringbuffer_empty(ringbuffer_t* rb) {
    return rb->count == 0;
}
