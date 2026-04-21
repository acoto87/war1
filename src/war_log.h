#pragma once

#include "SDL3/SDL.h"

void wlog_log(SDL_LogPriority priority, const char* file, int line, const char* fmt, ...);

#define logCritical(...) wlog_log(SDL_LOG_PRIORITY_CRITICAL, __FILE__, __LINE__, __VA_ARGS__)
#define logError(...)    wlog_log(SDL_LOG_PRIORITY_ERROR,    __FILE__, __LINE__, __VA_ARGS__)
#define logWarning(...)  wlog_log(SDL_LOG_PRIORITY_WARN,     __FILE__, __LINE__, __VA_ARGS__)
#define logInfo(...)     wlog_log(SDL_LOG_PRIORITY_INFO,     __FILE__, __LINE__, __VA_ARGS__)
#define logDebug(...)    wlog_log(SDL_LOG_PRIORITY_DEBUG,    __FILE__, __LINE__, __VA_ARGS__)
