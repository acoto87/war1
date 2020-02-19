WarState* createCollapseState(WarContext* context, WarEntity* entity)
{
    WarState* state = createState(context, entity, WAR_STATE_COLLAPSE);
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
    entity->sprite.enabled = false;

    WarSpriteAnimation* collapseAnim = createCollapseAnimation(context, entity, "collapse");

    setDelay(state, getMapScaledTime(context, getAnimationDuration(collapseAnim)));

    WarEntity* ruins = map->ruin;
    addRuinsPieces(context, ruins, position.x, position.y, unitSize.x);
    determineRuinTypes(context, ruins);

    setFreeTiles(map->finder, position.x, position.y, unitSize.x, unitSize.y);
    removeEntityFromSelection(context, entity->id);
}

void leaveCollapseState(WarContext* context, WarEntity* entity, WarState* state)
{
}

void updateCollapseState(WarContext* context, WarEntity* entity, WarState* state)
{
    removeEntityById(context, entity->id);
}

void freeCollapseState(WarState* state)
{
}