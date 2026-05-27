/**
 * xmi2mid.h - Single Header XMI to standard MIDI transcoding library.
 *
 * ============================================================================
 * USAGE
 * ============================================================================
 * In EXACTLY ONE C/C++ file, define XMI2MID_IMPLEMENTATION before including
 * this file to build the executable logic compilation path.
 *
 * Example:
 * #define XMI2MID_IMPLEMENTATION
 * #include "xmi2mid.h"
 *
 * This code is a port in C of the XMI2MID converter by Peter "Corsix" Cawley
 * in the War1gus repository. You can find the original C++ code here:
 * https://github.com/Wargus/war1gus/blob/master/xmi2mid.cpp.
 *
 * To understand more about these formats see:
 * http://www.shikadi.net/moddingwiki/XMI_Format
 * http://www.shikadi.net/moddingwiki/MID_Format
 * https://github.com/colxi/midi-parser-js/wiki/MIDI-File-Format-Specifications
 *
 *
 * ============================================================================
 * CUSTOM MEMORY ALLOCATOR REGISTRATION
 * ============================================================================
 * You can intercept heap operations by defining X2M_REALLOC and X2M_FREE
 * BEFORE including this header file.
 *
 * Example:
 * #define X2M_REALLOC(ptr, size) my_custom_realloc(ptr, size)
 * #define X2M_FREE(ptr)          my_custom_free(ptr)
 * #define XMI2MID_IMPLEMENTATION
 * #include "xmi2mid.h"
 */

#ifndef XMI2MID_SINGLE_HEADER_H
#define XMI2MID_SINGLE_HEADER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Transcodes raw XMI (Extended MIDI) data into a Standard MIDI File (SMF / .mid).
 *
 * @param[in]  xmiData     Pointer to the buffer containing the raw XMI file bytes.
 * @param[in]  xmiLength   The length of the raw input XMI buffer in bytes.
 * @param[out] midLength   Pointer receiving the calculated byte length of the returned MIDI data.
 * @return uint8_t* Dynamically allocated buffer containing the valid MIDI file, or NULL on failure.
 * The caller is responsible for disposing of this buffer via X2M_FREE().
 */
uint8_t* x2m_transcode(uint8_t* xmiData, size_t xmiLength, size_t* midLength);

#ifdef __cplusplus
}
#endif

#endif /* XMI2MID_SINGLE_HEADER_H */

/* ============================================================================
 * IMPLEMENTATION
 * ============================================================================ */
#ifdef XMI2MID_IMPLEMENTATION

#include <string.h>
#include <assert.h>

/* --- Custom Memory Allocation Interception Hook --- */
#if !defined(X2M_REALLOC) || !defined(X2M_FREE)
    #include <stdlib.h>
    #ifndef X2M_REALLOC
        #define X2M_REALLOC(ptr, size) realloc(ptr, size)
    #endif
    #ifndef X2M_FREE
        #define X2M_FREE(ptr)          free(ptr)
    #endif
#endif

/* --- MIDI Status & Event Type Constants --- */
#define MIDI_STATUS_MASK          0xF0
#define MIDI_DATA_BYTE_MASK       0x80

#define MIDI_EVENT_NOTE_OFF       0x80
#define MIDI_EVENT_NOTE_ON        0x90
#define MIDI_EVENT_POLY_TOUCH     0xA0
#define MIDI_EVENT_CONTROL_CHANGE 0xB0
#define MIDI_EVENT_PROGRAM_CHANGE 0xC0
#define MIDI_EVENT_CHAN_TOUCH     0xD0
#define MIDI_EVENT_PITCH_BEND     0xE0
#define MIDI_EVENT_SYSTEM_META    0xF0

#define MIDI_META_BYTE            0xFF
#define MIDI_META_END_OF_TRACK    0x2F
#define MIDI_META_TEMPO_CHANGE    0x51

#define XMI_TIMING_SCALER         3
#define MIDI_HEADER_SIZE          22

/* --- Internal Memory Stream Handlers --- */
typedef struct {
    const uint8_t* data;
    size_t    size;
    size_t    pos;
} x2m_input_stream_t;

typedef struct {
    uint8_t* data;
    size_t capacity;
    size_t pos;
} x2m_output_stream_t;

typedef struct {
    int32_t time;
    uint32_t bufferLength;
    uint8_t* buffer;
    uint8_t  type;
    uint8_t  data;
} midi_token_t;

typedef struct {
    midi_token_t* items;
    int32_t    count;
    int32_t    capacity;
} midi_token_list;

/* Pure-Pointer Stream Math Implementation Layer */
static inline bool x2m_in_read(x2m_input_stream_t* s, uint8_t* dest) {
    if (s->pos >= s->size) return false;
    *dest = s->data[s->pos++];
    return true;
}

static inline bool x2m_in_skip(x2m_input_stream_t* s, int32_t bytes) {
    if ((int32_t)s->pos + bytes < 0 || s->pos + bytes > s->size) return false;
    s->pos += bytes;
    return true;
}

static inline bool x2m_in_scan_to_evnt(x2m_input_stream_t* s) {
    if (s->size < 4 || s->pos > s->size - 4) return false;
    while (s->pos <= s->size - 4) {
        if (s->data[s->pos] == 'E' && s->data[s->pos+1] == 'V' &&
            s->data[s->pos+2] == 'N' && s->data[s->pos+3] == 'T') {
            return true;
        }
        s->pos++;
    }
    return false;
}

static inline bool x2m_out_reserve(x2m_output_stream_t* s, size_t needed) {
    if (s->pos + needed <= s->capacity) return true;
    size_t newCap = s->capacity == 0 ? 4096 : s->capacity * 2;
    while (s->pos + needed > newCap) newCap *= 2;
    uint8_t* nextAlloc = (uint8_t*)X2M_REALLOC(s->data, newCap);
    if (!nextAlloc) return false;
    s->data = nextAlloc;
    s->capacity = newCap;
    return true;
}

static inline bool x2m_out_write_byte(x2m_output_stream_t* s, uint8_t byte) {
    if (!x2m_out_reserve(s, 1)) return false;
    s->data[s->pos++] = byte;
    return true;
}

static inline bool x2m_out_write_bytes(x2m_output_stream_t* s, const uint8_t* src, size_t len) {
    if (len == 0) return true;
    if (!x2m_out_reserve(s, len)) return false;
    memcpy(&s->data[s->pos], src, len);
    s->pos += len;
    return true;
}

static inline bool x2m_out_write_uint16_t_be(x2m_output_stream_t* s, uint16_t value) {
    if (!x2m_out_reserve(s, 2)) return false;
    s->data[s->pos++] = (uint8_t)((value >> 8) & 0xFF);
    s->data[s->pos++] = (uint8_t)(value & 0xFF);
    return true;
}

static inline bool x2m_out_write_uint32_t_be(x2m_output_stream_t* s, uint32_t value) {
    if (!x2m_out_reserve(s, 4)) return false;
    s->data[s->pos++] = (uint8_t)((value >> 24) & 0xFF);
    s->data[s->pos++] = (uint8_t)((value >> 16) & 0xFF);
    s->data[s->pos++] = (uint8_t)((value >> 8) & 0xFF);
    s->data[s->pos++] = (uint8_t)(value & 0xFF);
    return true;
}

/* Fast Unrolled Local Variable-Length Quantity Processing */
static bool x2m_read_uint_var(x2m_input_stream_t* buffer, uint32_t* value) {
    uint32_t v = 0;
    uint8_t byte;
    for (int32_t i = 0; i < 4; ++i) {
        if (!x2m_in_read(buffer, &byte)) return false;
        v = (v << 7) | (uint32_t)(byte & 0x7F);
        if ((byte & 0x80) == 0) break;
    }
    *value = v;
    return true;
}

static inline bool x2m_write_uint_var(x2m_output_stream_t* buffer, uint32_t value) {
    if (value < 0x80) {
        return x2m_out_write_byte(buffer, (uint8_t)value);
    } else if (value < 0x4000) {
        return x2m_out_write_byte(buffer, (uint8_t)(((value >> 7) & 0x7F) | 0x80)) &&
               x2m_out_write_byte(buffer, (uint8_t)(value & 0x7F));
    } else if (value < 0x200000) {
        return x2m_out_write_byte(buffer, (uint8_t)(((value >> 14) & 0x7F) | 0x80)) &&
               x2m_out_write_byte(buffer, (uint8_t)(((value >> 7) & 0x7F) | 0x80)) &&
               x2m_out_write_byte(buffer, (uint8_t)(value & 0x7F));
    } else {
        return x2m_out_write_byte(buffer, (uint8_t)(((value >> 21) & 0x7F) | 0x80)) &&
               x2m_out_write_byte(buffer, (uint8_t)(((value >> 14) & 0x7F) | 0x80)) &&
               x2m_out_write_byte(buffer, (uint8_t)(((value >> 7) & 0x7F) | 0x80)) &&
               x2m_out_write_byte(buffer, (uint8_t)(value & 0x7F));
    }
}

static inline int compare_midi_tokens(const midi_token_t* left, const midi_token_t* right) {
    return left->time - right->time;
}

static midi_token_t* midi_token_list_append(midi_token_list* list, int32_t time, uint8_t type) {
    if (list->count >= list->capacity) {
        list->capacity = list->capacity == 0 ? 128 : list->capacity * 2;
        midi_token_t* nextAlloc = (midi_token_t*)X2M_REALLOC(list->items, list->capacity * sizeof(midi_token_t));
        if (!nextAlloc) return NULL;
        list->items = nextAlloc;
    }
    midi_token_t* element = &list->items[list->count];
    memset(element, 0, sizeof(midi_token_t));
    element->time = time;
    element->type = type;
    list->count++;
    return element;
}

static void midi_token_list_sort(midi_token_list* list, int32_t left, int32_t right, int32_t (*compareFn)(const midi_token_t* left, const midi_token_t* right))
{
    if (left >= right)
        return;

    int32_t middle = left + ((right - left) >> 1);
    midi_token_t p = list->items[middle];

    int32_t i = left - 1;
    int32_t j = right + 1;

    while (i < j)
    {
        do { i++; } while (compareFn(&list->items[i], &p) < 0);
        do { j--; } while (compareFn(&list->items[j], &p) > 0);

        if (i >= j)
            break;

        midi_token_t tmp = list->items[i];
        list->items[i] = list->items[j];
        list->items[j] = tmp;
    }

    midi_token_list_sort(list, left, j, compareFn);
    midi_token_list_sort(list, j + 1, right, compareFn);
}

/* --- Core Parser & Engine Conversion Loop Implementation --- */
uint8_t* x2m_transcode(uint8_t* xmiData, size_t xmiLength, size_t* midLength)
{
    x2m_input_stream_t  bufInput  = { xmiData, xmiLength, 0 };
    x2m_output_stream_t bufOutput = { NULL, 0, 0 };

    /* --- Step 1: Pre-allocating Output Array Limits --- */
    // Allocate up front to guarantee minimal realloc thrashing during loop steps
    if (!x2m_out_reserve(&bufOutput, xmiLength + 256)) {
        return NULL;
    }

    /* --- Step 2: Scan for XMI Audio Chunk Start --- */
    if (!x2m_in_scan_to_evnt(&bufInput) || !x2m_in_skip(&bufInput, 8)) {
        X2M_FREE(bufOutput.data);
        return NULL;
    }

    midi_token_list lstTokens = { NULL, 0, 0 };
    midi_token_t* token = NULL;
    int32_t tokenTime = 0;
    int32_t tempo = 500000;
    bool tempoSet = false;
    bool end = false;
    uint8_t tokenType, extendedType;
    uint32_t intVar;

    /* --- Step 3: Parse Pass (Tokens Extraction) --- */
    while (bufInput.pos < bufInput.size && !end)
    {
        /* Accumulate XMI Variable Delay Bytes */
        while (true)
        {
            if (!x2m_in_read(&bufInput, &tokenType)) {
                X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                return NULL;
            }
            if (tokenType & MIDI_DATA_BYTE_MASK) break;
            tokenTime += (int32_t)tokenType * XMI_TIMING_SCALER;
        }

        token = midi_token_list_append(&lstTokens, tokenTime, tokenType);
        if (!token) {
            X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
            return NULL;
        }
        token->buffer = (uint8_t*)(bufInput.data + bufInput.pos + 1);

        switch (tokenType & MIDI_STATUS_MASK)
        {
            case MIDI_EVENT_PROGRAM_CHANGE:
            case MIDI_EVENT_CHAN_TOUCH:
            {
                if (!x2m_in_read(&bufInput, &token->data)) {
                    X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                    return NULL;
                }
                token->buffer = NULL;
                break;
            }
            case MIDI_EVENT_NOTE_OFF:
            case MIDI_EVENT_POLY_TOUCH:
            case MIDI_EVENT_CONTROL_CHANGE:
            case MIDI_EVENT_PITCH_BEND:
            {
                if (!x2m_in_read(&bufInput, &token->data) || !x2m_in_skip(&bufInput, 1)) {
                    X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                    return NULL;
                }
                break;
            }
            case MIDI_EVENT_NOTE_ON:
            {
                if (!x2m_in_read(&bufInput, &extendedType) || !x2m_in_skip(&bufInput, 1) || !x2m_read_uint_var(&bufInput, &intVar)) {
                    X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                    return NULL;
                }
                token->data = extendedType;

                /* Inject synthesized implicit Note-Off counterpart */
                midi_token_t* offToken = midi_token_list_append(&lstTokens, tokenTime + intVar * XMI_TIMING_SCALER, tokenType);
                if (!offToken) {
                    X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                    return NULL;
                }
                offToken->data = extendedType;
                offToken->buffer = (uint8_t*)"\0";
                break;
            }
            case MIDI_EVENT_SYSTEM_META:
            {
                extendedType = 0;
                if (tokenType == MIDI_META_BYTE) {
                    if (!x2m_in_read(&bufInput, &extendedType)) {
                        X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                        return NULL;
                    }
                    if (extendedType == MIDI_META_END_OF_TRACK) {
                        end = true;
                    } else if (extendedType == MIDI_META_TEMPO_CHANGE) {
                        if (!tempoSet) {
                            if (bufInput.pos + 4 > bufInput.size) {
                                X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                                return NULL;
                            }
                            bufInput.pos += 1;
                            tempo = (bufInput.data[bufInput.pos] << 16) |
                                    (bufInput.data[bufInput.pos+1] << 8) |
                                        bufInput.data[bufInput.pos+2];
                            bufInput.pos += 3;
                            tempo *= XMI_TIMING_SCALER;
                            tempoSet = true;
                            bufInput.pos -= 4;
                        } else {
                            if (lstTokens.count > 0) lstTokens.count--;
                            if (!x2m_read_uint_var(&bufInput, &intVar) || !x2m_in_skip(&bufInput, intVar)) {
                                X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                                return NULL;
                            }
                            break;
                        }
                    }
                }

                token->data = extendedType;
                if (!x2m_read_uint_var(&bufInput, &token->bufferLength)) {
                    X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                    return NULL;
                }
                token->buffer = (uint8_t*)(bufInput.data + bufInput.pos);
                if (!x2m_in_skip(&bufInput, token->bufferLength)) {
                    X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                    return NULL;
                }
                break;
            }
        }
    }

    if (lstTokens.count == 0) {
        X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
        return NULL;
    }

    /* --- Step 4: Write Standard MIDI Header Chunk (MThd) --- */
    if (!x2m_out_write_bytes(&bufOutput, (const uint8_t*)"MThd\0\0\0\x06\0\0\0\x01", 12)) {
        X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
        return NULL;
    }
    if (!x2m_out_write_uint16_t_be(&bufOutput, (uint16_t)((tempo * XMI_TIMING_SCALER) / 25000))) {
        X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
        return NULL;
    }
    if (!x2m_out_write_bytes(&bufOutput, (const uint8_t*)"MTrk\xBA\xAD\xF0\x0D", 8)) {
        X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
        return NULL;
    }

    /* --- Step 5: Chronological Sorting --- */
    midi_token_list_sort(&lstTokens, 0, lstTokens.count - 1, compare_midi_tokens);

    tokenTime = 0;
    uint8_t tokenTypeState = 0;
    end = false;

    /* --- Step 6: Write Pass (Emit Sorted MIDI Stream) --- */
    for (int32_t i = 0; i < lstTokens.count && !end; i++)
    {
        midi_token_t t = lstTokens.items[i];

        if (!x2m_write_uint_var(&bufOutput, t.time - tokenTime)) {
            X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
            return NULL;
        }
        tokenTime = t.time;

        if (t.type >= MIDI_EVENT_SYSTEM_META) {
            tokenTypeState = t.type;
            if (!x2m_out_write_byte(&bufOutput, tokenTypeState)) {
                X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                return NULL;
            }

            if (tokenTypeState == MIDI_META_BYTE) {
                if (!x2m_out_write_byte(&bufOutput, t.data)) {
                    X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                    return NULL;
                }
                if (t.data == MIDI_META_END_OF_TRACK) end = true;
            }

            if (!x2m_write_uint_var(&bufOutput, t.bufferLength)) {
                X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                return NULL;
            }
            if (!x2m_out_write_bytes(&bufOutput, t.buffer, t.bufferLength)) {
                X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                return NULL;
            }
        } else {
            /* Running Status Optimization Frame */
            if (t.type != tokenTypeState) {
                tokenTypeState = t.type;
                if (!x2m_out_write_byte(&bufOutput, tokenTypeState)) {
                    X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                    return NULL;
                }
            }

            if (!x2m_out_write_byte(&bufOutput, t.data)) {
                X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                return NULL;
            }

            if (t.buffer) {
                if (!x2m_out_write_byte(&bufOutput, t.buffer[0])) {
                    X2M_FREE(bufOutput.data); X2M_FREE(lstTokens.items);
                    return NULL;
                }
            }
        }
    }

    /* --- Step 7: Final Length Patching & Extraction --- */
    uint32_t length = (uint32_t)(bufOutput.pos - MIDI_HEADER_SIZE);

    /* Safely patch Big Endian chunk size calculation back directly without seeking */
    bufOutput.data[18] = (uint8_t)((length >> 24) & 0xFF);
    bufOutput.data[19] = (uint8_t)((length >> 16) & 0xFF);
    bufOutput.data[20] = (uint8_t)((length >> 8) & 0xFF);
    bufOutput.data[21] = (uint8_t)(length & 0xFF);

    if (midLength) *midLength = bufOutput.pos;
    uint8_t* midData = bufOutput.data;

    X2M_FREE(lstTokens.items);

    return midData;
}

#endif /* XMI2MID_IMPLEMENTATION */
