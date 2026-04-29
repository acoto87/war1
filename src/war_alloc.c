#include "war_alloc.h"

#include <assert.h>

#define SHL_MZ_IMPLEMENTATION
#ifdef SHL_MZ_DEBUG
#   define SHL_MZ_AUDIT_IMPLEMENTATION
#   include "shl/memzone_audit.h"
#else
#   include "shl/memzone.h"
#endif

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

static bool is_in_zone(memzone_t* zone, void* p)
{
    if (!zone || !p) return false;
    uintptr_t ptr = (uintptr_t)p;
    uintptr_t z = (uintptr_t)zone;
    // maxSize is exactly the size of the whole allocated block
    return ptr >= z && ptr < (z + mz_maxSize(zone));
}

bool war_alloc_init(size_t permSize, size_t frameSize)
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

void war_alloc_free(void)
{
    if (permanentZone) mz_destroy(permanentZone);
    if (frameZone) mz_destroy(frameZone);
    permanentZone = NULL;
    frameZone = NULL;
    currentZone = NULL;
}

void war_set_zone(memzone_t* zone)
{
    currentZone = zone;
}

void war_reset_zone(void)
{
    currentZone = permanentZone;
}

void* war_malloc(size_t sz)
{
    return mz_alloc(currentZone, sz);
}

void* war_malloc_frame(size_t sz)
{
    return mz_alloc(frameZone, sz);
}

void* war_calloc(size_t n, size_t sz)
{
    // Guard against n*sz wrapping to a smaller value on overflow.
    if (n != 0 && sz > (size_t)-1 / n)
        return NULL;
    // NOTE: mz_alloc already returns zeroed memory
    return mz_alloc(currentZone, n * sz);
}

void* war_realloc(void* p, size_t sz)
{
    if (!p) return war_malloc(sz);
    if (sz == 0)
    {
        war_free(p);
        return NULL;
    }

    if (is_in_zone(permanentZone, p))
    {
        return mz_realloc(permanentZone, p, sz);
    }

    if (is_in_zone(frameZone, p))
    {
        return mz_realloc(frameZone, p, sz);
    }

    // fallback, if it's external or stdlib allocated
    return realloc(p, sz);
}

void war_free(void* p)
{
    if (!p) return;

    if (is_in_zone(permanentZone, p))
    {
        mz_free(permanentZone, p);
    }
    else if (is_in_zone(frameZone, p))
    {
        mz_free(frameZone, p);
    }
    else
    {
        free(p); // fallback
    }
}
