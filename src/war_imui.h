#pragma once

#include "war.h"
#include "war_ui.h"

// ---------------------------------------------------------------------------
// FNV-1a hash — converts a null-terminated string ID to a u32 item ID.
// ---------------------------------------------------------------------------
static inline u32 imui_hashCString(const char* str)
{
    u32 hash = 2166136261u;
    while (*str)
    {
        hash ^= (u32)(unsigned char)(*str++);
        hash *= 16777619u;
    }
    return hash;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

// Call once at the start of the render pass, before any imui_* draw calls.
// Resets hot_item, is_mouse_over_ui, and tooltip deferral state.
void imui_begin(WarContext* context);

// Call once at the very end of the render pass, after all imui_* draw calls.
// Renders any deferred tooltip and clears active_item when the mouse is up.
void imui_end(WarContext* context);

// ---------------------------------------------------------------------------
// Primitives (stateless draw calls — no input handling)
// ---------------------------------------------------------------------------

// Draw a text label.  Uses CreateUITextArgs (see war_ui.h / CREATE_UI_TEXT_ARGS_INIT).
void imui_text(WarContext* context, const char* id, const CreateUITextArgs* args);

// Draw a filled rectangle.  Uses CreateUIRectArgs (see CREATE_UI_RECT_ARGS_INIT).
void imui_rect(WarContext* context, const char* id, const CreateUIRectArgs* args);

// Draw a static sprite image.  Uses CreateUIImageArgs (see CREATE_UI_IMAGE_ARGS_INIT).
void imui_image(WarContext* context, const char* id, const CreateUIImageArgs* args);

// Draw a specific frame of a sprite resource.  Unlike imui_image (frame 0),
// this keys the cache on (resourceIndex, frameIndex) so multiple frames of the
// same sprite resource can be drawn in a single render pass without conflicts.
void imui_image_frame(WarContext* context, const char* id, const CreateUIImageArgs* args, s32 frameIndex);

// ---------------------------------------------------------------------------
// Interactive elements — return true the frame the element is clicked
// ---------------------------------------------------------------------------

// Image button (icon + background sprite).
// Uses CreateUIImageButtonArgs (see CREATE_UI_IMAGE_BUTTON_ARGS_INIT).
// The tooltip field, if non-empty, is deferred and drawn by imui_end().
bool imui_image_button(WarContext* context, const char* id, const CreateUIImageButtonArgs* args);

// Text button (text label + background sprite).
// Uses CreateUITextButtonArgs (see CREATE_UI_TEXT_BUTTON_ARGS_INIT).
// The tooltip field, if non-empty, is deferred and drawn by imui_end().
bool imui_text_button(WarContext* context, const char* id, const CreateUITextButtonArgs* args);
