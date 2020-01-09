#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

#define RESPONSE_READ_SIZE 2048
#define RESPONSE_MAX_SIZE (5 * 1024 * 1024) // 5MB

SOCKET connectToHost(const char* host);
bool requestResource(SOCKET sck, const char* resource, const char* host);
size32 readResponse(SOCKET sck, char responseBuffer[], size32 responseBufferLength);
size32 parseHeadersFromResponse(const char* response, s32 responseLength, SSMap* headers);
bool downloadFileFromUrl(const char* url, const char* filePath);
