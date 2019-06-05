/* 
    WAVE sound file writer for recording 16-bit output during program development.
    A single header library version in C89 of the Wave_Writer library written by Shay Green.

    Copyright (C) 2003-2005 by Shay Green. Permission is hereby granted, free
    of charge, to any person obtaining a copy of this software and associated
    documentation files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy, modify,
    merge, publish, distribute, sublicense, and/or sell copies of the Software, and
    to permit persons to whom the Software is furnished to do so, subject to the
    following conditions: The above copyright notice and this permission notice
    shall be included in all copies or substantial portions of the Software. THE
    SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
    PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
    COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/

#ifndef SHL_WAVE_WRITER_H
#define SHL_WAVE_WRITER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef short shl_sample_t;

#define SHL_WAVE_BUFFER_SIZE (32768 * 2)
#define SHL_WAVE_HEADER_SIZE 0x2C

#define SHL_U8_CAST(x) ((unsigned char)(x))

typedef struct _shl_wave_file
{
    FILE*   		_file;
    unsigned char*  _buffer;
    long    		sampleRate;
    long    		_sampleCount;
    long    		_bufferPos;
    int     		channelCount;
} shlWaveFile;

bool shlWaveInit(shlWaveFile *waveFile, long sampleRate, const char* filename);
void shlWaveStereo(shlWaveFile *waveFile, bool stereo);
long shlWaveSampleCount(shlWaveFile *waveFile);
bool shlWaveWrite(shlWaveFile *waveFile, const shl_sample_t *in, long count, int skip);
bool shlWaveFlush(shlWaveFile *waveFile, bool closeFile);

#ifdef __cplusplus
}
#endif

#ifdef SHL_WAVE_WRITER_IMPLEMENTATION

static bool __shlWaveFlush(shlWaveFile *waveFile)
{
    if (waveFile->_bufferPos && !fwrite(waveFile->_buffer, waveFile->_bufferPos, 1, waveFile->_file)) {
        return false;
    }

    waveFile->_bufferPos = 0;
    return true;
}

bool shlWaveInit(shlWaveFile *waveFile, long sampleRate, const char* filename)
{
    waveFile->_buffer = (unsigned char*) malloc(SHL_WAVE_BUFFER_SIZE);	
    if (!waveFile->_buffer) {
        return false;
    }

    waveFile->_file = fopen(filename, "wb");
    if (!waveFile->_file) {
        return false;
    }

    waveFile->sampleRate = sampleRate;
    waveFile->channelCount = 1;

    waveFile->_sampleCount = 0;
    waveFile->_bufferPos = SHL_WAVE_HEADER_SIZE;

    return true;
}

void shlWaveStereo(shlWaveFile *waveFile, bool stereo)
{
    waveFile->channelCount = stereo ? 2 : 1;
}

long shlWaveSampleCount(shlWaveFile *waveFile)
{
    return waveFile->_sampleCount;
}

bool shlWaveWrite(shlWaveFile *waveFile, const shl_sample_t *in, long count, int skip)
{
    waveFile->_sampleCount += count;
    while (count) {
        if (waveFile->_bufferPos >= SHL_WAVE_BUFFER_SIZE) {
            if (!__shlWaveFlush(waveFile)) {
                return false;
            }
        }

        long n = (unsigned long) (SHL_WAVE_BUFFER_SIZE - waveFile->_bufferPos) / sizeof (shl_sample_t);
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

        waveFile->_bufferPos = p - waveFile->_buffer;
    }

    return true;
}

bool shlWaveFlush(shlWaveFile *waveFile, bool closeFile)
{
    if (!__shlWaveFlush(waveFile)) {
        return false;
    }

    if (closeFile) {
        // generate header
        long sample_rate = waveFile->sampleRate;
        int channel_count = waveFile->channelCount;
        long ds = waveFile->_sampleCount * sizeof (shl_sample_t);
        long rs = SHL_WAVE_HEADER_SIZE - 8 + ds;
        int frame_size = channel_count * sizeof (shl_sample_t);
        long bps = sample_rate * frame_size;

        unsigned char header[SHL_WAVE_HEADER_SIZE] = {
            'R','I','F','F',
            SHL_U8_CAST(rs), SHL_U8_CAST(rs >> 8),                               // length of rest of file
            SHL_U8_CAST(rs >> 16), SHL_U8_CAST(rs >> 24),
            'W','A','V','E',
            'f','m','t',' ',
            0x10, 0, 0, 0,                                                       // size of fmt chunk
            1, 0,                                                                // uncompressed format
            SHL_U8_CAST(channel_count), 0,                                       // channel count
            SHL_U8_CAST(sample_rate), SHL_U8_CAST(sample_rate >> 8),             // sample rate
            SHL_U8_CAST(sample_rate >> 16), SHL_U8_CAST(sample_rate >> 24),
            SHL_U8_CAST(bps), SHL_U8_CAST(bps >> 8),                             // bytes per second
            SHL_U8_CAST(bps >> 16), SHL_U8_CAST(bps >> 24),
            SHL_U8_CAST(frame_size), 0,                                          // bytes per sample frame
            16, 0,                                                               // bits per sample
            'd','a','t','a',
            SHL_U8_CAST(ds), SHL_U8_CAST(ds >> 8),                               // size of sample data
            SHL_U8_CAST(ds >> 16), SHL_U8_CAST(ds >> 24)
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
    }

    return true;
}

#endif // SHL_WAVE_WRITER_IMPLEMENTATION
#endif // SHL_WAVE_WRITER_H