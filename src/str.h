#pragma once

#include <stdint.h>
#include <stdbool.h>

bool strEquals(const char* str1, const char* str2);
bool strCaseEquals(const char* str1, const char* str2, bool ignoreCase);
uint32_t strHashFNV32(const char* data);
void strFree(char* str);

void strInsertAt(char* str, int32_t index, char c);
void strRemoveAt(char* str, int32_t index);
bool strStartsWith(const char* str1, const char* str2);
bool strCaseStartsWith(const char* str1, const char* str2, bool ignoreCase);
const char* strSkipUntil(const char* str1, const char* str2);
int32_t strParseS32(const char* str);
bool strTryParseS32(const char* str, int32_t* value);
int64_t strParseS64(const char* str);
bool strTryParseS64(const char* str, int64_t* value);
