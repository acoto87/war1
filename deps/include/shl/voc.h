/*
    voc.h - Single-header library for Creative Voice File (VOC) I/O and PCM resampling.

    MIT License

    Copyright (c) 2026 Alejandro Coto Gutiérrez

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

    Reads and writes Creative Voice File (.voc) audio (block types 0x01, 0x08, 0x09).
    Supports mono and stereo PCM at 8 and 16 bits per sample. Includes a PCM
    resampler with fast paths for 2x/4x integer ratios on 8-bit mono and a general
    fixed-point 16.16 linear interpolation fallback.

    USAGE
    Define MINIVOC_IMPLEMENTATION in exactly one translation unit before including
    this header to compile the implementation:

        #define MINIVOC_IMPLEMENTATION
        #include "voc.h"

    In all other translation units include without the define:

        #include "voc.h"

    CUSTOM MEMORY ALLOCATOR
    Override the three allocator macros before the implementation include:

        #define MINIVOC_MALLOC(sz)         my_malloc(sz)
        #define MINIVOC_REALLOC(ptr, size) my_realloc(ptr, size)
        #define MINIVOC_FREE(ptr)          my_free(ptr)
        #define MINIVOC_IMPLEMENTATION
        #include "voc.h"

    ATTRIBUTION
    This code is a port in C of the VOC converter by Jimmy Salmon in the War1gus
    repository: https://github.com/Wargus/war1gus/blob/master/war1tool.cpp

    REFERENCES
    https://en.wikipedia.org/wiki/Creative_Voice_file
*/

#ifndef MINI_VOC_H
#define MINI_VOC_H

#include <stddef.h>
#include <stdint.h>

//--- Custom Memory Allocator Configuration ---
#if !defined(MINIVOC_MALLOC) || !defined(MINIVOC_REALLOC) || !defined(MINIVOC_FREE)
    #include <stdlib.h>
    #ifndef MINIVOC_MALLOC
        #define MINIVOC_MALLOC(sz)         malloc(sz)
    #endif
    #ifndef MINIVOC_REALLOC
        #define MINIVOC_REALLOC(ptr, size) realloc(ptr, size)
    #endif
    #ifndef MINIVOC_FREE
        #define MINIVOC_FREE(ptr)          free(ptr)
    #endif
#endif

typedef struct {
    uint32_t channels;
    uint32_t sample_rate;
    uint32_t bits_per_sample;
    uint32_t data_length; /* Total size of the raw PCM buffer in bytes */
    uint8_t* data;
} mv_audio_buffer;

#ifdef __cplusplus
extern "C" {
#endif

// Core API Functions
int32_t mv_read_memory(const void* buffer, size_t buffer_size, mv_audio_buffer* out_audio);
int32_t mv_read_file(const char* filename, mv_audio_buffer* out_audio);

void* mv_write_memory(const mv_audio_buffer* audio, size_t* out_buffer_size);
int32_t mv_write_file(const char* filename, const mv_audio_buffer* audio);

int32_t mv_resample_pcm(const mv_audio_buffer* src, mv_audio_buffer* dst, uint32_t target_sample_rate);
void   mv_free_buffer(mv_audio_buffer* audio);

#ifdef __cplusplus
}
#endif

#endif // MINIVOC_H

//==============================================================================
// IMPLEMENTATION CODE
//==============================================================================
#ifdef MINIVOC_IMPLEMENTATION

#include <string.h>
#include <stdio.h>

/* Inline memory parsing helpers to ensure optimization without generic memcpy */
static inline uint16_t mv_read_u16_le(const uint8_t* p) {
    return (uint16_t)(p[0] | (p[1] << 8));
}

static inline uint32_t mv_read_u24_le(const uint8_t* p) {
    return (uint32_t)(p[0] | (p[1] << 8) | (p[2] << 16));
}

static inline uint32_t mv_read_u32_le(const uint8_t* p) {
    return (uint32_t)(p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24));
}

static inline void mv_write_u16_le(uint8_t* p, uint16_t val) {
    p[0] = (uint8_t)(val & 0xFF);
    p[1] = (uint8_t)((val >> 8) & 0xFF);
}

static inline void mv_write_u32_le(uint8_t* p, uint32_t val) {
    p[0] = (uint8_t)(val & 0xFF);
    p[1] = (uint8_t)((val >> 8) & 0xFF);
    p[2] = (uint8_t)((val >> 16) & 0xFF);
    p[3] = (uint8_t)((val >> 24) & 0xFF);
}

// Parse a VOC file directly from a block of memory
int32_t mv_read_memory(const void* buffer, size_t buffer_size, mv_audio_buffer* out_audio) {
    if (!buffer || buffer_size < 26 || !out_audio) return 0;

    const uint8_t* src = (const uint8_t*)buffer;
    size_t rem = buffer_size;

    // Validate absolute baseline Creative Voice File string token
    if (memcmp(src, "Creative Voice File\x1A", 20) != 0) return 0;

    uint16_t data_offset   = mv_read_u16_le(src + 20);
    uint16_t version       = mv_read_u16_le(src + 22);
    uint16_t version_check = mv_read_u16_le(src + 24);

    // Quick hardware-based validation integrity mask
    if ((uint16_t)(~version + 0x1234) != version_check) return 0;
    if (data_offset >= buffer_size) return 0;

    // Move stream pointers directly to the sound data block list
    src = (const uint8_t*)buffer + data_offset;
    rem = buffer_size - data_offset;

    uint8_t* pcm_accumulator = NULL;
    size_t accumulated_bytes = 0;

    // Sound Blaster default legacy configuration fallbacks
    uint32_t current_rate = 11025;
    uint32_t current_bits = 8;
    uint32_t current_channels = 1;

    int ext_attributes_set = 0;
    int parse_error = 0;

    // Block Processing Stream Loop
    while (rem > 0 && !parse_error) {
        uint8_t block_type = *src;
        src++; rem--;

        if (block_type == 0x00) break; // End of file terminator token

        if (rem < 3) { parse_error = 1; break; }
        uint32_t block_len = mv_read_u24_le(src);
        src += 3; rem -= 3;

        if (rem < block_len) { parse_error = 1; break; }

        switch (block_type) {
            case 0x01: { // Standard 8-bit Uncompressed Voice Block
                if (block_len < 2) { parse_error = 1; break; }
                uint8_t divisor = src[0];
                uint8_t compression = src[1];

                if (compression != 0x00) { parse_error = 1; break; } // ADPCM modes not supported

                if (!ext_attributes_set) {
                    current_rate = 1000000 / (256 - divisor);
                    current_bits = 8;
                    current_channels = 1;
                }
                ext_attributes_set = 0; // Reset state tracking flag

                size_t data_len = block_len - 2;
                uint8_t* next_alloc = (uint8_t*)MINIVOC_REALLOC(pcm_accumulator, accumulated_bytes + data_len);
                if (!next_alloc) { parse_error = 1; break; }

                pcm_accumulator = next_alloc;
                memcpy(pcm_accumulator + accumulated_bytes, src + 2, data_len);
                accumulated_bytes += data_len;
                break;
            }
            case 0x08: { // Extended Attribute setup block (Precedes a type 0x01 block)
                if (block_len < 4) { parse_error = 1; break; }
                uint16_t div_code = mv_read_u16_le(src);
                uint8_t pack_method = src[2];
                uint8_t mode = src[3]; // 0 = Mono, 1 = Stereo

                if (pack_method != 0) { parse_error = 1; break; }

                current_channels = (mode == 1) ? 2 : 1;
                current_bits = 8;
                current_rate = 256000000 / (65536 - div_code);
                if (current_channels == 2) current_rate /= 2;

                ext_attributes_set = 1;
                break;
            }
            case 0x09: { // Modern Unified Audio Setup Block (VOC v2.00+)
                if (block_len < 12) { parse_error = 1; break; }
                uint32_t rate = mv_read_u32_le(src);
                uint8_t bits = src[4];
                uint8_t channels = src[5];
                uint16_t format = mv_read_u16_le(src + 6);

                if (format != 0x0000 && format != 0x0004) { parse_error = 1; break; } // Filter raw PCM only

                current_rate = rate;
                current_bits = bits;
                current_channels = channels;

                size_t data_len = block_len - 12;
                uint8_t* next_alloc = (uint8_t*)MINIVOC_REALLOC(pcm_accumulator, accumulated_bytes + data_len);
                if (!next_alloc) { parse_error = 1; break; }

                pcm_accumulator = next_alloc;
                memcpy(pcm_accumulator + accumulated_bytes, src + 12, data_len);
                accumulated_bytes += data_len;
                break;
            }
            default:
                // Ignore markers, looping points, silence ranges safely
                break;
        }
        src += block_len;
        rem -= block_len;
    }

    if (parse_error || accumulated_bytes == 0) {
        if (pcm_accumulator) MINIVOC_FREE(pcm_accumulator);
        return 0;
    }

    out_audio->channels = current_channels;
    out_audio->sample_rate = current_rate;
    out_audio->bits_per_sample = current_bits;
    out_audio->data_length = (uint32_t)accumulated_bytes;
    out_audio->data = pcm_accumulator;

    return 1;
}

// Read a VOC file from the filesystem
int32_t mv_read_file(const char* filename, mv_audio_buffer* out_audio) {
    FILE* f = fopen(filename, "rb");
    if (!f) return 0;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size <= 0) { fclose(f); return 0; }

    uint8_t* mem = (uint8_t*)MINIVOC_MALLOC(size);
    if (!mem) { fclose(f); return 0; }

    size_t read_bytes = fread(mem, 1, size, f);
    fclose(f);

    int32_t result = 0;
    if (read_bytes == (size_t)size) {
        result = mv_read_memory(mem, size, out_audio);
    }
    MINIVOC_FREE(mem);
    return result;
}

// Build a canonical raw VOC file image directly into a heap allocation
void* mv_write_memory(const mv_audio_buffer* audio, size_t* out_buffer_size) {
    if (!audio || !audio->data || !out_buffer_size) return NULL;

    // Total size = 26 (File Header) + 16 (Block 9 structural framing) + Payload data + 1 (Terminator byte)
    size_t total_size = 26 + 16 + audio->data_length + 1;
    uint8_t* out_buf = (uint8_t*)MINIVOC_MALLOC(total_size);
    if (!out_buf) return NULL;

    uint8_t* p = out_buf;

    /* Write Main VOC Header */
    memcpy(p, "Creative Voice File\x1A", 20); p += 20;
    mv_write_u16_le(p, 26);     p += 2; // Data Offset
    mv_write_u16_le(p, 0x0114); p += 2; // Version v1.20
    mv_write_u16_le(p, 0x111F); p += 2; // Checksum: (uint16_t)(~0x0114 + 0x1234) = 0x111F

    /* Setup Block Type 0x09 Entry Header */
    *p = 0x09; p++;
    uint32_t payload_len = 12 + audio->data_length;
    p[0] = (uint8_t)(payload_len & 0xFF);
    p[1] = (uint8_t)((payload_len >> 8) & 0xFF);
    p[2] = (uint8_t)((payload_len >> 16) & 0xFF);
    p += 3;

    /* Populate format parameters directly */
    mv_write_u32_le(p, audio->sample_rate); p += 4;
    *p = (uint8_t)audio->bits_per_sample;     p++;
    *p = (uint8_t)audio->channels;            p++;
    mv_write_u16_le(p, 0x0000);             p += 2; // Unsigned raw PCM code
    mv_write_u32_le(p, 0);                  p += 4; // Unused padding space

    /* Append Audio Stream & Terminator Byte */
    memcpy(p, audio->data, audio->data_length); p += audio->data_length;
    *p = 0x00;

    *out_buffer_size = total_size;
    return out_buf;
}

// Save sound buffer directly to the disk filesystem
int32_t mv_write_file(const char* filename, const mv_audio_buffer* audio) {
    size_t total_size = 0;
    void* file_image = mv_write_memory(audio, &total_size);
    if (!file_image) return 0;

    FILE* f = fopen(filename, "wb");
    if (!f) {
        MINIVOC_FREE(file_image);
        return 0;
    }

    size_t written = fwrite(file_image, 1, total_size, f);
    fclose(f);
    MINIVOC_FREE(file_image);

    return (written == total_size);
}

// Free resources associated with a buffer allocation
void mv_free_buffer(mv_audio_buffer* audio) {
    if (audio && audio->data) {
        MINIVOC_FREE(audio->data);
        audio->data = NULL;
        audio->data_length = 0;
    }
}

// --- OPTIMIZED FAST PATHS FOR MONO 8-BIT SIGNALS ---

static void mv_upsample_2x_u8(const uint8_t* src, uint8_t* dst, int32_t samples) {
    int32_t j = 0;
    for (int32_t i = 0; i < samples - 1; i++) {
        int32_t a = src[i];
        int32_t b = src[i + 1];
        dst[j++] = (uint8_t)a;
        dst[j++] = (uint8_t)(a + ((b - a) >> 1));
    }
    dst[j++] = src[samples - 1];
    dst[j++] = src[samples - 1];
}

static void mv_upsample_4x_u8(const uint8_t* src, uint8_t* dst, int32_t samples) {
    int32_t j = 0;
    for (int32_t i = 0; i < samples - 1; i++) {
        int32_t a = src[i];
        int32_t b = src[i + 1];
        int32_t diff = b - a;
        dst[j++] = (uint8_t)a;
        dst[j++] = (uint8_t)(a + ((diff * 1) >> 2));
        dst[j++] = (uint8_t)(a + ((diff * 2) >> 2));
        dst[j++] = (uint8_t)(a + ((diff * 3) >> 2));
    }
    for (int k = 0; k < 4; ++k) dst[j++] = src[samples - 1];
}

static void mv_downsample_2x_u8(const uint8_t* src, uint8_t* dst, int32_t target_samples) {
    for (int32_t i = 0; i < target_samples; i++) {
        dst[i] = (uint8_t)(((int32_t)src[i * 2] + src[i * 2 + 1]) >> 1);
    }
}

static void mv_downsample_4x_u8(const uint8_t* src, uint8_t* dst, int32_t target_samples) {
    for (int32_t i = 0; i < target_samples; i++) {
        int32_t idx = i * 4;
        dst[i] = (uint8_t)(((int32_t)src[idx] + src[idx + 1] + src[idx + 2] + src[idx + 3]) >> 2);
    }
}

// Decoupled streaming resampler interface
int32_t mv_resample_pcm(const mv_audio_buffer* src, mv_audio_buffer* dst, uint32_t target_sample_rate) {
    if (!src || !dst || !src->data || src->data_length == 0 || target_sample_rate == 0) return 0;

    dst->channels = src->channels;
    dst->bits_per_sample = src->bits_per_sample;
    dst->sample_rate = target_sample_rate;

    if (src->sample_rate == target_sample_rate) {
        dst->data_length = src->data_length;
        dst->data = (uint8_t*)MINIVOC_MALLOC(dst->data_length);
        if (!dst->data) return 0;
        memcpy(dst->data, src->data, src->data_length);
        return 1;
    }

    uint32_t bytes_per_sample = (src->bits_per_sample / 8) * src->channels;
    int32_t total_src_samples = src->data_length / bytes_per_sample;

    // Evaluate Fast-Paths exclusively for 8-bit Mono streams
    if (src->channels == 1 && src->bits_per_sample == 8) {
        if (target_sample_rate == src->sample_rate * 2) {
            dst->data_length = src->data_length * 2;
            dst->data = (uint8_t*)MINIVOC_MALLOC(dst->data_length);
            if (!dst->data) return 0;
            mv_upsample_2x_u8(src->data, dst->data, total_src_samples);
            return 1;
        }
        if (target_sample_rate == src->sample_rate * 4) {
            dst->data_length = src->data_length * 4;
            dst->data = (uint8_t*)MINIVOC_MALLOC(dst->data_length);
            if (!dst->data) return 0;
            mv_upsample_4x_u8(src->data, dst->data, total_src_samples);
            return 1;
        }
        if (src->sample_rate == target_sample_rate * 2) {
            dst->data_length = src->data_length / 2;
            dst->data = (uint8_t*)MINIVOC_MALLOC(dst->data_length);
            if (!dst->data) return 0;
            mv_downsample_2x_u8(src->data, dst->data, total_src_samples / 2);
            return 1;
        }
        if (src->sample_rate == target_sample_rate * 4) {
            dst->data_length = src->data_length / 4;
            dst->data = (uint8_t*)MINIVOC_MALLOC(dst->data_length);
            if (!dst->data) return 0;
            mv_downsample_4x_u8(src->data, dst->data, total_src_samples / 4);
            return 1;
        }
    }

    // GENERAL FALLBACK PATH: Fixed-point 16.16 linear interpolation resampler
    double scale_ratio = (double)src->sample_rate / (double)target_sample_rate;
    int32_t total_dst_samples = (int32_t)((double)total_src_samples / scale_ratio);
    dst->data_length = total_dst_samples * bytes_per_sample;
    dst->data = (uint8_t*)MINIVOC_MALLOC(dst->data_length);
    if (!dst->data) return 0;

    uint32_t step_fixed = (uint32_t)((scale_ratio * 65536.0) + 0.5);
    uint32_t current_fixed_pos = 0;

    for (int32_t i = 0; i < total_dst_samples; i++) {
        int32_t src_idx1 = (int32_t)(current_fixed_pos >> 16);
        int32_t src_idx2 = src_idx1 + 1;
        if (src_idx2 >= total_src_samples) src_idx2 = src_idx1;

        int32_t frac = (int32_t)(current_fixed_pos & 0xFFFF);

        for (uint32_t ch = 0; ch < src->channels; ch++) {
            if (src->bits_per_sample == 8) {
                size_t byte_pos1 = (size_t)src_idx1 * src->channels + ch;
                size_t byte_pos2 = (size_t)src_idx2 * src->channels + ch;

                int32_t a = src->data[byte_pos1];
                int32_t b = src->data[byte_pos2];

                size_t dest_pos = (size_t)i * src->channels + ch;
                dst->data[dest_pos] = (uint8_t)(a + (((b - a) * frac) >> 16));
            }
            else if (src->bits_per_sample == 16) {
                const int16_t* src_s16 = (const int16_t*)src->data;
                int16_t* dst_s16 = (int16_t*)dst->data;

                size_t sample_pos1 = (size_t)src_idx1 * src->channels + ch;
                size_t sample_pos2 = (size_t)src_idx2 * src->channels + ch;

                int32_t a = src_s16[sample_pos1];
                int32_t b = src_s16[sample_pos2];

                size_t dest_pos = (size_t)i * src->channels + ch;
                dst_s16[dest_pos] = (int16_t)(a + (((b - a) * frac) >> 16));
            }
        }
        current_fixed_pos += step_fixed;
    }

    return 1;
}

#endif // MINIVOC_IMPLEMENTATION
