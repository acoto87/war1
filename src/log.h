#pragma once

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

Log __log__;
pthread_mutex_t __logMutex__;

void initLog(LogSeverity severity)
{
    __log__ = (Log){0};
    __log__.severity = severity;

    pthread_mutex_init(&__logMutex__, NULL);
}

static int32_t __getFileNameIndex(const char* file)
{
    int32_t index = 0;

    size_t len = strlen(file);
    for (int32_t i = 1; i < len; i++)
    {
        if (file[i] == '/' || file[i] == '\\')
            index = i;
    }

    return index;
}

void __logInternal(LogSeverity severity, const char* file, const int32_t line, const char* message, ...)
{
    if (severity > __log__.severity)
    {
        return;
    }

    pthread_mutex_lock(&__logMutex__);

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

    int32_t fileNameIndex = __getFileNameIndex(file);
    fprintf(stdout, "[%s][%s][%s:(%d)]: ", severityStr, tstr, file + fileNameIndex + 1, line);

    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);

    pthread_mutex_unlock(&__logMutex__);
}

#define log(severity, message, ...) __logInternal(severity, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define logCritical(message, ...) __logInternal(LOG_SEVERITY_CRITICAL, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define logError(message, ...) __logInternal(LOG_SEVERITY_ERROR, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define logWarning(message, ...) __logInternal(LOG_SEVERITY_WARNING, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define logInfo(message, ...) __logInternal(LOG_SEVERITY_INFO, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define logDebug(message, ...) __logInternal(LOG_SEVERITY_DEBUG, __FILE__, __LINE__, message, ##__VA_ARGS__)
