#include "io.h"

#include <string.h>

#include "alloc.h"
#include "war_math.h"

uint32_t wio_flength(FILE *file)
{
    uint32_t pos = ftell(file);
    fseek(file, 0, SEEK_END);
    uint32_t length = ftell(file);
    fseek(file, pos, SEEK_SET);
    return length;
}

void wio_fileReadBytes(uint8_t *bytes, uint32_t length, FILE *file)
{
    uint8_t buffer[DEFAULT_BUFFER_SIZE];

    size_t total = 0;
    size_t sz = min(length, DEFAULT_BUFFER_SIZE);
    size_t read = fread(buffer, sizeof(uint8_t), sz, file);
    while (read && total < length)
    {
        memcpy(bytes + total, buffer, read);
        total += read;

        sz = min(length - total, DEFAULT_BUFFER_SIZE);
        read = fread(buffer, sizeof(uint8_t), sz, file);
    }
}

uint8_t wio_fileReadU8(FILE *file)
{
    uint8_t value;
    fread(&value, sizeof(uint8_t), 1, file);
    return value;
}

uint16_t wio_fileReadU16(FILE *file)
{
    uint16_t value;
    fread(&value, sizeof(uint16_t), 1, file);
    return value;
}

uint32_t wio_fileReadU32(FILE *file)
{
    uint32_t value;
    fread(&value, sizeof(uint32_t), 1, file);
    return value;
}

TextFile* fileReadAllText(char *filePath)
{
    FILE *file;

    // get the length of the file
    file = fopen(filePath, "rb");
    if (!file)
    {
        return NULL;
    }

    uint32_t fileLength = wio_flength(file);
    fclose(file);

    file = fopen(filePath, "r");
    if (!file)
    {
        return NULL;
    }

    TextFile *textFile = (TextFile*)xmalloc(sizeof(TextFile));
    textFile->length = fileLength;
    if (textFile->length > 0)
    {
        textFile->contents = (uint8_t*)xcalloc(textFile->length, 1);
        wio_fileReadBytes(textFile->contents, textFile->length, file);
    }

    fclose(file);
    return textFile;
}

void wio_fileDispose(TextFile *file)
{
    if (file->contents)
    {
        free(file->contents);
        file->contents = NULL;
    }

    free(file);
}
