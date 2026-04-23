/*
    memory_buffer.h - acoto87 (acoto87@gmail.com)

    MIT License

    Copyright (c) 2018 Alejandro Coto Gutiérrez

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

    Single-header in-memory byte buffer with sequential reads, random access
    seeking, automatic growth on writes, and endian-aware integer helpers.

    USAGE
    Include this header in all translation units that need the declarations.
    Define SHL_MEMORY_BUFFER_IMPLEMENTATION in exactly one translation unit
    before the include to compile the implementation:

        #define SHL_MEMORY_BUFFER_IMPLEMENTATION
        #include "memory_buffer.h"

    Include the header without that define everywhere else:

        #include "memory_buffer.h"

    CUSTOMISATION
    The implementation is self-contained and uses the standard C allocator
    directly. If you need different allocation behavior, adjust the
    implementation section itself.

    NOTES
    The buffer tracks a current read/write cursor, can grow automatically when
    seeking or writing past the current end, and provides helpers for endian-
    aware primitive reads and writes.
*/
#ifndef SHL_MEMORY_BUFFER_H
#define SHL_MEMORY_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _memory_buffer_t memory_buffer_t;

#define mb_end(buffer) ((buffer)->data + (buffer)->length)
#define mb_position(buffer) ((buffer)->_pointer - (buffer)->data)

void mb_initEmpty(memory_buffer_t* buffer);
void mb_initFromMemory(memory_buffer_t* buffer, uint8_t* data, size_t length);
void mb_free(memory_buffer_t* buffer);
uint8_t* mb_data(memory_buffer_t* buffer, size_t* length);

bool mb_seek(memory_buffer_t* buffer, uint32_t position);
bool mb_skip(memory_buffer_t* buffer, int32_t distance);

bool mb_scanTo(memory_buffer_t* buffer, const void* data, size_t length);

bool mb_read(memory_buffer_t* buffer, uint8_t* value);
bool mb_readBytes(memory_buffer_t* buffer, uint8_t* value, size_t count);
bool mb_readString(memory_buffer_t* buffer, char* str, size_t count);

bool mb_readInt16LE(memory_buffer_t* buffer, int16_t* value);
bool mb_readInt16BE(memory_buffer_t* buffer, int16_t* value);
bool mb_readUInt16LE(memory_buffer_t* buffer, uint16_t* value);
bool mb_readUInt16BE(memory_buffer_t* buffer, uint16_t* value);

bool mb_readInt24LE(memory_buffer_t* buffer, int32_t* value);
bool mb_readInt24BE(memory_buffer_t* buffer, int32_t* value);
bool mb_readUInt24LE(memory_buffer_t* buffer, uint32_t* value);
bool mb_readUInt24BE(memory_buffer_t* buffer, uint32_t* value);

bool mb_readInt32LE(memory_buffer_t* buffer, int32_t* value);
bool mb_readInt32BE(memory_buffer_t* buffer, int32_t* value);
bool mb_readUInt32LE(memory_buffer_t* buffer, uint32_t* value);
bool mb_readUInt32BE(memory_buffer_t* buffer, uint32_t* value);

bool mb_write(memory_buffer_t* buffer, uint8_t value);
bool mb_writeBytes(memory_buffer_t* buffer, uint8_t values[], size_t count);
bool mb_writeString(memory_buffer_t* buffer, const char* str, size_t count);

bool mb_writeInt16LE(memory_buffer_t* buffer, int16_t value);
bool mb_writeInt16BE(memory_buffer_t* buffer, int16_t value);
bool mb_writeUInt16LE(memory_buffer_t* buffer, uint16_t value);
bool mb_writeUInt16BE(memory_buffer_t* buffer, uint16_t value);

bool mb_writeInt24LE(memory_buffer_t* buffer, int32_t value);
bool mb_writeInt24BE(memory_buffer_t* buffer, int32_t value);
bool mb_writeUInt24LE(memory_buffer_t* buffer, uint32_t value);
bool mb_writeUInt24BE(memory_buffer_t* buffer, uint32_t value);

bool mb_writeInt32LE(memory_buffer_t* buffer, int32_t value);
bool mb_writeInt32BE(memory_buffer_t* buffer, int32_t value);
bool mb_writeUInt32LE(memory_buffer_t* buffer, uint32_t value);
bool mb_writeUInt32BE(memory_buffer_t* buffer, uint32_t value);

bool mb_isEOF(memory_buffer_t* buffer);

#ifdef __cplusplus
}
#endif

#ifdef SHL_MEMORY_BUFFER_IMPLEMENTATION

struct _memory_buffer_t
{
    uint8_t* data;
    size_t length;
    uint8_t* _pointer;
};

static bool mb__realloc(memory_buffer_t* buffer, size_t newLength)
{
    if(newLength <= buffer->length)
    {
        buffer->length = newLength;
        return true;
    }

    uint8_t* oldData = buffer->data;
    uint8_t* newData = (uint8_t*)calloc(newLength, sizeof(uint8_t));
    if (!newData)
    {
        return false;
    }

    size_t count = newLength > buffer->length ? buffer->length : newLength;
    memcpy(newData, buffer->data, count);

    buffer->_pointer = newData + mb_position(buffer);
    buffer->data = newData;
    buffer->length = newLength;

    free(oldData);
    return true;
}

void mb_initEmpty(memory_buffer_t* buffer)
{
    buffer->data = (uint8_t*)calloc(0, sizeof(uint8_t));
    buffer->length = 0;
    buffer->_pointer = buffer->data;
}

void mb_initFromMemory(memory_buffer_t* buffer, uint8_t* data, size_t length)
{
    buffer->data = data;
    buffer->length = length;
    buffer->_pointer = buffer->data;
}

void mb_free(memory_buffer_t* buffer)
{
    if (buffer->data)
        free((void*)buffer->data);

    buffer->data = NULL;
    buffer->length = 0;
    buffer->_pointer = NULL;
}

uint8_t* mb_data(memory_buffer_t* buffer, size_t* length)
{
    uint8_t* data = (uint8_t*)malloc(buffer->length);
    memcpy(data, buffer->data, buffer->length);
    *length = buffer->length;
    return data;
}

bool mb_seek(memory_buffer_t* buffer, uint32_t position)
{
    if (buffer->data + position > mb_end(buffer))
    {
        if (!mb__realloc(buffer, position))
            return false;
    }

    buffer->_pointer = buffer->data + position;
    return true;
}

bool mb_skip(memory_buffer_t* buffer, int32_t distance)
{
    if (distance < 0)
    {
        if (buffer->_pointer + distance < buffer->data)
            return false;
    }

    int64_t position = mb_position(buffer) + distance;
    if (position < 0 || position > UINT32_MAX)
        return false;

    return mb_seek(buffer, (uint32_t)position);
}

bool mb_scanTo(memory_buffer_t* buffer, const void* data, size_t length)
{
    while (buffer->_pointer + length <= mb_end(buffer))
    {
        // printf("%s\n", buffer->_pointer);
        if(memcmp(buffer->_pointer, data, length) == 0)
            return true;

        buffer->_pointer++;
    }

    return false;
}

bool mb_read(memory_buffer_t* buffer, uint8_t* value)
{
    return mb_readBytes(buffer, value, 1);
}

bool mb_readBytes(memory_buffer_t* buffer, uint8_t* values, size_t count)
{
    if (buffer->_pointer + count > mb_end(buffer))
        return false;

    memcpy(values, buffer->_pointer, count);
    buffer->_pointer += count;
    return true;
}

bool mb_readString(memory_buffer_t* buffer, char* str, size_t count)
{
    return mb_readBytes(buffer, (uint8_t*)str, count);
}

bool mb_readInt16LE(memory_buffer_t* buffer, int16_t* value)
{
    uint8_t byte0, byte1;
    if(mb_read(buffer, &byte0) && mb_read(buffer, &byte1))
    {
        *value = (byte1 << 8) | byte0;
        return true;
    }

    return false;
}

bool mb_readInt16BE(memory_buffer_t* buffer, int16_t* value)
{
    uint8_t byte0, byte1;
    if(mb_read(buffer, &byte0) && mb_read(buffer, &byte1))
    {
        *value = (byte0 << 8) | byte1;
        return true;
    }

    return false;
}

bool mb_readUInt16LE(memory_buffer_t* buffer, uint16_t* value)
{
    uint8_t byte0, byte1;
    if(mb_read(buffer, &byte0) && mb_read(buffer, &byte1))
    {
        *value = (byte1 << 8) | byte0;
        return true;
    }

    return false;
}

bool mb_readUInt16BE(memory_buffer_t* buffer, uint16_t* value)
{
    uint8_t byte0, byte1;
    if(mb_read(buffer, &byte0) && mb_read(buffer, &byte1))
    {
        *value = (byte0 << 8) | byte1;
        return true;
    }

    return false;
}

bool mb_readInt24LE(memory_buffer_t* buffer, int32_t* value)
{
    uint8_t byte0, byte1, byte2;
    if(mb_read(buffer, &byte0) && mb_read(buffer, &byte1) && mb_read(buffer, &byte2))
    {
        *value = (byte2 << 16) | (byte1 << 8) | byte0;
        return true;
    }

    return false;
}

bool mb_readInt24BE(memory_buffer_t* buffer, int32_t* value)
{
    uint8_t byte0, byte1, byte2;
    if(mb_read(buffer, &byte0) && mb_read(buffer, &byte1) && mb_read(buffer, &byte2))
    {
        *value = (byte0 << 16) | (byte1 << 8) | byte2;
        return true;
    }

    return false;
}

bool mb_readUInt24LE(memory_buffer_t* buffer, uint32_t* value)
{
    uint8_t byte0, byte1, byte2;
    if(mb_read(buffer, &byte0) && mb_read(buffer, &byte1) && mb_read(buffer, &byte2))
    {
        *value = (byte2 << 16) | (byte1 << 8) | byte0;
        return true;
    }

    return false;
}

bool mb_readUInt24BE(memory_buffer_t* buffer, uint32_t* value)
{
    uint8_t byte0, byte1, byte2;
    if(mb_read(buffer, &byte0) && mb_read(buffer, &byte1) && mb_read(buffer, &byte2))
    {
        *value = (byte0 << 16) | (byte1 << 8) | byte2;
        return true;
    }

    return false;
}

bool mb_readInt32LE(memory_buffer_t* buffer, int32_t* value)
{
    uint8_t byte0, byte1, byte2, byte3;
    if(mb_read(buffer, &byte0) && mb_read(buffer, &byte1) && mb_read(buffer, &byte2) && mb_read(buffer, &byte3))
    {
        *value = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
        return true;
    }

    return false;
}

bool mb_readInt32BE(memory_buffer_t* buffer, int32_t* value)
{
    uint8_t byte0, byte1, byte2, byte3;
    if(mb_read(buffer, &byte0) && mb_read(buffer, &byte1) && mb_read(buffer, &byte2) && mb_read(buffer, &byte3))
    {
        *value = (byte0 << 24) | (byte1 << 16) | (byte2 << 8) | byte3;
        return true;
    }

    return false;
}

bool mb_readUInt32LE(memory_buffer_t* buffer, uint32_t* value)
{
    uint8_t byte0, byte1, byte2, byte3;
    if(mb_read(buffer, &byte0) && mb_read(buffer, &byte1) && mb_read(buffer, &byte2) && mb_read(buffer, &byte3))
    {
        *value = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
        return true;
    }

    return false;
}

bool mb_readUInt32BE(memory_buffer_t* buffer, uint32_t* value)
{
    uint8_t byte0, byte1, byte2, byte3;
    if(mb_read(buffer, &byte0) && mb_read(buffer, &byte1) && mb_read(buffer, &byte2) && mb_read(buffer, &byte3))
    {
        *value = (byte0 << 24) | (byte1 << 16) | (byte2 << 8) | byte3;
        return true;
    }

    return false;
}

bool mb_write(memory_buffer_t* buffer, uint8_t value)
{
    return mb_writeBytes(buffer, &value, 1);
}

bool mb_writeBytes(memory_buffer_t* buffer, uint8_t values[], size_t count)
{
    if (buffer->_pointer + count >= mb_end(buffer))
    {
        if (!mb__realloc(buffer, mb_position(buffer) + count))
            return false;
    }

    memcpy(buffer->_pointer, values, count);
    buffer->_pointer += count;

    return true;
}

bool mb_writeString(memory_buffer_t* buffer, const char* str, size_t count)
{
    return mb_writeBytes(buffer, (uint8_t*)str, count);
}

bool mb_writeInt16LE(memory_buffer_t* buffer, int16_t value)
{
    return mb_writeBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)value,
            (uint8_t)(value >> 8)
        },
        sizeof(int16_t));
}

bool mb_writeInt16BE(memory_buffer_t* buffer, int16_t value)
{
    return mb_writeBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)(value >> 8),
            (uint8_t)value
        },
        sizeof(int16_t));
}

bool mb_writeUInt16LE(memory_buffer_t* buffer, uint16_t value)
{
    return mb_writeBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)value,
            (uint8_t)(value >> 8)
        },
        sizeof(uint16_t));
}

bool mb_writeUInt16BE(memory_buffer_t* buffer, uint16_t value)
{
    return mb_writeBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)(value >> 8),
            (uint8_t)value
        },
        sizeof(uint16_t));
}

bool mb_writeInt24LE(memory_buffer_t* buffer, int32_t value)
{
    return mb_writeBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)value,
            (uint8_t)(value >> 8),
            (uint8_t)(value >> 16)
        },
        3);
}

bool mb_writeInt24BE(memory_buffer_t* buffer, int32_t value)
{
    return mb_writeBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)(value >> 16),
            (uint8_t)(value >> 8),
            (uint8_t)value
        },
        3);
}

bool mb_writeUInt24LE(memory_buffer_t* buffer, uint32_t value)
{
    return mb_writeBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)value,
            (uint8_t)(value >> 8),
            (uint8_t)(value >> 16)
        },
        3);
}

bool mb_writeUInt24BE(memory_buffer_t* buffer, uint32_t value)
{
    return mb_writeBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)(value >> 16),
            (uint8_t)(value >> 8),
            (uint8_t)value
        },
        3);
}

bool mb_writeInt32LE(memory_buffer_t* buffer, int32_t value)
{
    return mb_writeBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)value,
            (uint8_t)(value >> 8),
            (uint8_t)(value >> 16),
            (uint8_t)(value >> 24)
        },
        sizeof(int32_t));
}

bool mb_writeInt32BE(memory_buffer_t* buffer, int32_t value)
{
    return mb_writeBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)(value >> 24),
            (uint8_t)(value >> 16),
            (uint8_t)(value >> 8),
            (uint8_t)value
        },
        sizeof(int32_t));
}

bool mb_writeUInt32LE(memory_buffer_t* buffer, uint32_t value)
{
    return mb_writeBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)value,
            (uint8_t)(value >> 8),
            (uint8_t)(value >> 16),
            (uint8_t)(value >> 24)
        },
        sizeof(uint32_t));
}

bool mb_writeUInt32BE(memory_buffer_t* buffer, uint32_t value)
{
    return mb_writeBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)(value >> 24),
            (uint8_t)(value >> 16),
            (uint8_t)(value >> 8),
            (uint8_t)value
        },
        sizeof(uint32_t));
}

bool mb_isEOF(memory_buffer_t* buffer)
{
    return buffer->_pointer == mb_end(buffer);
}

#endif // SHL_MEMORY_BUFFER_IMPLEMENTATION
#endif // SHL_MEMORY_BUFFER_H
