SOCKET connectToHost(const char* host)
{
    WSADATA wsaData;
    s32 status = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (status != 0)
    {
        logError("Couldn't initialize Websock\n");
        logError("Last error: %d\n", WSAGetLastError());

        return 0;
    }

    LPHOSTENT hostEntry = gethostbyname(host);
    if(!hostEntry)
    {
        logError("Couldn't retrieve host address from name %s\n", host);
        logError("Last error: %d\n", WSAGetLastError());

        WSACleanup();
        return 0;
    }

    SOCKET sck = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sck == INVALID_SOCKET)
    {
        logError("Couldn't create socket to host %s\n", host);
        logError("Last error: %d\n", WSAGetLastError());

        WSACleanup();
        return 0;
    }

    SOCKADDR_IN hostInfo;
    hostInfo.sin_family = AF_INET;
    hostInfo.sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list);
    hostInfo.sin_port = htons(80);

    status = connect(sck, (LPSOCKADDR)&hostInfo, sizeof(struct sockaddr));
    if(sck == SOCKET_ERROR || status != 0)
    {
        logError("Couldn't connect socket to host %s\n", host);
        return 0;
    }

    return sck;
}

bool requestResource(SOCKET sck, const char* resource, const char* host)
{
    s32 resourcelen = strlen(resource);
    s32 hostlen = strlen(host);
    s32 len = max(resourcelen, hostlen);

    if (resourcelen > 0 && resource[0] == '/')
    {
        resource++;
    }

    char message[20 + len];
    sprintf(message, "GET /%s HTTP/1.1\r\n", resource);
    s32 status = send(sck, message, strlen(message), 0);
    if (status == SOCKET_ERROR)
    {
        logError("Couldn't send request with message: %s\n", message);
        logError("Last error: %d\n", WSAGetLastError());

        return false;
    }

    memset(message, 0, sizeof(message));
    sprintf(message, "Host: %s\r\n\r\n", host);
    status = send(sck, message, strlen(message), 0);
    if (status == SOCKET_ERROR)
    {
        logError("Couldn't send request with message: %s\n", message);
        logError("Last error: %d\n", WSAGetLastError());

        return false;
    }

    return true;
}

size32 parseHeadersFromResponse(const char* response, s32 responseLength, SSMap* headers)
{
    // Example of headers in a HTTP response
    //
    // HTTP/1.1 200 OK
    // Server: nginx/1.14.0 (Ubuntu)
    // Date: Mon, 06 Jan 2020.00:52:11 GMT
    // Content-Type: application/octet-stream
    // Transfer-Encoding: chunked
    // Connection: keep-alive
    // Cache-Control: public, max-age=10800
    // Content-Disposition: attachment; filename="DATA.WAR"
    // Strict-Transport-Security: max-age=15724800
    // ..
    // ff3a..

    // parse status line
    const char* line = response;
    size32 lineLength = strcspn(line, "\r\n");

    size32 httpVersionLength = strcspn(line, " ");
    char* httpVersion = (char*)xcalloc(httpVersionLength + 1, sizeof(char));
    strncpy(httpVersion, line, httpVersionLength);
    SSMapSet(headers, "HttpVersion", httpVersion);

    line += httpVersionLength + 1;

    size32 responseStatusLength = lineLength - (httpVersionLength + 1);
    char* responseStatus = (char*)xcalloc(responseStatusLength + 1, sizeof(char));
    strncpy(responseStatus, line, responseStatusLength);
    SSMapSet(headers, "ResponseStatus", responseStatus);

    line += responseStatusLength + 2;

    // parse headers until there is a line that only have \r\n
    lineLength = strcspn(line, "\r\n");
    while (lineLength > 0)
    {
        size32 headerNameLength = strcspn(line, ":");
        char* headerName = (char*)xcalloc(headerNameLength + 1, sizeof(char));
        strncpy(headerName, line, headerNameLength);

        size32 headerValueLength = lineLength - (headerNameLength + 2);
        char* headerValue = (char*)xcalloc(headerValueLength + 1, sizeof(char));
        strncpy(headerValue, line + headerNameLength + 2, headerValueLength);

        SSMapSet(headers, headerName, headerValue);

        line += lineLength + 2;
        lineLength = strcspn(line, "\r\n");
    }

    size32 advance = (size32)(line - response) + 2;
    return advance;
}

size32 readResponse(SOCKET sck, char responseBuffer[], size32 responseBufferLength)
{
    char* responsePtr = responseBuffer;
    size32 responseLength = 0;
    s32 readFromSocket;

    do
    {
        size32 sizeToRead = min(RESPONSE_READ_SIZE, responseBufferLength - responseLength);
        if (sizeToRead <= 0)
            break;

        readFromSocket = recv(sck, responsePtr, sizeToRead, 0);
        if (readFromSocket == SOCKET_ERROR)
        {
            responseLength = -1;
            break;
        }

        responsePtr += readFromSocket;
        responseLength += readFromSocket;
    } while (readFromSocket > 0);

    return responseLength;
}

bool downloadFileFromUrl(const char* url, const char* filePath)
{
    if(!url || !filePath)
    {
        return false;
    }

    u16 shift = 0;
    if(strncasecmp(url, "http://", strlen("http://")) == 0)
    {
        shift = strlen("http://");
    }

    if (strncasecmp(url + shift, "www.", strlen("www.")) == 0)
    {
        shift += strlen("www.");
    }

    char cut[strlen(url) - shift + 1];
    strcpy(cut, url + shift);

    const char* host = strtok(cut, "/");
    const char* resource = url + shift + strlen(host);

    SOCKET sck = connectToHost(host);
    if (!sck)
    {
        logError("Couldn't download file from url %s\n", url);

        return false;
    }

    if (!requestResource(sck, resource, host))
    {
        logError("Couldn't download file from url %s\n", url);

        closesocket(sck);
        WSACleanup();

        return false;
    }

    char* response = (char*)xcalloc(RESPONSE_MAX_SIZE, sizeof(char));
    size32 responseLength = readResponse(sck, response, RESPONSE_MAX_SIZE);

    char* responsePtr = response;

    if (responseLength < 0)
    {
        logError("There was an error trying to receive the response from %s\n", url);
        logError("Last error: %d\n", WSAGetLastError());

        closesocket(sck);
        WSACleanup();

        free(response);

        return false;
    }

    FILE* fd = fopen(filePath, "wb");
    if (!fd)
    {
        logError("There was an error trying to create a new file at: %s\n", filePath);

        closesocket(sck);
        WSACleanup();

        free(response);

        return false;
    }

    if (responseLength == 0)
    {
        logError("The response was empty from %s\n", url);

        closesocket(sck);
        WSACleanup();

        fclose(fd);

        free(response);

        return true;
    }

    SSMapOptions options = (SSMapOptions){0};
    options.defaultValue = NULL;
    options.hashFn = strHashFNV32;
    options.equalsFn = strEquals;
    options.freeFn = strFree;

    SSMap headers;
    SSMapInit(&headers, options);

    size32 readFromResponse = parseHeadersFromResponse(response, responseLength, &headers);

    responsePtr += readFromResponse;
    responseLength -= readFromResponse;

    const char* responseStatus = SSMapGet(&headers, "ResponseStatus");
    if (!strEquals(responseStatus, "200 OK"))
    {
        logError("The response status is not successful, received %s\n", responseStatus);

        SSMapFree(&headers);

        closesocket(sck);
        WSACleanup();

        fclose(fd);

        free(response);

        return false;
    }

    const char* contentType = SSMapGet(&headers, "Content-Type");
    if (!strEquals(contentType, "application/octet-stream"))
    {
        logError("The content type of the response should be binary, received: %s\n", contentType);

        SSMapFree(&headers);

        closesocket(sck);
        WSACleanup();

        fclose(fd);

        free(response);

        return false;
    }

    const char* contentDisposition = SSMapGet(&headers, "Content-Disposition");
    if (!strstr(contentDisposition, "attachment"))
    {
        logError("The content disposition of the response should be 'attachment', received: %s\n", contentDisposition);

        SSMapFree(&headers);

        closesocket(sck);
        WSACleanup();

        fclose(fd);

        free(response);

        return false;
    }

    const char* transferEncoding = SSMapGet(&headers, "Transfer-Encoding");
    if (transferEncoding && strEquals(transferEncoding, "chunked"))
    {
        while (responseLength > 0)
        {
            s32 chunkSize = strtol(responsePtr, NULL, 16);

            // advance the characters of the chunk size and the \r\n
            s32 lengthToSkip = strcspn(responsePtr, "\r");
            responsePtr += lengthToSkip + 2;
            responseLength -= lengthToSkip + 2;

            if (chunkSize == 0)
            {
                break;
            }

            s32 sizeToWrite = min(chunkSize, responseLength);
            fwrite(responsePtr, sizeof(char), sizeToWrite, fd);

            responsePtr += sizeToWrite + 2;
            responseLength -= sizeToWrite + 2;
        }
    }
    else
    {
        // assume Transfer-Encoding: identity
        fwrite(responsePtr, sizeof(char), responseLength, fd);
    }

    SSMapFree(&headers);

    closesocket(sck);
    WSACleanup();

    fclose(fd);

    free(response);

    return true;
}