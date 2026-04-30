#include "war_projectiles.h"

#include <assert.h>

#include "war_animations.h"
#include "war_audio.h"
#include "war_entities.h"
#include "war_map.h"
#include "war_units.h"

typedef struct
{
    WarProjectileType type;
    s32 resourceIndex;
    s32 speed;          // in pixels/seconds
    s32 frameCount;     // number of frames that conform a single animation
    s32 frameStride;    // number of frames until next sprite from current animation
                        // this is due to some projectiles sprite having directional sprites
                        // so like sprites of units, they have 5 directions and 3 that are
                        // mirrored from others
} WarProjectileData;

WarProjectileData projectilesData[] =
{
    // type                             resourceIndex       speed                   frames  stride
    { WAR_PROJECTILE_ARROW,             349,                8 * MEGA_TILE_WIDTH,    1,      0 },
    { WAR_PROJECTILE_CATAPULT,          348,                4 * MEGA_TILE_WIDTH,    3,      5 },
    { WAR_PROJECTILE_FIREBALL,          347,                8 * MEGA_TILE_WIDTH,    5,      5 },
    { WAR_PROJECTILE_FIREBALL_2,        358,                8 * MEGA_TILE_WIDTH,    2,      5 },
    { WAR_PROJECTILE_WATER_ELEMENTAL,   357,                8 * MEGA_TILE_WIDTH,    2,      5 },
    { WAR_PROJECTILE_RAIN_OF_FIRE,      351,                6 * MEGA_TILE_WIDTH,    3,      1 },
};

WarProjectileData wproj_getProjectileData(WarProjectileType type)
{
    s32 index = 0;
    s32 length = arrayLength(projectilesData);
    while (index < length && projectilesData[index].type != type)
        index++;

    assert(index < length);
    return projectilesData[index];
}

void wproj_doProjectileTargetDamage(WarContext* context, WarEntity* entity)
{
    WarProjectileComponent* projectile = &entity->projectile;

    vec2 position = wmap_mapToTileCoordinatesV(projectile->target);

    WarEntity* sourceEntity = we_findEntity(context, projectile->sourceEntityId);
    WarEntity* targetEntity = we_findEntity(context, projectile->targetEntityId);

    // check if the attacker and the victim exists because it could be eliminated by other unit
    if (sourceEntity && targetEntity)
    {
        if (isWall(targetEntity))
        {
            WarWallPiece* piece = we_getWallPieceAtPosition(targetEntity, (s32)position.x, (s32)position.y);
            if (piece)
            {
                we_meleeWallAttack(context, sourceEntity, targetEntity, piece);
            }
        }
        else
        {
            we_meleeAttack(context, sourceEntity, targetEntity);
        }
    }
}

void wproj_doProjectileSplashDamage(WarContext* context, WarEntity* entity, s32 splashRadius)
{
    WarProjectileComponent* projectile = &entity->projectile;

    vec2 targetTile = wmap_mapToTileCoordinatesV(projectile->target);

    WarEntity* sourceEntity = we_findEntity(context, projectile->sourceEntityId);
    if (sourceEntity)
    {
        WarEntityList* nearUnits = we_getNearUnits(context, targetTile, splashRadius);
        for (s32 i = 0; i < nearUnits->count; i++)
        {
            WarEntity* targetEntity = nearUnits->items[i];
            if (targetEntity && wu_canAttack(context, sourceEntity, targetEntity))
            {
                we_meleeAttack(context, sourceEntity, targetEntity);
            }
        }
        WarEntityListFree(nearUnits);

        WarEntityList* walls = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_WALL);
        for (s32 i = 0; i < walls->count; i++)
        {
            WarEntity* targetEntity = walls->items[i];

            WarWallPieceList* pieces = &targetEntity->wall.pieces;
            for (s32 k = 0; k < pieces->count; k++)
            {
                WarWallPiece* piece = &pieces->items[k];
                vec2 piecePosition = vec2i(piece->tilex, piece->tiley);
                if (vec2_distanceInTiles(targetTile, piecePosition) <= splashRadius)
                {
                    we_meleeWallAttack(context, sourceEntity, targetEntity, piece);
                }
            }
        }
    }
}

void wproj_doRainOfFireProjectileSplashDamage(WarContext* context, WarEntity* entity, s32 splashRadius)
{
    WarProjectileComponent* projectile = &entity->projectile;

    vec2 targetTile = wmap_mapToTileCoordinatesV(projectile->target);

    WarEntityList* nearUnits = we_getNearUnits(context, targetTile, splashRadius);
    for (s32 i = 0; i < nearUnits->count; i++)
    {
        WarEntity* targetEntity = nearUnits->items[i];
        if (targetEntity &&
            !isDead(targetEntity) && !isGoingToDie(targetEntity) &&
            !isCollapsing(targetEntity) && !isGoingToCollapse(targetEntity))
        {
            we_takeDamage(context, targetEntity, 0, RAIN_OF_FIRE_PROJECTILE_DAMAGE);
        }
    }
    WarEntityListFree(nearUnits);

    WarEntityList* walls = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_WALL);
    for (s32 i = 0; i < walls->count; i++)
    {
        WarEntity* targetEntity = walls->items[i];

        WarWallPieceList* pieces = &targetEntity->wall.pieces;
        for (s32 k = 0; k < pieces->count; k++)
        {
            WarWallPiece* piece = &pieces->items[k];
            vec2 piecePosition = vec2i(piece->tilex, piece->tiley);
            if (vec2_distanceInTiles(targetTile, piecePosition) <= splashRadius)
            {
                we_takeWallDamage(context, targetEntity, piece, 0, RAIN_OF_FIRE_PROJECTILE_DAMAGE);
            }
        }
    }
}

bool wproj_updateProjectilePosition(WarContext* context, WarEntity* entity)
{
    WarTransformComponent* transform = &entity->transform;
    WarProjectileComponent* projectile = &entity->projectile;

    vec2 position = transform->position;
    vec2 target = projectile->target;

    f32 speed = (f32)projectile->speed;
    speed = wmap_getMapScaledSpeed(context, speed);

    vec2 direction = vec2_subv(target, position);
    f32 directionLength = vec2_length(direction);

    vec2 step = vec2_mulf(vec2_normalize(direction), speed * context->deltaTime);
    f32 stepLength = vec2_length(step);

    if (directionLength < stepLength)
    {
        step = direction;
    }

    vec2 newPosition = vec2_addv(position, step);
    f32 distance = vec2_distance(newPosition, target);

    transform->position = newPosition;

    return distance < MOVE_EPSILON;
}

void wproj_updateProjectileSprite(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    WarTransformComponent* transform = &entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarProjectileComponent* projectile = &entity->projectile;

    WarProjectileData data = wproj_getProjectileData(projectile->type);

    vec2 position = transform->position;
    vec2 origin = projectile->origin;
    vec2 target = projectile->target;

    f32 totalDistance = vec2_distance(origin, target);

    vec2 direction = vec2_subv(target, position);
    f32 directionLength = vec2_length(direction);

    f32 travelDistance = totalDistance - directionLength;
    f32 travelPercent = PERCENTABF(travelDistance, totalDistance);

    f32 angle = vec2_angleClockwise(VEC2_RIGHT, direction);

    // these are the angles at wich the frame index of the arrow
    // sprite needs to change and also where the x-scale needs to
    // be reversed
    f32 controlAngles[] = { 0, 45, 90, 135, 180, 225, 270, 315, 360 };
    s32 frameIndices[]  = { 2,  3,  4,   3,   2,   1,   0,   1,   2 };
    s32 frameScales[]   = { 1,  1,  1,  -1,  -1,  -1,   1,   1,   1 };

    s32 newFrameIndex = 0;
    vec2 newScale = VEC2_ONE;

    // find the current frame index and scale
    for (s32 i = 0; i < arrayLength(controlAngles); i++)
    {
        if (angle >= controlAngles[i] - 0.5f * 45 &&
            angle < controlAngles[i] + 0.5f * 45)
        {
            newFrameIndex = frameIndices[i];
            newScale.x *= frameScales[i];
            break;
        }
    }

    // the new frame index have to be changed for some projectiles
    // based on the travel percent of the projectile
    //
    s32 frameCount = data.frameCount;
    bool reverseFromHalf = false;

    // some projectiles have sequences like: 0 1 0 or 0 1 2 1 0
    // but the FIREBALL and RAIN_OF_FIRE projectiles have linear frame sequences
    if (projectile->type != WAR_PROJECTILE_FIREBALL &&
        projectile->type != WAR_PROJECTILE_RAIN_OF_FIRE)
    {
        frameCount = frameCount * 2 - 1;
        reverseFromHalf = true;
    }

    s32 q = (100 / frameCount);
    for (s32 k = 1; k < frameCount; k++)
    {
        if (q * k >= travelPercent)
            break;

        if (reverseFromHalf && k > frameCount / 2)
            newFrameIndex -= data.frameStride;
        else
            newFrameIndex += data.frameStride;
    }

    transform->scale = newScale;
    sprite->frameIndex = newFrameIndex;
}

void wproj_updateRainOfFireProjectileSprite(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    WarTransformComponent* transform = &entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarProjectileComponent* projectile = &entity->projectile;

    WarProjectileData data = wproj_getProjectileData(projectile->type);

    vec2 position = transform->position;
    vec2 origin = projectile->origin;
    vec2 target = projectile->target;
    s32 frameCount = data.frameCount;

    f32 totalDistance = vec2_distance(origin, target);

    vec2 direction = vec2_subv(target, position);
    f32 directionLength = vec2_length(direction);

    f32 travelDistance = totalDistance - directionLength;
    f32 travelPercent = PERCENTABF(travelDistance, totalDistance);

    // the new frame index have to be changed for some projectiles
    // based on the travel percent of the projectile
    //
    s32 newFrameIndex = 0;

    s32 q = (100 / frameCount);
    for (s32 k = 1; k < frameCount; k++)
    {
        if (q * k >= travelPercent)
            break;

        newFrameIndex += data.frameStride;
    }

    sprite->frameIndex = newFrameIndex;
}

void wproj_updateProjectile(WarContext* context, WarEntity* entity)
{
    WarSpriteComponent* sprite = &entity->sprite;
    WarProjectileComponent* projectile = &entity->projectile;

    if (sprite->enabled && projectile->enabled)
    {
        if (projectile->type == WAR_PROJECTILE_RAIN_OF_FIRE)
        {
            if (!wproj_updateProjectilePosition(context, entity))
            {
                wproj_updateRainOfFireProjectileSprite(context, entity);
            }
            else
            {
                wproj_doRainOfFireProjectileSplashDamage(context, entity, NEAR_RAIN_OF_FIRE_RADIUS);

                WarEntity* animEntity = we_createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                we_addAnimationsComponent(context, animEntity);

                wanim_createRainOfFireExplosionAnimation(context, animEntity, projectile->target);
                wa_createAudioWithPosition(context, WAR_CATAPULT_FIRE_EXPLOSION, projectile->target, false);
                we_removeEntityById(context, entity->id);
            }
        }
        else
        {
            if (!wproj_updateProjectilePosition(context, entity))
            {
                wproj_updateProjectileSprite(context, entity);
            }
            else
            {
                if (projectile->type == WAR_PROJECTILE_CATAPULT)
                {
                    wproj_doProjectileSplashDamage(context, entity, NEAR_CATAPULT_RADIUS);

                    WarEntity* animEntity = we_createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                    we_addAnimationsComponent(context, animEntity);

                    wanim_createExplosionAnimation(context, animEntity, projectile->target);
                    wa_createAudioWithPosition(context, WAR_CATAPULT_FIRE_EXPLOSION, projectile->target, false);
                }
                else
                {
                    wproj_doProjectileTargetDamage(context, entity);

                    if (projectile->type == WAR_PROJECTILE_ARROW)
                        wa_createAudioWithPosition(context, WAR_ARROW_SPEAR_HIT, projectile->target, false);
                }

                we_removeEntityById(context, entity->id);
            }
        }
    }
}

WarEntity* wproj_createProjectile(WarContext* context, WarProjectileType type,
                            WarEntityId sourceEntityId, WarEntityId targetEntityId,
                            vec2 origin, vec2 target)
{
    WarProjectileData data = wproj_getProjectileData(type);

    WarEntity* projectile = we_createEntity(context, WAR_ENTITY_TYPE_PROJECTILE, true);
    we_addTransformComponent(context, projectile, origin);
    we_addSpriteComponentFromResource(context, projectile, imageResourceRef(data.resourceIndex));
    we_addProjectileComponent(context, projectile, type, sourceEntityId, targetEntityId, origin, target, data.speed);

    return projectile;
}
