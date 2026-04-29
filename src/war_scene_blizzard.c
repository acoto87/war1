#include "war_scene_blizzard.h"

#include "shl/wstr.h"

#include "war_audio.h"
#include "war_scenes.h"
#include "war_ui.h"

void enterSceneBlizzard(WarContext* context)
{
    WarScene* scene = context->scene;
    scene->blizzard.time = 3.0f;

    createUIImage(context, wstr_fromCString("imgBackground"), imageResourceRef(216), VEC2_ZERO);
    waud_createAudio(context, WAR_LOGO, true);
}

void updateSceneBlizzard(WarContext* context)
{
    WarScene* scene = context->scene;

    scene->blizzard.time -= context->deltaTime;

    if (scene->blizzard.time <= 0)
    {
        WarScene* nextScene = createScene(context, WAR_SCENE_MAIN_MENU);
        setNextScene(context, nextScene, 0.3f);
    }
}
