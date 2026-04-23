/*
    wav.h - based on Wave_Writer by Shay Green

    MIT License

    Copyright (C) 2003-2005 Shay Green

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

    Single-header WAVE file writer for streaming 16-bit PCM samples to disk.
    The API uses the wav_ prefix and writes the final RIFF/WAVE header when the
    file is flushed and closed.

    USAGE
    Include this header in all translation units that need the declarations.
    Define SHL_WAV_IMPLEMENTATION in exactly one translation unit before the
    include to compile the implementation:

        #define SHL_WAV_IMPLEMENTATION
        #include "wav.h"

    Include the header without that define everywhere else:

        #include "wav.h"

    CUSTOMISATION
    The sample type and internal buffer size are controlled by compile-time
    macros near the top of the implementation. If you need different buffering
    or output behavior, adjust those constants or the implementation section.

    NOTES
    wav_init opens the output file and prepares the buffer, wav_write streams
    16-bit samples into it, and wav_flush writes the final header before
    optionally closing the file handle.
*/

#ifndef SHL_WAVE_WRITER_H
#define SHL_WAVE_WRITER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef short wav_sample_t;
typedef struct _wav_file_t wav_file_t;

#define WAV_BUFFER_SIZE (32768 * 2)
#define WAV_HEADER_SIZE 0x2C

#define WAV_U8_CAST(x) ((unsigned char)(x))

bool wav_init(wav_file_t* waveFile, long sampleRate, const char* filename);
void wav_stereo(wav_file_t* waveFile, bool stereo);
long wav_sampleCount(wav_file_t* waveFile);
bool wav_write(wav_file_t* waveFile, const wav_sample_t* in, long count, int skip);
bool wav_flush(wav_file_t* waveFile, bool closeFile);

#ifdef __cplusplus
}
#endif

#ifdef SHL_WAV_IMPLEMENTATION

#include <assert.h>
#include <limits.h>

struct _wav_file_t
{
    FILE*   		_file;
    unsigned char*  _buffer;
    long    		sampleRate;
    long    		_sampleCount;
    long    		_bufferPos;
    int     		channelCount;
};

static bool wav__flushBuffer(wav_file_t* waveFile)
{
    if (waveFile->_bufferPos && !fwrite(waveFile->_buffer, waveFile->_bufferPos, 1, waveFile->_file)) {
        return false;
    }

    waveFile->_bufferPos = 0;
    return true;
}

bool wav_init(wav_file_t* waveFile, long sampleRate, const char* filename)
{
    waveFile->_buffer = (unsigned char*) calloc(WAV_BUFFER_SIZE, sizeof(unsigned char));
    if (!waveFile->_buffer) {
        return false;
    }

    waveFile->_file = NULL;
    waveFile->_file = fopen(filename, "wb");
    if (!waveFile->_file) {
        free(waveFile->_buffer);
        waveFile->_buffer = NULL;
        return false;
    }

    waveFile->sampleRate = sampleRate;
    waveFile->channelCount = 1;

    waveFile->_sampleCount = 0;
    waveFile->_bufferPos = WAV_HEADER_SIZE;

    return true;
}

void wav_stereo(wav_file_t* waveFile, bool stereo)
{
    waveFile->channelCount = stereo ? 2 : 1;
}

long wav_sampleCount(wav_file_t* waveFile)
{
    return waveFile->_sampleCount;
}

bool wav_write(wav_file_t* waveFile, const wav_sample_t* in, long count, int skip)
{
    waveFile->_sampleCount += count;
    while (count) {
        if (waveFile->_bufferPos >= WAV_BUFFER_SIZE) {
            if (!wav__flushBuffer(waveFile)) {
                return false;
            }
        }

        long n = (unsigned long) (WAV_BUFFER_SIZE - waveFile->_bufferPos) / sizeof (wav_sample_t);
        if (n > count) n = count;
        count -= n;

        // convert to lsb first format
        unsigned char* p = &waveFile->_buffer[waveFile->_bufferPos];
        while (n--) {
            int s = *in;
            in += skip;
            *p++ = (unsigned char) s;
            *p++ = (unsigned char) (s >> 8);
        }

        assert(p - waveFile->_buffer <= LONG_MAX);
        waveFile->_bufferPos = (long)(p - waveFile->_buffer);
    }

    return true;
}

bool wav_flush(wav_file_t* waveFile, bool closeFile)
{
    if (!wav__flushBuffer(waveFile)) {
        return false;
    }

    if (closeFile) {
        // generate header
        long sample_rate = waveFile->sampleRate;
        int channel_count = waveFile->channelCount;
        long ds = waveFile->_sampleCount * sizeof (wav_sample_t);
        long rs = WAV_HEADER_SIZE - 8 + ds;
        int frame_size = channel_count * sizeof (wav_sample_t);
        long bps = sample_rate * frame_size;

        unsigned char header[WAV_HEADER_SIZE] = {
            'R','I','F','F',
            WAV_U8_CAST(rs), WAV_U8_CAST(rs >> 8),                               // length of rest of file
            WAV_U8_CAST(rs >> 16), WAV_U8_CAST(rs >> 24),
            'W','A','V','E',
            'f','m','t',' ',
            0x10, 0, 0, 0,                                                       // size of fmt chunk
            1, 0,                                                                // uncompressed format
            WAV_U8_CAST(channel_count), 0,                                       // channel count
            WAV_U8_CAST(sample_rate), WAV_U8_CAST(sample_rate >> 8),             // sample rate
            WAV_U8_CAST(sample_rate >> 16), WAV_U8_CAST(sample_rate >> 24),
            WAV_U8_CAST(bps), WAV_U8_CAST(bps >> 8),                             // bytes per second
            WAV_U8_CAST(bps >> 16), WAV_U8_CAST(bps >> 24),
            WAV_U8_CAST(frame_size), 0,                                          // bytes per sample frame
            16, 0,                                                               // bits per sample
            'd','a','t','a',
            WAV_U8_CAST(ds), WAV_U8_CAST(ds >> 8),                               // size of sample data
            WAV_U8_CAST(ds >> 16), WAV_U8_CAST(ds >> 24)
            // ...                                                               // sample data
        };

        // write header
        if (fseek(waveFile->_file, 0, SEEK_SET)) {
            return false;
        }

        if (!fwrite(header, sizeof header, 1, waveFile->_file)) {
            return false;
        }

        if (fclose(waveFile->_file)) {
            return false;
        }

        free(waveFile->_buffer);
        waveFile->_buffer = NULL;
        waveFile->_file = NULL;
    }

    return true;
}

#endif // SHL_WAV_IMPLEMENTATION
#endif // SHL_WAVE_WRITER_H
