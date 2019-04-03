/*  
    array.h - acoto87 (acoto87@gmail.com)

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
#ifndef SHL_ARRAY_H
#define SHL_ARRAY_H

#include <stdint.h>
#include <stdlib.h>

#define shlDeclareCreateArray(prefix, itemType) \
    itemType** prefix ## CreateArray(int32_t n, int32_t m);

#define shlDefineCreateArray(prefix, itemType) \
    itemType** prefix ## CreateArray(int32_t n, int32_t m) \
    { \
        itemType* values = (itemType*)calloc(m * n, sizeof(itemType)); \
        itemType** rows = (itemType**)malloc(n * sizeof(itemType*)); \
        for (int i = 0; i < n; ++i) \
        { \
            rows[i] = values + i * m; \
        } \
        return rows; \
    }

#define shlDeclareFreeArray(prefix, itemType) \
    void prefix ## FreeArray(itemType** arr);

#define shlDefineFreeArray(prefix, itemType) \
    void prefix ## FreeArray(itemType** arr) \
    { \
        free(*arr); \
        free(arr); \
    }

#endif // SHL_ARRAY_H
