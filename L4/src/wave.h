/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#ifndef _WAVE_H_
#define _WAVE_H_

enum WAVE_endian {
    WAVE_little_endian,
    WAVE_big_endian
};

enum WAVE_error {
    WAVERR_okay,
    WAVERR_invalid_header,
    WAVERR_unknown_audio_format,
    WAVERR_uncanonical_layout
};

typedef struct {
    // PCM format
    int channels;
    int sample_rate;
    int bit_depth;
    int endian;
    int frame_size;

    // Sequence info
    uint32_t frames;
    uint32_t position;

    // Wave data
    uint32_t size;
    const char* header;
    const char* data;
} wave_sequence_t;

int wave_create(wave_sequence_t* sequence, const void* data, uint32_t size);
uint32_t wave_read(wave_sequence_t* sequence, void* buffer, uint32_t samples);
uint32_t wave_seek(wave_sequence_t* sequence, uint32_t frame);
uint32_t wave_position(wave_sequence_t* sequence);
uint32_t wave_remaining(wave_sequence_t* sequence);

#endif
