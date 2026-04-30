#include "war_scene_briefing.h"

#include "shl/wstr.h"

#include "war_animations.h"
#include "war_audio.h"
#include "war_campaigns.h"
#include "war_entities.h"
#include "war_ui.h"

void wsbr_enterSceneBriefingHumans(WarContext* context)
{
    WarScene* scene = context->scene;

    WarCampaignMapData data = wcamp_getCampaignData(scene->briefing.mapType);

    scene->briefing.time = data.briefingDuration;

    wui_createUIImage(context, wstr_fromCString("imgBackground"), imageResourceRef(421), VEC2_ZERO);

    WarEntity* animEntity = we_createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
    we_addAnimationsComponent(context, animEntity);

    WarSpriteAnimation* anim1 = wanim_createAnimationFromResourceIndex(context, wstr_fromCString("anim1"), imageResourceRef(428), 0.2f, true);
    anim1->offset = vec2i(83, 37);
    anim1->loopDelay = 2.0f;
    wanim_addAnimationFramesRange(anim1, 0, 4);
    wanim_addAnimationFramesRange(anim1, 4, 0);
    wanim_addAnimation(animEntity, anim1);

    WarSpriteAnimation* anim2 = wanim_createAnimationFromResourceIndex(context, wstr_fromCString("anim2"), imageResourceRef(429), 0.2f, true);
    anim2->offset = vec2i(207, 29);
    anim1->loopDelay = 2.0f;
    wanim_addAnimationFramesRange(anim2, 0, 20);
    wanim_addAnimation(animEntity, anim2);

    WarSpriteAnimation* anim3 = wanim_createAnimationFromResourceIndex(context, wstr_fromCString("anim3"), imageResourceRef(430), 0.1f, true);
    anim3->offset = vec2i(21, 17);
    wanim_addAnimationFramesRange(anim3, 0, 20);
    wanim_addAnimation(animEntity, anim3);

    WarSpriteAnimation* anim4 = wanim_createAnimationFromResourceIndex(context, wstr_fromCString("anim4"), imageResourceRef(431), 0.1f, true);
    anim4->offset = vec2i(275, 21);
    wanim_addAnimationFramesRange(anim4, 0, 20);
    wanim_addAnimation(animEntity, anim4);

    WarEntity* briefingText = wui_createUIText(context, wstr_fromCString("txtBriefing"), 1, 10, data.briefingText, vec2i(20, 160));
    setUITextColor(briefingText, WAR_COLOR_RGB(255, 215, 138));
    setUITextMultiline(briefingText, true);
    setUITextBoundings(briefingText, vec2f((f32)(context->originalWindowWidth - 40), 200.0f));
    setUITextHorizontalAlign(briefingText, WAR_TEXT_ALIGN_LEFT);
    setUITextVerticalAlign(briefingText, WAR_TEXT_ALIGN_TOP);
    setUITextLineAlign(briefingText, WAR_TEXT_ALIGN_LEFT);
    setUITextWrapping(briefingText, WAR_TEXT_WRAP_CHAR);
    setUITextLineHeight(briefingText, 150);

    if (!isDemo(context))
        wa_createAudio(context, data.briefingAudioId, false);
}

void wsbr_enterSceneBriefingOrcs(WarContext* context)
{
    WarScene* scene = context->scene;

    WarCampaignMapData data = wcamp_getCampaignData(scene->briefing.mapType);

    scene->briefing.time = data.briefingDuration;

    wui_createUIImage(context, wstr_fromCString("imgBackground"), imageResourceRef(422), VEC2_ZERO);

    WarEntity* animEntity = we_createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
    we_addAnimationsComponent(context, animEntity);

    WarSpriteAnimation* anim1 = wanim_createAnimationFromResourceIndex(context, wstr_fromCString("anim1"), imageResourceRef(426), 0.2f, true);
    anim1->offset = vec2i(18, 67);
    anim1->loopDelay = 2.0f;
    wanim_addAnimationFramesRange(anim1, 0, 4);
    wanim_addAnimationFramesRange(anim1, 4, 0);
    wanim_addAnimation(animEntity, anim1);

    WarSpriteAnimation* anim2 = wanim_createAnimationFromResourceIndex(context, wstr_fromCString("anim2"), imageResourceRef(427), 0.2f, true);
    anim2->offset = vec2i(202, 52);
    anim1->loopDelay = 2.0f;
    wanim_addAnimationFramesRange(anim2, 0, 4);
    wanim_addAnimationFramesRange(anim2, 4, 0);
    wanim_addAnimation(animEntity, anim2);

    if (!isDemo(context))
    {
        WarSpriteAnimation* anim3 = wanim_createAnimationFromResourceIndex(context, wstr_fromCString("anim3"), imageResourceRef(425), 0.1f, true);
        anim3->offset = vec2i(140, 66);
        wanim_addAnimationFramesRange(anim3, 0, 30);
        wanim_addAnimation(animEntity, anim3);
    }

    WarEntity* briefingText = wui_createUIText(context, wstr_fromCString("txtBriefing"), 1, 10, data.briefingText, vec2i(20, 160));
    setUITextColor(briefingText, WAR_COLOR_RGB(255, 215, 138));
    setUITextMultiline(briefingText, true);
    setUITextBoundings(briefingText, vec2f((f32)(context->originalWindowWidth - 40), 200.0f));
    setUITextHorizontalAlign(briefingText, WAR_TEXT_ALIGN_LEFT);
    setUITextVerticalAlign(briefingText, WAR_TEXT_ALIGN_TOP);
    setUITextLineAlign(briefingText, WAR_TEXT_ALIGN_LEFT);
    setUITextWrapping(briefingText, WAR_TEXT_WRAP_CHAR);
    setUITextLineHeight(briefingText, 150);

    if (!isDemo(context))
        wa_createAudio(context, data.briefingAudioId, false);
}

void wsc_enterSceneBriefing(WarContext* context)
{
    WarScene* scene = context->scene;

    switch (scene->briefing.race)
    {
        case WAR_RACE_HUMANS:
        {
            wsbr_enterSceneBriefingHumans(context);
            break;
        }

        case WAR_RACE_ORCS:
        {
            wsbr_enterSceneBriefingOrcs(context);
            break;
        }

        default:
        {
            logError("Not allowed briefing scenes for race: %d", scene->briefing.race);
            break;
        }
    }
}

void wsc_updateSceneBriefing(WarContext* context)
{
    WarInput* input = &context->input;
    WarScene* scene = context->scene;

    scene->briefing.time -= context->deltaTime;

    WarEntity* txtBriefing = we_findUIEntity(context, wsv_fromCString("txtBriefing"));
    if (txtBriefing)
    {
        vec2 position = txtBriefing->transform.position;
        position.y -= 10 * context->deltaTime;
        txtBriefing->transform.position = position;
    }

    wanim_updateAnimations(context);

    if (scene->briefing.time <= 0 ||
        wasButtonPressed(input, WAR_MOUSE_LEFT) ||
        wasKeyPressed(input, WAR_KEY_ENTER) ||
        wasKeyPressed(input, WAR_KEY_SPACE))
    {
        WarMap* map = wmap_createMap(context, scene->briefing.mapType);
        wg_setNextMap(context, map, 1.0f);
    }
}
