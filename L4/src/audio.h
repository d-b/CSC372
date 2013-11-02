/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#ifndef _AUDIO_H_
#define _AUDIO_H_

// Audio types
typedef uint32_t sample_t;

// Audio information
typedef struct {
    int channels;
    int bit_depth;
    int sample_rate;
    int buffer_free;
    int buffer_size;
} audio_stats_t;

int audio_init(void);
int audio_sine(sample_t* buffer, int samples, int frequency, int amplitude);
int audio_square(sample_t* buffer, int samples, int frequency, int amplitude);
int audio_send(sample_t* buffer, int samples);
int audio_free(void);
int audio_stats(audio_stats_t* stats);
int audio_exit(void);

#endif
