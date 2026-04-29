/*
    memzone_audit.h - companion header for memzone.h

    MIT License

    Copyright (c) 2018 Alejandro Coto Gutiérrez

    Provides per-zone audit logging for memzone_t: every alloc, free, realloc,
    reset and destroy is recorded to a structured text file with a timestamp,
    call-site (file + line), the result, and a snapshot of zone state.

    USAGE
    In exactly one translation unit define both implementation guards before
    including this header:

        #define SHL_MZ_IMPLEMENTATION
        #define SHL_MZ_AUDIT_IMPLEMENTATION
        #include "memzone_audit.h"

    In all other translation units include without the guards (or include
    memzone.h directly if audit is not needed in that TU):

        #include "memzone_audit.h"

    This header includes memzone.h internally; you do not need to include it
    separately.

    PUBLIC API
    Once this header is included, the following new symbols become available:

        mz_audit_format_t               — SHL_MZ_AUDIT_FORMAT_VERBOSE or _COMPACT
        mz_initAudit(size, fmt, path)   — create zone and begin logging immediately
        mz_auditConfigure(z, fmt, path) — attach / reconfigure audit on existing zone
        mz_auditFlush(z)                — flush without closing the log
        mz_auditClose(z)                — flush and close the log

    The standard mutating API (mz_init, mz_destroy, mz_alloc, mz_free,
    mz_realloc, mz_allocAligned, mz_reset) is transparently redirected through
    the audit layer via macros so every call site's file and line number are
    captured without any source changes.

    OPTIONAL COMPILE-TIME KNOBS
    SHL_MZ_AUDIT_VERBOSE  — emit a block-list dump after each logged event
    SHL_MZ_AUDIT_APPEND   — open log files in append mode (default: truncate)
    SHL_MZ_AUDIT_FILE     — default log path (default: "memzone_audit.log")
    SHL_MZ_AUDIT_MAX_ZONES — max concurrent audited zones (default: 64)

    DESIGN NOTES
    Audit state is kept in a fixed external table (mz__audit_table) rather than
    embedded in memzone_t, so memzone.h itself remains unmodified and audit-free.
    SHL_MZ_AUDIT_IMPLEMENTATION must be compiled in the same translation unit as
    SHL_MZ_IMPLEMENTATION because the realloc wrapper inspects private internals
    (memblock_t, mz__findBlock, mz__allocationSize, etc.) that are only visible
    there.
*/

#ifndef SHL_MZ_AUDIT_H
#define SHL_MZ_AUDIT_H

#include "memzone.h"

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
   Format selector
   ---------------------------------------------------------------------- */

typedef enum
{
    SHL_MZ_AUDIT_FORMAT_VERBOSE,
    SHL_MZ_AUDIT_FORMAT_COMPACT
} mz_audit_format_t;

/* -------------------------------------------------------------------------
   Public control functions (not intercepted by macros)
   ---------------------------------------------------------------------- */

/*
 * mz_auditConfigure — attach or reconfigure audit logging on an existing zone.
 * Any previously open log file is closed first.  No INIT event is logged.
 * filepath=NULL uses the SHL_MZ_AUDIT_FILE default.
 */
void mz_auditConfigure(memzone_t* zone, mz_audit_format_t format, const char* filepath);

/* Flush the zone's log without closing it. */
void mz_auditFlush(memzone_t* zone);

/* Flush and close the zone's log.  Safe to call on a zone with no active log. */
void mz_auditClose(memzone_t* zone);

/* -------------------------------------------------------------------------
   Internal wrappers — invoked through the macros below; do not call directly
   ---------------------------------------------------------------------- */

memzone_t* mz__audit_init(size_t maxSize, const char* file, int line);
memzone_t* mz__audit_initWithLog(size_t maxSize, mz_audit_format_t format,
                                  const char* filepath,
                                  const char* file, int line);
void       mz__audit_destroy(memzone_t* zone, const char* file, int line);
void       mz__audit_reset(memzone_t* zone, const char* file, int line);
void*      mz__audit_alloc(memzone_t* zone, size_t size,
                            const char* file, int line);
void*      mz__audit_allocAligned(memzone_t* zone, size_t size, size_t alignment,
                                   const char* file, int line);
void       mz__audit_free(memzone_t* zone, void* p,
                           const char* file, int line);
void*      mz__audit_realloc(memzone_t* zone, void* p, size_t size,
                              const char* file, int line);

#ifdef __cplusplus
}
#endif

/* -------------------------------------------------------------------------
   Macro redirections — must be outside extern "C"
   Intercept every mutating public API call to capture file + line.
   ---------------------------------------------------------------------- */

#define mz_initAudit(sz, fmt, path) \
    mz__audit_initWithLog((sz), (fmt), (path), __FILE__, __LINE__)
#define mz_init(s)               mz__audit_init((s),               __FILE__, __LINE__)
#define mz_destroy(z)            mz__audit_destroy((z),            __FILE__, __LINE__)
#define mz_reset(z)              mz__audit_reset((z),              __FILE__, __LINE__)
#define mz_alloc(z, s)           mz__audit_alloc((z), (s),         __FILE__, __LINE__)
#define mz_allocAligned(z, s, a) mz__audit_allocAligned((z), (s), (a), __FILE__, __LINE__)
#define mz_free(z, p)            mz__audit_free((z), (p),          __FILE__, __LINE__)
#define mz_realloc(z, p, s)      mz__audit_realloc((z), (p), (s),  __FILE__, __LINE__)

/* =========================================================================
   Implementation — compiled only when SHL_MZ_AUDIT_IMPLEMENTATION is defined
   ========================================================================= */

#ifdef SHL_MZ_AUDIT_IMPLEMENTATION

#ifndef SHL_MZ_IMPLEMENTATION
#  error "SHL_MZ_AUDIT_IMPLEMENTATION requires SHL_MZ_IMPLEMENTATION in the same translation unit"
#endif

#include <time.h>

#ifndef SHL_MZ_AUDIT_FILE
#  define SHL_MZ_AUDIT_FILE "memzone_audit.log"
#endif

#ifndef SHL_MZ_AUDIT_MAX_ZONES
#  define SHL_MZ_AUDIT_MAX_ZONES 64
#endif

/* -------------------------------------------------------------------------
   External state table — no fields added to memzone_t
   ---------------------------------------------------------------------- */

typedef struct
{
    memzone_t*        zone;
    FILE*             fp;
    uint32_t          seq;
    mz_audit_format_t fmt;
    char              filepath[512];
} mz__audit_state_t;

static mz__audit_state_t mz__audit_table[SHL_MZ_AUDIT_MAX_ZONES];

static mz__audit_state_t* mz__audit_find(const memzone_t* zone)
{
    int i;
    for (i = 0; i < SHL_MZ_AUDIT_MAX_ZONES; i++)
        if (mz__audit_table[i].zone == zone)
            return &mz__audit_table[i];
    return NULL;
}

/* Acquire a slot for zone (reusing an existing slot if already registered). */
static mz__audit_state_t* mz__audit_acquire(memzone_t* zone, mz_audit_format_t fmt,
                                              const char* filepath)
{
    mz__audit_state_t* s = mz__audit_find(zone);
    if (s != NULL)
    {
        /* Close any open log before reconfiguring. */
        if (s->fp != NULL) { fflush(s->fp); fclose(s->fp); s->fp = NULL; }
    }
    else
    {
        int i;
        for (i = 0; i < SHL_MZ_AUDIT_MAX_ZONES; i++)
        {
            if (mz__audit_table[i].zone == NULL)
            {
                s = &mz__audit_table[i];
                break;
            }
        }
    }
    if (s == NULL) { return NULL; } /* table full */

    {
        const char* path = (filepath != NULL) ? filepath : SHL_MZ_AUDIT_FILE;
        size_t len       = strlen(path);
        size_t maxLen    = sizeof(s->filepath) - 1;
        if (len > maxLen) { len = maxLen; }
        memcpy(s->filepath, path, len);
        s->filepath[len] = '\0';
    }

    s->zone = zone;
    s->seq  = 0;
    s->fmt  = fmt;
    s->fp   = NULL;
    return s;
}

static void mz__audit_release(mz__audit_state_t* s)
{
    if (s == NULL) { return; }
    if (s->fp != NULL) { fflush(s->fp); fclose(s->fp); s->fp = NULL; }
    s->zone = NULL;
}

/* -------------------------------------------------------------------------
   Undef macro redirections so the wrapper functions below can call the real
   base API without recursing back through the audit layer.
   ---------------------------------------------------------------------- */
#undef mz_initAudit
#undef mz_init
#undef mz_destroy
#undef mz_reset
#undef mz_alloc
#undef mz_allocAligned
#undef mz_free
#undef mz_realloc

/* -------------------------------------------------------------------------
   Helpers
   ---------------------------------------------------------------------- */

static void mz__audit_timestamp(char* buf, size_t sz)
{
#if defined(_WIN32) || __STDC_VERSION__ >= 201112L
    struct timespec ts;
    if (timespec_get(&ts, TIME_UTC) != 0)
    {
        time_t sec = ts.tv_sec;
        struct tm* tm_info = localtime(&sec);
        if (tm_info != NULL)
        {
            char base[32];
            strftime(base, sizeof(base), "%Y-%m-%d %H:%M:%S", tm_info);
            snprintf(buf, sz, "%s.%03d", base, (int)(ts.tv_nsec / 1000000));
            return;
        }
    }
#endif
    {
        time_t t = time(NULL);
        struct tm* tm_info = localtime(&t);
        if (tm_info != NULL)
            strftime(buf, sz, "%Y-%m-%d %H:%M:%S.000", tm_info);
        else
            snprintf(buf, sz, "0000-00-00 00:00:00.000");
    }
}

/* Opens the log file for state s and writes the file header.
   Does nothing if the file is already open.                    */
static void mz__audit_open(mz__audit_state_t* s)
{
    if (s == NULL || s->fp != NULL) { return; }

#ifdef SHL_MZ_AUDIT_APPEND
    s->fp = fopen(s->filepath, "a");
#else
    s->fp = fopen(s->filepath, "w");
#endif

    if (s->fp == NULL)
    {
        fprintf(stderr, "memzone audit: could not open '%s' for writing\n", s->filepath);
        return;
    }

    {
        char ts[32];
        mz__audit_timestamp(ts, sizeof(ts));
        fprintf(s->fp,
            "# memzone audit log\n"
            "# started  : %s\n"
            "# format   : %s\n"
            "# mode     : %s\n"
            "\n",
            ts,
            s->fmt == SHL_MZ_AUDIT_FORMAT_COMPACT ? "compact" : "verbose",
#ifdef SHL_MZ_AUDIT_APPEND
            "append"
#else
            "truncate"
#endif
        );
    }
    fflush(s->fp);
}

static void mz__audit_zone_state(const memzone_t* zone, char* buf, size_t sz)
{
    if (zone == NULL)
    {
        snprintf(buf, sz, "(null zone)");
        return;
    }
    {
        int32_t blocks  = mz_blockCount(zone);
        float   frag    = mz_fragmentation(zone);
        size_t  used    = mz_usedSize(zone);
        size_t  max     = mz_maxSize(zone);
        float   pctFull = max > 0 ? ((float)used / (float)max) * 100.0f : 0.0f;
        snprintf(buf, sz,
            "used=%llu, max=%llu (%.2f%% full), blocks=%d, frag=%.2f%%, free=%llu bytes",
            (unsigned long long)used,
            (unsigned long long)max,
            pctFull,
            blocks,
            frag,
            (unsigned long long)(max - used));
    }
}

/* Counts live (non-free) blocks.  Requires private struct visibility. */
static int32_t mz__audit_live_count(const memzone_t* zone)
{
    int32_t count = 0;
    const memblock_t* rover;
    if (zone == NULL) { return 0; }
    rover = &zone->blockList;
    do
    {
        if (!MZ__IS_BLOCK_EMPTY(rover)) { count++; }
        rover = rover->next;
    } while (rover != &zone->blockList);
    return count;
}

#ifdef SHL_MZ_AUDIT_VERBOSE
static void mz__audit_block_list(FILE* fp, const memzone_t* zone)
{
    int32_t idx = 0;
    const memblock_t* rover;
    if (fp == NULL || zone == NULL) { return; }
    fprintf(fp, "  Block list :\n");
    rover = &zone->blockList;
    do
    {
        if (!MZ__IS_BLOCK_EMPTY(rover))
            fprintf(fp,
                "    [%03d] block=%p  size=%-8llu  user=%p  next=%p  prev=%p\n",
                idx,
                (const void*)rover,
                (unsigned long long)rover->size,
                (void*)rover->user,
                (const void*)rover->next,
                (const void*)rover->prev);
        else
            fprintf(fp,
                "    [%03d] block=%p  size=%-8llu  FREE  next=%p  prev=%p\n",
                idx,
                (const void*)rover,
                (unsigned long long)rover->size,
                (const void*)rover->next,
                (const void*)rover->prev);
        idx++;
        rover = rover->next;
    } while (rover != &zone->blockList);
}
#else
#  define mz__audit_block_list(fp, zone) ((void)0)
#endif /* SHL_MZ_AUDIT_VERBOSE */

/* -------------------------------------------------------------------------
   Verbose-format event helpers
   ---------------------------------------------------------------------- */

static void mz__audit_verbose_sep(FILE* fp)
{
    fprintf(fp,
        "================================================================================\n");
}

static void mz__audit_verbose_div(FILE* fp)
{
    fprintf(fp,
        "--------------------------------------------------------------------------------\n");
}

static void mz__audit_verbose_open(FILE* fp, uint32_t seq, const char* op,
                                    const char* ts, const char* file, int line)
{
    mz__audit_verbose_sep(fp);
    fprintf(fp, "EVENT #%04u  %-16s %s  %s:%d\n", seq, op, ts, file, line);
    mz__audit_verbose_div(fp);
}

static void mz__audit_verbose_close(FILE* fp, const memzone_t* zone)
{
    if (zone != NULL)
    {
        char state[256];
        mz__audit_zone_state(zone, state, sizeof(state));
        fprintf(fp, "  Zone after : %s\n", state);
        mz__audit_block_list(fp, zone);
    }
    mz__audit_verbose_sep(fp);
    fputc('\n', fp);
}

/* -------------------------------------------------------------------------
   Wrapper functions
   ---------------------------------------------------------------------- */

memzone_t* mz__audit_init(size_t maxSize, const char* file, int line)
{
    (void)file; (void)line;
    return mz_init(maxSize);
}

memzone_t* mz__audit_initWithLog(size_t maxSize, mz_audit_format_t format,
                                  const char* filepath,
                                  const char* file, int line)
{
    memzone_t* zone = mz_init(maxSize);
    if (zone == NULL) { return NULL; }

    {
        mz__audit_state_t* s = mz__audit_acquire(zone, format, filepath);
        if (s == NULL) { return zone; }

        mz__audit_open(s);
        if (s->fp == NULL) { return zone; }

        {
            char ts[32];
            mz__audit_timestamp(ts, sizeof(ts));
            s->seq++;

            if (s->fmt == SHL_MZ_AUDIT_FORMAT_COMPACT)
            {
                char state[256] = "";
                mz__audit_zone_state(zone, state, sizeof(state));
                fprintf(s->fp,
                    "#%04u INIT     ts=%s site=%s:%d zone=%p maxSize=%llu result=OK %s\n",
                    s->seq, ts, file, line,
                    (void*)zone,
                    (unsigned long long)maxSize,
                    state);
            }
            else
            {
                mz__audit_verbose_open(s->fp, s->seq, "INIT", ts, file, line);
                fprintf(s->fp, "  Zone       : %p\n", (void*)zone);
                fprintf(s->fp, "  Max size   : %llu bytes\n", (unsigned long long)maxSize);
                fprintf(s->fp, "  Result     : [OK]\n");
                mz__audit_verbose_close(s->fp, zone);
            }
            fflush(s->fp);
        }
    }
    return zone;
}

void mz__audit_destroy(memzone_t* zone, const char* file, int line)
{
    mz__audit_state_t* s = mz__audit_find(zone);
    if (s != NULL && s->fp != NULL)
    {
        FILE*             fp         = s->fp;
        mz_audit_format_t fmt        = s->fmt;
        uint32_t          seq        = ++s->seq;
        int32_t           liveAllocs = mz__audit_live_count(zone);
        char              ts[32];

        mz__audit_timestamp(ts, sizeof(ts));

        if (fmt == SHL_MZ_AUDIT_FORMAT_COMPACT)
        {
            char state[256] = "";
            mz__audit_zone_state(zone, state, sizeof(state));
            fprintf(fp,
                "#%04u DESTROY  ts=%s site=%s:%d zone=%p %s live_allocs=%d\n",
                seq, ts, file, line,
                (void*)zone,
                state,
                liveAllocs);
        }
        else
        {
            char state[256];
            mz__audit_verbose_open(fp, seq, "DESTROY", ts, file, line);
            fprintf(fp, "  Zone       : %p\n", (void*)zone);
            mz__audit_zone_state(zone, state, sizeof(state));
            fprintf(fp, "  Zone final : %s\n", state);
            fprintf(fp, "  Live allocs: %d\n", liveAllocs);
            mz__audit_block_list(fp, zone);
            mz__audit_verbose_sep(fp);
            fputc('\n', fp);
        }
        fflush(fp);
        fclose(fp);
        s->fp = NULL;
    }
    if (s != NULL) { mz__audit_release(s); }
    mz_destroy(zone);
}

void mz__audit_reset(memzone_t* zone, const char* file, int line)
{
    mz__audit_state_t* s = mz__audit_find(zone);
    if (s == NULL || s->fp == NULL)
    {
        mz_reset(zone);
        return;
    }

    {
        char ts[32];
        char stateBefore[256] = "(null zone)";

        mz__audit_timestamp(ts, sizeof(ts));
        s->seq++;
        mz__audit_zone_state(zone, stateBefore, sizeof(stateBefore));
        mz_reset(zone);

        if (s->fmt == SHL_MZ_AUDIT_FORMAT_COMPACT)
        {
            char stateAfter[256] = "(null zone)";
            mz__audit_zone_state(zone, stateAfter, sizeof(stateAfter));
            fprintf(s->fp,
                "#%04u RESET    ts=%s site=%s:%d zone=%p before=[%s] after=[%s]\n",
                s->seq, ts, file, line,
                (void*)zone, stateBefore, stateAfter);
        }
        else
        {
            mz__audit_verbose_open(s->fp, s->seq, "RESET", ts, file, line);
            fprintf(s->fp, "  Zone       : %p\n", (void*)zone);
            fprintf(s->fp, "  Zone before: %s\n", stateBefore);
            mz__audit_verbose_close(s->fp, zone);
        }
        fflush(s->fp);
    }
}

void* mz__audit_alloc(memzone_t* zone, size_t size, const char* file, int line)
{
    void* result = mz_alloc(zone, size);

    {
        mz__audit_state_t* s = mz__audit_find(zone);
        if (s != NULL && s->fp != NULL)
        {
            char   ts[32];
            size_t alignment = mz_alignment();

            mz__audit_timestamp(ts, sizeof(ts));
            s->seq++;

            if (s->fmt == SHL_MZ_AUDIT_FORMAT_COMPACT)
            {
                char state[256] = "";
                mz__audit_zone_state(zone, state, sizeof(state));
                fprintf(s->fp,
                    "#%04u ALLOC    ts=%s site=%s:%d zone=%p size=%llu align=%llu ptr=%p result=%s %s\n",
                    s->seq, ts, file, line,
                    (void*)zone,
                    (unsigned long long)size,
                    (unsigned long long)alignment,
                    result,
                    result != NULL ? "OK" : "FAIL",
                    state);
            }
            else
            {
                mz__audit_verbose_open(s->fp, s->seq, "ALLOC", ts, file, line);
                fprintf(s->fp, "  Zone       : %p\n", (void*)zone);
                fprintf(s->fp, "  Request    : size=%llu bytes, alignment=%llu bytes\n",
                    (unsigned long long)size, (unsigned long long)alignment);
                if (result != NULL)
                    fprintf(s->fp, "  Result     : ptr=%p [OK]\n", result);
                else
                    fprintf(s->fp, "  Result     : ptr=NULL [FAIL]\n");
                mz__audit_verbose_close(s->fp, zone);
            }
            fflush(s->fp);
        }
    }
    return result;
}

void* mz__audit_allocAligned(memzone_t* zone, size_t size, size_t alignment,
                               const char* file, int line)
{
    void* result = mz_allocAligned(zone, size, alignment);

    {
        mz__audit_state_t* s = mz__audit_find(zone);
        if (s != NULL && s->fp != NULL)
        {
            char ts[32];
            mz__audit_timestamp(ts, sizeof(ts));
            s->seq++;

            if (s->fmt == SHL_MZ_AUDIT_FORMAT_COMPACT)
            {
                char state[256] = "";
                mz__audit_zone_state(zone, state, sizeof(state));
                fprintf(s->fp,
                    "#%04u ALLOC_ALIGNED ts=%s site=%s:%d zone=%p size=%llu align=%llu ptr=%p result=%s %s\n",
                    s->seq, ts, file, line,
                    (void*)zone,
                    (unsigned long long)size,
                    (unsigned long long)alignment,
                    result,
                    result != NULL ? "OK" : "FAIL",
                    state);
            }
            else
            {
                mz__audit_verbose_open(s->fp, s->seq, "ALLOC_ALIGNED", ts, file, line);
                fprintf(s->fp, "  Zone       : %p\n", (void*)zone);
                fprintf(s->fp, "  Request    : size=%llu bytes, alignment=%llu bytes\n",
                    (unsigned long long)size, (unsigned long long)alignment);
                if (result != NULL)
                    fprintf(s->fp, "  Result     : ptr=%p [OK]\n", result);
                else
                    fprintf(s->fp, "  Result     : ptr=NULL [FAIL]\n");
                mz__audit_verbose_close(s->fp, zone);
            }
            fflush(s->fp);
        }
    }
    return result;
}

void mz__audit_free(memzone_t* zone, void* p, const char* file, int line)
{
    size_t allocSz  = 0;
    bool   validPtr = false;

    if (zone != NULL && p != NULL)
    {
        allocSz  = mz_allocationSize(zone, p);
        validPtr = mz_contains(zone, p);
    }

    mz_free(zone, p);

    {
        mz__audit_state_t* s = mz__audit_find(zone);
        if (s != NULL && s->fp != NULL)
        {
            char ts[32];
            mz__audit_timestamp(ts, sizeof(ts));
            s->seq++;

            if (s->fmt == SHL_MZ_AUDIT_FORMAT_COMPACT)
            {
                char state[256] = "";
                mz__audit_zone_state(zone, state, sizeof(state));
                fprintf(s->fp,
                    "#%04u FREE     ts=%s site=%s:%d zone=%p ptr=%p alloc_size=%llu result=%s %s\n",
                    s->seq, ts, file, line,
                    (void*)zone,
                    p,
                    (unsigned long long)allocSz,
                    validPtr ? "OK" : "INVALID",
                    state);
            }
            else
            {
                mz__audit_verbose_open(s->fp, s->seq, "FREE", ts, file, line);
                fprintf(s->fp, "  Zone       : %p\n", (void*)zone);
                if (validPtr)
                    fprintf(s->fp, "  Pointer    : %p, alloc_size=%llu bytes\n",
                        p, (unsigned long long)allocSz);
                else
                    fprintf(s->fp,
                        "  Pointer    : %p  [INVALID - not a live allocation in this zone]\n", p);
                mz__audit_verbose_close(s->fp, zone);
            }
            fflush(s->fp);
        }
    }
}

void* mz__audit_realloc(memzone_t* zone, void* p, size_t size,
                         const char* file, int line)
{
    size_t      oldAllocSz = 0;
    const char* strategy   = "unknown";
    void*       result;

    if (p == NULL)
    {
        strategy = "equivalent to alloc (old ptr is NULL)";
    }
    else if (size == 0)
    {
        strategy = "equivalent to free (new size is 0)";
    }
    else if (zone != NULL)
    {
        const memblock_t* block = mz__findBlock(zone, p);
        if (block == NULL)
        {
            strategy = "invalid pointer";
        }
        else
        {
            size_t alignedSize = 0;
            oldAllocSz = mz__allocationSize(block);
            (void)mz__alignUp(size, mz_alignment(), &alignedSize);

            if (oldAllocSz >= alignedSize)
            {
                strategy = "no-op (block already has sufficient capacity)";
            }
            else
            {
                const memblock_t* next = block->next;
                bool canExpand =
                    (next != block) &&
                    MZ__IS_BLOCK_EMPTY(next) &&
                    mz__isNextBlockAdjacent(block) &&
                    (next->size >= (alignedSize - oldAllocSz));
                strategy = canExpand
                    ? "in-place expand (absorbed adjacent free block)"
                    : "alloc + copy + free (no usable adjacent space)";
            }
        }
    }

    result = mz_realloc(zone, p, size);

    {
        mz__audit_state_t* s = mz__audit_find(zone);
        if (s != NULL && s->fp != NULL)
        {
            char ts[32];
            bool ok = (result != NULL || size == 0);

            mz__audit_timestamp(ts, sizeof(ts));
            s->seq++;

            if (s->fmt == SHL_MZ_AUDIT_FORMAT_COMPACT)
            {
                char state[256] = "";
                mz__audit_zone_state(zone, state, sizeof(state));
                fprintf(s->fp,
                    "#%04u REALLOC  ts=%s site=%s:%d zone=%p old_ptr=%p old_size=%llu"
                    " new_size=%llu strategy=%s ptr=%p result=%s %s\n",
                    s->seq, ts, file, line,
                    (void*)zone,
                    p,
                    (unsigned long long)oldAllocSz,
                    (unsigned long long)size,
                    strategy,
                    result,
                    ok ? "OK" : "FAIL",
                    state);
            }
            else
            {
                mz__audit_verbose_open(s->fp, s->seq, "REALLOC", ts, file, line);
                fprintf(s->fp, "  Zone       : %p\n", (void*)zone);
                fprintf(s->fp, "  Old ptr    : %p, old_size=%llu bytes\n",
                    p, (unsigned long long)oldAllocSz);
                fprintf(s->fp, "  Request    : new_size=%llu bytes\n", (unsigned long long)size);
                fprintf(s->fp, "  Strategy   : %s\n", strategy);
                if (ok)
                {
                    if (result == p)
                        fprintf(s->fp, "  Result     : ptr=%p [OK] (same address)\n", result);
                    else if (result != NULL)
                        fprintf(s->fp, "  Result     : ptr=%p [OK] (new address)\n", result);
                    else
                        fprintf(s->fp, "  Result     : ptr=NULL [OK] (freed, size was 0)\n");
                }
                else
                {
                    fprintf(s->fp, "  Result     : ptr=NULL [FAIL]\n");
                }
                mz__audit_verbose_close(s->fp, zone);
            }
            fflush(s->fp);
        }
    }
    return result;
}

/* -------------------------------------------------------------------------
   Public audit control functions
   ---------------------------------------------------------------------- */

void mz_auditConfigure(memzone_t* zone, mz_audit_format_t format, const char* filepath)
{
    mz__audit_state_t* s;
    if (zone == NULL) { return; }
    s = mz__audit_acquire(zone, format, filepath);
    if (s == NULL) { return; }
    mz__audit_open(s);
}

void mz_auditFlush(memzone_t* zone)
{
    mz__audit_state_t* s = mz__audit_find(zone);
    if (s != NULL && s->fp != NULL)
        fflush(s->fp);
}

void mz_auditClose(memzone_t* zone)
{
    mz__audit_state_t* s = mz__audit_find(zone);
    if (s != NULL && s->fp != NULL)
    {
        fflush(s->fp);
        fclose(s->fp);
        s->fp = NULL;
    }
}

/* Restore macro redirections so user code in the same TU (after the
   implementation block) still routes through the audit layer.         */
#define mz_initAudit(sz, fmt, path) \
    mz__audit_initWithLog((sz), (fmt), (path), __FILE__, __LINE__)
#define mz_init(s)               mz__audit_init((s),               __FILE__, __LINE__)
#define mz_destroy(z)            mz__audit_destroy((z),            __FILE__, __LINE__)
#define mz_reset(z)              mz__audit_reset((z),              __FILE__, __LINE__)
#define mz_alloc(z, s)           mz__audit_alloc((z), (s),         __FILE__, __LINE__)
#define mz_allocAligned(z, s, a) mz__audit_allocAligned((z), (s), (a), __FILE__, __LINE__)
#define mz_free(z, p)            mz__audit_free((z), (p),          __FILE__, __LINE__)
#define mz_realloc(z, p, s)      mz__audit_realloc((z), (p), (s),  __FILE__, __LINE__)

#endif /* SHL_MZ_AUDIT_IMPLEMENTATION */

#endif /* SHL_MZ_AUDIT_H */
