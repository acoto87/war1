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

void initLog(LogSeverity severity)
{
    __log__ = (Log){0};
    __log__.severity = severity;
}

int32_t __getFileNameIndex(const char* file)
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
    if (severity <= __log__.severity)
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

        int32_t fileNameIndex = __getFileNameIndex(file);
        fprintf(stdout, "[%s:(%d)][%s][%s]: ", file + fileNameIndex + 1, line, tstr, severityStr);

        va_list args;
        va_start(args, message);
        vprintf(message, args);
        va_end(args);
    }
}

#define log(severity, message, ...) __logInternal(severity, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define logCritical(message, ...) __logInternal(LOG_SEVERITY_CRITICAL, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define logError(message, ...) __logInternal(LOG_SEVERITY_ERROR, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define logWarning(message, ...) __logInternal(LOG_SEVERITY_WARNING, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define logInfo(message, ...) __logInternal(LOG_SEVERITY_INFO, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define logDebug(message, ...) __logInternal(LOG_SEVERITY_DEBUG, __FILE__, __LINE__, message, ##__VA_ARGS__)
