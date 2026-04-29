#pragma once

#include <stdbool.h>

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

#include "war_types.h"

bool wnet_initNetwork(void);
bool wnet_cleanNetwork(void);
WarSocket wnet_connectToHost(StringView host);
bool wnet_requestResource(WarSocket sck, StringView resource, StringView host);
s32 wnet_readResponse(WarSocket sck, char responseBuffer[], s32 responseBufferLength);
s32 wnet_parseHeadersFromResponse(StringView response, s32 responseLength, StringViewMap* headers);
bool wnet_downloadFileFromUrl(WarContext* context, StringView url, StringView filePath);
