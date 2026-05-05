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

memzone_t* permanentZone = NULL;
memzone_t* frameZone = NULL;
memzone_t* currentZone = NULL;

static void zoneReporter(const memzone_t* zone, mz_report_t report, const void* ptr, const char* message, void* userData)
{
    NOT_USED(zone);
    NOT_USED(report);
    NOT_USED(ptr);
    NOT_USED(userData);

    logError("memzone: %s", message);
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

bool wm_allocInit(size_t permSize, size_t frameSize)
{
#ifdef SHL_MZ_DEBUG
    permanentZone = mz_initAudit(permSize, SHL_MZ_AUDIT_FORMAT_COMPACT, "permanent_zone.log");
#else
    permanentZone = mz_init(permSize);
#endif

    if (!permanentZone)
    {
        logError("Failed to initialize permanentZone.");
        return false;
    }

#ifdef SHL_MZ_DEBUG
    frameZone = mz_initAudit(frameSize, SHL_MZ_AUDIT_FORMAT_COMPACT, "frame_zone.log");
#else
    frameZone = mz_init(frameSize);
#endif

    if (!frameZone)
    {
        logError("Failed to initialize frameZone.");
        mz_destroy(permanentZone);
        permanentZone = NULL;
        return false;
    }

    currentZone = permanentZone;

    mz_setReporter(permanentZone, zoneReporter, NULL);
    mz_setReporter(frameZone, zoneReporter, NULL);
    return true;
}

void wm_allocFree(void)
{
    if (permanentZone) mz_destroy(permanentZone);
    if (frameZone) mz_destroy(frameZone);
    permanentZone = NULL;
    frameZone = NULL;
    currentZone = NULL;
}

void wm_setZone(memzone_t* zone)
{
    currentZone = zone;
}

void wm_resetZone(void)
{
    currentZone = permanentZone;
}

void* wm__alloc(size_t sz, const char* file, int line)
{
    return allocInZone(currentZone, sz, file, line);
}

void* wm__allocFrame(size_t sz, const char* file, int line)
{
    return allocInZone(frameZone, sz, file, line);
}

void* wm__calloc(size_t n, size_t sz, const char* file, int line)
{
    // Guard against n*sz wrapping to a smaller value on overflow.
    if (n != 0 && sz > (size_t)-1 / n) return NULL;
    // NOTE: mz_alloc already returns zeroed memory
    return allocInZone(currentZone, n * sz, file, line);
}

void* wm__realloc(void* p, size_t sz, const char* file, int line)
{
    if (!p) return wm__alloc(sz, file, line);
    if (sz == 0)
    {
        wm__free(p, file, line);
        return NULL;
    }

    if (isInZone(permanentZone, p))
    {
        return reallocInZone(permanentZone, p, sz, file, line);
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

void wm__free(void* p, const char* file, int line)
{
    if (!p) return;

    if (isInZone(permanentZone, p))
    {
        freeInZone(permanentZone, p, file, line);
    }
    else if (isInZone(frameZone, p))
    {
        freeInZone(frameZone, p, file, line);
    }
    else
    {
        NOT_USED(file);
        NOT_USED(line);
        free(p); // fallback
    }
}
