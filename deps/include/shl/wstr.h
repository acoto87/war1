/*
    wstr.h - acoto87 (acoto87@gmail.com)

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

    Single-header string library providing two complementary types:

      StringView  - a non-owning, read-only view into an existing character buffer
                    (wsv_* functions, prefix stands for "wstr view")
      String      - a heap-allocated, null-terminated, mutable string
                    (wstr_* functions)

    USAGE
    -----
    In exactly one translation unit, define SHL_WSTR_IMPLEMENTATION before
    including this header to compile the implementation:

        #define SHL_WSTR_IMPLEMENTATION
        #include "wstr.h"

    All other translation units include it without the define:

        #include "wstr.h"

    CUSTOMISATION
    -------------
    Override the allocator before the implementation include if needed:

        #define WSTR_MALLOC(sz)       my_malloc(sz)
        #define WSTR_REALLOC(p, sz)   my_realloc(p, sz)
        #define WSTR_FREE(p)          my_free(p)
        #define SHL_WSTR_IMPLEMENTATION
        #include "wstr.h"
*/

#ifndef SHL_WSTR_H
#define SHL_WSTR_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
   Sentinel value returned by search functions when nothing is found
   ========================================================================= */

#define WSV_NPOS ((size_t)-1)

/* =========================================================================
   Compile-time literal helper
   ========================================================================= */

#define WSV_LITERAL(text) ((StringView){(text), sizeof(text) - 1})

/* =========================================================================
   Types
   ========================================================================= */

typedef struct
{
    const char* data;
    size_t length;
} StringView;

typedef struct
{
    char* data;
    size_t length;
    size_t capacity;
} String;

/* =========================================================================
   StringView API (wsv_*)
   ========================================================================= */

StringView wsv_empty(void);
StringView wsv_fromCString(const char* text);
StringView wsv_fromParts(const char* text, size_t length);
StringView wsv_fromRange(const char* begin, const char* end);
StringView wsv_fromString(const String* string);
bool       wsv_isEmpty(StringView view);
const char* wsv_data(StringView view);
size_t     wsv_length(StringView view);
StringView wsv_slice(StringView view, size_t index, size_t length);
StringView wsv_subview(StringView view, size_t index);
StringView wsv_trimLeft(StringView view);
StringView wsv_trimRight(StringView view);
StringView wsv_trim(StringView view);
bool       wsv_equals(StringView left, StringView right);
bool       wsv_equalsIgnoreCase(StringView left, StringView right);
bool       wsv_startsWith(StringView view, StringView prefix);
bool       wsv_startsWithIgnoreCase(StringView view, StringView prefix);
size_t     wsv_findChar(StringView view, char c);
size_t     wsv_find(StringView view, StringView needle);
size_t     wsv_findAny(StringView view, StringView chars);
StringView wsv_skipChars(StringView view, StringView chars);
StringView wsv_takeUntilAny(StringView view, StringView chars);
bool       wsv_splitOnce(StringView view, StringView separator, StringView* left, StringView* right);
StringView wsv_chopByDelimiter(StringView* remaining, char delimiter);
bool       wsv_nextToken(StringView* remaining, StringView separators, StringView* token);
uint32_t   wsv_hashFNV32(StringView view);
int32_t    wsv_parseS32(StringView view);
bool       wsv_tryParseS32(StringView view, int32_t* value);
int64_t    wsv_parseS64(StringView view);
bool       wsv_tryParseS64(StringView view, int64_t* value);
bool       wsv_copyToBuffer(StringView view, char* buffer, size_t capacity);
String     wsv_toString(StringView view);

/* =========================================================================
   String API (wstr_*)
   ========================================================================= */

String     wstr_make(void);
String     wstr_withCapacity(size_t capacity);
String     wstr_fromCString(const char* text);
String     wstr_fromCStringFormat(const char* textFormat, ...);
String     wstr_fromCStringFormatv(const char* textFormat, va_list args);
String     wstr_fromView(StringView view);
String     wstr_adopt(char* buffer, size_t length, size_t capacity);
void       wstr_freePtr(String* string);
void       wstr_free(String string);
void       wstr_clear(String* string);
StringView wstr_view(const String* string);
const char* wstr_cstr(const String* string);
bool       wstr_isEmpty(const String* string);
bool       wstr_reserve(String* string, size_t capacity);
bool       wstr_resize(String* string, size_t length);
bool       wstr_assign(String* string, StringView view);
bool       wstr_assignCString(String* string, const char* text);
bool       wstr_append(String* string, StringView view);
bool       wstr_appendCString(String* string, const char* text);
bool       wstr_appendChar(String* string, char c);
bool       wstr_insert(String* string, size_t index, StringView view);
bool       wstr_removeRange(String* string, size_t index, size_t length);
bool       wstr_setFormat(String* string, const char* fmt, ...);
bool       wstr_setFormatv(String* string, const char* fmt, va_list args);
bool       wstr_appendFormat(String* string, const char* fmt, ...);
bool       wstr_appendFormatv(String* string, const char* fmt, va_list args);

#ifdef __cplusplus
}
#endif

/* =========================================================================
   Implementation
   ========================================================================= */

#ifdef SHL_WSTR_IMPLEMENTATION

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WSTR_MALLOC
#define WSTR_MALLOC(sz)     malloc(sz)
#endif

#ifndef WSTR_REALLOC
#define WSTR_REALLOC(p, sz) realloc((p), (sz))
#endif

#ifndef WSTR_FREE
#define WSTR_FREE(p)        free(p)
#endif

#ifndef FNV_PRIME_32
#define FNV_PRIME_32  0x01000193u
#endif
#ifndef FNV_OFFSET_32
#define FNV_OFFSET_32 0x811c9dc5u
#endif

/* ----- internal helpers -------------------------------------------------- */

static char wstr__toLowerAscii(char c)
{
    return (char)tolower((unsigned char)c);
}

static bool wsv__containsChar(StringView chars, char c)
{
    for (size_t i = 0; i < chars.length; i++)
    {
        if (chars.data[i] == c)
        {
            return true;
        }
    }

    return false;
}

static bool wstr__isAliased(const String* string, StringView view)
{
    if (string == NULL || string->data == NULL || view.data == NULL || view.length == 0)
    {
        return false;
    }

    const char* begin = string->data;
    const char* end = string->data + string->length;
    return view.data >= begin && view.data <= end;
}

static bool wstr__grow(String* string, size_t minCapacity)
{
    if (string == NULL)
    {
        return false;
    }

    if (minCapacity <= string->capacity)
    {
        return true;
    }

    if (minCapacity == (size_t)-1)
    {
        return false;
    }

    size_t nextCapacity = string->capacity > 0 ? string->capacity : 16;
    while (nextCapacity < minCapacity)
    {
        size_t doubled = nextCapacity * 2;
        if (doubled <= nextCapacity)
        {
            nextCapacity = minCapacity;
            break;
        }

        nextCapacity = doubled;
    }

    if (nextCapacity + 1 < nextCapacity)
    {
        return false;
    }

    char* data = string->data != NULL
        ? (char*)WSTR_REALLOC(string->data, nextCapacity + 1)
        : (char*)WSTR_MALLOC(nextCapacity + 1);
    if (data == NULL)
    {
        return false;
    }

    string->data = data;
    string->capacity = nextCapacity;
    string->data[string->length] = 0;
    return true;
}

static bool wstr__appendFormatAt(String* string, size_t offset, const char* fmt, va_list args)
{
    if (string == NULL || fmt == NULL)
    {
        return false;
    }

    if (!wstr__grow(string, offset + 1))
    {
        return false;
    }

    for (;;)
    {
        size_t available = string->capacity - offset;
        va_list argsCopy;
        va_copy(argsCopy, args);
        int required = vsnprintf(string->data + offset, available + 1, fmt, argsCopy);
        va_end(argsCopy);

        if (required >= 0 && (size_t)required <= available)
        {
            string->length = offset + (size_t)required;
            return true;
        }

        size_t needed = required >= 0 ? offset + (size_t)required : (string->capacity * 2);
        if (needed < offset + 32)
        {
            needed = offset + 32;
        }

        if (!wstr__grow(string, needed))
        {
            return false;
        }
    }
}

static bool wsv__tryParseInteger(StringView view, int64_t minValue, int64_t maxValue, int64_t* value)
{
    if (value == NULL)
    {
        return false;
    }

    view = wsv_trim(view);
    if (view.length == 0)
    {
        return false;
    }

    size_t index = 0;
    bool negative = false;
    if (view.data[index] == '+' || view.data[index] == '-')
    {
        negative = view.data[index] == '-';
        index++;
    }

    if (index >= view.length)
    {
        return false;
    }

    int base = 10;
    if (view.data[index] == '0' && (index + 1) < view.length)
    {
        char next = view.data[index + 1];
        if (next == 'x' || next == 'X')
        {
            base = 16;
            index += 2;
        }
        else
        {
            base = 8;
        }
    }

    if (index >= view.length)
    {
        return false;
    }

    uint64_t limit = negative ? (uint64_t)LLONG_MAX + 1ull : (uint64_t)LLONG_MAX;
    uint64_t result = 0;
    bool hasDigits = false;

    for (; index < view.length; index++)
    {
        unsigned char c = (unsigned char)view.data[index];
        int digit;
        if (c >= '0' && c <= '9')
        {
            digit = c - '0';
        }
        else if (c >= 'a' && c <= 'f')
        {
            digit = 10 + (c - 'a');
        }
        else if (c >= 'A' && c <= 'F')
        {
            digit = 10 + (c - 'A');
        }
        else
        {
            return false;
        }

        if (digit >= base)
        {
            return false;
        }

        if (result > (limit - (uint64_t)digit) / (uint64_t)base)
        {
            return false;
        }

        result = result * (uint64_t)base + (uint64_t)digit;
        hasDigits = true;
    }

    if (!hasDigits)
    {
        return false;
    }

    int64_t signedValue;
    if (negative)
    {
        if (result == (uint64_t)LLONG_MAX + 1ull)
        {
            signedValue = LLONG_MIN;
        }
        else
        {
            signedValue = -(int64_t)result;
        }
    }
    else
    {
        signedValue = (int64_t)result;
    }

    if (signedValue < minValue || signedValue > maxValue)
    {
        return false;
    }

    *value = signedValue;
    return true;
}

/* ----- StringView implementation ----------------------------------------- */

StringView wsv_empty(void)
{
    return (StringView){0};
}

StringView wsv_fromCString(const char* text)
{
    if (text == NULL)
    {
        return wsv_empty();
    }

    return wsv_fromParts(text, strlen(text));
}

StringView wsv_fromParts(const char* text, size_t length)
{
    if (text == NULL && length == 0)
    {
        return wsv_empty();
    }

    return (StringView){text, length};
}

StringView wsv_fromRange(const char* begin, const char* end)
{
    if (begin == NULL || end == NULL || end < begin)
    {
        return wsv_empty();
    }

    return wsv_fromParts(begin, (size_t)(end - begin));
}

StringView wsv_fromString(const String* string)
{
    if (string == NULL)
    {
        return wsv_empty();
    }

    return wsv_fromParts(string->data, string->length);
}

bool wsv_isEmpty(StringView view)
{
    return view.length == 0;
}

const char* wsv_data(StringView view)
{
    return view.data != NULL ? view.data : "";
}

size_t wsv_length(StringView view)
{
    return view.length;
}

StringView wsv_slice(StringView view, size_t index, size_t length)
{
    if (index >= view.length)
    {
        return wsv_empty();
    }

    size_t available = view.length - index;
    if (length > available)
    {
        length = available;
    }

    return wsv_fromParts(view.data + index, length);
}

StringView wsv_subview(StringView view, size_t index)
{
    if (index >= view.length)
    {
        return wsv_empty();
    }

    return wsv_fromParts(view.data + index, view.length - index);
}

StringView wsv_trimLeft(StringView view)
{
    size_t index = 0;
    while (index < view.length && isspace((unsigned char)view.data[index]))
    {
        index++;
    }

    return wsv_subview(view, index);
}

StringView wsv_trimRight(StringView view)
{
    while (view.length > 0 && isspace((unsigned char)view.data[view.length - 1]))
    {
        view.length--;
    }

    return view;
}

StringView wsv_trim(StringView view)
{
    return wsv_trimRight(wsv_trimLeft(view));
}

bool wsv_equals(StringView left, StringView right)
{
    if (left.length != right.length)
    {
        return false;
    }

    if (left.length == 0)
    {
        return true;
    }

    return memcmp(left.data, right.data, left.length) == 0;
}

bool wsv_equalsIgnoreCase(StringView left, StringView right)
{
    if (left.length != right.length)
    {
        return false;
    }

    for (size_t i = 0; i < left.length; i++)
    {
        if (wstr__toLowerAscii(left.data[i]) != wstr__toLowerAscii(right.data[i]))
        {
            return false;
        }
    }

    return true;
}

bool wsv_startsWith(StringView view, StringView prefix)
{
    return prefix.length <= view.length && wsv_equals(wsv_slice(view, 0, prefix.length), prefix);
}

bool wsv_startsWithIgnoreCase(StringView view, StringView prefix)
{
    return prefix.length <= view.length && wsv_equalsIgnoreCase(wsv_slice(view, 0, prefix.length), prefix);
}

size_t wsv_findChar(StringView view, char c)
{
    for (size_t i = 0; i < view.length; i++)
    {
        if (view.data[i] == c)
        {
            return i;
        }
    }

    return WSV_NPOS;
}

size_t wsv_find(StringView view, StringView needle)
{
    if (needle.length == 0)
    {
        return 0;
    }

    if (needle.length > view.length)
    {
        return WSV_NPOS;
    }

    size_t last = view.length - needle.length;
    for (size_t i = 0; i <= last; i++)
    {
        if (view.data[i] == needle.data[0] && memcmp(view.data + i, needle.data, needle.length) == 0)
        {
            return i;
        }
    }

    return WSV_NPOS;
}

size_t wsv_findAny(StringView view, StringView chars)
{
    for (size_t i = 0; i < view.length; i++)
    {
        if (wsv__containsChar(chars, view.data[i]))
        {
            return i;
        }
    }

    return WSV_NPOS;
}

StringView wsv_skipChars(StringView view, StringView chars)
{
    size_t index = 0;
    while (index < view.length && wsv__containsChar(chars, view.data[index]))
    {
        index++;
    }

    return wsv_subview(view, index);
}

StringView wsv_takeUntilAny(StringView view, StringView chars)
{
    size_t index = wsv_findAny(view, chars);
    if (index == WSV_NPOS)
    {
        return view;
    }

    return wsv_slice(view, 0, index);
}

bool wsv_splitOnce(StringView view, StringView separator, StringView* left, StringView* right)
{
    if (left == NULL || right == NULL)
    {
        return false;
    }

    if (separator.length == 0)
    {
        *left = wsv_empty();
        *right = view;
        return true;
    }

    size_t index = wsv_find(view, separator);
    if (index == WSV_NPOS)
    {
        *left = view;
        *right = wsv_empty();
        return false;
    }

    *left = wsv_slice(view, 0, index);
    *right = wsv_subview(view, index + separator.length);
    return true;
}

StringView wsv_chopByDelimiter(StringView* remaining, char delimiter)
{
    if (remaining == NULL)
    {
        return wsv_empty();
    }

    size_t index = wsv_findChar(*remaining, delimiter);
    if (index == WSV_NPOS)
    {
        StringView token = *remaining;
        *remaining = wsv_empty();
        return token;
    }

    StringView token = wsv_slice(*remaining, 0, index);
    *remaining = wsv_subview(*remaining, index + 1);
    return token;
}

bool wsv_nextToken(StringView* remaining, StringView separators, StringView* token)
{
    if (remaining == NULL || token == NULL)
    {
        return false;
    }

    *remaining = wsv_skipChars(*remaining, separators);
    if (remaining->length == 0)
    {
        *token = wsv_empty();
        return false;
    }

    size_t index = wsv_findAny(*remaining, separators);
    if (index == WSV_NPOS)
    {
        *token = *remaining;
        *remaining = wsv_empty();
        return true;
    }

    *token = wsv_slice(*remaining, 0, index);
    *remaining = wsv_subview(*remaining, index);
    return true;
}

uint32_t wsv_hashFNV32(StringView view)
{
    uint32_t hash = FNV_OFFSET_32;
    for (size_t i = 0; i < view.length; i++)
    {
        hash = ((uint32_t)(unsigned char)view.data[i] ^ hash) * FNV_PRIME_32;
    }

    return hash;
}

int32_t wsv_parseS32(StringView view)
{
    int32_t value = 0;
    wsv_tryParseS32(view, &value);
    return value;
}

bool wsv_tryParseS32(StringView view, int32_t* value)
{
    int64_t parsedValue = 0;
    if (!wsv__tryParseInteger(view, INT32_MIN, INT32_MAX, &parsedValue))
    {
        return false;
    }

    if (value != NULL)
    {
        *value = (int32_t)parsedValue;
    }

    return true;
}

int64_t wsv_parseS64(StringView view)
{
    int64_t value = 0;
    wsv_tryParseS64(view, &value);
    return value;
}

bool wsv_tryParseS64(StringView view, int64_t* value)
{
    int64_t parsedValue = 0;
    if (!wsv__tryParseInteger(view, LLONG_MIN, LLONG_MAX, &parsedValue))
    {
        return false;
    }

    if (value != NULL)
    {
        *value = parsedValue;
    }

    return true;
}

bool wsv_copyToBuffer(StringView view, char* buffer, size_t capacity)
{
    if (buffer == NULL || capacity == 0 || view.length >= capacity)
    {
        if (buffer != NULL && capacity > 0)
        {
            buffer[0] = 0;
        }

        return false;
    }

    if (view.length > 0)
    {
        memcpy(buffer, view.data, view.length);
    }

    buffer[view.length] = 0;
    return true;
}

String wsv_toString(StringView view)
{
    return wstr_fromView(view);
}

/* ----- String implementation --------------------------------------------- */

String wstr_make(void)
{
    return (String){0};
}

String wstr_withCapacity(size_t capacity)
{
    String string = wstr_make();
    wstr_reserve(&string, capacity);
    return string;
}

String wstr_fromCString(const char* text)
{
    return wstr_fromView(wsv_fromCString(text));
}

String wstr_fromCStringFormat(const char* textFormat, ...)
{
    va_list args;
    va_start(args, textFormat);
    String string = wstr_fromCStringFormatv(textFormat, args);
    va_end(args);
    return string;
}

String wstr_fromCStringFormatv(const char* textFormat, va_list args)
{
    String string = wstr_make();
    if (!wstr_setFormatv(&string, textFormat, args))
    {
        wstr_freePtr(&string);
    }
    return string;
}

String wstr_fromView(StringView view)
{
    String string = wstr_make();
    wstr_assign(&string, view);
    return string;
}

String wstr_adopt(char* buffer, size_t length, size_t capacity)
{
    if (buffer == NULL)
    {
        return wstr_make();
    }

    if (length > capacity)
    {
        length = capacity;
    }

    buffer[length] = 0;
    return (String){buffer, length, capacity};
}

void wstr_freePtr(String* string)
{
    if (string == NULL)
    {
        return;
    }

    if (string->data != NULL)
    {
        WSTR_FREE(string->data);
    }

    *string = wstr_make();
}

void wstr_free(String string)
{
    if (string.data != NULL)
    {
        WSTR_FREE(string.data);
    }
}

void wstr_clear(String* string)
{
    if (string == NULL)
    {
        return;
    }

    string->length = 0;
    if (string->data != NULL)
    {
        string->data[0] = 0;
    }
}

StringView wstr_view(const String* string)
{
    return wsv_fromString(string);
}

const char* wstr_cstr(const String* string)
{
    return string != NULL && string->data != NULL ? string->data : "";
}

bool wstr_isEmpty(const String* string)
{
    return string == NULL || string->length == 0;
}

bool wstr_reserve(String* string, size_t capacity)
{
    return wstr__grow(string, capacity);
}

bool wstr_resize(String* string, size_t length)
{
    if (string == NULL)
    {
        return false;
    }

    if (!wstr__grow(string, length))
    {
        return false;
    }

    if (length > string->length)
    {
        memset(string->data + string->length, 0, length - string->length);
    }

    string->length = length;
    if (string->data != NULL)
    {
        string->data[length] = 0;
    }

    return true;
}

bool wstr_assign(String* string, StringView view)
{
    if (string == NULL)
    {
        return false;
    }

    if (wstr__isAliased(string, view))
    {
        String temp = wsv_toString(view);
        bool ok = wstr_assign(string, wstr_view(&temp));
        wstr_free(temp);
        return ok;
    }

    if (!wstr_resize(string, view.length))
    {
        return false;
    }

    if (view.length > 0)
    {
        memcpy(string->data, view.data, view.length);
        string->data[view.length] = 0;
    }

    return true;
}

bool wstr_assignCString(String* string, const char* text)
{
    return wstr_assign(string, wsv_fromCString(text));
}

bool wstr_append(String* string, StringView view)
{
    if (string == NULL)
    {
        return false;
    }

    if (view.length == 0)
    {
        return true;
    }

    if (wstr__isAliased(string, view))
    {
        String temp = wsv_toString(view);
        bool ok = wstr_append(string, wstr_view(&temp));
        wstr_free(temp);
        return ok;
    }

    size_t oldLength = string->length;
    if (!wstr_resize(string, string->length + view.length))
    {
        return false;
    }

    memcpy(string->data + oldLength, view.data, view.length);
    string->data[string->length] = 0;
    return true;
}

bool wstr_appendCString(String* string, const char* text)
{
    return wstr_append(string, wsv_fromCString(text));
}

bool wstr_appendChar(String* string, char c)
{
    return wstr_append(string, wsv_fromParts(&c, 1));
}

bool wstr_insert(String* string, size_t index, StringView view)
{
    if (string == NULL || index > string->length)
    {
        return false;
    }

    if (view.length == 0)
    {
        return true;
    }

    if (wstr__isAliased(string, view))
    {
        String temp = wsv_toString(view);
        bool ok = wstr_insert(string, index, wstr_view(&temp));
        wstr_free(temp);
        return ok;
    }

    size_t oldLength = string->length;
    if (!wstr_resize(string, oldLength + view.length))
    {
        return false;
    }

    memmove(
        string->data + index + view.length,
        string->data + index,
        oldLength - index);
    memcpy(string->data + index, view.data, view.length);
    string->data[string->length] = 0;
    return true;
}

bool wstr_removeRange(String* string, size_t index, size_t length)
{
    if (string == NULL || index > string->length)
    {
        return false;
    }

    if (length == 0 || index == string->length)
    {
        return true;
    }

    size_t clampedLength = length;
    if (clampedLength > string->length - index)
    {
        clampedLength = string->length - index;
    }

    memmove(
        string->data + index,
        string->data + index + clampedLength,
        string->length - index - clampedLength + 1);
    string->length -= clampedLength;
    return true;
}

bool wstr_setFormat(String* string, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool ok = wstr_setFormatv(string, fmt, args);
    va_end(args);
    return ok;
}

bool wstr_setFormatv(String* string, const char* fmt, va_list args)
{
    if (string == NULL)
    {
        return false;
    }

    wstr_clear(string);
    return wstr__appendFormatAt(string, 0, fmt, args);
}

bool wstr_appendFormat(String* string, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool ok = wstr_appendFormatv(string, fmt, args);
    va_end(args);
    return ok;
}

bool wstr_appendFormatv(String* string, const char* fmt, va_list args)
{
    if (string == NULL)
    {
        return false;
    }

    return wstr__appendFormatAt(string, string->length, fmt, args);
}

#endif /* SHL_WSTR_IMPLEMENTATION */

#endif /* SHL_WSTR_H */
