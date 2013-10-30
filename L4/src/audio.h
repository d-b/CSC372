/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#ifndef _AUDIO_H_
#define _AUDIO_H_

// Audio types
typedef uval32 sample_t;

// Audio parameters
#define AUDIO_SAMPLE_RATE   48000 // 48KHz
#define AUDIO_SAMPLE_BUFFER 1024  // 1024 sample buffer

int audio_init(void);
int audio_sine(sample_t* buffer, int samples);
int audio_square(sample_t* buffer, int samples);
int audio_send(sample_t* buffer, int samples);
int audio_exit(void);

#endif
