#include "war_net.h"

#include <errno.h>
#include <stdlib.h>

#include "shl/wstr.h"

#include "alloc.h"
#include "war_log.h"

#define WAR_REQUEST_MESSAGE_MAX_SIZE 2048
#define WAR_URL_BUFFER_MAX_SIZE 1024

#if _WIN32
#define closeSocket closesocket
#else
#define closeSocket close
#endif

bool initNetwork(void)
{
#if _WIN32
    WSADATA wsaData;
    s32 status = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (status != 0)
    {
        logError("Couldn't initialize Websock. Errno: %d", status);
        return false;
    }
#endif

    return true;
}

bool cleanNetwork(void)
{
#if _WIN32
    s32 status = WSACleanup();
    if (status == SOCKET_ERROR)
    {
        logError("Couldn't cleanup Websock. Errno: %d", WSAGetLastError());
        return false;
    }
#endif

    return true;
}

WarSocket connectToHost(StringView host)
{
    const char* hostStr = wsv_data(host);
    struct hostent* hostEntry = gethostbyname(hostStr);
    if(!hostEntry)
    {
#if _WIN32
        logError("Couldn't create socket to host %s. Errno: %d", hostStr, WSAGetLastError());
#else
        logError("Couldn't create socket to host %s. Errno: %d", hostStr, h_errno);
#endif

        return 0;
    }

    WarSocket sck = socket(AF_INET, SOCK_STREAM, 0);
    if(sck == INVALID_SOCKET)
    {
#if _WIN32
        logError("Couldn't create socket to host %s. Last error: %d", hostStr, WSAGetLastError());
#else
        logError("Couldn't create socket to host %s. Last error: %d", hostStr, errno);
#endif

        return 0;
    }

    struct sockaddr_in hostInfo;
    hostInfo.sin_family = AF_INET;
    hostInfo.sin_addr = *((struct in_addr*)*hostEntry->h_addr_list);
    hostInfo.sin_port = htons(80);

    s32 status = connect(sck, (struct sockaddr*)&hostInfo, sizeof(struct sockaddr));
    if(status == SOCKET_ERROR)
    {
#if _WIN32
        logError("Couldn't connect socket to host %s. Last error: %d", hostStr, WSAGetLastError());
#else
        logError("Couldn't connect socket to host %s. Last error: %d", hostStr, errno);
#endif

        closeSocket(sck);

        return 0;
    }

    return sck;
}

bool requestResource(WarSocket sck, StringView resource, StringView host)
{
    const char* resourceStr = wsv_data(resource);
    const char* hostStr = wsv_data(host);
    size32 resourcelen = strlen(resourceStr);
    size32 hostlen = strlen(hostStr);
    size32 requestLength;
    size32 hostRequestLength;

    if (resourcelen > 0 && resourceStr[0] == '/')
    {
        resourceStr++;
        resourcelen--;
    }

    requestLength = (sizeof("GET / HTTP/1.1\r\n") - 1) + resourcelen + 1;
    hostRequestLength = (sizeof("Host: \r\n\r\n") - 1) + hostlen + 1;

    if (requestLength > WAR_REQUEST_MESSAGE_MAX_SIZE ||
        hostRequestLength > WAR_REQUEST_MESSAGE_MAX_SIZE)
    {
        logError("The host or resource are too long to build the HTTP request: host=%s, resource=%s.", hostStr, resourceStr);
        return false;
    }

    char message[WAR_REQUEST_MESSAGE_MAX_SIZE];
    snprintf(message, sizeof(message), "GET /%s HTTP/1.1\r\n", resourceStr);
    s32 status = send(sck, message, (s32)strlen(message), 0);
    if (status == SOCKET_ERROR)
    {
#if _WIN32
        logError("Couldn't send request with message: %s. Errno: %d", message, WSAGetLastError());
#else
        logError("Couldn't send request with message: %s. Errno: %d", message, errno);
#endif

        return false;
    }

    memset(message, 0, sizeof(message));
    snprintf(message, sizeof(message), "Host: %s\r\n\r\n", hostStr);
    status = send(sck, message, (s32)strlen(message), 0);
    if (status == SOCKET_ERROR)
    {
#if _WIN32
        logError("Couldn't send request with message: %s. Errno: %d", message, WSAGetLastError());
#else
        logError("Couldn't send request with message: %s. Errno: %d", message, errno);
#endif

        return false;
    }

    return true;
}

s32 parseHeadersFromResponse(StringView response, s32 responseLength, StringViewMap* headers)
{
    NOT_USED(responseLength);
    StringView view = response;

    size_t headersEnd = wsv_find(view, wsv_fromCString("\r\n\r\n"));
    if (headersEnd == WSV_NPOS)
    {
        return 0;
    }

    StringView headersText = wsv_slice(view, 0, headersEnd);

    // parse status line
    StringView line = wsv_chopByDelimiter(&headersText, '\n');
    line = wsv_trimRight(line); // Remove '\r'

    StringView httpVersion;
    if (wsv_splitOnce(line, wsv_fromCString(" "), &httpVersion, &line))
    {
        StringViewMapSet(headers, wsv_fromCString("HttpVersion"), wsv_toString(httpVersion));

        line = wsv_trimLeft(line);
        StringViewMapSet(headers, wsv_fromCString("ResponseStatus"), wsv_toString(line));
    }

    // parse headers
    while (headersText.length > 0)
    {
        line = wsv_chopByDelimiter(&headersText, '\n');
        line = wsv_trimRight(line); // Remove '\r'

        if (wsv_isEmpty(line))
        {
            continue;
        }

        StringView headerName, headerValue;
        if (wsv_splitOnce(line, wsv_fromCString(":"), &headerName, &headerValue))
        {
            headerValue = wsv_trimLeft(headerValue);
            StringViewMapSet(headers, headerName, wsv_toString(headerValue));
        }
    }

    // Return the number of bytes consumed including the \r\n\r\n
    return (s32)(headersEnd + 4);
}

s32 readResponse(WarSocket sck, char responseBuffer[], s32 responseBufferLength)
{
    char* responsePtr = responseBuffer;
    s32 responseLength = 0;
    s32 readFromSocket;

    do
    {
        size32 sizeToRead = min(RESPONSE_READ_SIZE, responseBufferLength - responseLength);
        if (sizeToRead <= 0)
            break;

        readFromSocket = recv(sck, responsePtr, (s32)sizeToRead, 0);
        if (readFromSocket == SOCKET_ERROR)
        {
#if _WIN32
        logError("Couldn't receive the response from socket %d. Errno: %d", (int)sck, WSAGetLastError());
#else
        logError("Couldn't receive the response from socket %d. Errno: %d", sck, errno);
#endif

            responseLength = -1;
            break;
        }

        responsePtr += readFromSocket;
        responseLength += readFromSocket;
    } while (readFromSocket > 0);

    return responseLength;
}

bool downloadFileFromUrl(StringView url, StringView filePath)
{
    const char* urlStr = wsv_data(url);
    const char* filePathStr = wsv_data(filePath);

    if(!urlStr || !filePathStr)
    {
        return false;
    }

    u16 shift = 0;
    if(strncasecmp(urlStr, "http://", strlen("http://")) == 0)
    {
        shift = (u16)strlen("http://");
    }

    if (strncasecmp(urlStr + shift, "www.", strlen("www.")) == 0)
    {
        shift += (u16)strlen("www.");
    }

    size32 urlLength = strlen(urlStr);
    if (shift > urlLength)
    {
        logError("The url has an invalid prefix offset: %s", urlStr);
        return false;
    }

    size32 cutLength = urlLength - shift + 1;
    if (cutLength > WAR_URL_BUFFER_MAX_SIZE)
    {
        logError("The url is too long to parse: %s", urlStr);
        return false;
    }

    char cut[WAR_URL_BUFFER_MAX_SIZE];
    strcpy(cut, urlStr + shift);

    const char* host = strtok(cut, "/");
    const char* resource = urlStr + shift + strlen(host);

    if (!initNetwork())
    {
        logError("Couldn't initialize the network.");
        return false;
    }

    WarSocket sck = connectToHost(wsv_fromCString(host));
    if (!sck)
    {
        logError("Couldn't connect to host: %s", host);
        cleanNetwork();
        return false;
    }

    if (!requestResource(sck, wsv_fromCString(resource), wsv_fromCString(host)))
    {
        logError("Couldn't download file from url %s", urlStr);
        closeSocket(sck);
        cleanNetwork();
        return false;
    }

    char* response = (char*)xcalloc(RESPONSE_MAX_SIZE, sizeof(char));
    char* responsePtr = response;

    s32 responseLength = readResponse(sck, response, RESPONSE_MAX_SIZE);

    if (responseLength < 0)
    {
        logError("Couldn't receive response from url %s", urlStr);

        closeSocket(sck);
        cleanNetwork();
        free(response);

        return false;
    }

    SDL_IOStream *stream = SDL_IOFromFile(filePathStr, "wb");
    if (!stream)
    {
        logError("Couldn't create a new file at: %s", filePathStr);

        closeSocket(sck);
        cleanNetwork();
        free(response);

        return false;
    }

    if (responseLength == 0)
    {
        logError("The response was empty from %s", urlStr);

        closeSocket(sck);
        cleanNetwork();
        free(response);
        SDL_CloseIO(stream);

        return true;
    }

    StringViewMapOptions options = StringViewMapDefaultOptions;

    StringViewMap headers;
    StringViewMapInit(&headers, options);

    s32 readFromResponse = parseHeadersFromResponse(wsv_fromParts(response, responseLength), responseLength, &headers);

    responsePtr += readFromResponse;
    responseLength -= readFromResponse;

    String responseStatus = StringViewMapGet(&headers, wsv_fromCString("ResponseStatus"));
    if (!responseStatus.data || !wsv_equals(wstr_view(&responseStatus), wsv_fromCString("200 OK")))
    {
        logError("The response status is not successful, received %s", responseStatus.data ? wstr_cstr(&responseStatus) : "NULL");

        StringViewMapFree(&headers);

        closeSocket(sck);
        cleanNetwork();
        free(response);
        SDL_CloseIO(stream);

        return false;
    }

    String contentType = StringViewMapGet(&headers, wsv_fromCString("Content-Type"));
    if (!contentType.data || !wsv_equals(wstr_view(&contentType), wsv_fromCString("application/octet-stream")))
    {
        logError("The content type of the response should be binary, received: %s", contentType.data ? wstr_cstr(&contentType) : "NULL");

        StringViewMapFree(&headers);

        closeSocket(sck);
        cleanNetwork();
        free(response);
        SDL_CloseIO(stream);

        return false;
    }

    String contentDisposition = StringViewMapGet(&headers, wsv_fromCString("Content-Disposition"));
    if (!contentDisposition.data || wsv_find(wstr_view(&contentDisposition), wsv_fromCString("attachment")) == WSV_NPOS)
    {
        logError("The content disposition of the response should be 'attachment', received: %s", contentDisposition.data ? wstr_cstr(&contentDisposition) : "NULL");

        StringViewMapFree(&headers);

        closeSocket(sck);
        cleanNetwork();
        free(response);
        SDL_CloseIO(stream);

        return false;
    }

    String transferEncoding = StringViewMapGet(&headers, wsv_fromCString("Transfer-Encoding"));
    if (transferEncoding.data && wsv_equals(wstr_view(&transferEncoding), wsv_fromCString("chunked")))
    {
        while (responseLength > 0)
        {
            s32 chunkSize = strtol(responsePtr, NULL, 16);

            // advance the characters of the chunk size and the \r\n
            s32 lengthToSkip = (s32)strcspn(responsePtr, "\r");
            responsePtr += lengthToSkip + 2;
            responseLength -= lengthToSkip + 2;

            if (chunkSize == 0)
            {
                break;
            }

            s32 sizeToWrite = min(chunkSize, responseLength);
            SDL_WriteIO(stream, responsePtr, sizeToWrite);

            responsePtr += sizeToWrite + 2;
            responseLength -= sizeToWrite + 2;
        }
    }
    else
    {
        // assume Transfer-Encoding: identity
        SDL_WriteIO(stream, responsePtr, responseLength);
    }

    StringViewMapFree(&headers);

    closeSocket(sck);
    cleanNetwork();
    free(response);
    SDL_CloseIO(stream);

    return true;
}
