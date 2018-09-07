#pragma once

#include <stdio.h>
#include <time.h>

typedef enum
{
    LOG_SEVERITY_CRITICAL,
    LOG_SEVERITY_ERROR,
    LOG_SEVERITY_WARNING,
    LOG_SEVERITY_INFO,
    LOG_SEVERITY_DEBUG
} LogSeverity;

typedef struct
{
    LogSeverity severity;
} Log;

static Log __log;

inline void initLog(LogSeverity severity)
{
    __log = (Log){0};
    __log.severity = severity;
}

static void __logInternal(LogSeverity severity, const char* message, ...)
{
    if (severity < __log.severity)
    {
        time_t t = time(NULL);
        struct tm* timeInfo = localtime(&t);

        char tstr[80];
        strftime(tstr, 80, "%X", timeInfo);

        char* severityStr;
        
        switch (severity)
        {
            case LOG_SEVERITY_CRITICAL:     severityStr = "CRITICAL";   break;
            case LOG_SEVERITY_ERROR:        severityStr = "ERROR";      break;
            case LOG_SEVERITY_WARNING:      severityStr = "WARNING";    break;
            case LOG_SEVERITY_INFO:         severityStr = "INFO";       break;
            case LOG_SEVERITY_DEBUG:        severityStr = "DEBUG";      break;
            default:                        severityStr = "UNKOWN";     break;
        }

        printf("[%s][%s]: ", tstr, severityStr);

        va_list args;
        va_start(args, message);
        vprintf(message, args);
        va_end(args);

        printf("\n");
    }
}

#define logCritical(message, ...) __logInternal(LOG_SEVERITY_CRITICAL, message, __VA_ARGS__)
#define logError(message, ...) __logInternal(LOG_SEVERITY_ERROR, message, __VA_ARGS__)
#define logWarning(message, ...) __logInternal(LOG_SEVERITY_WARNING, message, __VA_ARGS__)
#define logInfo(message, ...) __logInternal(LOG_SEVERITY_INFO, message, __VA_ARGS__)
#define logDebug(message, ...) __logInternal(LOG_SEVERITY_DEBUG, message, __VA_ARGS__)
