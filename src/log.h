#pragma once

#include <stdint.h>

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

void initLog(LogSeverity severity);
void __log(LogSeverity severity, const char* file, int32_t line, const char* message, ...);

#define logCritical(message, ...) __log(LOG_SEVERITY_CRITICAL, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define logError(message, ...) __log(LOG_SEVERITY_ERROR, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define logWarning(message, ...) __log(LOG_SEVERITY_WARNING, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define logInfo(message, ...) __log(LOG_SEVERITY_INFO, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define logDebug(message, ...) __log(LOG_SEVERITY_DEBUG, __FILE__, __LINE__, message, ##__VA_ARGS__)
