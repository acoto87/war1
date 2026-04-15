#pragma once

#if _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET WarSocket;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
typedef int WarSocket;
#endif

#define RESPONSE_READ_SIZE 2048
#define RESPONSE_MAX_SIZE (5 * 1024 * 1024) // 5MB

bool initNetwork();
bool cleanNetwork();
WarSocket connectToHost(const char* host);
bool requestResource(WarSocket sck, const char* resource, const char* host);
s32 readResponse(WarSocket sck, char responseBuffer[], s32 responseBufferLength);
s32 parseHeadersFromResponse(const char* response, s32 responseLength, SSMap* headers);
bool downloadFileFromUrl(const char* url, const char* filePath);
