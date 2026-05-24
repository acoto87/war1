#pragma once

#include "war_editor.h"

typedef enum
{
    WE_VALID_ERROR = 0,
    WE_VALID_WARNING = 1,
    WE_VALID_INFO = 2,
} WarValidationSeverity;

typedef struct
{
    WarValidationSeverity severity;
    char                  message[160];
    bool                  hasTile;
    s32                   tx;
    s32                   ty;
} WarValidationResult;

#define WE_VALIDATION_MAX_RESULTS 256

void wevalid_validate(WarEditorMap* map, WarValidationResult* results, s32* count);
void wevalid_run(WarEditorContext* ctx);
const WarValidationResult* wevalid_getResults(s32* outCount);
void wevalid_drawPanel(WarEditorContext* ctx);
