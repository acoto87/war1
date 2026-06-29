#include <assert.h>
#include <stdlib.h>

#include "shl/wstr.h"

#include "war_entities.h"

#include "war_animations.h"
#include "war_audio.h"
#include "war_font.h"
#include "war_map.h"
#include "war_map_ui.h"
#include "war_render.h"
#include "war_sprites.h"
#include "war_units.h"
#include "war_collections.h"

shlDefineList(WarRoadPieceList, WarRoadPiece)
shlDefineList(WarWallPieceList, WarWallPiece)
shlDefineList(WarRuinPieceList, WarRuinPiece)
shlDefineList(WarTreeList, WarTree)
shlDefineList(WarEntityIdList, WarEntityId)
shlDefineSet(WarEntityIdSet, WarEntityId)
shlDefineList(WarEntityList, WarEntity*)
shlDefineMap(WarEntityMap, WarEntityType, WarEntityList*)
shlDefineMap(WarUnitMap, WarUnitType, WarEntityList*)
shlDefineMap(WarEntityIdMap, WarEntityId, WarEntity*)

bool we_equalsRoadPiece(const WarRoadPiece r1, const WarRoadPiece r2)
{
    return r1.type == r2.type && r1.player == r2.player &&
           r1.tilex == r2.tilex && r1.tiley == r2.tiley;
}

bool we_equalsWallPiece(const WarWallPiece w1, const WarWallPiece w2)
{
    return w1.type == w2.type && w1.player == w2.player &&
           w1.tilex == w2.tilex && w1.tiley == w2.tiley;
}

bool we_equalsRuinPiece(const WarRuinPiece r1, const WarRuinPiece r2)
{
    return r1.type == r2.type &&
           r1.tilex == r2.tilex && r1.tiley == r2.tiley;
}

bool we_equalsTree(const WarTree t1, const WarTree t2)
{
    return t1.tilex == t2.tilex && t1.tiley == t2.tiley;
}

bool we_equalsEntityId(const WarEntityId id1, const WarEntityId id2)
{
    return id1 == id2;
}

u32 we_hashEntityId(const WarEntityId id)
{
    return id;
}

bool we_equalsEntity(const WarEntity* e1, const WarEntity* e2)
{
    return e1->id == e2->id;
}

u32 we_hashEntityType(const WarEntityType type)
{
    return type;
}

bool we_equalsEntityType(const WarEntityType t1, const WarEntityType t2)
{
    return t1 == t2;
}

bool we_isComponentEnabled(WarContext* context, WarEntity* entity, WarComponentType componentType)
{
    assert(context);
    assert(entity);
    assert(componentType >= 0 && componentType < COMP_COUNT);

    u16 idx = entity->components[(s32)componentType];
    if (idx == INVALID_COMP_INDEX)
    {
        // Component not present;
        return false;
    }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);

    switch (componentType)
    {
        case COMP_TRANSFORM: return manager->transforms.enabled[idx];
        case COMP_SPRITE: return manager->sprites.enabled[idx];
        case COMP_UNIT: return manager->units.enabled[idx];
        case COMP_ANIMATIONS: return manager->animations.enabled[idx];
        case COMP_ROAD: return manager->roads.enabled[idx];
        case COMP_WALL: return manager->walls.enabled[idx];
        case COMP_RUIN: return manager->ruins.enabled[idx];
        case COMP_FOREST: return manager->forests.enabled[idx];
        case COMP_STATE_MACHINE: return manager->stateMachines.enabled[idx];
        case COMP_UI: return manager->uis.enabled[idx];
        case COMP_TEXT: return manager->texts.enabled[idx];
        case COMP_RECT: return manager->rects.enabled[idx];
        case COMP_BUTTON: return manager->buttons.enabled[idx];
        case COMP_AUDIO: return manager->audios.enabled[idx];
        case COMP_CURSOR: return manager->cursors.enabled[idx];
        case COMP_PROJECTILE: return manager->projectiles.enabled[idx];
        case COMP_POISON_CLOUD: return manager->poisonClouds.enabled[idx];
        case COMP_SIGHT: return manager->sights.enabled[idx];
        default: assert(false && "Invalid component type"); return false; break;
    }
}

void we_setComponentEnabled(WarContext* context, WarEntity* entity, WarComponentType componentType, bool enabled)
{
    assert(context);
    assert(entity);
    assert(componentType >= 0 && componentType < COMP_COUNT);

    u16 idx = entity->components[(s32)componentType];
    if (idx == INVALID_COMP_INDEX)
    {
        // Component not present; nothing to enable/disable
        return;
    }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);

    switch (componentType)
    {
        case COMP_TRANSFORM: manager->transforms.enabled[idx] = enabled; break;
        case COMP_SPRITE: manager->sprites.enabled[idx] = enabled; break;
        case COMP_UNIT: manager->units.enabled[idx] = enabled; break;
        case COMP_ANIMATIONS: manager->animations.enabled[idx] = enabled; break;
        case COMP_ROAD: manager->roads.enabled[idx] = enabled; break;
        case COMP_WALL: manager->walls.enabled[idx] = enabled; break;
        case COMP_RUIN: manager->ruins.enabled[idx] = enabled; break;
        case COMP_FOREST: manager->forests.enabled[idx] = enabled; break;
        case COMP_STATE_MACHINE: manager->stateMachines.enabled[idx] = enabled; break;
        case COMP_UI: manager->uis.enabled[idx] = enabled; break;
        case COMP_TEXT: manager->texts.enabled[idx] = enabled; break;
        case COMP_RECT: manager->rects.enabled[idx] = enabled; break;
        case COMP_BUTTON: manager->buttons.enabled[idx] = enabled; break;
        case COMP_AUDIO: manager->audios.enabled[idx] = enabled; break;
        case COMP_CURSOR: manager->cursors.enabled[idx] = enabled; break;
        case COMP_PROJECTILE: manager->projectiles.enabled[idx] = enabled; break;
        case COMP_POISON_CLOUD: manager->poisonClouds.enabled[idx] = enabled; break;
        case COMP_SIGHT: manager->sights.enabled[idx] = enabled; break;
        default: assert(false && "Invalid component type"); break;
    }
}

void we_enableComponent(WarContext* context, WarEntity* entity, WarComponentType componentType)
{
    we_setComponentEnabled(context, entity, componentType, true);
}

void we_disableComponent(WarContext* context, WarEntity* entity, WarComponentType componentType)
{
    we_setComponentEnabled(context, entity, componentType, false);
}

WarTransformComponent* we_getTransformComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_TRANSFORM];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->transforms.dense[idx];
}

WarSpriteComponent* we_getSpriteComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_SPRITE];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->sprites.dense[idx];
}

WarUnitComponent* we_getUnitComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_UNIT];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->units.dense[idx];
}

WarAnimationsComponent* we_getAnimationsComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_ANIMATIONS];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->animations.dense[idx];
}

WarRoadComponent* we_getRoadComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_ROAD];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->roads.dense[idx];
}

WarWallComponent* we_getWallComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_WALL];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->walls.dense[idx];
}

WarRuinComponent* we_getRuinComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_RUIN];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->ruins.dense[idx];
}

WarForestComponent* we_getForestComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_FOREST];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->forests.dense[idx];
}

WarStateMachineComponent* we_getStateMachineComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_STATE_MACHINE];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->stateMachines.dense[idx];
}

WarUIComponent* we_getUIComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_UI];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->uis.dense[idx];
}

WarTextComponent* we_getTextComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_TEXT];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->texts.dense[idx];
}

WarRectComponent* we_getRectComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_RECT];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->rects.dense[idx];
}

WarButtonComponent* we_getButtonComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_BUTTON];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->buttons.dense[idx];
}

WarAudioComponent* we_getAudioComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_AUDIO];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->audios.dense[idx];
}

WarCursorComponent* we_getCursorComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_CURSOR];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->cursors.dense[idx];
}

WarProjectileComponent* we_getProjectileComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_PROJECTILE];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->projectiles.dense[idx];
}

WarPoisonCloudComponent* we_getPoisonCloudComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_POISON_CLOUD];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->poisonClouds.dense[idx];
}

WarSightComponent* we_getSightComponent(WarContext* context, const WarEntity* entity)
{
    if (!entity) return NULL;
    u16 idx = entity->components[COMP_SIGHT];
    if (idx == INVALID_COMP_INDEX) return NULL;
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->sights.dense[idx];
}

WarTransformComponent* we_addTransformComponent(WarContext* context, WarEntity* entity, WarTransformComponent params)
{
    TracyCZoneN(ctx, "we_addTransformComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);

    WarTransformStorage* store = &manager->transforms;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    store->dense[idx] = params;
    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_TRANSFORM] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

void we_removeTransformComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;

    u16 idx = entity->components[COMP_TRANSFORM];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);

    WarTransformStorage* store = &manager->transforms;
    u16 lastIdx = (u16)(store->count - 1);

    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_TRANSFORM] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_TRANSFORM] = INVALID_COMP_INDEX;
}

WarSpriteComponent* we_addSpriteComponent(WarContext* context, WarEntity* entity, WarSpriteComponent params)
{
    TracyCZoneN(ctx, "we_addSpriteComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarSpriteStorage* store = &manager->sprites;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    store->dense[idx] = params;
    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_SPRITE] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

WarSpriteComponent* we_addSpriteComponentFromResource(WarContext* context, WarEntity* entity, WarSpriteResourceRef spriteResourceRef)
{
    TracyCZoneN(ctx, "we_addSpriteComponentFromResource", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    if (spriteResourceRef.resourceIndex < 0)
    {
        logWarning("Trying to create a sprite component with invalid resource index: %d", spriteResourceRef.resourceIndex);
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarSprite sprite = wspr_createSpriteFromResourceIndex(context, spriteResourceRef);
    WarSpriteComponent* comp = we_addSpriteComponent(context, entity, WAR_SPRITE_COMPONENT_INIT(
        .resourceIndex = spriteResourceRef.resourceIndex,
        .sprite = sprite
    ));

    TracyCZoneEnd(ctx);

    return comp;
}

void we_removeSpriteComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;
    u16 idx = entity->components[COMP_SPRITE];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarSpriteStorage* store = &manager->sprites;

    wspr_freeSprite(context, store->dense[idx].sprite);

    u16 lastIdx = (u16)(store->count - 1);
    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_SPRITE] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_SPRITE] = INVALID_COMP_INDEX;
}

WarUnitComponent* we_addUnitComponent(WarContext* context, WarEntity* entity, WarUnitComponent params)
{
    TracyCZoneN(ctx, "we_addUnitComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarUnitStorage* store = &manager->units;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    store->dense[idx] = params;
    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_UNIT] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

void we_removeUnitComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;
    u16 idx = entity->components[COMP_UNIT];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarUnitStorage* store = &manager->units;
    u16 lastIdx = (u16)(store->count - 1);

    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_UNIT] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_UNIT] = INVALID_COMP_INDEX;
}

WarRoadComponent* we_addRoadComponent(WarContext* context, WarEntity* entity, WarRoadPieceList pieces)
{
    TracyCZoneN(ctx, "we_addRoadComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarRoadStorage* store = &manager->roads;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    WarRoadComponent* comp = &store->dense[idx];
    *comp = (WarRoadComponent){0};
    comp->pieces = pieces;

    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_ROAD] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

void we_removeRoadComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;
    u16 idx = entity->components[COMP_ROAD];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarRoadStorage* store = &manager->roads;

    WarRoadPieceListFree(&store->dense[idx].pieces);

    u16 lastIdx = (u16)(store->count - 1);
    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_ROAD] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_ROAD] = INVALID_COMP_INDEX;
}

WarWallComponent* we_addWallComponent(WarContext* context, WarEntity* entity, WarWallPieceList pieces)
{
    TracyCZoneN(ctx, "we_addWallComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarWallStorage* store = &manager->walls;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    WarWallComponent* comp = &store->dense[idx];
    *comp = (WarWallComponent){0};
    comp->pieces = pieces;

    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_WALL] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

void we_removeWallComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;
    u16 idx = entity->components[COMP_WALL];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarWallStorage* store = &manager->walls;

    WarWallPieceListFree(&store->dense[idx].pieces);

    u16 lastIdx = (u16)(store->count - 1);
    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_WALL] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_WALL] = INVALID_COMP_INDEX;
}

WarRuinComponent* we_addRuinComponent(WarContext* context, WarEntity* entity, WarRuinPieceList pieces)
{
    TracyCZoneN(ctx, "we_addRuinComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarRuinStorage* store = &manager->ruins;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    WarRuinComponent* comp = &store->dense[idx];
    *comp = (WarRuinComponent){0};
    comp->pieces = pieces;

    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_RUIN] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

void we_removeRuinComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;
    u16 idx = entity->components[COMP_RUIN];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarRuinStorage* store = &manager->ruins;

    WarRuinPieceListFree(&store->dense[idx].pieces);

    u16 lastIdx = (u16)(store->count - 1);
    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_RUIN] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_RUIN] = INVALID_COMP_INDEX;
}

WarForestComponent* we_addForestComponent(WarContext* context, WarEntity* entity, WarTreeList trees)
{
    TracyCZoneN(ctx, "we_addForestComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarForestStorage* store = &manager->forests;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    WarForestComponent* comp = &store->dense[idx];
    *comp = (WarForestComponent){0};
    comp->trees = trees;

    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_FOREST] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

void we_removeForestComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;
    u16 idx = entity->components[COMP_FOREST];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarForestStorage* store = &manager->forests;

    WarTreeListFree(&store->dense[idx].trees);

    u16 lastIdx = (u16)(store->count - 1);
    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_FOREST] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_FOREST] = INVALID_COMP_INDEX;
}

WarStateMachineComponent* we_addStateMachineComponent(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "we_addStateMachineComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarStateMachineStorage* store = &manager->stateMachines;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    WarStateMachineComponent* comp = &store->dense[idx];
    *comp = (WarStateMachineComponent){0};
    comp->currentState   = NULL;
    comp->nextState      = NULL;
    comp->leaveState = false;
    comp->enterState = false;

    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_STATE_MACHINE] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

void we_removeStateMachineComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;
    u16 idx = entity->components[COMP_STATE_MACHINE];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarStateMachineStorage* store = &manager->stateMachines;
    WarStateMachineComponent* comp = &store->dense[idx];

    if (comp->currentState)
        wst_leaveState(context, entity, comp->currentState);

    if (comp->nextState)
        wst_leaveState(context, entity, comp->nextState);

    u16 lastIdx = (u16)(store->count - 1);
    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_STATE_MACHINE] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_STATE_MACHINE] = INVALID_COMP_INDEX;
}

WarAnimationsComponent* we_addAnimationsComponent(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "we_addAnimationsComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarAnimationsStorage* store = &manager->animations;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    WarAnimationsComponent* comp = &store->dense[idx];
    *comp = (WarAnimationsComponent){0};
    WarSpriteAnimationListInit(&comp->animations, wm_globalAllocator());

    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_ANIMATIONS] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

void we_removeAnimationsComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;
    u16 idx = entity->components[COMP_ANIMATIONS];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);

    WarAnimationsStorage* store = &manager->animations;
    WarSpriteAnimationList* animList = &store->dense[idx].animations;
    for (s32 i = 0; i < animList->count; i++)
    {
        S32ListFree(&animList->items[i].frames);
    }

    WarSpriteAnimationListFree(animList);

    u16 lastIdx = (u16)(store->count - 1);
    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_ANIMATIONS] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_ANIMATIONS] = INVALID_COMP_INDEX;
}

WarUIComponent* we_addUIComponent(WarContext* context, WarEntity* entity, String name)
{
    TracyCZoneN(ctx, "we_addUIComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarUIStorage* store = &manager->uis;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    WarUIComponent* comp = &store->dense[idx];
    *comp = (WarUIComponent){0};
    comp->name = name;

    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_UI] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

void we_removeUIComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;
    u16 idx = entity->components[COMP_UI];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarUIStorage* store = &manager->uis;

    u16 lastIdx = (u16)(store->count - 1);
    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_UI] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_UI] = INVALID_COMP_INDEX;
}

WarTextComponent* we_addTextComponent(WarContext* context, WarEntity* entity, WarTextComponent params)
{
    TracyCZoneN(ctx, "we_addTextComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarTextStorage* store = &manager->texts;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    store->dense[idx] = params;
    store->dense[idx].text = wstr_make();  // NOTE: text ownership is transferred via wui_setUIText below

    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_TEXT] = idx;

    wui_setUIText(context, entity, params.text);

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

void we_removeTextComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;
    u16 idx = entity->components[COMP_TEXT];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarTextStorage* store = &manager->texts;

    wui_clearUIText(context, entity);

    u16 lastIdx = (u16)(store->count - 1);
    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_TEXT] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_TEXT] = INVALID_COMP_INDEX;
}

WarRectComponent* we_addRectComponent(WarContext* context, WarEntity* entity, WarRectComponent params)
{
    TracyCZoneN(ctx, "we_addRectComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarRectStorage* store = &manager->rects;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    store->dense[idx] = params;
    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_RECT] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

void we_removeRectComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;
    u16 idx = entity->components[COMP_RECT];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarRectStorage* store = &manager->rects;
    u16 lastIdx = (u16)(store->count - 1);

    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_RECT] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_RECT] = INVALID_COMP_INDEX;
}

WarButtonComponent* we_addButtonComponent(WarContext* context, WarEntity* entity, WarButtonComponent params)
{
    TracyCZoneN(ctx, "we_addButtonComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarButtonStorage* store = &manager->buttons;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    store->dense[idx] = params;
    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_BUTTON] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

WarButtonComponent* we_addButtonComponentFromResource(WarContext* context, WarEntity* entity, WarSpriteResourceRef normalRef, WarSpriteResourceRef pressedRef)
{
    TracyCZoneN(ctx, "we_addButtonComponentFromResource", 1);

    if (normalRef.resourceIndex < 0)
    {
        logWarning("Trying to create a button component with invalid resource index: %d", normalRef.resourceIndex);
        TracyCZoneEnd(ctx);
        return NULL;
    }

    if (pressedRef.resourceIndex < 0)
    {
        logWarning("Trying to create a button component with invalid resource index: %d", pressedRef.resourceIndex);
        TracyCZoneEnd(ctx);
        return NULL;
    }

    WarSprite normalSprite = wspr_createSpriteFromResourceIndex(context, normalRef);
    WarSprite pressedSprite = wspr_createSpriteFromResourceIndex(context, pressedRef);
    WarButtonComponent* comp = we_addButtonComponent(context, entity, WAR_BUTTON_COMPONENT_INIT(
        .normalSprite  = normalSprite,
        .pressedSprite = pressedSprite,
    ));

    TracyCZoneEnd(ctx);

    return comp;
}

void we_removeButtonComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;
    u16 idx = entity->components[COMP_BUTTON];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarButtonStorage* store = &manager->buttons;
    WarButtonComponent* comp = &store->dense[idx];

    wui_clearUITooltip(context, entity);
    wspr_freeSprite(context, comp->normalSprite);
    wspr_freeSprite(context, comp->pressedSprite);

    u16 lastIdx = (u16)(store->count - 1);
    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_BUTTON] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_BUTTON] = INVALID_COMP_INDEX;
}

WarAudioComponent* we_addAudioComponent(WarContext* context, WarEntity* entity, WarAudioComponent params)
{
    TracyCZoneN(ctx, "we_addAudioComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarAudioStorage* store = &manager->audios;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    store->dense[idx] = params;

    if (store->dense[idx].type == WAR_AUDIO_MIDI)
    {
        assert(store->dense[idx].resourceIndex >= 0);

        WarResource* resource = &context->resources[store->dense[idx].resourceIndex];
        assert(resource->type == WAR_RESOURCE_TYPE_XMID || resource->type == WAR_RESOURCE_TYPE_WAVE || resource->type == WAR_RESOURCE_TYPE_VOC);

        u8* midiData   = resource->audio.data;
        s32 midiLength = resource->audio.length;

        store->dense[idx].firstMessage   = tml_load_memory(midiData, midiLength);
        store->dense[idx].currentMessage = store->dense[idx].firstMessage;
        if (!store->dense[idx].firstMessage)
        {
            logError("Could not load MIDI from resource: %d", store->dense[idx].resourceIndex);
        }
    }

    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_AUDIO] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

void we_removeAudioComponent(WarContext* context, WarEntity* entity)
{
    NOT_USED(context);

    if (!entity) return;
    u16 idx = entity->components[COMP_AUDIO];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarAudioStorage* store = &manager->audios;
    WarAudioComponent* comp = &store->dense[idx];

    if (comp->firstMessage)
    {
        // Deallocates the whole array of messages
        tml_free(comp->firstMessage);
    }
    comp->firstMessage   = NULL;
    comp->currentMessage = NULL;

    u16 lastIdx = (u16)(store->count - 1);
    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_AUDIO] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_AUDIO] = INVALID_COMP_INDEX;
}

WarProjectileComponent* we_addProjectileComponent(WarContext* context, WarEntity* entity, WarProjectileComponent params)
{
    TracyCZoneN(ctx, "we_addProjectileComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarProjectileStorage* store = &manager->projectiles;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    store->dense[idx] = params;
    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_PROJECTILE] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

void we_removeProjectileComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;
    u16 idx = entity->components[COMP_PROJECTILE];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarProjectileStorage* store = &manager->projectiles;
    u16 lastIdx = (u16)(store->count - 1);

    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_PROJECTILE] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_PROJECTILE] = INVALID_COMP_INDEX;
}

WarCursorComponent* we_addCursorComponent(WarContext* context, WarEntity* entity, WarCursorComponent params)
{
    TracyCZoneN(ctx, "we_addCursorComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarCursorStorage* store = &manager->cursors;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    store->dense[idx] = params;
    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_CURSOR] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

void we_removeCursorComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;
    u16 idx = entity->components[COMP_CURSOR];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarCursorStorage* store = &manager->cursors;
    u16 lastIdx = (u16)(store->count - 1);

    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_CURSOR] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_CURSOR] = INVALID_COMP_INDEX;
}

WarPoisonCloudComponent* we_addPoisonCloudComponent(WarContext* context, WarEntity* entity, WarPoisonCloudComponent params)
{
    TracyCZoneN(ctx, "we_addPoisonCloudComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarPoisonCloudStorage* store = &manager->poisonClouds;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    store->dense[idx] = params;
    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_POISON_CLOUD] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

void we_removePoisonCloudComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;
    u16 idx = entity->components[COMP_POISON_CLOUD];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarPoisonCloudStorage* store = &manager->poisonClouds;
    u16 lastIdx = (u16)(store->count - 1);

    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_POISON_CLOUD] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_POISON_CLOUD] = INVALID_COMP_INDEX;
}

WarSightComponent* we_addSightComponent(WarContext* context, WarEntity* entity, WarSightComponent params)
{
    TracyCZoneN(ctx, "we_addSightComponent", 1);

    if (!entity) { TracyCZoneEnd(ctx); return NULL; }

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarSightStorage* store = &manager->sights;
    assert(store->count < MAX_ENTITIES_COUNT);

    u16 idx = (u16)store->count;
    store->dense[idx] = params;
    store->enabled[idx] = true;
    store->owners[idx] = entity->id;
    store->count++;
    entity->components[COMP_SIGHT] = idx;

    TracyCZoneEnd(ctx);

    return &store->dense[idx];
}

void we_removeSightComponent(WarContext* context, WarEntity* entity)
{
    if (!entity) return;
    u16 idx = entity->components[COMP_SIGHT];
    if (idx == INVALID_COMP_INDEX) return;

    WarEntityManager* manager = we_getEntityManager(context);
    assert(manager);
    WarSightStorage* store = &manager->sights;
    u16 lastIdx = (u16)(store->count - 1);

    if (idx != lastIdx)
    {
        store->dense[idx]  = store->dense[lastIdx];
        store->enabled[idx] = store->enabled[lastIdx];
        store->owners[idx] = store->owners[lastIdx];

        WarEntity* movedEntity = we_findEntity(context, store->owners[idx]);
        assert(movedEntity != NULL);
        movedEntity->components[COMP_SIGHT] = idx;
    }

    store->enabled[lastIdx] = false;
    store->owners[lastIdx] = 0;
    store->count--;
    entity->components[COMP_SIGHT] = INVALID_COMP_INDEX;
}

// Entities
WarEntity* we_createEntity(WarContext* context, WarEntityType type, bool addToScene)
{
    TracyCZoneN(ctx, "we_createEntity", 1);

    WarEntityManager* manager = we_getEntityManager(context);

    // Find first empty slot (id == 0 means unused; slot 0 is permanently reserved)
    WarEntity* entity = NULL;
    for (u32 i = 1; i < MAX_ENTITIES_COUNT; i++)
    {
        if (manager->entities[i].id == 0)
        {
            entity = &manager->entities[i];
            break;
        }
    }
    if (!entity)
    {
        logError("Entity pool exhausted (MAX_ENTITIES_COUNT=%d)", MAX_ENTITIES_COUNT);
        TracyCZoneEnd(ctx);
        return NULL;
    }

    entity->id   = (u16)++manager->nextEntityId;
    entity->type = (u16)type;

    manager->entityCount++;

    if (addToScene)
    {
        WarEntityIdMapSet(&manager->entitiesById, entity->id, entity);

        WarEntityList* entitiesOfType = we_getEntitiesOfType(context, type);
        WarEntityListAdd(entitiesOfType, entity);

        if (wui_isUIEntity(entity))
        {
            WarEntityListAdd(&manager->uiEntities, entity);
        }
    }

    TracyCZoneEnd(ctx);

    return entity;
}

WarEntity* we_createUnit(WarContext* context, const CreateUnitArgs* args)
{
    WarUnitType type = args->type;
    s32 x = args->x;
    s32 y = args->y;
    u8 player = args->player;
    WarResourceKind resourceKind = args->resourceKind;
    u32 amount = args->amount;
    bool addToMap = args->addToMap;
    TracyCZoneN(ctx, "we_createUnit", 1);

    WarMap* map = context->map;

    const WarUnitData* unitData = wu_getUnitData(type);

    WarEntity* entity = we_createEntity(context, WAR_ENTITY_TYPE_UNIT, addToMap);
    we_addTransformComponent(context, entity, WAR_TRANSFORM_COMPONENT_INIT(
        .position = vec2i(x * MEGA_TILE_WIDTH, y * MEGA_TILE_HEIGHT)
    ));
    we_addUnitComponent(context, entity, WAR_UNIT_COMPONENT_INIT(
        .type         = type,
        .direction    = rand() % WAR_DIRECTION_COUNT,
        .tilex        = x,
        .tiley        = y,
        .sizex        = unitData->sizex,
        .sizey        = unitData->sizey,
        .player       = player,
        .resourceKind = resourceKind,
        .amount       = amount,
    ));

    s32 spriteIndex = unitData->resourceIndex;
    if (spriteIndex == 0)
    {
        logError("Sprite for unit of type %d is not configure properly. Default to footman sprite.", type);
        spriteIndex = 279;
    }
    we_addSpriteComponentFromResource(context, entity, imageResourceRef(spriteIndex));

    wact_addUnitActions(context, entity);
    we_addAnimationsComponent(context, entity);
    we_addStateMachineComponent(context, entity);

    if (wu_isDudeUnit(context, entity))
    {
        const WarUnitStats* stats = wu_getUnitStats(type);

        WarUnitComponent* uc = we_getUnitComponent(context, entity);
        assert(uc);

        uc->maxhp = stats->hp;
        uc->hp = stats->hp;
        uc->maxMana = stats->mana;
        uc->mana = wu_isSummonUnit(context, entity) ? stats->mana : 100;
        uc->armor = stats->armor;
        uc->range = stats->range;
        uc->minDamage = stats->minDamage;
        uc->rndDamage = stats->rndDamage;
        uc->decay = stats->decay;
        uc->manaTime = 1;
    }
    else if(wu_isBuildingUnit(context, entity))
    {
        const WarBuildingStats* stats = wu_getBuildingStats(type);

        WarUnitComponent* uc = we_getUnitComponent(context, entity);
        assert(uc);

        uc->maxhp = stats->hp;
        uc->hp = stats->hp;
        uc->armor = stats->armor;
    }

    // NOTE: we_createUnit does NOT set an initial state.
    // The caller is responsible for calling we_setInitialIdleState or
    // wst_changeNextState with the appropriate initial state.

    if (addToMap)
    {
        WarEntityManager* manager = &map->entityManager;
        WarEntityList* list = WarUnitMapGet(&manager->unitsByType, type);
        WarEntityListAdd(list, entity);
    }

    TracyCZoneEnd(ctx);

    return entity;
}

WarEntity* we_createDude(WarContext* context, const CreateUnitArgs* args)
{
    WarUnitType type = args->type;
    s32 x = args->x;
    s32 y = args->y;
    u8 player = args->player;
    bool isGoingToTrain = args->isGoingToTrain;
    assert(wu_isDudeUnitType(type));

    return we_createUnit(context, CREATE_UNIT_ARGS_INIT(.type=type, .x=x, .y=y, .player=player, .addToMap=!isGoingToTrain));
}

WarEntity* we_createBuilding(WarContext* context, const CreateUnitArgs* args)
{
    WarUnitType type = args->type;
    s32 x = args->x;
    s32 y = args->y;
    u8 player = args->player;
    bool isGoingToBuild = args->isGoingToBuild;
    assert(wu_isBuildingUnitType(type));

    WarEntity* entity = we_createUnit(context, CREATE_UNIT_ARGS_INIT(.type=type, .x=x, .y=y, .player=player, .addToMap=true));

    if (isGoingToBuild)
    {
        const WarBuildingStats* stats = wu_getBuildingStats(type);
        WarState* buildState = wst_createBuildState(context, entity, (f32)stats->buildTime);
        wst_changeNextState(context, entity, buildState, true, true);
    }
    else
    {
        we_setInitialIdleState(context, entity);
    }

    return entity;
}

void we_setInitialIdleState(WarContext* context, WarEntity* entity)
{
    WarState* idleState = wst_createIdleState(context, entity, wu_isDudeUnit(context, entity));
    wst_changeNextState(context, entity, idleState, true, true);
}

WarEntity* we_findEntity(WarContext* context, WarEntityId id)
{
    TracyCZoneN(ctx, "we_findEntity", 1);

    WarEntity* result = NULL;
    if (id != 0)
    {
        WarEntityManager* manager = we_getEntityManager(context);
        for (u32 i = 1; i < MAX_ENTITIES_COUNT; i++)
        {
            if (manager->entities[i].id == id)
            {
                result = &manager->entities[i];
                break;
            }
        }
    }

    TracyCZoneEnd(ctx);

    return result;
}

WarEntity* we_findClosestUnitOfType(WarContext* context, WarEntity* entity, WarUnitType type)
{
    TracyCZoneN(ctx, "we_findClosestUnitOfType", 1);

    WarEntity* result = NULL;
    s32 minDst = INT32_MAX;

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    assert(units);

    for (s32 i = 0; i < units->count; i++)
    {
        WarEntity* target = units->items[i];
        if (wu_isUnitOfType(context, target, type))
        {
            s32 dst = wu_unitDistanceInTiles(context, entity, target);
            if (dst < minDst)
            {
                result = target;
                minDst = dst;
            }
        }
    }

    TracyCZoneEnd(ctx);

    return result;
}

WarEntity* we_findUIEntity(WarContext* context, StringView name)
{
    TracyCZoneN(ctx, "we_findUIEntity", 1);

    WarEntity* result = NULL;

    WarEntityList* entities = we_getUIEntities(context);
    for (s32 i = 0; i < entities->count; i++)
    {
        WarEntity* entity = entities->items[i];
        if (entity && wui_isUIEntity(entity))
        {
            WarUIComponent* uiComp = we_getUIComponent(context, entity);
            assert(uiComp);

            if (wsv_equals(wsv_fromString(&uiComp->name), name))
            {
                result = entity;
                break;
            }
        }
    }

    TracyCZoneEnd(ctx);

    return result;
}

WarEntity* we_findEntityUnderCursor(WarContext* context, bool includeTrees, bool includeWalls)
{
    TracyCZoneN(ctx, "we_findEntityUnderCursor", 1);

    WarInput* input = &context->input;

    WarMap* map = context->map;
    assert(map);

    vec2 targetPoint = wmap_screenToMapCoordinatesV(context, input->pos);
    vec2 targetTile = wmap_mapToTileCoordinatesV(targetPoint);

    WarEntity* entityUnderCursor = NULL;

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* entity = units->items[i];
        if (entity)
        {
            WarUnitComponent* unit = we_getUnitComponent(context, entity);
            if (unit)
            {
                // don't change the cursor for dead units or corpses
                if (wst_isDead(context, entity) || wst_isGoingToDie(context, entity) || wu_isCorpseUnit(context, entity))
                {
                    continue;
                }

                // don't change the cursor for collased buildings
                if (wst_isCollapsing(context, entity) || wst_isGoingToCollapse(context, entity))
                {
                    continue;
                }

                // don't change the cursor for non-visible units
                if (!wmap_isUnitPartiallyVisible(context, map, entity))
                {
                    continue;
                }
            }

            rect unitRect = wu_getUnitRect(context, entity);
            if (rect_containsf(unitRect, targetPoint.x, targetPoint.y))
            {
                entityUnderCursor = entity;
                break;
            }
        }
    }

    if (includeTrees && !entityUnderCursor)
    {
        WarEntityList* forests = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_FOREST);
        for (s32 i = 0; i < forests->count; i++)
        {
            WarEntity* forest = forests->items[i];
            if (forest)
            {
                WarForestComponent* forestComp = we_getForestComponent(context, forest);
                assert(forestComp);

                WarTreeList* trees = &forestComp->trees;
                for (s32 k = 0; k < trees->count; k++)
                {
                    WarTree tree = trees->items[k];
                    if (tree.tilex == (s32)targetTile.x &&
                        tree.tiley == (s32)targetTile.y)
                    {
                        if (tree.type != WAR_TREE_NONE &&
                            tree.type != WAR_TREE_CHOPPED)
                        {
                            entityUnderCursor = forest;
                        }

                        break;
                    }
                }
            }

            if (entityUnderCursor)
            {
                break;
            }
        }
    }

    // if there is no unit under the cursor, check the walls
    if (includeWalls && !entityUnderCursor)
    {
        WarEntityList* walls = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_WALL);
        for (s32 i = 0; i < walls->count; i++)
        {
            WarEntity* wall = walls->items[i];
            if (wall)
            {
                WarWallComponent* wallComp = we_getWallComponent(context, wall);
                assert(wallComp);

                WarWallPieceList* pieces = &wallComp->pieces;
                for (s32 k = 0; k < pieces->count; k++)
                {
                    WarWallPiece piece = pieces->items[k];
                    if (piece.tilex == (s32)targetTile.x &&
                        piece.tiley == (s32)targetTile.y)
                    {
                        if (piece.hp > 0)
                        {
                            entityUnderCursor = wall;
                        }

                        break;
                    }
                }
            }

            if (entityUnderCursor)
            {
                break;
            }
        }
    }

    TracyCZoneEnd(ctx);

    return entityUnderCursor;
}

void we_removeEntity(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "we_removeEntity", 1);

    WarEntityManager* manager = we_getEntityManager(context);

    // Cache secondary-index metadata BEFORE any component is stripped.
    bool           isUI        = wui_isUIEntity(entity);
    bool           isUnitEnt   = wu_isUnit(entity);
    WarEntityType  entityType  = (WarEntityType)entity->type;
    WarEntityId    entityId    = entity->id;
    WarUnitType    unitType    = WAR_UNIT_FOOTMAN; // safe default
    if (isUnitEnt)
    {
        WarUnitComponent* uc = we_getUnitComponent(context, entity);
        if (uc)
            unitType = uc->type;
    }

    // Strip all 18 components (dense storage, each is a no-op if absent)
    we_removeTransformComponent(context, entity);
    we_removeSpriteComponent(context, entity);
    we_removeUnitComponent(context, entity);
    we_removeRoadComponent(context, entity);
    we_removeWallComponent(context, entity);
    we_removeRuinComponent(context, entity);
    we_removeForestComponent(context, entity);
    we_removeStateMachineComponent(context, entity);
    we_removeAnimationsComponent(context, entity);
    we_removeUIComponent(context, entity);
    we_removeTextComponent(context, entity);
    we_removeRectComponent(context, entity);
    we_removeButtonComponent(context, entity);
    we_removeAudioComponent(context, entity);
    we_removeCursorComponent(context, entity);
    we_removeProjectileComponent(context, entity);
    we_removePoisonCloudComponent(context, entity);
    we_removeSightComponent(context, entity);

    // Update secondary indices
    if (isUI)
        WarEntityListRemove(&manager->uiEntities, entity, we_equalsEntity);

    if (isUnitEnt)
    {
        WarEntityList* unitTypeList = WarUnitMapGet(&manager->unitsByType, unitType);
        if (unitTypeList)
            WarEntityListRemove(unitTypeList, entity, we_equalsEntity);
    }

    WarEntityList* entityTypeList = WarEntityMapGet(&manager->entitiesByType, entityType);
    if (entityTypeList)
        WarEntityListRemove(entityTypeList, entity, we_equalsEntity);

    WarEntityIdMapRemove(&manager->entitiesById, entityId);

    // Free the flat-pool slot and return it for reuse
    *entity = (WarEntity){0};
    manager->entityCount--;

    TracyCZoneEnd(ctx);
}

void we_removeEntityById(WarContext* context, WarEntityId id)
{
    TracyCZoneN(ctx, "we_removeEntityById", 1);

    WarEntity* entity = we_findEntity(context, id);
    if (entity)
    {
        SDL_LockMutex(context->__mutex);
        we_removeEntity(context, entity);
        SDL_UnlockMutex(context->__mutex);
    }

    TracyCZoneEnd(ctx);
}

void we_initEntityManager(WarContext* context, WarEntityManager* manager)
{
    TracyCZoneN(ctx, "we_initEntityManager", 1);

    NOT_USED(context);

    // Zero the entire flat entity pool (id==0 marks every slot as empty)
    memset(manager->entities, 0, sizeof(manager->entities));
    manager->entityCount  = 0;
    manager->nextEntityId = 0;

    // Reserve slot 0 in every dense storage (idx==0 == INVALID_COMP_INDEX == "absent").
    // The first real component added to any store will go to index 1.
    manager->transforms.count    = 1;
    manager->sprites.count       = 1;
    manager->units.count         = 1;
    manager->animations.count    = 1;
    manager->roads.count         = 1;
    manager->walls.count         = 1;
    manager->ruins.count         = 1;
    manager->forests.count       = 1;
    manager->stateMachines.count = 1;
    manager->uis.count           = 1;
    manager->texts.count         = 1;
    manager->rects.count         = 1;
    manager->buttons.count       = 1;
    manager->audios.count        = 1;
    manager->cursors.count       = 1;
    manager->projectiles.count   = 1;
    manager->poisonClouds.count  = 1;
    manager->sights.count        = 1;

    // initialize entity by type map
    WarEntityMapInit(&manager->entitiesByType, wm_globalAllocator(), we_hashEntityType, we_equalsEntityType);
    for (WarEntityType type = WAR_ENTITY_TYPE_IMAGE; type < WAR_ENTITY_TYPE_COUNT; type++)
    {
        WarEntityList* list = (WarEntityList*)wm_alloc(sizeof(WarEntityList));
        WarEntityListInit(list, wm_globalAllocator());
        WarEntityMapSet(&manager->entitiesByType, type, list);
    }

    // initialize unit by type map
    WarUnitMapInit(&manager->unitsByType, wm_globalAllocator(), wu_hashUnitType, wu_equalsUnitType);
    for (WarUnitType type = WAR_UNIT_FOOTMAN; type < WAR_UNIT_COUNT; type++)
    {
        WarEntityList* list = (WarEntityList*)wm_alloc(sizeof(WarEntityList));
        WarEntityListInit(list, wm_globalAllocator());
        WarUnitMapSet(&manager->unitsByType, type, list);
    }

    // initialize the entities by id map
    WarEntityIdMapInit(&manager->entitiesById, wm_globalAllocator(), we_hashEntityId, we_equalsEntityId);

    // initialize ui entities list
    WarEntityListInit(&manager->uiEntities, wm_globalAllocator());

    TracyCZoneEnd(ctx);
}

WarEntityManager* we_getEntityManager(WarContext* context)
{
    if (context->scene)
        return &context->scene->entityManager;

    if (context->map)
        return &context->map->entityManager;

    logError("There is no map or scene active.");
    return NULL;
}

WarEntityList* we_getEntitiesOfType(WarContext* context, WarEntityType type)
{
    TracyCZoneN(ctx, "we_getEntitiesOfType", 1);
    WarEntityManager* manager = we_getEntityManager(context);
    WarEntityList* list = WarEntityMapGet(&manager->entitiesByType, type);
    TracyCZoneEnd(ctx);
    return list;
}

WarEntityList* we_getUnitsOfType(WarContext* context, WarUnitType type)
{
    TracyCZoneN(ctx, "we_getUnitsOfType", 1);
    WarEntityManager* manager = we_getEntityManager(context);
    WarEntityList* list = WarUnitMapGet(&manager->unitsByType, type);
    TracyCZoneEnd(ctx);
    return list;
}

WarEntityList* we_getUIEntities(WarContext* context)
{
    WarEntityManager* manager = we_getEntityManager(context);
    return &manager->uiEntities;
}

// Render entities
s32 renderCompareUnits(const WarEntity* e1, const WarEntity* e2, void* userdata)
{
    WarContext* context = (WarContext*)userdata;
    assert(context != NULL);
    assert(wu_isUnit(e1));
    assert(wu_isUnit(e2));

    if (e1->id == e2->id)
    {
        return 0;
    }

    bool isDead1 = wu_isCorpseUnit(context, (WarEntity*)e1) || wst_isDead(context, (WarEntity*)e1) || wst_isGoingToDie(context, (WarEntity*)e1);
    bool isDead2 = wu_isCorpseUnit(context, (WarEntity*)e2) || wst_isDead(context, (WarEntity*)e2) || wst_isGoingToDie(context, (WarEntity*)e2);

    if (isDead1 && !isDead2)
    {
        return -1;
    }

    if (!isDead1 && isDead2)
    {
        return 1;
    }

    vec2 p1 = wu_getUnitPosition(context, (WarEntity*)e1, false);
    vec2 p2 = wu_getUnitPosition(context, (WarEntity*)e2, false);

    if (p1.y != p2.y)
    {
        return (p1.y < p2.y) ? -1 : 1;
    }

    return (e1->id < e2->id) ? -1 : 1;
}

s32 renderCompareProjectiles(const WarEntity* e1, const WarEntity* e2, void* userdata)
{
    WarContext* context = (WarContext*)userdata;
    assert(context != NULL);

    if (e1->id == e2->id)
    {
        return 0;
    }

    WarTransformComponent* t1 = we_getTransformComponent(context, (WarEntity*)e1);
    WarTransformComponent* t2 = we_getTransformComponent(context, (WarEntity*)e2);

    vec2 p1 = t1 ? t1->position : VEC2_ZERO;
    vec2 p2 = t2 ? t2->position : VEC2_ZERO;

    if (p1.y != p2.y)
    {
        return (p1.y < p2.y) ? -1 : 1;
    }

    return (e1->id < e2->id) ? -1 : 1;
}

void renderImage(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "renderImage", 1);

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    if (we_isComponentEnabled(context, entity, COMP_UI) &&
        we_isComponentEnabled(context, entity, COMP_SPRITE))
    {
        WarSpriteComponent* sprite = we_getSpriteComponent(context, entity);
        assert(sprite);

        if (sprite->frameIndex >= 0)
        {
            wr_save(context);

            if (sprite->sprite.framesCount > 1)
            {
                WarSpriteFrame frame = wspr_getSpriteFrame(context, sprite->sprite, sprite->frameIndex);
                wspr_updateSpriteImage(context, sprite->sprite, frame.data);

                wr_translate(context, -(f32)frame.dx, -(f32)frame.dy);
            }

            wr_translate(context, transform->position.x, transform->position.y);
            wspr_renderSprite(context, sprite->sprite, VEC2_ZERO, VEC2_ONE);
            wr_restore(context);
        }
    }


    TracyCZoneEnd(ctx);
}

void renderRoad(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "renderRoad", 1);

    if (we_isComponentEnabled(context, entity, COMP_SPRITE) &&
        we_isComponentEnabled(context, entity, COMP_ROAD))
    {
        WarSpriteComponent* sprite = we_getSpriteComponent(context, entity);
        assert(sprite);

        WarRoadComponent* road = we_getRoadComponent(context, entity);
        assert(road);

        // the roads are only for forest and swamp maps
        WarMapTilesetType tilesetType = context->map->tilesetType;
        assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

        WarRoadPieceList* pieces = &road->pieces;
        for (s32 i = 0; i < pieces->count; i++)
        {
            // get the index of the tile in the spritesheet of the map,
            // corresponding to the current tileset type (forest, swamp)
            const WarRoadData* roadData = wu_getRoadData(pieces->items[i].type);

            s32 tileIndex = (tilesetType == MAP_TILESET_FOREST)
                ? roadData->tileIndexForest
                : roadData->tileIndexSwamp;

            // the position in the world of the road piece tile
            s32 x = pieces->items[i].tilex;
            s32 y = pieces->items[i].tiley;

            // coordinates in pixels of the road piece tile
            s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
            s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

            wr_save(context);
            wr_translate(context, (f32)(x * MEGA_TILE_WIDTH), (f32)(y * MEGA_TILE_HEIGHT));

            rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            wr_subImage(context, sprite->sprite.texture, rs, rd, VEC2_ONE);

            wr_restore(context);
        }
    }

    TracyCZoneEnd(ctx);
}

void renderWall(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "renderWall", 1);

    if (we_isComponentEnabled(context, entity, COMP_SPRITE) &&
        we_isComponentEnabled(context, entity, COMP_WALL))
    {
        WarSpriteComponent* sprite = we_getSpriteComponent(context, entity);
        assert(sprite);

        WarWallComponent* wall = we_getWallComponent(context, entity);
        assert(wall);

        // the walls are only for forest and swamp maps
        WarMapTilesetType tilesetType = context->map->tilesetType;
        assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

        WarWallPieceList* pieces = &wall->pieces;
        for (s32 i = 0; i < pieces->count; i++)
        {
            WarWallPiece* piece = &pieces->items[i];

            // get the index of the tile in the spritesheet of the map,
            // corresponding to the current tileset type (forest, swamp)
            const WarWallData* wallData = wu_getWallData(piece->type);

            s32 tileIndex = 0;

            s32 hpPercent = PERCENTABI(piece->hp, piece->maxhp);
            if (hpPercent <= 0)
            {
                tileIndex = (tilesetType == MAP_TILESET_FOREST)
                    ? wallData->tileDestroyedForest : wallData->tileDestroyedSwamp;
            }
            else if(hpPercent < 50)
            {
                tileIndex = (tilesetType == MAP_TILESET_FOREST)
                    ? wallData->tileDamagedForest : wallData->tileDamagedSwamp;
            }
            else
            {
                tileIndex = (tilesetType == MAP_TILESET_FOREST)
                    ? wallData->tileForest : wallData->tileSwamp;
            }

            // the position in the world of the wall piece tile
            s32 x = piece->tilex;
            s32 y = piece->tiley;

            // coordinates in pixels of the wall piece tile
            s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
            s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

            wr_save(context);
            wr_translate(context, (f32)(x * MEGA_TILE_WIDTH), (f32)(y * MEGA_TILE_HEIGHT));

            rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            wr_subImage(context, sprite->sprite.texture, rs, rd, VEC2_ONE);

            wr_restore(context);
        }
    }

    TracyCZoneEnd(ctx);
}

void renderRuin(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "renderRuin", 1);

    if (we_isComponentEnabled(context, entity, COMP_SPRITE) &&
        we_isComponentEnabled(context, entity, COMP_RUIN))
    {
        WarSpriteComponent* sprite = we_getSpriteComponent(context, entity);
        assert(sprite);

        WarRuinComponent* ruin = we_getRuinComponent(context, entity);
        assert(ruin);

        // the walls are only for forest and swamp maps
        WarMapTilesetType tilesetType = context->map->tilesetType;
        assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

        WarRuinPieceList* pieces = &ruin->pieces;
        for (s32 i = 0; i < ruin->pieces.count; i++)
        {
            WarRuinPiece* piece = &pieces->items[i];
            if (piece->type == WAR_RUIN_PIECE_NONE)
                continue;

            // get the index of the tile in the spritesheet of the map,
            // corresponding to the current tileset type (forest, swamp)
            const WarRuinData* ruinData = wu_getRuinData(piece->type);

            s32 tileIndex = (tilesetType == MAP_TILESET_FOREST)
                ? ruinData->tileIndexForest : ruinData->tileIndexSwamp;

            // the position in the world of the road piece tile
            s32 x = piece->tilex;
            s32 y = piece->tiley;

            // coordinates in pixels of the road piece tile
            s32 tilePixelX = (tileIndex % TILESET_TILES_PER_ROW) * MEGA_TILE_WIDTH;
            s32 tilePixelY = ((tileIndex / TILESET_TILES_PER_ROW) * MEGA_TILE_HEIGHT);

            wr_save(context);
            wr_translate(context, (f32)(x * MEGA_TILE_WIDTH), (f32)(y * MEGA_TILE_HEIGHT));

            rect rs = recti(tilePixelX, tilePixelY, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            rect rd = recti(0, 0, MEGA_TILE_WIDTH, MEGA_TILE_HEIGHT);
            wr_subImage(context, sprite->sprite.texture, rs, rd, VEC2_ONE);

            wr_restore(context);
        }
    }

    TracyCZoneEnd(ctx);
}

void renderForest(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "renderForest", 1);

    if (we_isComponentEnabled(context, entity, COMP_FOREST))
    {
        WarForestComponent* forest = we_getForestComponent(context, entity);
        assert(forest);

        // the wood are only for forest and swamp maps
        WarMapTilesetType tilesetType = context->map->tilesetType;
        assert(tilesetType == MAP_TILESET_FOREST || tilesetType == MAP_TILESET_SWAMP);

        WarTreeList* trees = &forest->trees;
        for (s32 i = 0; i < trees->count; i++)
        {
            WarTree* tree = &trees->items[i];

            if (tree->type == WAR_TREE_NONE)
                continue;

            const WarTreeData* data = wu_getTreeData(tree->type);

            // the position in the world of the wood tile
            s32 x = tree->tilex;
            s32 y = tree->tiley;

            s32 prevTileIndex = wmap_getMapTileIndex(context, x, y);
            s32 newTileIndex = (tilesetType == MAP_TILESET_FOREST) ? data->tileIndexForest : data->tileIndexSwamp;

            if (prevTileIndex != newTileIndex)
                logDebug("different tile index for tree (%d, %d), prev: %d, new: %d", x, y, prevTileIndex, newTileIndex);

            wmap_setMapTileIndex(context, x, y, newTileIndex);
        }
    }

    TracyCZoneEnd(ctx);
}

void renderUnit(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "renderUnit", 1);

    WarMap* map = context->map;

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    // position of the unit in the map
    vec2 position = transform->position;

    // scale of the unit: this is modified by animations when the animation indicates that it
    // should flip horizontally or vertically or both
    vec2 scale = transform->scale;

    // size of the original sprite
    vec2 frameSize = wu_getUnitFrameSize(context, entity);

    // size of the unit
    vec2 unitSize = wu_getUnitSpriteSize(context, entity);

    // the unit is visible if it's partially on the clear areas of the fog
    bool isVisible = wmap_isUnitPartiallyVisible(context, map, entity);

    wr_translate(context, -0.5f * frameSize.x, -0.5f * frameSize.y);
    wr_translate(context, 0.5f * unitSize.x, 0.5f * unitSize.y);
    wr_translate(context, position.x, position.y);

#ifdef DEBUG_RENDER_UNIT_INFO
    wr_fillRect(context, wu_getUnitFrameRect(context, entity), WAR_COLOR_RGBA(0, 0, 128, 128));
    wr_fillRect(context, wu_getUnitSpriteRect(context, entity), WAR_COLOR_GRAY_TRANSPARENT);
    wr_fillRect(context, rectv(wu_getUnitSpriteCenter(context, entity), VEC2_ONE), WAR_COLOR_RGB(255, 0, 0));
#endif

    if (we_isComponentEnabled(context, entity, COMP_SPRITE) && (isVisible || unit->hasBeenSeen))
    {
        WarSpriteComponent* sprite = we_getSpriteComponent(context, entity);
        assert(sprite);

        wr_save(context);

        if (wu_isDudeUnit(context, entity))
        {
            if (unit->invisible)
            {
                wr_globalAlpha(context, 0.5f);
            }

            if (unit->invulnerable)
            {
                rect unitRect = wu_getUnitSpriteRect(context, entity);
                unitRect = rect_expand(unitRect, -1, -1);
                wr_strokeRect(context, unitRect, WAR_COLOR_BLUE_INVULNERABLE, 1);
            }
        }

        WarSpriteFrame frame = wspr_getSpriteFrame(context, sprite->sprite, sprite->frameIndex);
        wspr_updateSpriteImage(context, sprite->sprite, frame.data);
        wspr_renderSprite(context, sprite->sprite, VEC2_ZERO, scale);

        wr_restore(context);
    }

    if (we_isComponentEnabled(context, entity, COMP_ANIMATIONS) && isVisible)
    {
        WarAnimationsComponent* animations = we_getAnimationsComponent(context, entity);
        assert(animations);

        for (s32 i = 0; i < animations->animations.count; i++)
        {
            WarSpriteAnimation* anim = &animations->animations.items[i];
            if (anim->status == WAR_ANIM_STATUS_RUNNING)
            {
                wr_save(context);

                wr_translate(context, anim->offset.x, anim->offset.y);
                wr_scale(context, anim->scale.x, anim->scale.y);

#ifdef DEBUG_RENDER_UNIT_ANIMATIONS
                // size of the original sprite
                vec2 animFrameSize = vec2i(anim->sprite.frameWidth, anim->sprite.frameHeight);

                wr_fillRect(context, rectv(VEC2_ZERO, animFrameSize), WAR_COLOR_GRAY_TRANSPARENT);
#endif

                s32 animFrameIndex = (s32)(anim->animTime * anim->frames.count);
                animFrameIndex = CLAMP(animFrameIndex, 0, anim->frames.count - 1);

                s32 spriteFrameIndex = anim->frames.items[animFrameIndex];
                WarSpriteFrame frame = wspr_getSpriteFrame(context, anim->sprite, spriteFrameIndex);

                wspr_updateSpriteImage(context, anim->sprite, frame.data);
                wspr_renderSprite(context, anim->sprite, VEC2_ZERO, VEC2_ONE);

                wr_restore(context);
            }
        }
    }

    TracyCZoneEnd(ctx);
}

void renderText(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "renderText", 1);

    if (we_isComponentEnabled(context, entity, COMP_UI) &&
        we_isComponentEnabled(context, entity, COMP_TEXT))
    {
        WarTransformComponent* transform = we_getTransformComponent(context, entity);
        assert(transform);

        WarTextComponent* text = we_getTextComponent(context, entity);

        wr_save(context);
        wr_translate(context, transform->position.x, transform->position.y);
        wr_scale(context, transform->scale.x, transform->scale.y);

        WarFontParams params;
        params.fontIndex = text->fontIndex;
        params.fontSize = text->fontSize;
        params.lineHeight = text->lineHeight;
        params.fontColor = text->fontColor;
        params.highlightColor = text->highlightColor;
        params.highlightIndex = text->highlightIndex;
        params.highlightCount = text->highlightCount;
        params.boundings = text->boundings;
        params.horizontalAlign = text->horizontalAlign;
        params.verticalAlign = text->verticalAlign;
        params.lineAlign = text->lineAlign;
        params.wrapping = text->wrapping;
        params.trimming = text->trimming;
        params.fontSprite = context->fontSprites[text->fontIndex];
        params.fontData = getFontData(text->fontIndex);

        if (text->multiline)
            wfont_renderMultiSpriteText(context, wstr_view(&text->text), 0, 0, params);
        else
            wfont_renderSingleSpriteText(context, wstr_view(&text->text), 0, 0, params);

        wr_restore(context);
    }

    TracyCZoneEnd(ctx);
}

void renderRect(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "renderRect", 1);

    if (we_isComponentEnabled(context, entity, COMP_UI) &&
        we_isComponentEnabled(context, entity, COMP_RECT))
    {
        WarTransformComponent* transform = we_getTransformComponent(context, entity);
        assert(transform);

        WarRectComponent* rectComp = we_getRectComponent(context, entity);

        wr_save(context);
        wr_translate(context, transform->position.x, transform->position.y);
        wr_scale(context, transform->scale.x, transform->scale.y);

        wr_fillRect(context, rectf(0.0f, 0.0f, rectComp->size.x, rectComp->size.y), rectComp->color);

        wr_restore(context);
    }

    TracyCZoneEnd(ctx);
}

void renderButton(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "renderButton", 1);

    WarTransformComponent* transform = we_getTransformComponent(context, entity);
    assert(transform);

    WarButtonComponent* button = we_getButtonComponent(context, entity);
    WarTextComponent* text = we_getTextComponent(context, entity);
    WarSpriteComponent* sprite = we_getSpriteComponent(context, entity);

    if (we_isComponentEnabled(context, entity, COMP_UI) &&
        we_isComponentEnabled(context, entity, COMP_BUTTON))
    {
        wr_save(context);
        wr_translate(context, transform->position.x, transform->position.y);
        wr_scale(context, transform->scale.x, transform->scale.y);

        // render background
        {
            WarSprite backgroundSprite = button->active ? button->pressedSprite : button->normalSprite;
            wspr_renderSprite(context, backgroundSprite, VEC2_ZERO, VEC2_ONE);
        }

        // render foreground
        {
            if (we_isComponentEnabled(context, entity, COMP_SPRITE))
            {
                vec2 backgroundSize = vec2i(button->normalSprite.frameWidth, button->normalSprite.frameHeight);
                vec2 foregroundSize = vec2i(sprite->sprite.frameWidth, sprite->sprite.frameHeight);
                vec2 offset = vec2_half(vec2_subv(backgroundSize, foregroundSize));

                if (button->active)
                    offset = vec2_addv(offset, vec2i(0, 1));

                wr_translate(context, offset.x, offset.y);

                WarSpriteFrame frame = wspr_getSpriteFrame(context, sprite->sprite, sprite->frameIndex);
                wspr_updateSpriteImage(context, sprite->sprite, frame.data);
                wspr_renderSprite(context, sprite->sprite, VEC2_ZERO, VEC2_ONE);
            }
        }

        // render text
        {
            if (we_isComponentEnabled(context, entity, COMP_TEXT))
            {
                WarFontParams params;
                params.fontIndex = text->fontIndex;
                params.fontSize = text->fontSize;
                params.fontColor = text->fontColor;
                params.highlightColor = text->highlightColor;
                params.highlightIndex = button->hot ? ALL_HIGHLIGHT : text->highlightIndex;
                params.highlightCount = text->highlightCount;
                params.boundings = text->boundings;
                params.horizontalAlign = text->horizontalAlign;
                params.verticalAlign = text->verticalAlign;
                params.lineAlign = text->lineAlign;
                params.wrapping = text->wrapping;
                params.fontSprite = context->fontSprites[text->fontIndex];
                params.fontData = getFontData(text->fontIndex);

                wfont_renderSingleSpriteText(context, wstr_view(&text->text), 0, 0, params);
            }
        }

        wr_restore(context);
    }

    TracyCZoneEnd(ctx);
}

void renderProjectile(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "renderProjectile", 1);

    if (we_isComponentEnabled(context, entity, COMP_SPRITE))
    {
        WarTransformComponent* transform = we_getTransformComponent(context, entity);
        assert(transform);

        WarSpriteComponent* sprite = we_getSpriteComponent(context, entity);
        assert(sprite);

        vec2 position = transform->position;
        vec2 scale = transform->scale;

        WarSpriteFrame frame = wspr_getSpriteFrame(context, sprite->sprite, sprite->frameIndex);

#ifdef DEBUG_RENDER_PROJECTILES
        {
            wr_save(context);

            wr_translate(context, -sprite->sprite.frameWidth/2,-sprite->sprite.frameHeight/2);
            wr_translate(context, position.x, position.y);

            rect r = rectf(0, 0, sprite->sprite.frameWidth, sprite->sprite.frameHeight);
            wr_fillRect(context, r, WAR_COLOR_GRAY_TRANSPARENT);

            wr_restore(context);
        }

        {
            wr_save(context);

            wr_translate(context, -frame.w/2,-frame.h/2);
            wr_translate(context, position.x, position.y);

            rect r = rectf(0, 0, frame.w, frame.h);
            wr_fillRect(context, r, WAR_COLOR_RED_TRANSPARENT);

            wr_restore(context);
        }

        {
            WarProjectileComponent* projectile = we_getProjectileComponent(context, entity);
            assert(projectile);

            wr_save(context);

            wr_strokeLine(context, projectile->origin, projectile->target, wr_getColorFromList(entity->id), 0.5f);
            wr_fillRect(context, rectv(projectile->origin, VEC2_ONE), WAR_COLOR_RGB(255, 0, 255));
            wr_fillRect(context, rectv(projectile->target, VEC2_ONE), WAR_COLOR_RGB(255, 0, 255));

            wr_restore(context);
        }
#endif

        wr_translate(context, -(f32)frame.dx, -(f32)frame.dy);
        wr_translate(context, -0.5f * frame.w, -0.5f * frame.h);
        wr_translate(context, position.x, position.y);

        wspr_updateSpriteImage(context, sprite->sprite, frame.data);
        wspr_renderSprite(context, sprite->sprite, VEC2_ZERO, scale);
    }

    TracyCZoneEnd(ctx);
}

void renderMinimap(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "RenderMinimap", 1);

    WarMap* map = context->map;

    WarTransformComponent* minimapTransform = we_getTransformComponent(context, entity);
    assert(minimapTransform);

    vec2 position = minimapTransform->position;

    WarSpriteFrame* frame0 = &map->minimapSprite.frames[0];
    WarSpriteFrame* frame1 = &map->minimapSprite.frames[1];

    // copy the minimap base to the first frame which is the one that will be rendered
    // copy only the visible tiles/pixels
    for(s32 y = 0; y < MAP_TILES_HEIGHT; y++)
    {
        for(s32 x = 0; x < MAP_TILES_WIDTH; x++)
        {
            s32 index = y * MAP_TILES_WIDTH + x;
            WarMapTile* tile = &map->tiles[index];

            if (!map->fowEnabled ||
                tile->state == MAP_TILE_STATE_VISIBLE ||
                tile->state == MAP_TILE_STATE_FOG)
            {
                frame0->data[index * 4 + 0] = frame1->data[index * 4 + 0];
                frame0->data[index * 4 + 1] = frame1->data[index * 4 + 1];
                frame0->data[index * 4 + 2] = frame1->data[index * 4 + 2];
                frame0->data[index * 4 + 3] = frame1->data[index * 4 + 3];
            }
            else
            {
                frame0->data[index * 4 + 0] = 0;
                frame0->data[index * 4 + 1] = 0;
                frame0->data[index * 4 + 2] = 0;
                frame0->data[index * 4 + 3] = 255;
            }
        }
    }

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* unitEntity = units->items[i];
        if (unitEntity)
        {
            WarUnitComponent* unit = we_getUnitComponent(context, unitEntity);
            WarTransformComponent* transform = we_getTransformComponent(context, unitEntity);

            if (unit && transform && wu_displayUnitOnMinimap(context, unitEntity) && (wmap_isUnitPartiallyVisible(context, map, unitEntity) || unit->hasBeenSeen))
            {
                s32 tileX = (s32)(transform->position.x / MEGA_TILE_WIDTH);
                s32 tileY = (s32)(transform->position.y / MEGA_TILE_HEIGHT);

                WarColor color = wu_getUnitColorOnMinimap(context, unitEntity);

                for(s32 y = 0; y < unit->sizey; y++)
                {
                    for(s32 x = 0; x < unit->sizex; x++)
                    {
                        s32 pixel = (tileY + y) * MINIMAP_WIDTH + (tileX + x);
                        frame0->data[pixel * 4 + 0] = color.r;
                        frame0->data[pixel * 4 + 1] = color.g;
                        frame0->data[pixel * 4 + 2] = color.b;
                    }
                }
            }
        }
    }

    wr_save(context);
    wr_translate(context, position.x, position.y);

    // render base
    wspr_updateSpriteImage(context, map->minimapSprite, map->minimapSprite.frames[0].data);
    wspr_renderSprite(context, map->minimapSprite, VEC2_ZERO, VEC2_ONE);

    // render viewport
    wr_translate(context, (f32)map->camera.viewport.x * MINIMAP_MAP_WIDTH_RATIO, (f32)map->camera.viewport.y * MINIMAP_MAP_HEIGHT_RATIO);
    wr_strokeRect(context, rectf(0.0f, 0.0f, (f32)MINIMAP_VIEWPORT_WIDTH, (f32)MINIMAP_VIEWPORT_HEIGHT), WAR_COLOR_WHITE, 1.0f);

    wr_restore(context);

    TracyCZoneEnd(ctx);
}

void renderAnimation(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "RenderAnimation", 1);

    if (we_isComponentEnabled(context, entity, COMP_ANIMATIONS))
    {
        WarAnimationsComponent* animations = we_getAnimationsComponent(context, entity);
        assert(animations);

        WarTransformComponent* transform = we_getTransformComponent(context, entity);
        if (transform)
        {
            wr_translate(context, transform->position.x, transform->position.y);
        }

        for (s32 i = 0; i < animations->animations.count; i++)
        {
            WarSpriteAnimation* anim = &animations->animations.items[i];
            if (anim->status == WAR_ANIM_STATUS_RUNNING)
            {
                wr_save(context);

                wr_translate(context, anim->offset.x, anim->offset.y);
                wr_scale(context, anim->scale.x, anim->scale.y);

                s32 animFrameIndex = (s32)(anim->animTime * anim->frames.count);
                animFrameIndex = CLAMP(animFrameIndex, 0, anim->frames.count - 1);

                s32 spriteFrameIndex = anim->frames.items[animFrameIndex];
                WarSpriteFrame frame = wspr_getSpriteFrame(context, anim->sprite, spriteFrameIndex);

                wspr_updateSpriteImage(context, anim->sprite, frame.data);
                wspr_renderSprite(context, anim->sprite, VEC2_ZERO, VEC2_ONE);

                wr_restore(context);
            }
        }
    }

    TracyCZoneEnd(ctx);
}

void we_renderEntity(WarContext* context, WarEntity* entity)
{
    static WarRenderFunc renderFuncs[WAR_ENTITY_TYPE_COUNT] =
    {
        NULL,               // WAR_ENTITY_TYPE_NONE
        renderImage,        // WAR_ENTITY_TYPE_IMAGE
        renderUnit,         // WAR_ENTITY_TYPE_UNIT
        renderRoad,         // WAR_ENTITY_TYPE_ROAD
        renderWall,         // WAR_ENTITY_TYPE_WALL
        renderRuin,         // WAR_ENTITY_TYPE_RUIN
        renderForest,       // WAR_ENTITY_TYPE_FOREST
        renderText,         // WAR_ENTITY_TYPE_TEXT
        renderRect,         // WAR_ENTITY_TYPE_RECT
        renderButton,       // WAR_ENTITY_TYPE_BUTTON
        renderImage,        // WAR_ENTITY_TYPE_CURSOR
        NULL,               // WAR_ENTITY_TYPE_AUDIO
        renderProjectile,   // WAR_ENTITY_TYPE_PROJECTILE
        NULL,               // WAR_ENTITY_TYPE_RAIN_OF_FIRE
        renderAnimation,    // WAR_ENTITY_TYPE_POISON_CLOUD
        NULL,               // WAR_ENTITY_TYPE_SIGHT
        renderMinimap,      // WAR_ENTITY_TYPE_MINIMAP
        renderAnimation,    // WAR_ENTITY_TYPE_ANIMATION
    };

    TracyCZoneN(ctx, "we_renderEntity", 1);

    if (entity->id != 0)
    {
        WarRenderFunc renderFunc = renderFuncs[(s32)entity->type];
        if (!renderFunc)
        {
            logError("Entity of type %d can't be render. renderFunc = NULL", entity->type);
            return;
        }

        wr_save(context);
        renderFunc(context, entity);
        wr_restore(context);
    }

    TracyCZoneEnd(ctx);
}

void we_renderEntitiesOfType(WarContext* context, WarEntityType type)
{
    static WarRenderCompareFunc renderCompareFuncs[WAR_ENTITY_TYPE_COUNT] =
    {
        NULL,                       // WAR_ENTITY_TYPE_NONE,
        NULL,                       // WAR_ENTITY_TYPE_IMAGE,
        renderCompareUnits,         // WAR_ENTITY_TYPE_UNIT,
        NULL,                       // WAR_ENTITY_TYPE_ROAD,
        NULL,                       // WAR_ENTITY_TYPE_WALL,
        NULL,                       // WAR_ENTITY_TYPE_RUIN,
        NULL,                       // WAR_ENTITY_TYPE_FOREST,
        NULL,                       // WAR_ENTITY_TYPE_TEXT,
        NULL,                       // WAR_ENTITY_TYPE_RECT,
        NULL,                       // WAR_ENTITY_TYPE_BUTTON,
        NULL,                       // WAR_ENTITY_TYPE_CURSOR,
        NULL,                       // WAR_ENTITY_TYPE_AUDIO,
        renderCompareProjectiles,   // WAR_ENTITY_TYPE_PROJECTILE,
        NULL,                       // WAR_ENTITY_TYPE_RAIN_OF_FIRE,
        NULL,                       // WAR_ENTITY_TYPE_POISON_CLOUD,
        NULL,                       // WAR_ENTITY_TYPE_SIGHT,
        NULL,                       // WAR_ENTITY_TYPE_MINIMAP,
        NULL,                       // WAR_ENTITY_TYPE_ANIMATION,
    };

    TracyCZoneN(ctx, "we_renderEntitiesOfType", 1);

    WarEntityList* entities = we_getEntitiesOfType(context, type);

    // lookup the render compare function and sort the list
    if (inRange(type, WAR_ENTITY_TYPE_NONE, WAR_ENTITY_TYPE_COUNT))
    {
        WarRenderCompareFunc compareFunc = renderCompareFuncs[type];
        if (compareFunc)
        {
            // the idea of sorting the entities before render is
            // to allow render order to be the most correct as possible
            // for instance, corpses need to render before every other unit
            // and then render units by the `y` position on the map

            WarEntityListSort(entities, compareFunc, context);
        }
    }

    for(s32 i = 0; i < entities->count; i++)
    {
        WarEntity* entity = entities->items[i];
        if (entity)
        {
            we_renderEntity(context, entity);
        }
    }

    TracyCZoneEnd(ctx);
}

void we_renderUnitSelection(WarContext* context)
{
    TracyCZoneN(ctx, "RenderUnitSelection", 1);

    WarMap* map = context->map;

    WarEntityIdList* selectedEntities = &map->selectedEntities;
    for (s32 i = 0; i < selectedEntities->count; i++)
    {
        WarEntityId entityId = selectedEntities->items[i];
        WarEntity* entity = we_findEntity(context, entityId);
        if (entity &&
            we_isComponentEnabled(context, entity, COMP_UNIT) &&
            we_isComponentEnabled(context, entity, COMP_SPRITE))
        {
            WarTransformComponent* transform = we_getTransformComponent(context, entity);
            assert(transform);

            // size of the original sprite
            vec2 frameSize = wu_getUnitFrameSize(context, entity);

            // size of the unit
            vec2 unitSize = wu_getUnitSpriteSize(context, entity);

            // position of the unit in the map
            vec2 position = transform->position;

            wr_save(context);
            wr_translate(context, -0.5f * frameSize.x, -0.5f * frameSize.y);
            wr_translate(context, 0.5f * unitSize.x, 0.5f * unitSize.y);
            wr_translate(context, position.x, position.y);

            rect selr = rectf(
                0.5f * (frameSize.x - unitSize.x),
                0.5f * (frameSize.y - unitSize.y),
                unitSize.x,
                unitSize.y
            );

            WarColor color = WAR_COLOR_WHITE_SELECTION;
            if (wu_isFriendlyUnit(context, entity))
                color = WAR_COLOR_GREEN_SELECTION;
            else if (wu_isEnemyUnit(context, entity))
                color = WAR_COLOR_RED_SELECTION;

            wr_strokeRect(context, selr, color, 1.0f);

            wr_restore(context);
        }
    }

    TracyCZoneEnd(ctx);
}

void we_increaseUpgradeLevel(WarContext* context, WarPlayerInfo* player, WarUpgradeType upgrade)
{
    assert(hasRemainingUpgrade(player, upgrade));

    incrementUpgradeLevel(player, upgrade);

    switch (upgrade)
    {
        case WAR_UPGRADE_ARROWS:
        case WAR_UPGRADE_SPEARS:
        {
            WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
            for (s32 i = 0; i < units->count; i++)
            {
                WarEntity* entity = units->items[i];
                WarUnitComponent* uc = entity ? we_getUnitComponent(context, entity) : NULL;
                if (uc && uc->player == player->index)
                {
                    if (uc->type == WAR_UNIT_ARCHER ||
                        uc->type == WAR_UNIT_SPEARMAN)
                    {
                        uc->minDamage += 2;
                    }
                }
            }

            break;
        }

        case WAR_UPGRADE_SWORDS:
        case WAR_UPGRADE_AXES:
        {
            WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
            for (s32 i = 0; i < units->count; i++)
            {
                WarEntity* entity = units->items[i];
                WarUnitComponent* uc = entity ? we_getUnitComponent(context, entity) : NULL;
                if (uc && uc->player == player->index)
                {
                    if (uc->type == WAR_UNIT_FOOTMAN ||
                        uc->type == WAR_UNIT_GRUNT ||
                        uc->type == WAR_UNIT_KNIGHT ||
                        uc->type == WAR_UNIT_RAIDER)
                    {
                        if (getUpgradeLevel(player, upgrade) == 1)
                        {
                            uc->minDamage += 1;
                            uc->rndDamage += 1;
                        }
                        else if (getUpgradeLevel(player, upgrade) == 2)
                        {
                            uc->minDamage += 2;
                            uc->rndDamage += 2;
                        }
                    }
                }
            }

            break;
        }

        case WAR_UPGRADE_HORSES:
        case WAR_UPGRADE_WOLVES:
        {
            WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
            for (s32 i = 0; i < units->count; i++)
            {
                WarEntity* entity = units->items[i];
                WarUnitComponent* uc = entity ? we_getUnitComponent(context, entity) : NULL;
                if (uc && uc->player == player->index)
                {
                    if (wu_isDudeUnit(context, entity))
                    {
                        uc->speed++;
                    }
                }
            }

            break;
        }

        case WAR_UPGRADE_SHIELD:
        {
            WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
            for (s32 i = 0; i < units->count; i++)
            {
                WarEntity* entity = units->items[i];
                WarUnitComponent* uc = entity ? we_getUnitComponent(context, entity) : NULL;
                if (uc && uc->player == player->index)
                {
                    if (getUpgradeLevel(player, upgrade) == 1)
                    {
                        uc->armor += 1;
                    }
                    else if (getUpgradeLevel(player, upgrade) == 2)
                    {
                        uc->armor += 2;
                    }
                }
            }

            break;
        }

        default:
        {
            logInfo("This upgrade type %d doesn't increase any value of the units", upgrade);
            break;
        }
    }
}

bool we_enoughPlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood)
{
    NOT_USED(context);

    return player->gold >= gold && player->wood >= wood;
}

bool we_decreasePlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood)
{
    if (player->gold < gold)
    {
        wmui_setFlashStatus(context, 1.5f, wstr_fromCString("NOT ENOUGH GOLD... MINE MORE GOLD"));
        return false;
    }

    if (player->wood < wood)
    {
        wmui_setFlashStatus(context, 1.5f, wstr_fromCString("NOT ENOUGH LUMBER... CHOP MORE TREES"));
        return false;
    }

    player->gold -= gold;
    player->wood -= wood;
    return true;
}

void we_increasePlayerResources(WarContext* context, WarPlayerInfo* player, s32 gold, s32 wood)
{
    NOT_USED(context);

    player->gold += gold;
    player->wood += wood;
}

bool we_increaseUnitHp(WarContext* context, WarEntity* entity, s32 hp)
{
    assert(wu_isUnit(entity));

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    unit->hp += hp;
    unit->hp = MIN(unit->hp, unit->maxhp);

    return true;
}

bool we_decreaseUnitHp(WarContext* context, WarEntity* entity, s32 hp)
{
    assert(wu_isUnit(entity));

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    unit->hp -= hp;
    unit->hp = MAX(unit->hp, 0);

    return true;
}

bool we_decreaseUnitMana(WarContext* context, WarEntity* entity, s32 mana)
{
    assert(wu_isUnit(entity));

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    if (unit->mana < mana)
    {
        wmui_setFlashStatus(context, 1.5f, wstr_fromCString("NOT ENOUGH MANA"));
        return false;
    }

    unit->mana = MAX(unit->mana - mana, 0);
    return true;
}

void we_increaseUnitMana(WarContext* context, WarEntity* entity, s32 mana)
{
    assert(wu_isUnit(entity));

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);
    unit->mana = CLAMP(unit->mana + mana, 0, unit->maxMana);
}

bool we_enoughFarmFood(WarContext* context, WarPlayerInfo* player)
{
    WarUnitType farmType = wu_getUnitTypeForRace(WAR_UNIT_FARM_HUMANS, player->race);
    s32 farmCount = wu_getNumberOfBuildingsOfType(context, player->index, farmType, true);
    s32 foodCount = farmCount * 4 + 1;

    s32 dudesCount = wu_getTotalNumberOfDudes(context, player->index);
    return dudesCount + 1 <= foodCount;
}

bool we_checkFarmFood(WarContext* context, WarPlayerInfo* player)
{
    if (!we_enoughFarmFood(context, player))
    {
        wmui_setFlashStatus(context, 1.5f, wstr_fromCString("NOT ENOUGH FOOD... BUILD MORE FARMS"));
        return false;
    }

    return true;
}

bool we_checkRectToBuild(WarContext* context, s32 x, s32 y, s32 w, s32 h)
{
    WarMap* map = context->map;

    for (s32 dy = 0; dy < h; dy++)
    {
        for (s32 dx = 0; dx < w; dx++)
        {
            s32 xx = x + dx;
            s32 yy = y + dy;
            if (inRange(xx, 0, MAP_TILES_WIDTH) && inRange(yy, 0, MAP_TILES_HEIGHT))
            {
                if (!isEmpty(map->finder, xx, yy) || wmap_isTileUnkown(map, xx, yy))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

bool we_checkTileToBuild(WarContext* context, WarUnitType buildingToBuild, s32 x, s32 y)
{
    const WarUnitData* unitData = wu_getUnitData(buildingToBuild);

    if (!we_checkRectToBuild(context, x, y, unitData->sizex, unitData->sizey))
    {
        wmui_setFlashStatus(context, 1.5f, wstr_fromCString("CAN'T BUILD THERE"));
        return false;
    }

    return true;
}

bool we_checkTileToBuildRoadOrWall(WarContext* context, s32 x, s32 y)
{
    if (!we_checkRectToBuild(context, x, y, 1, 1))
    {
        wmui_setFlashStatus(context, 1.5f, wstr_fromCString("CAN'T BUILD THERE"));
        return false;
    }

    return true;
}

WarEntityList* we_getNearUnits(WarContext* context, vec2 tilePosition, s32 distance)
{
    TracyCZoneN(ctx, "GetNearUnits", 1);

    WarEntityList* nearUnits = (WarEntityList*)wm_allocFrame(sizeof(WarEntityList));
    WarEntityListInit(nearUnits, wm_frameAllocator());

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* other = units->items[i];
        if (other && wu_tileInRange(context, other, tilePosition, distance))
        {
            WarEntityListAdd(nearUnits, other);
        }
    }

    TracyCZoneEnd(ctx);
    return nearUnits;
}

WarEntity* we_getNearEnemy(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "GetNearEnemy", 1);

    vec2 position = wu_getUnitCenterPosition(context, entity, true);

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* other = units->items[i];
        if (other && wu_areEnemies(context, entity, other) && wu_canAttack(context, entity, other))
        {
            if (wu_isUnit(other))
            {
                WarUnitComponent* uc = we_getUnitComponent(context, other);
                if (uc && uc->invisible)
                    continue;
            }

            if (wu_tileInRange(context, other, position, NEAR_ENEMY_RADIUS))
            {
                TracyCZoneEnd(ctx);
                return other;
            }
        }
    }

    TracyCZoneEnd(ctx);
    return NULL;
}

bool we_isBeingAttackedBy(WarContext* context, WarEntity* entity, WarEntity* other)
{
    if (!wst_isFollowing(context, other) && !wst_isMoving(context, other))
    {
        WarState* attackState = wst_getAttackState(context, other);
        return attackState && attackState->attack.targetEntityId == entity->id;
    }

    return false;
}

bool we_isBeingAttacked(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "we_isBeingAttacked", 1);

    bool result = false;

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* other = units->items[i];
        if (we_isBeingAttackedBy(context, entity, other))
        {
            result = true;
            break;
        }
    }

    TracyCZoneEnd(ctx);

    return result;
}

WarEntity* we_getAttacker(WarContext* context, WarEntity* entity)
{
    TracyCZoneN(ctx, "we_getAttacker", 1);

    WarEntity* result = NULL;

    WarEntityList* units = we_getEntitiesOfType(context, WAR_ENTITY_TYPE_UNIT);
    for(s32 i = 0; i < units->count; i++)
    {
        WarEntity* other = units->items[i];
        if (other && we_isBeingAttackedBy(context, entity, other))
        {
            result = other;
            break;
        }
    }

    TracyCZoneEnd(ctx);

    return result;
}

WarEntity* we_getAttackTarget(WarContext* context, WarEntity* entity)
{
    if (!wst_isFollowing(context, entity) && !wst_isMoving(context, entity))
    {
        WarState* attackState = wst_getAttackState(context, entity);
        if (attackState)
        {
            WarEntityId targetEntityId = (WarEntityId)attackState->attack.targetEntityId;
            return we_findEntity(context, targetEntityId);
        }
    }

    return NULL;
}

s32 we_getTotalDamage(s32 minDamage, s32 rndDamage, s32 armor)
{
    return minDamage + MAX(rndDamage - armor, 0);
}

void we_takeDamage(WarContext* context, WarEntity *entity, s32 minDamage, s32 rndDamage)
{
    WarMap* map = context->map;

    assert(wu_isUnit(entity));

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    if (unit->invulnerable)
        return;

    WarPlayerInfo* player = &map->players[unit->player];
    if (player->godMode)
        return;

    // Minimal damage + [Random damage - Enemy's Armor]
    s32 damage = we_getTotalDamage(minDamage, rndDamage, unit->armor);
    we_decreaseUnitHp(context, entity, damage);

    if (unit->hp == 0)
    {
        if (wu_isBuildingUnit(context, entity))
        {
            WarState* collapseState = wst_createCollapseState(context, entity);
            wst_changeNextState(context, entity, collapseState, true, true);

#ifndef WAR_EDITOR_BUILD
            vec2 position = wu_getUnitCenterPosition(context, entity, false);
            wa_createAudioRandomWithPosition(context, CREATE_AUDIO_ARGS_INIT(.randomFromId = WAR_BUILDING_COLLAPSE_1, .randomToId = WAR_BUILDING_COLLAPSE_3, .position = position, .hasPosition = true, .loop = false));
#endif
        }
        else
        {
            WarState* deathState = wst_createDeathState(context, entity);
            wst_changeNextState(context, entity, deathState, true, true);

#ifndef WAR_EDITOR_BUILD
            vec2 position = wu_getUnitCenterPosition(context, entity, false);

            if (unit->type == WAR_UNIT_SCORPION || unit->type == WAR_UNIT_SPIDER)
            {
                wa_createAudioWithPosition(context, CREATE_AUDIO_ARGS_INIT(.audioId=WAR_DEAD_SPIDER_SCORPION, .position = position, .hasPosition = true, .loop = false));
            }
            else if (unit->type == WAR_UNIT_CATAPULT_HUMANS || unit->type == WAR_UNIT_CATAPULT_ORCS)
            {
                wa_createAudioRandomWithPosition(context, CREATE_AUDIO_ARGS_INIT(.randomFromId = WAR_BUILDING_COLLAPSE_1, .randomToId = WAR_BUILDING_COLLAPSE_3, .position = position, .hasPosition = true, .loop = false));
            }
            else
            {
                WarAudioId audioId = wu_isHumanUnit(context, entity)? WAR_HUMAN_DEAD : WAR_ORC_DEAD;
                wa_createAudioWithPosition(context, CREATE_AUDIO_ARGS_INIT(.audioId = audioId, .position = position, .hasPosition = true, .loop = false));
            }
#endif
        }
    }
    else if (wu_isBuildingUnit(context, entity))
    {
        s32 hpPercent = PERCENTABI(unit->hp, unit->maxhp);
        if(hpPercent <= 33)
        {
            if (!wanim_containsAnimation(context, entity, wsv_fromCString("hugeDamage")))
            {
                wanim_removeAnimation(context, entity, wsv_fromCString("littleDamage"));
                wanim_createDamageAnimation(context, entity, wstr_fromCString("hugeDamage"), 2);
            }
        }
        else if(hpPercent <= 66)
        {
            if (!wanim_containsAnimation(context, entity, wsv_fromCString("littleDamage")))
            {
                wanim_createDamageAnimation(context, entity, wstr_fromCString("littleDamage"), 1);
            }
        }
    }
}

void we_takeWallDamage(WarContext* context, WarEntity* entity, WarWallPiece* piece, s32 minDamage, s32 rndDamage)
{
    NOT_USED(context);
    NOT_USED(entity);

    assert(wu_isWall(entity));

    // Minimal damage + [Random damage - Enemy's Armor]
    s32 damage = we_getTotalDamage(minDamage, rndDamage, 0);
    piece->hp -= damage;
    piece->hp = MAX(piece->hp, 0);
}

void we_rangeAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity)
{
    assert(wu_isUnit(entity));

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    if (wu_isMagicUnit(context, entity))
    {
        // Since the attack of magic units are considered "kind of spells"
        // it will consume mana, at 2 per shot.
        if (we_decreaseUnitMana(context, entity, 2))
        {
            vec2 origin = wu_getUnitCenterPosition(context, entity, false);
            vec2 target = wu_getUnitCenterPosition(context, targetEntity, false);
            WarProjectileType projectileType = wu_getProjectileType(unit->type);
            wproj_createProjectile(context, projectileType, entity->id, targetEntity->id, origin, target);
        }
        else
        {
            // wcmd_stop attacking if the magic unit rans out of mana
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
        }
    }
    else
    {
        vec2 origin = wu_getUnitCenterPosition(context, entity, false);
        vec2 target = wu_getUnitCenterPosition(context, targetEntity, false);
        WarProjectileType projectileType = wu_getProjectileType(unit->type);
        wproj_createProjectile(context, projectileType, entity->id, targetEntity->id, origin, target);
    }
}

void we_rangeWallAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity, WarWallPiece* piece)
{
    assert(wu_isUnit(entity));

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    if (wu_isMagicUnit(context, entity))
    {
        // Since the attack of magic units are considered "kind of spells"
        // it will consume mana, at 2 per shot.
        if (we_decreaseUnitMana(context, entity, 2))
        {
            vec2 origin = wu_getUnitCenterPosition(context, entity, false);
            vec2 target = wmap_tileToMapCoordinatesV(vec2i(piece->tilex, piece->tiley), true);
            WarProjectileType projectileType = wu_getProjectileType(unit->type);
            wproj_createProjectile(context, projectileType, entity->id, targetEntity->id, origin, target);
        }
        else
        {
            // wcmd_stop attacking if the magic unit rans out of mana
            WarState* idleState = wst_createIdleState(context, entity, true);
            wst_changeNextState(context, entity, idleState, true, true);
        }
    }
    else
    {
        vec2 origin = wu_getUnitCenterPosition(context, entity, false);
        vec2 target = wmap_tileToMapCoordinatesV(vec2i(piece->tilex, piece->tiley), true);
        WarProjectileType projectileType = wu_getProjectileType(unit->type);
        wproj_createProjectile(context, projectileType, entity->id, targetEntity->id, origin, target);
    }
}

void we_meleeAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity)
{
    WarMap* map = context->map;

    assert(wu_isUnit(entity));

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    // every unit has a 20 percent chance to miss (except catapults)
    if (wu_isCatapultUnit(context, entity) || chance(80))
    {
        s32 minDamage = unit->minDamage;
        s32 rndDamage = unit->rndDamage;

        WarPlayerInfo* player = &map->players[unit->player];
        if (player->godMode)
        {
            minDamage = GOD_MODE_MIN_DAMAGE;
        }

        we_takeDamage(context, targetEntity, minDamage, rndDamage);
    }
}

void we_meleeWallAttack(WarContext* context, WarEntity* entity, WarEntity* targetEntity, WarWallPiece* piece)
{
    WarMap* map = context->map;

    assert(wu_isUnit(entity));

    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    assert(unit);

    // every unit has a 20 percent chance to miss (except catapults)
    if (wu_isCatapultUnit(context, entity) || chance(80))
    {
        s32 minDamage = unit->minDamage;
        s32 rndDamage = unit->rndDamage;

        WarPlayerInfo* player = &map->players[unit->player];
        if (player->godMode)
        {
            minDamage = GOD_MODE_MIN_DAMAGE;
        }

        we_takeWallDamage(context, targetEntity, piece, minDamage, rndDamage);
    }
}

s32 mine(WarContext* context, WarEntity* goldmine, s32 amount)
{
    assert(goldmine);
    assert(wu_isUnit(goldmine));

    WarUnitComponent* unit = we_getUnitComponent(context, goldmine);
    assert(unit);

    if (unit->amount < amount)
        amount = unit->amount;

    unit->amount -= amount;
    unit->amount = MAX(unit->amount, 0);

    if (unit->amount == 0)
    {
        if (!wst_isCollapsing(context, goldmine) && !wst_isGoingToCollapse(context, goldmine))
        {
            WarState* collapseState = wst_createCollapseState(context, goldmine);
            wst_changeNextState(context, goldmine, collapseState, true, true);

#ifndef WAR_EDITOR_BUILD
            vec2 position = wu_getUnitCenterPosition(context, goldmine, false);
            wa_createAudioRandomWithPosition(context, CREATE_AUDIO_ARGS_INIT(.randomFromId=WAR_BUILDING_COLLAPSE_1, .randomToId=WAR_BUILDING_COLLAPSE_3, .position=position, .hasPosition=true, .loop=false));
#endif
        }
    }

    return amount;
}
