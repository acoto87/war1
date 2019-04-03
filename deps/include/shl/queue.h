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

    This is a single header file with macros to declare and define a strongly typed queue with push, pop and peek operations.
*/
#ifndef SHL_QUEUE_H
#define SHL_QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

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
        uint32_t head; \
        uint32_t tail; \
        uint32_t count; \
        uint32_t capacity; \
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
    void typeName ## __resize(typeName *queue) \
    { \
        uint32_t oldCapacity = queue->capacity; \
        itemType* old = queue->items; \
        \
        queue->capacity = oldCapacity << 1; \
        queue->items = (itemType *)calloc(queue->capacity, sizeof(itemType)); \
        \
        if (queue->head > queue->tail) \
        { \
            memcpy(queue->items, old + queue->head, (oldCapacity - queue->head) * sizeof(itemType)); \
            memcpy(queue->items + oldCapacity - queue->head, old, ((queue->head + queue->count) % oldCapacity) * sizeof(itemType)); \
        } \
        else \
        { \
            memcpy(queue->items, old + queue->head, queue->count * sizeof(itemType)); \
        } \
        \
        queue->head = 0; \
        queue->tail = queue->count; \
    } \
    \
    void typeName ## Init(typeName *queue, typeName ## Options options) \
    { \
        queue->defaultValue = options.defaultValue; \
        queue->equalsFn = options.equalsFn; \
        queue->freeFn = options.freeFn; \
        queue->capacity = 8; \
        queue->count = 0; \
        queue->head = 0; \
        queue->tail = 0; \
        queue->items = (itemType *)calloc(queue->capacity, sizeof(itemType)); \
    } \
    \
    void typeName ## Free(typeName *queue) \
    { \
        if (!queue->items) \
            return; \
        \
        typeName ## Clear(queue); \
        \
        free(queue->items); \
        queue->items = 0; \
    } \
    \
    void typeName ## Push(typeName *queue, itemType value) \
    { \
        if (!queue->items) \
            return; \
        \
        if (queue->count == queue->capacity) \
            typeName ## __resize(queue); \
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
        queue->head++; \
        queue->count--; \
        return value; \
    } \
    \
    bool typeName ## Contains(typeName *queue, itemType value) \
    { \
        if (!queue->items) \
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
                queue->freeFn(queue->items[i]); \
        } \
        \
        queue->count = 0; \
    }

#endif // SHL_QUEUE_H