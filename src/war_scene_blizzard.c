#include "war_scene_blizzard.h"

#include "shl/wstr.h"

#include "war_audio.h"
#include "war_scenes.h"
#include "war_ui.h"

void wsc_enterSceneBlizzard(WarContext* context)
{
    WarScene* scene = context->scene;
    scene->blizzard.time = 3.0f;

    wui_createUIImage(context, wstr_fromCString("imgBackground"), CREATE_UI_IMAGE_ARGS_INIT(
        .spriteRef = imageResourceRef(216),
        .position  = VEC2_ZERO,
    ));
    wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_LOGO, .loop=true));
}

void wsc_updateSceneBlizzard(WarContext* context)
{
    WarScene* scene = context->scene;

    scene->blizzard.time -= context->deltaTime;

    if (scene->blizzard.time <= 0)
    {
        WarScene* nextScene = wsc_createScene(context, WAR_SCENE_MAIN_MENU);
        wg_setNextScene(context, nextScene, 0.3f);
    }
}
