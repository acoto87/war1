/*
    queue.h - acoto87 (acoto87@gmail.com)

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

    Single-header macro library to declare and define strongly typed queues
    implemented as dynamically resized circular buffers.

    USAGE
    Declare a queue type with shlDeclareQueue(name, type), then define it once
    with shlDefineQueue(name, type) in a C source file.

    CUSTOMISATION
    Provide a default value for empty reads, an equality function for
    Contains, and a free function if queue elements own resources. Values are
    stored by copy in a resizable circular buffer.

    NOTES
    Push appends to the tail, Pop removes from the head, and Peek returns the
    next item without removing it. Clear releases per-item resources when a
    free hook is configured.
*/

#ifndef SHL_QUEUE_H
#define SHL_QUEUE_H

#include "shl_internal.h"

#define shlDeclareQueue(typeName, itemType) \
    typedef struct \
    { \
        itemType defaultValue; \
        bool (*equalsFn)(const itemType item1, const itemType item2); \
        void (*freeFn)(itemType item); \
    } typeName ## Options; \
    \
    typedef struct \
    { \
        int32_t head; \
        int32_t tail; \
        int32_t count; \
        int32_t capacity; \
        bool (*equalsFn)(const itemType item1, const itemType item2); \
        void (*freeFn)(itemType item); \
        itemType defaultValue; \
        itemType *items; \
    } typeName; \
    \
    void typeName ## Init(typeName* queue, typeName ## Options options); \
    void typeName ## Free(typeName* queue); \
    void typeName ## Push(typeName* queue, itemType value); \
    itemType typeName ## Peek(typeName* queue); \
    itemType typeName ## Pop(typeName* queue); \
    bool typeName ## Contains(typeName* queue, itemType value); \
    void typeName ## Clear(typeName* queue);

#define shlDefineQueue(typeName, itemType) \
    void typeName ## Init(typeName *queue, typeName ## Options options) \
    { \
        queue->defaultValue = options.defaultValue; \
        queue->equalsFn = options.equalsFn; \
        queue->freeFn = options.freeFn; \
        queue->capacity = SHL__INITIAL_CAPACITY; \
        queue->count = 0; \
        queue->head = 0; \
        queue->tail = 0; \
        queue->items = (itemType *)SHL_CALLOC((size_t)queue->capacity, sizeof(itemType)); \
    } \
    \
    void typeName ## Free(typeName *queue) \
    { \
        if (!queue->items) \
            return; \
        \
        typeName ## Clear(queue); \
        \
        SHL_FREE(queue->items); \
        queue->items = 0; \
    } \
    \
    void typeName ## Push(typeName *queue, itemType value) \
    { \
        if (!queue->items) \
            return; \
        \
        if (queue->count == queue->capacity) \
            shl__resizeCircularArray((void**)&queue->items, &queue->capacity, &queue->head, &queue->tail, queue->count, sizeof(itemType)); \
        \
        queue->items[queue->tail] = value; \
        queue->tail = (queue->tail + 1) % queue->capacity; \
        queue->count++; \
    } \
    \
    itemType typeName ## Peek(typeName *queue) \
    { \
        if (!queue->items || queue->count == 0) \
            return queue->defaultValue; \
        \
        return queue->items[queue->head]; \
    } \
    \
    itemType typeName ## Pop(typeName *queue) \
    { \
        if (!queue->items || queue->count == 0) \
            return queue->defaultValue; \
        \
        itemType value = queue->items[queue->head]; \
        queue->items[queue->head] = queue->defaultValue; \
        queue->head = (queue->head + 1) % queue->capacity; \
        queue->count--; \
        return value; \
    } \
    \
    bool typeName ## Contains(typeName *queue, itemType value) \
    { \
        if (!queue->items) \
            return false; \
        \
        if (!queue->equalsFn) \
            return false; \
        \
        for(int32_t i = 0; i < queue->count; i++) \
        { \
            if (queue->equalsFn(queue->items[(queue->head + i) % queue->capacity], value)) \
                return true; \
        } \
        \
        return false; \
    } \
    void typeName ## Clear(typeName* queue) \
    { \
        if (!queue->items) \
            return; \
        \
        if (queue->freeFn) \
        { \
            for(int32_t i = 0; i < queue->count; i++) \
            { \
                int32_t index = (queue->head + i) % queue->capacity; \
                queue->freeFn(queue->items[index]); \
                queue->items[index] = queue->defaultValue; \
            } \
        } \
        \
        queue->count = 0; \
        queue->head = 0; \
        queue->tail = 0; \
    }

#endif // SHL_QUEUE_H
