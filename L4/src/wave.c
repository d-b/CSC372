/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "user.h"

// Header strings
static const char id_chunk_riff[]     = "RIFF";
static const char id_chunk_rifx[]     = "RIFX";
static const char chunk_format_wave[] = "WAVE";
static const char id_subchunk_fmt[]   = "fmt ";
static const char id_subchunk_data[]  = "data";

// WAVE audio header
#pragma pack(1)
typedef struct {
    // Chunk header
    char     chunk_id[4];
    uint32_t chunk_size;
    char     chunk_format[4];

    // Format subchunk
    char     fmt_id[4];
    char     fmt_size[4];
    uint16_t fmt_format;
    uint16_t fmt_channels;
    uint32_t fmt_sample_rate;
    uint32_t fmt_byte_rate;
    uint16_t fmt_frame_size;
    uint16_t fmt_bit_depth;

    // Data subchunk
    char     data_id[4];
    uint32_t data_size;
    char     data_contents[1];
} wave_header_t;

/*
 * wave_create
 *
 * Create a wave sequence
 */
int wave_create(wave_sequence_t* sequence, const void* data, uint32_t size) {
    // The givens
    sequence->header = (const char *) data;
    sequence->size = size;
    sequence->position = 0;

    // Size sanity check
    if(size < sizeof(wave_header_t))
        return WAVERR_invalid_header;

    //
    // Attempt to validate the header
    //
    const wave_header_t* hdr = (const wave_header_t*) data;
    
    // See if it is a RIFF
    if(memcmp(hdr->chunk_id, id_chunk_riff, sizeof(hdr->chunk_id)) == 0)
        sequence->endian = WAVE_little_endian;
    // See if it is a RIFX
    else if(memcmp(hdr->chunk_id, id_chunk_rifx, sizeof(hdr->chunk_id)) == 0)
        sequence->endian = WAVE_big_endian;
    // Invalid RIFF/RIFX file
    else
        return WAVERR_invalid_header;

    // Make sure its actually a WAVE file
    if(memcmp(hdr->chunk_format, chunk_format_wave, sizeof(hdr->chunk_format)) != 0)
        return WAVERR_unknown_audio_format;

    // Make sure we have valid format & data chunks
    if(memcmp(hdr->fmt_id,  id_subchunk_fmt,  sizeof(hdr->fmt_id )) != 0 ||
       memcmp(hdr->data_id, id_subchunk_data, sizeof(hdr->data_id)) != 0)
        return WAVERR_uncanonical_layout;

    // Check audio format
    if(hdr->fmt_format != 1)
        return WAVERR_unknown_audio_format;

    // Now populate our sequence with the format data
    sequence->channels = hdr->fmt_channels;
    sequence->sample_rate = hdr->fmt_sample_rate;
    sequence->bit_depth = hdr->fmt_bit_depth;
    sequence->frame_size = hdr->fmt_frame_size;
    sequence->data = hdr->data_contents;

    // Calculate number of frames
    size_t data_size = size - sizeof(wave_header_t) - sizeof(hdr->data_contents);
    sequence->frames = data_size/sequence->frame_size;

    // All okay
    return WAVERR_okay;
}

/*
 * wave_read
 *
 * Read from a wave sequence
 */
uint32_t wave_read(wave_sequence_t* sequence, void* buffer, uint32_t samples) {
    // Ensure sure we will not read past the end of the sequence
    uint32_t frames = samples/sequence->channels;
    uint32_t remaining = sequence->frames - sequence->position;
    if(frames > remaining) frames = remaining;

    // Copy frames into the buffer and advance position
    memcpy(buffer, sequence->data + (sequence->position * sequence->frame_size), frames * (sequence->frame_size));
    sequence->position += frames;

    // Return samples copied
    return frames * sequence->channels;
}

/*
 * wave_seek
 *
 * Seek to the specified position in the wave sequence
 */
uint32_t wave_seek(wave_sequence_t* sequence, uint32_t frame) {
    sequence->position = frame % sequence->frames;
    return sequence->position;
}

/*
 * wave_position
 *
 * Return the next frame in the wave sequence
 */
uint32_t wave_position(wave_sequence_t* sequence) {
    return sequence->position;
}

/*
 * wave_remaining
 *
 * Return the number of frames remaining in the wave sequence
 */
uint32_t wave_remaining(wave_sequence_t* sequence) {
    return sequence->frames - sequence->position;
}
