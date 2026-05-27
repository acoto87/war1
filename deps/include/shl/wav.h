/*
    wav.h - Single-header library for WAV file I/O and PCM resampling.

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

    Reads and writes RIFF/WAVE PCM audio files (8-bit and 16-bit, mono and stereo).
    Includes a PCM resampler with fast paths for 2x/4x integer ratios on 8-bit mono
    and a general fixed-point 16.16 linear interpolation fallback.

    USAGE
    Define MINIWAVE_IMPLEMENTATION in exactly one translation unit before including
    this header to compile the implementation:

        #define MINIWAVE_IMPLEMENTATION
        #include "wav.h"

    In all other translation units include without the define:

        #include "wav.h"

    CUSTOM MEMORY ALLOCATOR
    Override the three allocator macros before the implementation include:

        #define MINIWAVE_MALLOC(sz)         my_malloc(sz)
        #define MINIWAVE_REALLOC(ptr, size) my_realloc(ptr, size)
        #define MINIWAVE_FREE(ptr)          my_free(ptr)
        #define MINIWAVE_IMPLEMENTATION
        #include "wav.h"

    REFERENCES
    https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
*/

#ifndef MINIWAVE_H
#define MINIWAVE_H

#include <stddef.h>
#include <stdint.h>

//--- Custom Memory Allocator Configuration ---
#if !defined(MINIWAVE_MALLOC) || !defined(MINIWAVE_REALLOC) || !defined(MINIWAVE_FREE)
    #include <stdlib.h>
    #ifndef MINIWAVE_MALLOC
        #define MINIWAVE_MALLOC(sz)         malloc(sz)
    #endif
    #ifndef MINIWAVE_REALLOC
        #define MINIWAVE_REALLOC(ptr, size) realloc(ptr, size)
    #endif
    #ifndef MINIWAVE_FREE
        #define MINIWAVE_FREE(ptr)          free(ptr)
    #endif
#endif

#pragma pack(push, 1)
typedef struct {
    char     riff_marker[4];     // "RIFF"
    uint32_t overall_size;       // File size - 8 bytes
    char     wave_marker[4];     // "WAVE"
    char     fmt_marker[4];      // "fmt "
    uint32_t fmt_length;         // Length of format data (16)
    uint16_t audio_format;       // 1 for PCM (uncompressed)
    uint16_t channels;           // 1 = Mono, 2 = Stereo
    uint32_t sample_rate;        // e.g. 11025, 44100
    uint32_t byte_rate;          // sample_rate * channels * (bits_per_sample/8)
    uint16_t block_align;        // channels * (bits_per_sample/8)
    uint16_t bits_per_sample;    // 8 or 16 bits
    char     data_marker[4];     // "data"
    uint32_t data_size;          // Number of bytes for raw audio PCM
} mw_wav_header;
#pragma pack(pop)

typedef struct {
    uint32_t channels;
    uint32_t sample_rate;
    uint32_t bits_per_sample;
    uint32_t data_length;
    uint8_t* data;
} mw_audio_buffer;

#ifdef __cplusplus
extern "C" {
#endif

// Core API Functions
int32_t mw_read_memory(const void* buffer, size_t buffer_size, mw_audio_buffer* out_audio);
int32_t mw_read_file(const char* filename, mw_audio_buffer* out_audio);

void* mw_write_memory(const mw_audio_buffer* audio, size_t* out_buffer_size);
int32_t mw_write_file(const char* filename, const mw_audio_buffer* audio);

int32_t mw_resample_pcm(const mw_audio_buffer* src, mw_audio_buffer* dst, uint32_t target_sample_rate);
void   mw_free_buffer(mw_audio_buffer* audio);

#ifdef __cplusplus
}
#endif

#endif // MINIWAVE_H

//==============================================================================
// IMPLEMENTATION CODE
//==============================================================================
#ifdef MINIWAVE_IMPLEMENTATION

#include <string.h>
#include <stdio.h>

// Parse a wave file directly from a block of memory
int32_t mw_read_memory(const void* buffer, size_t buffer_size, mw_audio_buffer* out_audio) {
    if (!buffer || buffer_size < sizeof(mw_wav_header) || !out_audio) return 0;

    const mw_wav_header* header = (const mw_wav_header*)buffer;

    // Validate absolute baseline headers to ensure asset safety
    if (memcmp(header->riff_marker, "RIFF", 4) != 0 ||
        memcmp(header->wave_marker, "WAVE", 4) != 0 ||
        memcmp(header->fmt_marker, "fmt ", 4) != 0) {
        return 0;
    }

    // Locate data chunk safely (handles metadata variations)
    const uint8_t* data_ptr = (const uint8_t*)buffer + 20 + header->fmt_length;
    while (data_ptr + 8 <= (const uint8_t*)buffer + buffer_size) {
        if (memcmp(data_ptr, "data", 4) == 0) {
            break;
        }
        uint32_t chunk_size = *(const uint32_t*)(data_ptr + 4);
        data_ptr += 8 + chunk_size; // Skip metadata chunks like LIST, metadata, tags
    }

    if (data_ptr + 8 > (const uint8_t*)buffer + buffer_size) return 0;

    uint32_t data_size = *(const uint32_t*)(data_ptr + 4);
    data_ptr += 8; // Advance past chunk header to start of raw samples

    if (data_ptr + data_size > (const uint8_t*)buffer + buffer_size) return 0;

    out_audio->channels = header->channels;
    out_audio->sample_rate = header->sample_rate;
    out_audio->bits_per_sample = header->bits_per_sample;
    out_audio->data_length = data_size;

    out_audio->data = (uint8_t*)MINIWAVE_MALLOC(data_size);
    if (!out_audio->data) return 0;

    memcpy(out_audio->data, data_ptr, data_size);
    return 1;
}

// Read a wave file from the filesystem
int32_t mw_read_file(const char* filename, mw_audio_buffer* out_audio) {
    FILE* f = fopen(filename, "rb");
    if (!f) return 0;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t* mem = (uint8_t*)MINIWAVE_MALLOC(size);
    if (!mem) {
        fclose(f);
        return 0;
    }

    size_t read_bytes = fread(mem, 1, size, f);
    fclose(f);

    if (read_bytes != (size_t)size) {
        MINIWAVE_FREE(mem);
        return 0;
    }

    int32_t result = mw_read_memory(mem, size, out_audio);
    MINIWAVE_FREE(mem);
    return result;
}

// Build a canonical raw WAV file image directly into a heap allocation
void* mw_write_memory(const mw_audio_buffer* audio, size_t* out_buffer_size) {
    if (!audio || !audio->data || !out_buffer_size) return NULL;

    size_t total_size = sizeof(mw_wav_header) + audio->data_length;
    uint8_t* out_buf = (uint8_t*)MINIWAVE_MALLOC(total_size);
    if (!out_buf) return NULL;

    mw_wav_header* header = (mw_wav_header*)out_buf;

    memcpy(header->riff_marker, "RIFF", 4);
    header->overall_size = (uint32_t)(total_size - 8);
    memcpy(header->wave_marker, "WAVE", 4);
    memcpy(header->fmt_marker, "fmt ", 4);

    header->fmt_length = 16;
    header->audio_format = 1; // PCM
    header->channels = (uint16_t)audio->channels;
    header->sample_rate = audio->sample_rate;
    header->bits_per_sample = (uint16_t)audio->bits_per_sample;

    header->block_align = (uint16_t)(audio->channels * (audio->bits_per_sample / 8));
    header->byte_rate = audio->sample_rate * header->block_align;

    memcpy(header->data_marker, "data", 4);
    header->data_size = audio->data_length;

    memcpy(out_buf + sizeof(mw_wav_header), audio->data, audio->data_length);

    *out_buffer_size = total_size;
    return out_buf;
}

// Save sound buffer directly to the disk filesystem
int32_t mw_write_file(const char* filename, const mw_audio_buffer* audio) {
    size_t total_size = 0;
    void* file_image = mw_write_memory(audio, &total_size);
    if (!file_image) return 0;

    FILE* f = fopen(filename, "wb");
    if (!f) {
        MINIWAVE_FREE(file_image);
        return 0;
    }

    size_t written = fwrite(file_image, 1, total_size, f);
    fclose(f);
    MINIWAVE_FREE(file_image);

    return (written == total_size);
}

// Free resources associated with a buffer allocation
void mw_free_buffer(mw_audio_buffer* audio) {
    if (audio && audio->data) {
        MINIWAVE_FREE(audio->data);
        audio->data = NULL;
        audio->data_length = 0;
    }
}

// --- OPTIMIZED FAST PATHS FOR MONO 8-BIT SIGNALS ---

static void mw_upsample_2x_u8(const uint8_t* src, uint8_t* dst, int32_t samples) {
    int32_t j = 0;
    for (int32_t i = 0; i < samples - 1; i++) {
        int32_t a = src[i];
        int32_t b = src[i + 1];
        dst[j++] = (uint8_t)a;
        dst[j++] = (uint8_t)(a + ((b - a) >> 1)); // Midpoint fast division
    }
    dst[j++] = src[samples - 1];
    dst[j++] = src[samples - 1];
}

static void mw_upsample_4x_u8(const uint8_t* src, uint8_t* dst, int32_t samples) {
    int32_t j = 0;
    for (int32_t i = 0; i < samples - 1; i++) {
        int32_t a = src[i];
        int32_t b = src[i + 1];
        int32_t diff = b - a;
        dst[j++] = (uint8_t)a;
        dst[j++] = (uint8_t)(a + ((diff * 1) >> 2)); // 25% Interpolation
        dst[j++] = (uint8_t)(a + ((diff * 2) >> 2)); // 50% Interpolation
        dst[j++] = (uint8_t)(a + ((diff * 3) >> 2)); // 75% Interpolation
    }
    for (int k = 0; k < 4; ++k) dst[j++] = src[samples - 1];
}

static void mw_downsample_2x_u8(const uint8_t* src, uint8_t* dst, int32_t target_samples) {
    for (int32_t i = 0; i < target_samples; i++) {
        // Average matching sequential sample sets down via bitshifts
        dst[i] = (uint8_t)(((int32_t)src[i * 2] + src[i * 2 + 1]) >> 1);
    }
}

static void mw_downsample_4x_u8(const uint8_t* src, uint8_t* dst, int32_t target_samples) {
    for (int32_t i = 0; i < target_samples; i++) {
        int32_t idx = i * 4;
        dst[i] = (uint8_t)(((int32_t)src[idx] + src[idx + 1] + src[idx + 2] + src[idx + 3]) >> 2);
    }
}

// Decoupled streaming resampler interface.
// Uses fast-paths for factors of 2 and 4 on 8-bit mono streams, fallback linear tracker otherwise.
int32_t mw_resample_pcm(const mw_audio_buffer* src, mw_audio_buffer* dst, uint32_t target_sample_rate) {
    if (!src || !dst || !src->data || src->data_length == 0 || target_sample_rate == 0) return 0;

    dst->channels = src->channels;
    dst->bits_per_sample = src->bits_per_sample;
    dst->sample_rate = target_sample_rate;

    // Perfect structural identity match: skip computations and duplicate memory block directly
    if (src->sample_rate == target_sample_rate) {
        dst->data_length = src->data_length;
        dst->data = (uint8_t*)MINIWAVE_MALLOC(dst->data_length);
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
            dst->data = (uint8_t*)MINIWAVE_MALLOC(dst->data_length);
            if (!dst->data) return 0;
            mw_upsample_2x_u8(src->data, dst->data, total_src_samples);
            return 1;
        }
        if (target_sample_rate == src->sample_rate * 4) {
            dst->data_length = src->data_length * 4;
            dst->data = (uint8_t*)MINIWAVE_MALLOC(dst->data_length);
            if (!dst->data) return 0;
            mw_upsample_4x_u8(src->data, dst->data, total_src_samples);
            return 1;
        }
        if (src->sample_rate == target_sample_rate * 2) {
            dst->data_length = src->data_length / 2;
            dst->data = (uint8_t*)MINIWAVE_MALLOC(dst->data_length);
            if (!dst->data) return 0;
            mw_downsample_2x_u8(src->data, dst->data, total_src_samples / 2);
            return 1;
        }
        if (src->sample_rate == target_sample_rate * 4) {
            dst->data_length = src->data_length / 4;
            dst->data = (uint8_t*)MINIWAVE_MALLOC(dst->data_length);
            if (!dst->data) return 0;
            mw_downsample_4x_u8(src->data, dst->data, total_src_samples / 4);
            return 1;
        }
    }

    // GENERAL FALLBACK PATH: Fixed-point 16.16 linear interpolation resampler
    // Handled safely for varying configurations (Channels: 1/2, Bits: 8/16)
    double scale_ratio = (double)src->sample_rate / (double)target_sample_rate;
    int32_t total_dst_samples = (int32_t)((double)total_src_samples / scale_ratio);
    dst->data_length = total_dst_samples * bytes_per_sample;
    dst->data = (uint8_t*)MINIWAVE_MALLOC(dst->data_length);
    if (!dst->data) return 0;

    // Fixed point step calculations
    uint32_t step_fixed = (uint32_t)((scale_ratio * 65536.0) + 0.5);
    uint32_t current_fixed_pos = 0;

    for (int32_t i = 0; i < total_dst_samples; i++) {
        int32_t src_idx1 = (int32_t)(current_fixed_pos >> 16);
        int32_t src_idx2 = src_idx1 + 1;
        if (src_idx2 >= total_src_samples) src_idx2 = src_idx1;

        // Extract fractional coefficient mask
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

#endif // MINIWAVE_IMPLEMENTATION
