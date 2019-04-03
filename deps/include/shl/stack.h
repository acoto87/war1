/*  
    stack.h - acoto87 (acoto87@gmail.com)

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

    This is a single header file with macros to declare and define a strongly typed stack with push, pop and peek operations.
*/
#ifndef SHL_STACK_H
#define SHL_STACK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define shlDeclareStack(typeName, itemType) \
    typedef struct \
    { \
        itemType defaultValue; \
        bool (*equalsFn)(const itemType item1, const itemType item2); \
        void (*freeFn)(itemType item); \
    } typeName ## Options; \
    \
    typedef struct \
    { \
        uint32_t count; \
        uint32_t capacity; \
        bool (*equalsFn)(const itemType item1, const itemType item2); \
        void (*freeFn)(itemType item); \
        itemType defaultValue; \
        itemType *items; \
    } typeName; \
    \
    void typeName ## Init(typeName *stack, typeName ## Options options); \
    void typeName ## Free(typeName *stack); \
    void typeName ## Push(typeName *stack, itemType value); \
    bool typeName ## Contains(typeName *stack, itemType value); \
    itemType typeName ## Peek(typeName *stack); \
    itemType typeName ## Pop(typeName *stack); \
    void typeName ## Clear(typeName *stack);

#define shlDefineStack(typeName, itemType) \
    void typeName ## __resize(typeName *stack) \
    { \
        uint32_t oldCapacity = stack->capacity; \
        \
        stack->capacity = oldCapacity << 1; \
        stack->items = (itemType *)realloc(stack->items, stack->capacity * sizeof(itemType)); \
    } \
    \
    void typeName ## Init(typeName *stack, typeName ## Options options) \
    { \
        stack->defaultValue = options.defaultValue; \
        stack->equalsFn = options.equalsFn; \
        stack->freeFn = options.freeFn; \
        stack->capacity = 8; \
        stack->count = 0; \
        stack->items = (itemType *)calloc(stack->capacity, sizeof(itemType)); \
    } \
    \
    void typeName ## Free(typeName *stack) \
    { \
        if (!stack->items) \
            return; \
        \
        typeName ## Clear(stack); \
        \
        free(stack->items); \
        stack->items = 0; \
    } \
    \
    void typeName ## Push(typeName *stack, itemType value) \
    { \
        if (!stack->items) \
            return; \
        \
        if (stack->count == stack->capacity) \
            typeName ## __resize(stack); \
        \
        stack->items[stack->count] = value; \
        stack->count++; \
    } \
    \
    itemType typeName ## Peek(typeName *stack) \
    { \
        if (!stack->items || stack->count == 0) \
            return stack->defaultValue; \
        \
        return stack->items[stack->count - 1]; \
    } \
    \
    itemType typeName ## Pop(typeName *stack) \
    { \
        if (!stack->items || stack->count == 0) \
            return stack->defaultValue; \
        \
        itemType item = stack->items[stack->count - 1]; \
        stack->count--; \
        return item; \
    } \
    \
    bool typeName ## Contains(typeName *stack, itemType value) \
    { \
        if (!stack->items) \
            return false; \
        \
        if (!stack->equalsFn) \
            return false; \
        \
        for(int32_t i = 0; i < stack->count; i++) \
        { \
            if (stack->equalsFn(stack->items[i], value)) \
                return true; \
        } \
        \
        return false; \
    } \
    \
    void typeName ## Clear(typeName* stack) \
    { \
        if (!stack->items) \
            return; \
        \
        if (stack->freeFn) \
        { \
            for(int32_t i = 0; i < stack->count; i++) \
                stack->freeFn(stack->items[i]); \
        } \
        \
        stack->count = 0; \
    }

#endif // SHL_STACK_H