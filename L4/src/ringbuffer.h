/*
 * CSC372 - RTOS
 *
 * Ring buffer
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

// Ring buffer type
typedef struct {
    int size;
    int stride;
    int start;
    int count;
    char* buffer;
} ringbuffer_t;

void ringbuffer_init(ringbuffer_t* rb, void* buffer, int size, int stride);
int ringbuffer_write(ringbuffer_t* rb, const void* src, int elements);
int ringbuffer_read(ringbuffer_t* rb, void* dest, int elements);
int ringbuffer_remaining(ringbuffer_t* rb);
int ringbuffer_empty(ringbuffer_t* rb);

#endif
