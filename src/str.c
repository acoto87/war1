#include "str.h"

bool strEquals(const char* str1, const char* str2)
{
    return strcmp(str1, str2) == 0;
}

bool strCaseEquals(const char* str1, const char* str2, bool ignoreCase)
{
    return ignoreCase
        ? strcasecmp(str1, str2) == 0
        : strcmp(str1, str2) == 0;
}

uint32_t strHashFNV32(const char* data)
{
#define FNV_PRIME_32 0x01000193
#define FNV_OFFSET_32 0x811c9dc5

    uint32_t hash = FNV_OFFSET_32;
    while(*data != 0)
        hash = (*data++ ^ hash) * FNV_PRIME_32;

    return hash;
}

void strFree(char* str)
{
    free((void*)str);
}

void strInsertAt(char* str, int32_t index, char c)
{
    int32_t length = (int32_t)strlen(str) + 1; // count the \0
    memmove(str + index + 1, str + index, length - index);
    str[index] = c;
}

void strRemoveAt(char* str, int32_t index)
{
    int32_t length = (int32_t)strlen(str) + 1; // count the \0
    memmove(str + index, str + index + 1, length - index - 1);
}

bool strStartsWith(const char* str1, const char* str2)
{
    return strncmp(str1, str2, strlen(str2)) == 0;
}

bool strCaseStartsWith(const char* str1, const char* str2, bool ignoreCase)
{
    return ignoreCase
        ? strncasecmp(str1, str2, strlen(str2)) == 0
        : strncmp(str1, str2, strlen(str2)) == 0;
}

const char* strSkipUntil(const char* str1, const char* str2)
{
    char* str = (char*)str1;
    while (strchr(str2, *str))
    {
        str++;
    }

    return str;
}

int32_t strParseS32(const char* str)
{
    return strtol(str, NULL, 0);
}

bool strTryParseS32(const char* str, int32_t* value)
{
    char* following;
    *value = strtol(str, &following, 0);
    return following > str;
}

int64_t strParseS64(const char* str)
{
    return strtoll(str, NULL, 0);
}

bool strTryParseS64(const char* str, int64_t* value)
{
    char* following;
    *value = strtoll(str, &following, 0);
    return following > str;
}