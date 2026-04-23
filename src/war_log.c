#include "war_log.h"

#include <stdarg.h>
#include <stdio.h>

void wlog_log(SDL_LogPriority priority, const char* file, int line, const char* fmt, ...)
{
    // Find short filename by stripping directory prefix
    const char* shortFile = file;
    for (const char* p = file; *p; p++)
        if (*p == '/' || *p == '\\') shortFile = p + 1;

    // Format the message into a local buffer
    char buf[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    // Strip trailing newlines — SDL_LogMessage appends its own
    int len = (int)strlen(buf);
    while (len > 0 && buf[len - 1] == '\n') buf[--len] = '\0';

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, priority, "[%s:(%d)]: %s", shortFile, line, buf);
}
