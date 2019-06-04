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

typedef struct _MemoryBuffer
{
    uint8_t* data;
    size_t length;
    uint8_t* _pointer;
} MemoryBuffer;

#define mbEnd(buffer) ((buffer)->data + (buffer)->length)
#define mbPosition(buffer) ((buffer)->_pointer - (buffer)->data)

void mbInitEmpty(MemoryBuffer* buffer);
void mbInitFromMemory(MemoryBuffer* buffer, uint8_t* data, size_t length);
void mbFree(MemoryBuffer* buffer);
uint8_t* mbGetData(MemoryBuffer* buffer, size_t* length);

bool mbSeek(MemoryBuffer* buffer, uint32_t position);
bool mbSkip(MemoryBuffer* buffer, int32_t distance);

bool mbScanTo(MemoryBuffer* buffer, const void* data, size_t length);

bool mbRead(MemoryBuffer* buffer, uint8_t* value);
bool mbReadBytes(MemoryBuffer* buffer, uint8_t* value, size_t count);
bool mbReadString(MemoryBuffer* buffer, char* str, size_t count);

bool mbReadInt16LE(MemoryBuffer* buffer, int16_t* value);
bool mbReadInt16BE(MemoryBuffer* buffer, int16_t* value);
bool mbReadUInt16LE(MemoryBuffer* buffer, uint16_t* value);
bool mbReadUInt16BE(MemoryBuffer* buffer, uint16_t* value);

bool mbReadInt24LE(MemoryBuffer* buffer, int32_t* value);
bool mbReadInt24BE(MemoryBuffer* buffer, int32_t* value);
bool mbReadUInt24LE(MemoryBuffer* buffer, uint32_t* value);
bool mbReadUInt24BE(MemoryBuffer* buffer, uint32_t* value);

bool mbReadInt32LE(MemoryBuffer* buffer, int32_t* value);
bool mbReadInt32BE(MemoryBuffer* buffer, int32_t* value);
bool mbReadUInt32LE(MemoryBuffer* buffer, uint32_t* value);
bool mbReadUInt32BE(MemoryBuffer* buffer, uint32_t* value);

bool mbWrite(MemoryBuffer* buffer, uint8_t value);
bool mbWriteBytes(MemoryBuffer* buffer, uint8_t values[], size_t count);
bool mbWriteString(MemoryBuffer* buffer, const char* str, size_t count);

bool mbWriteInt16LE(MemoryBuffer* buffer, int16_t value);
bool mbWriteInt16BE(MemoryBuffer* buffer, int16_t value);
bool mbWriteUInt16LE(MemoryBuffer* buffer, uint16_t value);
bool mbWriteUInt16BE(MemoryBuffer* buffer, uint16_t value);

bool mbWriteInt24LE(MemoryBuffer* buffer, int32_t value);
bool mbWriteInt24BE(MemoryBuffer* buffer, int32_t value);
bool mbWriteUInt24LE(MemoryBuffer* buffer, uint32_t value);
bool mbWriteUInt24BE(MemoryBuffer* buffer, uint32_t value);

bool mbWriteInt32LE(MemoryBuffer* buffer, int32_t value);
bool mbWriteInt32BE(MemoryBuffer* buffer, int32_t value);
bool mbWriteUInt32LE(MemoryBuffer* buffer, uint32_t value);
bool mbWriteUInt32BE(MemoryBuffer* buffer, uint32_t value);

bool mbIsEOF(MemoryBuffer* buffer);

#ifdef __cplusplus
}
#endif

#ifdef SHL_MEMORY_BUFFER_IMPLEMENTATION

static bool mb__realloc(MemoryBuffer* buffer, size_t newLength)
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

    buffer->_pointer = newData + mbPosition(buffer); 
    buffer->data = newData;
    buffer->length = newLength;

    free(oldData);
    return true;
}

void mbInitEmpty(MemoryBuffer* buffer)
{
    buffer->data = (uint8_t*)calloc(0, sizeof(uint8_t));
    buffer->length = 0;
    buffer->_pointer = buffer->data;
}

void mbInitFromMemory(MemoryBuffer* buffer, uint8_t* data, size_t length)
{
    buffer->data = data;
    buffer->length = length;
    buffer->_pointer = buffer->data;
}

void mbFree(MemoryBuffer* buffer)
{
    if (buffer->data)
        free((void*)buffer->data);

    buffer->data = NULL;
    buffer->length = 0;
    buffer->_pointer = NULL;
}

uint8_t* mbGetData(MemoryBuffer* buffer, size_t* length)
{
    uint8_t* data = (uint8_t*)malloc(buffer->length);
    memcpy(data, buffer->data, buffer->length);
    *length = buffer->length;
    return data;
}

bool mbSeek(MemoryBuffer* buffer, uint32_t position)
{
    if (buffer->data + position > mbEnd(buffer))
    {
        if (!mb__realloc(buffer, position))
            return false;
    }

    buffer->_pointer = buffer->data + position;
    return true;
}

bool mbSkip(MemoryBuffer* buffer, int32_t distance)
{
    if (distance < 0)
    {
        if (buffer->_pointer + distance < buffer->data)
            return false;
    }

    return mbSeek(buffer, mbPosition(buffer) + distance);
}

bool mbScanTo(MemoryBuffer* buffer, const void* data, size_t length)
{
    while (buffer->_pointer + length <= mbEnd(buffer))
    {
        // printf("%s\n", buffer->_pointer);
        if(memcmp(buffer->_pointer, data, length) == 0)
            return true;

        buffer->_pointer++;
    }

    return false;
}

bool mbRead(MemoryBuffer* buffer, uint8_t* value)
{
    return mbReadBytes(buffer, value, 1);
}

bool mbReadBytes(MemoryBuffer* buffer, uint8_t* values, size_t count)
{
    if (buffer->_pointer + count > mbEnd(buffer))
        return false;

    memcpy(values, buffer->_pointer, count);
    buffer->_pointer += count;
    return true;
}

bool mbReadString(MemoryBuffer* buffer, char* str, size_t count)
{
    return mbReadBytes(buffer, (uint8_t*)str, count);
}

bool mbReadInt16LE(MemoryBuffer* buffer, int16_t* value)
{
    uint8_t byte0, byte1;
    if(mbRead(buffer, &byte0) && mbRead(buffer, &byte1))
    {
        *value = (byte1 << 8) | byte0;
        return true;
    }
    
    return false;
}

bool mbReadInt16BE(MemoryBuffer* buffer, int16_t* value)
{
    uint8_t byte0, byte1;
    if(mbRead(buffer, &byte0) && mbRead(buffer, &byte1))
    {
        *value = (byte0 << 8) | byte1;
        return true;
    }
    
    return false;
}

bool mbReadUInt16LE(MemoryBuffer* buffer, uint16_t* value)
{
    uint8_t byte0, byte1;
    if(mbRead(buffer, &byte0) && mbRead(buffer, &byte1))
    {
        *value = (byte1 << 8) | byte0;
        return true;
    }
    
    return false;
}

bool mbReadUInt16BE(MemoryBuffer* buffer, uint16_t* value)
{
    uint8_t byte0, byte1;
    if(mbRead(buffer, &byte0) && mbRead(buffer, &byte1))
    {
        *value = (byte0 << 8) | byte1;
        return true;
    }
    
    return false;
}

bool mbReadInt24LE(MemoryBuffer* buffer, int32_t* value)
{
    uint8_t byte0, byte1, byte2;
    if(mbRead(buffer, &byte0) && mbRead(buffer, &byte1) && mbRead(buffer, &byte2))
    {
        *value = (byte2 << 16) | (byte1 << 8) | byte0;
        return true;
    }
    
    return false;
}

bool mbReadInt24BE(MemoryBuffer* buffer, int32_t* value)
{
    uint8_t byte0, byte1, byte2;
    if(mbRead(buffer, &byte0) && mbRead(buffer, &byte1) && mbRead(buffer, &byte2))
    {
        *value = (byte0 << 16) | (byte1 << 8) | byte2;
        return true;
    }
    
    return false;
}

bool mbReadUInt24LE(MemoryBuffer* buffer, uint32_t* value)
{
    uint8_t byte0, byte1, byte2;
    if(mbRead(buffer, &byte0) && mbRead(buffer, &byte1) && mbRead(buffer, &byte2))
    {
        *value = (byte2 << 16) | (byte1 << 8) | byte0;
        return true;
    }
    
    return false;
}

bool mbReadUInt24BE(MemoryBuffer* buffer, uint32_t* value)
{
    uint8_t byte0, byte1, byte2;
    if(mbRead(buffer, &byte0) && mbRead(buffer, &byte1) && mbRead(buffer, &byte2))
    {
        *value = (byte0 << 16) | (byte1 << 8) | byte2;
        return true;
    }
    
    return false;
}

bool mbReadInt32LE(MemoryBuffer* buffer, int32_t* value)
{
    uint8_t byte0, byte1, byte2, byte3;
    if(mbRead(buffer, &byte0) && mbRead(buffer, &byte1) && mbRead(buffer, &byte2) && mbRead(buffer, &byte3))
    {
        *value = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
        return true;
    }
    
    return false;
}

bool mbReadInt32BE(MemoryBuffer* buffer, int32_t* value)
{
    uint8_t byte0, byte1, byte2, byte3;
    if(mbRead(buffer, &byte0) && mbRead(buffer, &byte1) && mbRead(buffer, &byte2) && mbRead(buffer, &byte3))
    {
        *value = (byte0 << 24) | (byte1 << 16) | (byte2 << 8) | byte3;
        return true;
    }
    
    return false;
}

bool mbReadUInt32LE(MemoryBuffer* buffer, uint32_t* value)
{
    uint8_t byte0, byte1, byte2, byte3;
    if(mbRead(buffer, &byte0) && mbRead(buffer, &byte1) && mbRead(buffer, &byte2) && mbRead(buffer, &byte3))
    {
        *value = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
        return true;
    }
    
    return false;
}

bool mbReadUInt32BE(MemoryBuffer* buffer, uint32_t* value)
{
    uint8_t byte0, byte1, byte2, byte3;
    if(mbRead(buffer, &byte0) && mbRead(buffer, &byte1) && mbRead(buffer, &byte2) && mbRead(buffer, &byte3))
    {
        *value = (byte0 << 24) | (byte1 << 16) | (byte2 << 8) | byte3;
        return true;
    }
    
    return false;
}

bool mbWrite(MemoryBuffer* buffer, uint8_t value)
{
    return mbWriteBytes(buffer, &value, 1);
}

bool mbWriteBytes(MemoryBuffer* buffer, uint8_t values[], size_t count)
{
    if (buffer->_pointer + count >= mbEnd(buffer))
    {
        if (!mb__realloc(buffer, mbPosition(buffer) + count))
            return false;
    }

    memcpy(buffer->_pointer, values, count);
    buffer->_pointer += count;

    return true;
}

bool mbWriteString(MemoryBuffer* buffer, const char* str, size_t count)
{
    return mbWriteBytes(buffer, (uint8_t*)str, count);
}

bool mbWriteInt16LE(MemoryBuffer* buffer, int16_t value)
{
    return mbWriteBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)value,
            (uint8_t)(value >> 8)
        }, 
        sizeof(int16_t));
}

bool mbWriteInt16BE(MemoryBuffer* buffer, int16_t value)
{
    return mbWriteBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)(value >> 8),
            (uint8_t)value
        }, 
        sizeof(int16_t));
}

bool mbWriteUInt16LE(MemoryBuffer* buffer, uint16_t value)
{
    return mbWriteBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)value,
            (uint8_t)(value >> 8)
        }, 
        sizeof(uint16_t));
}

bool mbWriteUInt16BE(MemoryBuffer* buffer, uint16_t value)
{
    return mbWriteBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)(value >> 8),
            (uint8_t)value
        }, 
        sizeof(uint16_t));
}

bool mbWriteInt24LE(MemoryBuffer* buffer, int32_t value)
{
    return mbWriteBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)value,
            (uint8_t)(value >> 8),
            (uint8_t)(value >> 16)
        }, 
        sizeof(int32_t));
}

bool mbWriteInt24BE(MemoryBuffer* buffer, int32_t value)
{
    return mbWriteBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)(value >> 16),
            (uint8_t)(value >> 8),
            (uint8_t)value
        }, 
        sizeof(int32_t));
}

bool mbWriteUInt24LE(MemoryBuffer* buffer, uint32_t value)
{
    return mbWriteBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)value,
            (uint8_t)(value >> 8),
            (uint8_t)(value >> 16)
        }, 
        sizeof(uint32_t));
}

bool mbWriteUInt24BE(MemoryBuffer* buffer, uint32_t value)
{
    return mbWriteBytes(
        buffer,
        (uint8_t[])
        {
            (uint8_t)(value >> 16),
            (uint8_t)(value >> 8),
            (uint8_t)value
        }, 
        sizeof(uint32_t));
}

bool mbWriteInt32LE(MemoryBuffer* buffer, int32_t value)
{
    return mbWriteBytes(
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

bool mbWriteInt32BE(MemoryBuffer* buffer, int32_t value)
{
    return mbWriteBytes(
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

bool mbWriteUInt32LE(MemoryBuffer* buffer, uint32_t value)
{
    return mbWriteBytes(
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

bool mbWriteUInt32BE(MemoryBuffer* buffer, uint32_t value)
{
    return mbWriteBytes(
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

bool mbIsEOF(MemoryBuffer* buffer)
{
    return buffer->_pointer == mbEnd(buffer);
}

#endif // SHL_MEMORY_BUFFER_IMPLEMENTATION
#endif // SHL_MEMORY_BUFFER_H
