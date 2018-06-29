#pragma once

#define ASSERT(expression) if(!(expression)) {*(int *)0 = 0;}

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define BIT0_MASK 0x01
#define BIT1_MASK 0x02
#define BIT2_MASK 0x04
#define BIT3_MASK 0x08
#define BIT4_MASK 0x10
#define BIT5_MASK 0x20
#define BIT6_MASK 0x40
#define BIT7_MASK 0x80

#define U8LOW_MASK 0x0F
#define U8HIGH_MASK 0xF0
#define U16LOW_MASK 0xFF
#define U16HIGH_MASK 0xFF00

#define ISNEG(x) ((x) & 0x80)
#define HAS_FLAG(x, mask) (((x) & (mask)) == (mask))
#define ISBETWEEN(x, a, b) ((x) >= (0) && (x) < (b))

#define GetBitFlag(v, f) ((v) & (1 << (f)) ? 1 : 0)
#define SetBitFlag(v, f) (*(v) = (*(v) | (1 << (f))))
#define ClearBitFlag(v, f) (*(v) = *(v) ^ (*(v) & (1 << (f))))

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef u8 b8;
typedef u16 b16;
typedef u32 b32;
typedef u64 b64;

typedef size_t size32;

#define TRUE  1
#define FALSE 0

#define NULL 0

#define internal static
#define global static
#define local static

/* time stuff */
global s64 globalPerfCountFrequency;
global LARGE_INTEGER initialCounter;

inline LARGE_INTEGER getWallClock(void)
{
    LARGE_INTEGER result;
    QueryPerformanceCounter(&result);
    return result;
}

inline f32 getSecondsElapsed(LARGE_INTEGER start, LARGE_INTEGER end)
{
    f32 result = ((f32)(end.QuadPart - start.QuadPart) / (f32)globalPerfCountFrequency);
    return result;
}

inline void initWallClock()
{
    LARGE_INTEGER perfCountFrequencyResult;
    QueryPerformanceFrequency(&perfCountFrequencyResult);

    globalPerfCountFrequency = perfCountFrequencyResult.QuadPart;
    initialCounter = getWallClock();
}

/* debug stuff */
inline char* debugText(char *debugBuffer, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    memset(debugBuffer, 0, sizeof(debugBuffer));
    vsprintf(debugBuffer, fmt, ap);
    return debugBuffer;
}

typedef enum _FileStatus {
    FILE_STATUS_OK,
    FILE_STATUS_ERROR
} FileStatus;

/* files */
typedef struct _TextFile
{
    FileStatus status;
    u64 length;

    union {
        char *contents;

        struct {
            u32 lineCount;
            char **lines;
        };
    };
} TextFile;

inline u64 flength(FILE *file)
{
    u64 pos = ftell(file);
    fseek(file, 0, SEEK_END);
    u64 length = ftell(file);
    fseek(file, pos, SEEK_SET);
    return length;
}

inline TextFile readAllText(char *filePath)
{
#define BUFFER_SIZE 1024

    FILE *file;
    u64 fileLength = 0;

    // get the length of the file
    file = fopen(filePath, "rb");
    if (file)
    {
        fileLength = flength(file);
        fclose(file);
    }

    TextFile textFile = {};
    textFile.status = FILE_STATUS_OK;

    file = fopen(filePath, "r");
    if (!file)
    {
        textFile.status = FILE_STATUS_ERROR;
        return textFile;    
    }

    textFile.length = fileLength;
    if (textFile.length > 0)
    {
        textFile.contents = (char*)malloc(textFile.length);
        
        char buffer[BUFFER_SIZE];
        size32 read = 0, total = 0;
        while (read = fread(buffer, 1, BUFFER_SIZE, file))
        {
            memcpy(textFile.contents + total, buffer, read);
            total += read;
        }

        memset(textFile.contents + total, 0, textFile.length - total);
    }
    
    fclose(file);
    return textFile;
}

inline TextFile readAllLines(char *filePath)
{
#define BUFFER_SIZE 1024

    TextFile file = {};
    file.length = 0;
    file.lineCount = 0;
    file.lines = NULL;

    char *line = NULL;

    char buffer[BUFFER_SIZE];

    FILE *in = fopen(filePath, "r");
    if (in)
    {
        file.length = flength(in);

        u32 read = fread(buffer, 1, BUFFER_SIZE, in);
        while (read > 0)
        {
            for (s32 i = 0; i < BUFFER_SIZE; i++)
            {
                if (buffer[i] == '\n')
                {
                    sb_push(line, 0);
                    sb_push(file.lines, line);

                    file.lineCount++;
                    line = NULL;
                }
                else
                {
                    sb_push(line, buffer[i]);
                }
            }

            read = fread(buffer, 1, BUFFER_SIZE, in);
        }
    }

    return file;
}

inline void diposeFile(TextFile file)
{
    if (file.contents)
    {
        free(file.contents);
        file.contents = NULL;
    }
    else if (file.lines)
    {
        for (s32 i=0; i<sb_count(file.lines); i++)
        {
            sb_free(file.lines[i]);
        }

        sb_free(file.lines);
    }
}