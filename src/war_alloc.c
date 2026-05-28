#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#define SHL_MZ_IMPLEMENTATION
#ifdef SHL_MZ_DEBUG
#   define SHL_MZ_AUDIT_IMPLEMENTATION
#   include "shl/memzone_audit.h"
#else
#   include "shl/memzone.h"
#endif

#include "war_alloc.h"
#include "war_log.h"

memzone_t* globalZone = NULL;
memzone_t* frameZone = NULL;
memzone_t* audioZone = NULL;

static void zoneReporter(const memzone_t* zone, mz_report_t report, const void* ptr, const char* message, void* userData)
{
    NOT_USED(userData);

    const char* zoneName = "unknown";
    if (zone == globalZone)
        zoneName = "globalZone";
    else if (zone == frameZone)
        zoneName = "frameZone";
    else if (zone == audioZone)
        zoneName = "audioZone";

    const char* reportName = NULL;
    switch (report)
    {
        case MZ_REPORT_ALLOCATION_FAILURE: reportName = "allocation failure"; break;
        case MZ_REPORT_INVALID_FREE: reportName = "invalid free"; break;
        case MZ_REPORT_VALIDATION_FAILURE: reportName = "validation failure"; break;
        default: reportName = "unknown"; break;
    }

    logError("Memory zone report [%s]: %s, %s, for %p.", zoneName, reportName, message, ptr);
}

static bool initZone(size_t size, const char* logFileName, memzone_t** outZone)
{
#ifdef SHL_MZ_DEBUG
    *outZone = mz_initAudit(size, SHL_MZ_AUDIT_FORMAT_COMPACT, logFileName);
#else
    NOT_USED(logFileName);
    *outZone = mz_init(size);
#endif

    return *outZone;
}

static bool isInZone(memzone_t* zone, void* p)
{
    if (!zone || !p) return false;
    uintptr_t ptr = (uintptr_t)p;
    uintptr_t z = (uintptr_t)zone;
    // maxSize is exactly the size of the whole allocated block
    return ptr >= z && ptr < (z + mz_maxSize(zone));
}

static void* allocInZone(memzone_t* zone, size_t sz, const char* file, int line)
{
#ifdef SHL_MZ_DEBUG
    return mz__audit_alloc(zone, sz, file, line);
#else
    NOT_USED(file);
    NOT_USED(line);
    return mz_alloc(zone, sz);
#endif
}

static void* reallocInZone(memzone_t* zone, void* p, size_t sz, const char* file, int line)
{
#ifdef SHL_MZ_DEBUG
    return mz__audit_realloc(zone, p, sz, file, line);
#else
    NOT_USED(file);
    NOT_USED(line);
    return mz_realloc(zone, p, sz);
#endif
}

static void freeInZone(memzone_t* zone, void* p, const char* file, int line)
{
#ifdef SHL_MZ_DEBUG
    mz__audit_free(zone, p, file, line);
#else
    NOT_USED(file);
    NOT_USED(line);
    mz_free(zone, p);
#endif
}

bool wm_allocInit(size_t globalSize, size_t frameSize, size_t audioSize)
{
    if (!initZone(globalSize, "global_zone.log", &globalZone))
    {
        logError("Failed to initialize globalZone.");
        return false;
    }

    if (!initZone(frameSize, "frame_zone.log", &frameZone))
    {
        logError("Failed to initialize frameZone.");
        mz_destroy(globalZone);
        globalZone = NULL;
        return false;
    }

    if (!initZone(audioSize, "audio_zone.log", &audioZone))
    {
        logError("Failed to initialize audioZone.");
        mz_destroy(globalZone);
        mz_destroy(frameZone);
        globalZone = NULL;
        frameZone = NULL;
        return false;
    }

    mz_setReporter(globalZone, zoneReporter, NULL);
    mz_setReporter(frameZone, zoneReporter, NULL);
    mz_setReporter(audioZone, zoneReporter, NULL);
    return true;
}

void wm_allocFree(void)
{
    if (globalZone) mz_destroy(globalZone);
    if (frameZone) mz_destroy(frameZone);
    if (audioZone) mz_destroy(audioZone);
    globalZone = NULL;
    frameZone = NULL;
    audioZone = NULL;
}

void* wm__alloc(size_t sz, const char* file, int line)
{
    return allocInZone(globalZone, sz, file, line);
}

void* wm__allocFrame(size_t sz, const char* file, int line)
{
    return allocInZone(frameZone, sz, file, line);
}

void* wm__allocAudio(size_t sz, const char* file, int line)
{
    return allocInZone(audioZone, sz, file, line);
}

void* wm__realloc(void* p, size_t sz, const char* file, int line)
{
    if (!p) return wm__alloc(sz, file, line);
    if (sz == 0)
    {
        wm__free(p, file, line);
        return NULL;
    }

    if (isInZone(globalZone, p))
    {
        return reallocInZone(globalZone, p, sz, file, line);
    }

    if (isInZone(frameZone, p))
    {
        return reallocInZone(frameZone, p, sz, file, line);
    }

    // fallback, if it's external or stdlib allocated
    NOT_USED(file);
    NOT_USED(line);
    return realloc(p, sz);
}

void* wm__reallocAudio(void* p, size_t sz, const char* file, int line)
{
    if (!p) return wm__allocAudio(sz, file, line);
    if (sz == 0)
    {
        wm__free(p, file, line);
        return NULL;
    }

    return reallocInZone(audioZone, p, sz, file, line);
}

void wm__free(void* p, const char* file, int line)
{
    if (!p) return;

    if (isInZone(globalZone, p))
    {
        freeInZone(globalZone, p, file, line);
    }
    else if (isInZone(frameZone, p))
    {
        freeInZone(frameZone, p, file, line);
    }
    else if (isInZone(audioZone, p))
    {
        freeInZone(audioZone, p, file, line);
    }
    else
    {
        NOT_USED(file);
        NOT_USED(line);
        free(p); // fallback
    }
}
