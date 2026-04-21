#pragma once

#include <stdio.h>
#include <stdint.h>

#define DEFAULT_BUFFER_SIZE (4 * 1024)

typedef struct
{
    uint32_t length;
    uint8_t *contents;
} TextFile;

uint32_t wio_flength(FILE *file);
void wio_fileReadBytes(uint8_t *bytes, uint32_t length, FILE *file);
uint8_t wio_fileReadU8(FILE *file);
uint16_t wio_fileReadU16(FILE *file);
uint32_t wio_fileReadU32(FILE *file);
TextFile* fileReadAllText(char *filePath);
void wio_fileDispose(TextFile *file);
