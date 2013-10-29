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

int audio_init();
int audio_sine(sample_t* buffer, int samples, int frequency);
int audio_square(sample_t* buffer, int samples, int frequency);
int audio_send(sample_t* buffer, int samples);
int audio_exit();

#endif
