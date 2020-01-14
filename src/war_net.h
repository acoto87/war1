#pragma once

#if _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#endif

#define RESPONSE_READ_SIZE 2048
#define RESPONSE_MAX_SIZE (5 * 1024 * 1024) // 5MB

bool initNetwork();
bool cleanNetwork();
u32 connectToHost(const char* host);
bool requestResource(u32 sck, const char* resource, const char* host);
s32 readResponse(u32 sck, char responseBuffer[], s32 responseBufferLength);
s32 parseHeadersFromResponse(const char* response, s32 responseLength, SSMap* headers);
bool downloadFileFromUrl(const char* url, const char* filePath);
