/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "user.h"

// Audio codec addresses
#define AUDIO_CODEC_BASE      ((uval32*)0x10003040)
#define AUDIO_CODEC_CTL       (AUDIO_CODEC_BASE+0)
#define AUDIO_CODEC_FIFOSPACE (AUDIO_CODEC_BASE+1)
#define AUDIO_CODEC_LEFTCHAN  (AUDIO_CODEC_BASE+2)
#define AUDIO_CODEC_RIGHTCHAN (AUDIO_CODEC_BASE+3)

// Audio parameters
#define AUDIO_SAMPLE_RATE   48000 // 48KHz
#define AUDIO_SAMPLE_BUFFER 1024  // 1024 sample buffer

// Internal types
typedef struct {
    int samples;
    sample_t buffer[AUDIO_SAMPLE_BUFFER];
} sample_buffer_t;

// Static globals
static sample_buffer_t audio_samples;

/*
 * audio_init
 *
 * Initialize audio subsystem
 */
int audio_init() {
    memset(&audio_samples, 0, sizeof(audio_samples));
}

/*
 * audio_send
 *
 * Add samples to the internal sample buffer
 */
int audio_send(sample_t* buffer, int samples) {

}
