/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "user.h"

#ifdef RASPI
#include <rpi/audio.h>
#endif

// Native audio driver parameters
#ifdef NATIVE
#define AUDIO_CODEC_BASE      ((volatile int*)0x10003040)
#define AUDIO_CODEC_CTL       *(AUDIO_CODEC_BASE+0)
#define AUDIO_CODEC_FIFOSPACE *(AUDIO_CODEC_BASE+1)
#define AUDIO_CODEC_LEFTCHAN  *(AUDIO_CODEC_BASE+2)
#define AUDIO_CODEC_RIGHTCHAN *(AUDIO_CODEC_BASE+3)
#endif

// Thread parameters
#define AUDIO_THREAD_BUFFER 128

// Thread internals
static sample_t audio_thread_buffer[AUDIO_THREAD_BUFFER];
static int audio_thread_exit;

// Sample ring buffer
static sample_t rb_buffer[AUDIO_SAMPLE_BUFFER];
static ringbuffer_t rb_samples;

/*
 * min
 *
 * Find the minimum of two integers
 */
inline int min(int a, int b) {
    return (a < b) ? a : b;
}

/*
 * audio_thread
 *
 * Send samples to the audio hardware
 */
void audio_thread(void) {
    while(!audio_thread_exit) {
    // Native implementation
    #ifdef NATIVE
        // See how many samples we should send
        int count = min(AUDIO_CODEC_FIFOSPACE & 0xFF000000,
                        AUDIO_CODEC_FIFOSPACE & 0x00FF0000);
        count = min(count, AUDIO_THREAD_BUFFER);

        // Read the required samples from the ring buffer
        count = ringbuffer_read(&rb_samples, audio_thread_buffer, count);

        // Send the samples to the audio hardware
        int i; for(i = 0; i < count; i++) {
            AUDIO_CODEC_LEFTCHAN  = audio_thread_buffer[i];
            AUDIO_CODEC_RIGHTCHAN = audio_thread_buffer[i];
        }
    // Raspberry Pi implementation
    #endif

        // Yield to other threads
        SysCall(SYS_YIELD, 0, 0, 0);
    }
}

/*
 * audio_init
 *
 * Initialize audio subsystem
 */
int audio_init(void) {
    // Set initial thread parameters
    audio_thread_exit = 0;
    // Initialize the ring buffer
    ringbuffer_init(&rb_samples, rb_buffer, AUDIO_SAMPLE_BUFFER, sizeof(sample_t));
    // Start the audio processing thread
    SysCall(SYS_CREATE, (uval32) &audio_thread, (uval32) malloc(STACKSIZE), 1);
    // Successful startup
    return 0;
}

/*
 * audio_send
 *
 * Add samples to the internal sample buffer
 */
int audio_send(sample_t* buffer, int samples) {
    return ringbuffer_write(&rb_samples, buffer, samples);
}

/*
 * audio_exit
 *
 * Shutdown the audio subsystem
 */
int audio_exit(void) {
    audio_thread_exit = 1;
    return 0;
}
