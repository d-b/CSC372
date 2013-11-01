/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "user.h"

#ifdef PLATFORM_RPI
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

// Platform independent configuration
#define AUDIO_BUFFER_SAMPLES 1024

// Platform specific configuration
#ifdef NATIVE
#define AUDIO_DEVICE_BUFFER_SAMPLES 128
#define AUDIO_CHANNELS 2
#define AUDIO_BIT_DEPTH 32
#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_DEVICE_BUFFER_SAMPLES 1024 
#elif PLATFORM_RPI
#define AUDIO_CHANNELS 2
#define AUDIO_BIT_DEPTH 32
#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_DEVICE_BUFFER_SAMPLES 1024
#define AUDIO_DEVICE_BUFFERS 10
#define AUDIO_SLEEP_TIME 10
#define AUDIO_MIN_LATENCY_TIME 20
static AUDIOPLAY_STATE_T* audio_playstate;
#else
#define AUDIO_CHANNELS 0
#define AUDIO_BIT_DEPTH 0
#define AUDIO_SAMPLE_RATE 0
#define AUDIO_DEVICE_BUFFER_SAMPLES 0
#endif 

// Thread internals
static sample_t audio_thread_buffer[AUDIO_DEVICE_BUFFER_SAMPLES];
static int audio_thread_exit;

// Sample ring buffer
static sample_t rb_buffer[AUDIO_BUFFER_SAMPLES];
static ringbuffer_t rb_samples;

// Assert macro
#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT(expr) audio_assert((expr),#expr,__FILE__,__LINE__)

/*
 * audio_assert
 *
 * Assert for the audio subsystem
 */
inline void audio_assert(int success, const char* expr, const char* file, int line) {
    if(!success) {
        printk("Audio: Assertion '%s' failed at line %d of file %s!\n", expr, line, file);
        SysCall(SYS_DESTROY, 0, 0, 0);
    }
}

/*
 * min
 *
 * Find the minimum of two integers
 */
inline int min(int a, int b) {
    return (a < b) ? a : b;
}

/*
 * audio_thread_init
 *
 * Prepare for audio processing
 */
void audio_thread_init(void) {
#ifdef PLATFORM_RPI
    int ret;
    int buffer_size = AUDIO_DEVICE_BUFFER_SAMPLES * (AUDIO_BIT_DEPTH>>3) * 2;
    ret = audioplay_create(&audio_playstate,
                           AUDIO_SAMPLE_RATE,    /* Sample rate */
                           AUDIO_CHANNELS,       /* Channels */
                           AUDIO_BIT_DEPTH,      /* Bit depth */
                           AUDIO_DEVICE_BUFFERS, /* Buffers */
                           buffer_size           /* Buffer size */);
    ASSERT(ret == 0);
    ret = audioplay_set_dest(audio_playstate, "local");
    ASSERT(ret == 0);
#endif
}

/*
 * audio_thread
 *
 * Send samples to the audio hardware
 */
void audio_thread(void) {
    // Run postinit sequence
    audio_thread_init();

    // Start processing audio
    while(!audio_thread_exit) {
    // Native implementation
    #ifdef NATIVE
        // See how many samples we should send
        int count = min(AUDIO_CODEC_FIFOSPACE & 0xFF000000,
                        AUDIO_CODEC_FIFOSPACE & 0x00FF0000);
        count = min(count, AUDIO_DEVICE_BUFFER_SAMPLES);

        // Read the required samples from the ring buffer
        count = ringbuffer_read(&rb_samples, audio_thread_buffer, count);

        // Send the samples to the audio hardware
        int i; for(i = 0; i < count; i++) {
            AUDIO_CODEC_LEFTCHAN  = audio_thread_buffer[i];
            AUDIO_CODEC_RIGHTCHAN = audio_thread_buffer[i];
        }
    // Raspberry Pi implementation
    #elif PLATFORM_RPI
        // Read the required samples from the ring buffer
        int count;
        while((count = ringbuffer_read(&rb_samples, audio_thread_buffer, AUDIO_DEVICE_BUFFER_SAMPLES)) == 0)
            SysCall(SYS_YIELD, 0, 0, 0);

        // Acquire a device buffer
        uint8_t* devbuff;
        while((devbuff = audioplay_get_buffer(audio_playstate)) == NULL)
            usleep(AUDIO_SLEEP_TIME * 1000);
        sample_t* samples = (sample_t*) devbuff;

        // Write samples to the device buffer
        for(int i = 0; i < count; i++)
            for(int j = 0; j < 2; j++)
                samples[i*2 + j] = audio_thread_buffer[i];

        // Wait before sending the next packet as required
        uint32_t latency;
        while((latency = audioplay_get_latency(audio_playstate)) > (AUDIO_SAMPLE_RATE * (AUDIO_MIN_LATENCY_TIME + AUDIO_SLEEP_TIME) / 1000))
            usleep(AUDIO_SLEEP_TIME*1000);

        // Start playing the sequence in the buffer
        int ret = audioplay_play_buffer(audio_playstate, devbuff, count * (AUDIO_BIT_DEPTH>>3) * 2);
        ASSERT(ret == 0);
    #endif

        // Yield to other threads
        SysCall(SYS_YIELD, 0, 0, 0);
    }

    // Cleanup system
#ifdef PLATFORM_RPI
    int ret = audioplay_delete(audio_playstate);
    ASSERT(ret == 0);
#endif
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
    ringbuffer_init(&rb_samples, rb_buffer, AUDIO_BUFFER_SAMPLES, sizeof(sample_t));
    // Start the audio processing thread
    SysCall(SYS_CREATE, (uval32) &audio_thread, (uval32) malloc(STACKSIZE), 1);
    // Successful startup
    return 0;
}

/*
 * audio_sine
 *
 * Write sine wave of the specified frequency to the given buffer
 */
int audio_sine(sample_t* buffer, int samples, int frequency, int amplitude) {
    samples = min(samples, AUDIO_SAMPLE_RATE/frequency);
    int i; for(i = 0; i < samples; i++)
        buffer[i] = (sample_t) (sin(((double)i/samples)*2*PI)*amplitude);
    return samples;
}

/*
 * audio_square
 *
 * Write square wave of the specified frequency to the given buffer
 */
int audio_square(sample_t* buffer, int samples, int frequency, int amplitude) {
    samples = min(samples, AUDIO_SAMPLE_RATE/frequency);
    int i; for(i = 0; i < samples; i++)
        buffer[i] = (i <= (samples/2)) ? amplitude : -amplitude;
    return samples;
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
 * audio_free
 *
 * Return the free space in the sample buffer
 */
int audio_free(void) {
    return ringbuffer_remaining(&rb_samples);
}

/*
 * audio_stats
 *
 * Obtain statistics about the audio subsystem
 */
int audio_stats(audio_stats_t* stats) {
    stats->channels = AUDIO_CHANNELS;
    stats->bit_depth = AUDIO_BIT_DEPTH;
    stats->sample_rate = AUDIO_SAMPLE_RATE;
    stats->buffer_free = audio_free();
    stats->buffer_size = AUDIO_BUFFER_SAMPLES;
    return 0;
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
