#include "../war_state_machine.h"

WarState* createCollapseState(WarContext* context, WarEntity* entity)
{
    WarState* state = createState(context, entity, WAR_STATE_COLLAPSE);
    state->delay = 17 * 0.1f;
    return state;
}

void enterCollapseState(WarContext* context, WarEntity* entity, WarState* state)
{
    WarMap* map = context->map;
    vec2 unitSize = getUnitSize(entity);
    vec2 position = vec2MapToTileCoordinates(entity->transform.position);
    
    removeAnimation(context, entity, "littleDamage");
    removeAnimation(context, entity, "hugeDamage");

    // disable the sprite component to just render the animation
    // should the collapse animation be a collapse action for buildings? 
    // how to manage the scale in that case?
    entity->sprite.enabled = false;

    vec2 unitFrameSize = getUnitFrameSize(entity);
    vec2 unitSpriteSize = getUnitSpriteSize(entity);

    WarSprite sprite = createSpriteFromResourceIndex(context, BUILDING_COLLAPSE_RESOURCE);
    WarSpriteAnimation* anim = createAnimation("collapse", sprite, 0.1f, false);

    vec2 animFrameSize = vec2i(anim->sprite.frameWidth, anim->sprite.frameHeight);

    // this is the scale of the explosion animation sprites with respect to the size of the building
    f32 animScale = unitSpriteSize.x / animFrameSize.x;

    // if the offset is based on the size of the frame, and it's scaled, then the offset must take into
    // account the scale to make the calculations
    f32 offsetx = halff(unitFrameSize.x - unitSpriteSize.x);
    f32 offsety = halff(unitFrameSize.y - unitSpriteSize.y) - (animFrameSize.y * animScale - unitSpriteSize.y);

    anim->scale = vec2f(animScale, animScale);
    anim->offset = vec2f(offsetx, offsety);

    for(s32 i = 0; i < 17; i++)
        addAnimationFrame(anim, i);
    
    addAnimation(entity, anim);

    setFreeTiles(map->finder, position.x, position.y, unitSize.x, unitSize.y);
}

void leaveCollapseState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void updateCollapseState(WarContext* context, WarEntity* entity, WarState* state)
{
    // then the state machine enter for the second time in this state
    // then I can safely remove the entity and some ruins should be spawned

    WarMap* map = context->map;

    WarSprite sprite = createSpriteFromResourceIndex(context, 190);
    WarSpriteAnimation* anim = createAnimation("ruins", sprite, 0.2f, true);
    
    addAnimationFrame(anim, 0);

    // for(s32 i = 0; i < 4; i++)
    //     addAnimationFrame(anim, i);
    
    addAnimation(entity, anim);

    WarSpriteAnimationListAdd(&map->animations, anim);

    // deselect the entity and remove it!
    // removeEntityFromSelection(context, entity->id);

    removeEntityById(context, entity->id);

    // if the entity is removed here, then the collapse animation doesn't show
    // this is because the animation is tied to the entity
    // for this two solutions:
    //   1. delay the removing of the entity until the animation is finished
    //   2. the collapse animation is not tied to the entity
    // I need to know when the animation finish to spawn a ruins entity,
    // or the ruins entity spawn at this same moment.
    //
    // removeEntityById(context, selEntity->id);
}

void freeCollapseState(WarState* state)
{
}