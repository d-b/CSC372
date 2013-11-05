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

// Platform specific configuration
#ifdef NATIVE
#define AUDIO_CHANNELS 2
#define AUDIO_BIT_DEPTH 32
#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_BUFFER_FRAMES 1024
#define AUDIO_DEVICE_BUFFER_FRAMES 128
#elif PLATFORM_RPI
#define AUDIO_CHANNELS 2
#define AUDIO_BIT_DEPTH 32
#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_BUFFER_FRAMES 1024
#define AUDIO_DEVICE_BUFFER_FRAMES 1024
#define AUDIO_DEVICE_BUFFERS 10
#define AUDIO_SLEEP_TIME 10
#define AUDIO_MIN_LATENCY_TIME 20
static AUDIOPLAY_STATE_T* audio_playstate;
#else
#define AUDIO_CHANNELS 2
#define AUDIO_BIT_DEPTH 32
#define AUDIO_SAMPLE_RATE 48000
#define AUDIO_BUFFER_FRAMES 1024
#define AUDIO_DEVICE_BUFFER_FRAMES 1024
#endif

// Platform independent configuration
#define AUDIO_BUFFER_SAMPLES AUDIO_BUFFER_FRAMES * AUDIO_CHANNELS
#define AUDIO_DEVICE_BUFFER_SAMPLES AUDIO_DEVICE_BUFFER_FRAMES * AUDIO_CHANNELS
#define AUDIO_DFT_WINDOW_SIZE 2048
#define AUDIO_DFT_HOP_SIZE (AUDIO_DFT_WINDOW_SIZE/4)

// Thread internals
static sample_t audio_thread_buffer[AUDIO_DEVICE_BUFFER_SAMPLES];
static int audio_thread_exit;

// Sample ring buffer
static sample_t rb_buffer[AUDIO_BUFFER_SAMPLES];
static ringbuffer_t rb_samples;

//
// Safe access to audio codec
//
#ifdef NATIVE
int audio_device_fifo_left_space() {
    uval32 value = 0;
    void* fifospace = (void*) &AUDIO_CODEC_FIFOSPACE;
    asm("ldw r2, %0" : : "m"(fifospace) : "r2");
    asm("ldwio r3, 0(r2)" ::: "r3", "r2");
    asm("stw r3, %0" : "=m"(value) :: "r3");
    return value & 0xFF000000 >> 24;
}
int audio_device_fifo_right_space() {
    uval32 value = 0;
    void* fifospace = (void*) &AUDIO_CODEC_FIFOSPACE;
    asm("ldw r2, %0" : : "m"(fifospace) : "r2");
    asm("ldwio r3, 0(r2)" ::: "r3", "r2");
    asm("stw r3, %0" : "=m"(value) :: "r3");
    return value & 0x00FF0000 >> 16;
}
int audio_device_fifo_left_write(sample_t sample) {
    void* fiforight = (void*) &AUDIO_CODEC_LEFTCHAN;
    asm("ldw r2, %0" : : "m"(fiforight) : "r2");
    asm("ldw r3, %0" : : "m"(sample) : "r3");
    asm("stwio r3, 0(r2)");
}
int audio_device_fifo_right_write(sample_t sample) {
    void* fiforight = (void*) &AUDIO_CODEC_RIGHTCHAN;
    asm("ldw r2, %0" : : "m"(fiforight) : "r2");
    asm("ldw r3, %0" : : "m"(sample) : "r3");
    asm("stwio r3, 0(r2)");
}
#endif

/*
 * min
 *
 * Find the minimum of two integers
 */
inline int min(int a, int b) {
    return (a < b) ? a : b;
}

/*
 * min
 *
 * Find the minimum of two integers
 */
inline int max(int a, int b) {
    return (a > b) ? a : b;
}

/*
 * audio_thread_init
 *
 * Prepare for audio processing
 */
void audio_thread_init(void) {
#ifdef PLATFORM_RPI
    int ret;
    int buffer_size = AUDIO_DEVICE_BUFFER_SAMPLES * (AUDIO_BIT_DEPTH>>3);
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
        int count; while((count = audio_device_fifo_left_space()) > 0) {
            // Read the required samples from the ring buffer
            count = ringbuffer_read(&rb_samples, audio_thread_buffer, count * AUDIO_CHANNELS);
            if(!count) break;

            // Send the samples to the audio hardware
            int i; for(i = 0; i < count; i += AUDIO_CHANNELS) {
                audio_device_fifo_left_write(audio_thread_buffer[i + 0]);
                audio_device_fifo_right_write(audio_thread_buffer[i + 1]);
            }            
        }
    // Raspberry Pi implementation
    #elif PLATFORM_RPI
        // Acquire a device buffer
        uint8_t* devbuff;
        while((devbuff = audioplay_get_buffer(audio_playstate)) == NULL)
            usleep(AUDIO_SLEEP_TIME * 1000);

        // Read the required samples from the ring buffer
        int count;
        while((count = ringbuffer_read(&rb_samples, devbuff, AUDIO_DEVICE_BUFFER_SAMPLES)) == 0)
            SysCall(SYS_YIELD, 0, 0, 0);

        // Wait before sending the next packet as required
        uint32_t latency;
        while((latency = audioplay_get_latency(audio_playstate)) > (AUDIO_SAMPLE_RATE * (AUDIO_MIN_LATENCY_TIME + AUDIO_SLEEP_TIME) / 1000))
            usleep(AUDIO_SLEEP_TIME * 1000);

        // Start playing the sequence in the buffer
        int ret = audioplay_play_buffer(audio_playstate, devbuff, count * (AUDIO_BIT_DEPTH>>3));
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
    ringbuffer_init(&rb_samples, rb_buffer, AUDIO_BUFFER_SAMPLES, (AUDIO_BIT_DEPTH>>3));
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
    int frames = min(samples/AUDIO_CHANNELS, AUDIO_SAMPLE_RATE/frequency);
    int i, j; for(i = 0; i < frames; i++) {
        sample_t sample = (sample_t) (sin(((double)i/frames)*2*PI)*amplitude);
        for(j = 0; j < AUDIO_CHANNELS; j++)
            buffer[AUDIO_CHANNELS * i + j] = sample;
    } return frames * AUDIO_CHANNELS;
}

/*
 * audio_square
 *
 * Write square wave of the specified frequency to the given buffer
 */
int audio_square(sample_t* buffer, int samples, int frequency, int amplitude) {
    int frames = min(samples/AUDIO_CHANNELS, AUDIO_SAMPLE_RATE/frequency);
    int i, j; for(i = 0; i < frames; i++) {
        sample_t sample = (i <= (frames/2)) ? amplitude : -amplitude;
        for(j = 0; j < AUDIO_CHANNELS; j++)
            buffer[AUDIO_CHANNELS * i + j] = sample;
    } return frames * AUDIO_CHANNELS;
}

/*
 * audio_add
 *
 * Add two sample streams together
 */
int audio_add(const sample_t* stream1, int samples1, const sample_t* stream2, int samples2, sample_t* output) {
    int frames = max(samples1, samples2)/AUDIO_CHANNELS;
    int separation = min(samples1, samples2)/AUDIO_CHANNELS;
    int i; for(i = 0; i < frames; i++) {
        int j; for(j = 0; j < AUDIO_CHANNELS; j++) {
            int x = AUDIO_CHANNELS * i + j;
            if(i >= separation)
                output[x] = (samples1 > samples2) ? stream1[x] : stream2[x];
            else output[x] = stream1[x] + stream2[x];
        }
    } return frames;
}

/*
 * audio_send
 *
 * Add samples to the internal sample buffer
 */
int audio_send(sample_t* buffer, int samples) {
    samples -= samples % AUDIO_CHANNELS;
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
