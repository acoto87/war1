#pragma once

#include <stdio.h>
#include <stdint.h>

#define DEFAULT_BUFFER_SIZE (4 * 1024)

typedef struct
{
    uint32_t length;
    uint8_t *contents;
} TextFile;

uint32_t flength(FILE *file);
void fileReadBytes(uint8_t *bytes, uint32_t length, FILE *file);
uint8_t fileReadU8(FILE *file);
uint16_t fileReadU16(FILE *file);
uint32_t fileReadU32(FILE *file);
TextFile* fileReadAllText(char *filePath);
void fileDispose(TextFile *file);
