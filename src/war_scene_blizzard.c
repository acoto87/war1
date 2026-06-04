#include "war_scene_blizzard.h"

#include "war_audio.h"
#include "war_imui.h"
#include "war_scenes.h"

void wsc_enterSceneBlizzard(WarContext* context)
{
    WarScene* scene = context->scene;
    scene->blizzard.endRealTime = context->realTime + 3.0;

    wa_createAudio(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_LOGO, .loop=true));
}

void wsc_updateSceneBlizzard(WarContext* context)
{
    WarScene* scene = context->scene;

    if (context->realTime >= scene->blizzard.endRealTime)
    {
        WarScene* nextScene = wsc_createScene(context, WAR_SCENE_MAIN_MENU);
        wg_setNextScene(context, nextScene, 0.3f);
    }
}

void wsc_renderSceneBlizzard(WarContext* context)
{
    imui_image(context, "imgBackground", CREATE_UI_IMAGE_ARGS_INIT(
        .spriteRef = imageResourceRef(216),
        .position  = VEC2_ZERO,
    ));
}
