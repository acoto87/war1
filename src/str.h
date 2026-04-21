#pragma once

#include <stdint.h>
#include <stdbool.h>

bool wutil_strEquals(const char* str1, const char* str2);
bool wutil_strCaseEquals(const char* str1, const char* str2, bool ignoreCase);
uint32_t strHashFNV32(const char* data);
void wutil_strFree(char* str);

void wutil_strInsertAt(char* str, int32_t index, char c);
void wutil_strRemoveAt(char* str, int32_t index);
bool wutil_strStartsWith(const char* str1, const char* str2);
bool wutil_strCaseStartsWith(const char* str1, const char* str2, bool ignoreCase);
const char* wutil_strSkipUntil(const char* str1, const char* str2);
int32_t wutil_strParseS32(const char* str);
bool wutil_strTryParseS32(const char* str, int32_t* value);
int64_t wutil_strParseS64(const char* str);
bool wutil_strTryParseS64(const char* str, int64_t* value);
