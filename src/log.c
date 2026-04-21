#include "SDL3/SDL_mutex.h"
#include "log.h"

static Log __log__;
static SDL_Mutex* __logMutex__;

static const char* __getFileName(const char* file)
{
    int32_t index = 0;

    size_t len = strlen(file);
    for (int32_t i = len - 1; i >= 0; i--)
    {
        if (file[i] == '/' || file[i] == '\\')
        {
            index = i;
            break;
        }
    }

    return file + index + 1;
}

void initLog(LogSeverity severity)
{
    __log__ = (Log){0};
    __log__.severity = severity;
    __logMutex__ = SDL_CreateMutex();
}

void __log(LogSeverity severity, const char* file, int32_t line, const char* message, ...)
{
    if (severity > __log__.severity)
    {
        return;
    }

    SDL_LockMutex(__logMutex__);

    time_t t = time(NULL);
    struct tm* timeInfo = localtime(&t);

    char tstr[80];
    strftime(tstr, 80, "%X", timeInfo);

    char* severityStr;

    switch (severity)
    {
        case LOG_SEVERITY_CRITICAL: severityStr = "CRIT";   break;
        case LOG_SEVERITY_ERROR:    severityStr = "ERR";      break;
        case LOG_SEVERITY_WARNING:  severityStr = "WRN";    break;
        case LOG_SEVERITY_INFO:     severityStr = "INF";       break;
        case LOG_SEVERITY_DEBUG:    severityStr = "DBG";      break;
        default:                    severityStr = "UNK";     break;
    }

    const char* fileName = file ? __getFileName(file) : "unknown";
    fprintf(stdout, "[%s][%s][%s:(%d)]: ", severityStr, tstr, fileName, line);

    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);

    SDL_UnlockMutex(__logMutex__);
}
