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

WarProjectileData getProjectileData(WarProjectileType type)
{
    s32 index = 0;
    s32 length = arrayLength(projectilesData);
    while (index < length && projectilesData[index].type != type)
        index++;

    assert(index < length);
    return projectilesData[index];
}

void doProjectileTargetDamage(WarContext* context, WarEntity* entity)
{
    WarProjectileComponent* projectile = &entity->projectile;

    vec2 position = vec2MapToTileCoordinates(projectile->target);

    WarEntity* sourceEntity = findEntity(context, projectile->sourceEntityId);
    WarEntity* targetEntity = findEntity(context, projectile->targetEntityId);

    // check if the attacker and the victim exists because it could be eliminated by other unit
    if (sourceEntity && targetEntity)
    {
        if (isWall(targetEntity))
        {
            WarWallPiece* piece = getWallPieceAtPosition(targetEntity, position.x, position.y);
            if (piece)
            {
                meleeWallAttack(context, sourceEntity, targetEntity, piece);
            }
        }
        else
        {
            meleeAttack(context, sourceEntity, targetEntity);
        }
    }
}

void doProjectileSplashDamage(WarContext* context, WarEntity* entity, s32 splashRadius)
{
    WarProjectileComponent* projectile = &entity->projectile;

    vec2 targetTile = vec2MapToTileCoordinates(projectile->target);

    WarEntity* sourceEntity = findEntity(context, projectile->sourceEntityId);
    if (sourceEntity)
    {
        WarEntityList* nearUnits = getNearUnits(context, targetTile, splashRadius);
        for (s32 i = 0; i < nearUnits->count; i++)
        {
            WarEntity* targetEntity = nearUnits->items[i];
            if (targetEntity && canAttack(context, sourceEntity, targetEntity))
            {
                meleeAttack(context, sourceEntity, targetEntity);
            }
        }
        WarEntityListFree(nearUnits);

        WarEntityList* walls = getEntitiesOfType(context, WAR_ENTITY_TYPE_WALL);
        for (s32 i = 0; i < walls->count; i++)
        {
            WarEntity* targetEntity = walls->items[i];

            WarWallPieceList* pieces = &targetEntity->wall.pieces;
            for (s32 k = 0; k < pieces->count; k++)
            {
                WarWallPiece* piece = &pieces->items[k];
                vec2 piecePosition = vec2i(piece->tilex, piece->tiley);
                if (vec2DistanceInTiles(targetTile, piecePosition) <= splashRadius)
                {
                    meleeWallAttack(context, sourceEntity, targetEntity, piece);
                }
            }
        }
    }
}

void doRainOfFireProjectileSplashDamage(WarContext* context, WarEntity* entity, s32 splashRadius)
{
    WarProjectileComponent* projectile = &entity->projectile;

    vec2 targetTile = vec2MapToTileCoordinates(projectile->target);

    WarEntityList* nearUnits = getNearUnits(context, targetTile, splashRadius);
    for (s32 i = 0; i < nearUnits->count; i++)
    {
        WarEntity* targetEntity = nearUnits->items[i];
        if (targetEntity &&
            !isDead(targetEntity) && !isGoingToDie(targetEntity) &&
            !isCollapsing(targetEntity) && !isGoingToCollapse(targetEntity))
        {
            takeDamage(context, targetEntity, 0, RAIN_OF_FIRE_PROJECTILE_DAMAGE);
        }
    }
    WarEntityListFree(nearUnits);

    WarEntityList* walls = getEntitiesOfType(context, WAR_ENTITY_TYPE_WALL);
    for (s32 i = 0; i < walls->count; i++)
    {
        WarEntity* targetEntity = walls->items[i];

        WarWallPieceList* pieces = &targetEntity->wall.pieces;
        for (s32 k = 0; k < pieces->count; k++)
        {
            WarWallPiece* piece = &pieces->items[k];
            vec2 piecePosition = vec2i(piece->tilex, piece->tiley);
            if (vec2DistanceInTiles(targetTile, piecePosition) <= splashRadius)
            {
                takeWallDamage(context, targetEntity, piece, 0, RAIN_OF_FIRE_PROJECTILE_DAMAGE);
            }
        }
    }
}

bool updateProjectilePosition(WarContext* context, WarEntity* entity)
{
    WarTransformComponent* transform = &entity->transform;
    WarProjectileComponent* projectile = &entity->projectile;

    vec2 position = transform->position;
    vec2 target = projectile->target;

    f32 speed = projectile->speed;
    speed = getMapScaledSpeed(context, speed);

    vec2 direction = vec2Subv(target, position);
    f32 directionLength = vec2Length(direction);

    vec2 step = vec2Mulf(vec2Normalize(direction), speed * context->deltaTime);
    f32 stepLength = vec2Length(step);

    if (directionLength < stepLength)
    {
        step = direction;
    }

    vec2 newPosition = vec2Addv(position, step);
    f32 distance = vec2Distance(newPosition, target);

    transform->position = newPosition;

    return distance < MOVE_EPSILON;
}

void updateProjectileSprite(WarContext* context, WarEntity* entity)
{
    WarTransformComponent* transform = &entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarProjectileComponent* projectile = &entity->projectile;

    WarProjectileData data = getProjectileData(projectile->type);

    vec2 position = transform->position;
    vec2 origin = projectile->origin;
    vec2 target = projectile->target;

    f32 totalDistance = vec2Distance(origin, target);

    vec2 direction = vec2Subv(target, position);
    f32 directionLength = vec2Length(direction);

    f32 travelDistance = totalDistance - directionLength;
    f32 travelPercent = percentabi(travelDistance, totalDistance);

    f32 angle = vec2ClockwiseAngle(VEC2_RIGHT, direction);

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
        if (angle >= controlAngles[i] - halff(45) &&
            angle < controlAngles[i] + halff(45))
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
    if (projectile->type != WAR_PROJECTILE_FIREBALL ||
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

void updateRainOfFireProjectileSprite(WarContext* context, WarEntity* entity)
{
    WarTransformComponent* transform = &entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarProjectileComponent* projectile = &entity->projectile;

    WarProjectileData data = getProjectileData(projectile->type);

    vec2 position = transform->position;
    vec2 origin = projectile->origin;
    vec2 target = projectile->target;
    s32 frameCount = data.frameCount;

    f32 totalDistance = vec2Distance(origin, target);

    vec2 direction = vec2Subv(target, position);
    f32 directionLength = vec2Length(direction);

    f32 travelDistance = totalDistance - directionLength;
    f32 travelPercent = percentabi(travelDistance, totalDistance);

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

void updateProjectile(WarContext* context, WarEntity* entity)
{
    WarSpriteComponent* sprite = &entity->sprite;
    WarProjectileComponent* projectile = &entity->projectile;

    if (sprite->enabled && projectile->enabled)
    {
        if (projectile->type == WAR_PROJECTILE_RAIN_OF_FIRE)
        {
            if (!updateProjectilePosition(context, entity))
            {
                updateRainOfFireProjectileSprite(context, entity);
            }
            else
            {
                doRainOfFireProjectileSplashDamage(context, entity, NEAR_RAIN_OF_FIRE_RADIUS);

                WarEntity* animEntity = createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                addAnimationsComponent(context, animEntity);

                createRainOfFireExplosionAnimation(context, animEntity, projectile->target);
                createAudioWithPosition(context, WAR_CATAPULT_FIRE_EXPLOSION, projectile->target, false);
                removeEntityById(context, entity->id);
            }
        }
        else
        {
            if (!updateProjectilePosition(context, entity))
            {
                updateProjectileSprite(context, entity);
            }
            else
            {
                if (projectile->type == WAR_PROJECTILE_CATAPULT)
                {
                    doProjectileSplashDamage(context, entity, NEAR_CATAPULT_RADIUS);

                    WarEntity* animEntity = createEntity(context, WAR_ENTITY_TYPE_ANIMATION, true);
                    addAnimationsComponent(context, animEntity);

                    createExplosionAnimation(context, animEntity, projectile->target);
                    createAudioWithPosition(context, WAR_CATAPULT_FIRE_EXPLOSION, projectile->target, false);
                }
                else
                {
                    doProjectileTargetDamage(context, entity);

                    if (projectile->type == WAR_PROJECTILE_ARROW)
                        createAudioWithPosition(context, WAR_ARROW_SPEAR_HIT, projectile->target, false);
                }

                removeEntityById(context, entity->id);
            }
        }
    }
}

WarEntity* createProjectile(WarContext* context, WarProjectileType type,
                            WarEntityId sourceEntityId, WarEntityId targetEntityId,
                            vec2 origin, vec2 target)
{
    WarProjectileData data = getProjectileData(type);

    WarEntity* projectile = createEntity(context, WAR_ENTITY_TYPE_PROJECTILE, true);
    addTransformComponent(context, projectile, origin);
    addSpriteComponentFromResource(context, projectile, imageResourceRef(data.resourceIndex));
    addProjectileComponent(context, projectile, type, sourceEntityId, targetEntityId, origin, target, data.speed);

    return projectile;
}
