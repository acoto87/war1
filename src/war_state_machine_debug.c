#include <assert.h>
#include <stdarg.h>

#include "SDL3/SDL.h"

#include "war_state_machine_debug.h"
#include "war_cheats.h"
#include "war_entities.h"
#include "war_map.h"
#include "war_units.h"

static const char* stateTypeNames[WAR_STATE_COUNT] =
{
    "IDLE",
    "MOVE",
    "PATROL",
    "FOLLOW",
    "ATTACK",
    "GOLD",
    "MINING",
    "WOOD",
    "CHOPPING",
    "DELIVER",
    "DEATH",
    "COLLAPSE",
    "TRAIN",
    "UPGRADE",
    "BUILD",
    "REPAIR",
    "REPAIRING",
    "CAST",
    "WAIT",
};

static const char* fsmOpNames[] =
{
    "NONE",
    "PUSH",
    "POP",
    "REPLACE",
    "RESET",
};

static const char* spellTypeNames[] =
{
    "HEALING",
    "FAR_SIGHT",
    "INVISIBILITY",
    "RAIN_OF_FIRE",
    "POISON_CLOUD",
    "RAISE_DEAD",
    "DARK_VISION",
    "UNHOLY_ARMOR",
    "SUMMON_SPIDER",
    "SUMMON_SCORPION",
    "SUMMON_DAEMON",
    "SUMMON_WATER_ELEMENTAL",
};

static s32 appendf(char* buffer, s32 bufferLen, s32 offset, const char* fmt, ...)
{
    if (offset >= bufferLen)
        return offset;

    va_list args;
    va_start(args, fmt);
    s32 written = SDL_vsnprintf(buffer + offset, (size_t)(bufferLen - offset), fmt, args);
    va_end(args);

    if (written < 0)
        return offset;

    if (offset + written >= bufferLen)
        return bufferLen - 1;

    return offset + written;
}

static StringView getUnitDisplayName(WarContext* context, WarEntity* entity)
{
    WarUnitComponent* unit = we_getUnitComponent(context, entity);
    if (unit)
    {
        const WarUnitData* unitData = wu_getUnitData(unit->type);
        if (unitData)
            return unitData->name;
    }

    return wsv_fromCString("ENTITY");
}

static const char* getSpellDisplayName(WarSpellType type)
{
    if (type >= WAR_SPELL_HEALING && type <= WAR_SUMMON_WATER_ELEMENTAL)
        return spellTypeNames[type];

    return "UNKNOWN_SPELL";
}

static s32 appendStateDetail(WarContext* context, WarStateBase* state, char* buffer, s32 bufferLen, s32 offset)
{
    assert(state);

    switch (state->type)
    {
        case WAR_STATE_IDLE:
        {
            WarStateIdle* idle = (WarStateIdle*)state;
            return appendf(buffer, bufferLen, offset, "look=%d", idle->lookAround ? 1 : 0);
        }
        case WAR_STATE_MOVE:
        {
            WarStateMove* move = (WarStateMove*)state;
            return appendf(buffer, bufferLen, offset,
                "wp=%d/%d vel=(%.1f,%.1f) stuck=%.1fs recovery=%d",
                move->waypointsIndex, move->waypointsCount,
                move->rvoVelocity.x, move->rvoVelocity.y,
                move->progress.noProgressTime,
                move->progress.recoveryAttempt);
        }
        case WAR_STATE_PATROL:
        {
            WarStatePatrol* patrol = (WarStatePatrol*)state;
            return appendf(buffer, bufferLen, offset,
                "wp=%d/%d dir=%d",
                patrol->waypointsIndex, patrol->waypointsCount,
                patrol->dir);
        }
        case WAR_STATE_FOLLOW:
        {
            WarStateFollow* follow = (WarStateFollow*)state;
            return appendf(buffer, bufferLen, offset,
                "target=#%d dist=%d pos=(%.1f,%.1f)",
                (s32)follow->targetEntityId,
                follow->targetDistance,
                follow->targetPosition.x,
                follow->targetPosition.y);
        }
        case WAR_STATE_ATTACK:
        {
            WarStateAttack* attack = (WarStateAttack*)state;
            return appendf(buffer, bufferLen, offset,
                "target=#%d pos=(%.1f,%.1f)",
                (s32)attack->targetEntityId,
                attack->targetPosition.x,
                attack->targetPosition.y);
        }
        case WAR_STATE_GOLD:
        {
            WarStateGold* gold = (WarStateGold*)state;
            return appendf(buffer, bufferLen, offset, "mine=#%d", gold->goldmineId);
        }
        case WAR_STATE_MINING:
        {
            WarStateMining* mining = (WarStateMining*)state;
            return appendf(buffer, bufferLen, offset, "mine=#%d time=%.1fs", mining->goldmineId, mining->miningTime);
        }
        case WAR_STATE_WOOD:
        {
            WarStateWood* wood = (WarStateWood*)state;
            return appendf(buffer, bufferLen, offset,
                "forest=#%d pos=(%.1f,%.1f)",
                wood->forestId,
                wood->position.x,
                wood->position.y);
        }
        case WAR_STATE_CHOPPING:
        {
            WarStateChopping* chopping = (WarStateChopping*)state;
            return appendf(buffer, bufferLen, offset,
                "forest=#%d pos=(%.1f,%.1f)",
                chopping->forestId,
                chopping->position.x,
                chopping->position.y);
        }
        case WAR_STATE_DELIVER:
        {
            WarStateDeliver* deliver = (WarStateDeliver*)state;
            return appendf(buffer, bufferLen, offset,
                "th=#%d cycle=%d src=%d srcId=#%d pos=(%.1f,%.1f)",
                deliver->townHallId,
                deliver->cycle ? 1 : 0,
                (s32)deliver->sourceKind,
                (s32)deliver->sourceId,
                deliver->sourcePosition.x,
                deliver->sourcePosition.y);
        }
        case WAR_STATE_TRAIN:
        {
            WarStateTrain* train = (WarStateTrain*)state;
            const WarUnitData* unitData = wu_getUnitData(train->unitToBuild);
            const char* unitName = unitData ? unitData->name.data : "UNKNOWN";
            s32 progress = 0;
            if (train->totalBuildTime > 0.0f)
            {
                f32 percent = (train->buildTime / train->totalBuildTime) * 100.0f;
                progress = (s32)CLAMP(percent, 0.0f, 100.0f);
            }
            return appendf(buffer, bufferLen, offset, "unit=%s progress=%d%% cancelled=%d", unitName, progress, train->cancelled ? 1 : 0);
        }
        case WAR_STATE_UPGRADE:
        {
            WarStateUpgrade* upgrade = (WarStateUpgrade*)state;
            const char* upgradeName = "UNKNOWN_UPGRADE";
            if (upgrade->upgradeToBuild >= WAR_UPGRADE_ARROWS && upgrade->upgradeToBuild <= WAR_UPGRADE_SHIELD)
                upgradeName = upgradeNames[upgrade->upgradeToBuild].data;
            s32 progress = 0;
            if (upgrade->totalBuildTime > 0.0f)
            {
                f32 percent = (upgrade->buildTime / upgrade->totalBuildTime) * 100.0f;
                progress = (s32)CLAMP(percent, 0.0f, 100.0f);
            }
            return appendf(buffer, bufferLen, offset, "upg=%s progress=%d%% cancelled=%d", upgradeName, progress, upgrade->cancelled ? 1 : 0);
        }
        case WAR_STATE_BUILD:
        {
            WarStateBuild* build = (WarStateBuild*)state;
            s32 progress = 0;
            if (build->totalBuildTime > 0.0f)
            {
                f32 percent = (build->buildTime / build->totalBuildTime) * 100.0f;
                progress = (s32)CLAMP(percent, 0.0f, 100.0f);
            }
            return appendf(buffer, bufferLen, offset,
                "worker=#%d progress=%d%% cancelled=%d",
                (s32)build->workerId,
                progress,
                build->cancelled ? 1 : 0);
        }
        case WAR_STATE_REPAIR:
        {
            WarStateRepair* repair = (WarStateRepair*)state;
            return appendf(buffer, bufferLen, offset, "building=#%d", (s32)repair->buildingId);
        }
        case WAR_STATE_REPAIRING:
        {
            WarStateRepairing* repairing = (WarStateRepairing*)state;
            return appendf(buffer, bufferLen, offset,
                "building=#%d inside=%d",
                (s32)repairing->buildingId,
                repairing->insideBuilding ? 1 : 0);
        }
        case WAR_STATE_CAST:
        {
            WarStateCast* cast = (WarStateCast*)state;
            return appendf(buffer, bufferLen, offset,
                "spell=%s target=#%d pos=(%d,%d)",
                getSpellDisplayName(cast->spellType),
                (s32)cast->targetEntityId,
                (s32)cast->targetPosition.x,
                (s32)cast->targetPosition.y);
        }
        case WAR_STATE_WAIT:
        {
            WarStateWait* wait = (WarStateWait*)state;
            f64 remaining = MAX(wait->waitEndGameTime - context->gameTime, 0.0);
            return appendf(buffer, bufferLen, offset, "remaining=%.1fs", remaining);
        }
        case WAR_STATE_DEATH:
        case WAR_STATE_COLLAPSE:
        default:
            return offset;
    }
}

void wstdbg_updateDebugText(WarContext* context)
{
    WarMap* map = context->map;
    if (!map)
    {
        wcheatp_setDebugText(context, wsv_fromCString(""));
        return;
    }

    if (!context->debugRender.flags[WAR_DEBUG_RENDER_STATE_MACHINE])
    {
        wcheatp_setDebugText(context, wsv_fromCString(""));
        return;
    }

    if (map->selectedEntities.count != 1)
    {
        wcheatp_setDebugText(context, wsv_fromCString(""));
        return;
    }

    WarEntityId selectedId = map->selectedEntities.items[0];
    WarEntity* entity = we_findEntity(context, selectedId);
    if (!entity)
    {
        wcheatp_setDebugText(context, wsv_fromCString(""));
        return;
    }

    WarStateMachineComponent* sm = we_getStateMachineComponent(context, entity);
    if (!sm)
    {
        wcheatp_setDebugText(context, wsv_fromCString(""));
        return;
    }

    char buffer[1024];
    buffer[0] = '\0';
    s32 offset = 0;

    StringView unitName = getUnitDisplayName(context, entity);
    offset = appendf(buffer, (s32)sizeof(buffer), offset,
        "=== %.*s #%d (depth=%d) ===\n",
        (int)unitName.length, unitName.data, (s32)entity->id, sm->depth);

    if (sm->depth == 0)
    {
        offset = appendf(buffer, (s32)sizeof(buffer), offset, "NO_STATE\n");
    }
    else
    {
        for (s32 i = (s32)sm->depth - 1; i >= 0; i--)
        {
            WarStateBase* state = wst_deref(context, sm->stack[i]);
            if (!state)
            {
                offset = appendf(buffer, (s32)sizeof(buffer), offset, "<INVALID_STATE>\n");
                continue;
            }

            const char* stateName = state->type >= WAR_STATE_IDLE && state->type < WAR_STATE_COUNT
                ? stateTypeNames[state->type]
                : "UNKNOWN";
            offset = appendf(buffer, (s32)sizeof(buffer), offset, "%-10s ", stateName);
            offset = appendStateDetail(context, state, buffer, (s32)sizeof(buffer), offset);
            offset = appendf(buffer, (s32)sizeof(buffer), offset, "\n");
        }
    }

    WarTransitionRequest* pending = &sm->pending;

    if (pending->operation != WAR_STATE_OP_NONE)
    {
        switch (pending->operation)
        {
        case WAR_STATE_OP_POP:
            offset = appendf(buffer, (s32)sizeof(buffer), offset, "pending: >>POP\n");
            break;
        case WAR_STATE_OP_PUSH:
        case WAR_STATE_OP_REPLACE:
        case WAR_STATE_OP_RESET:
            if (WAR_STATE_REF_IS_VALID(pending->stateRef))
            {
                const char* pendingName = pending->stateRef.type >= WAR_STATE_IDLE && pending->stateRef.type < WAR_STATE_COUNT
                    ? stateTypeNames[pending->stateRef.type] : "UNKNOWN";
                offset = appendf(buffer, (s32)sizeof(buffer), offset, "pending: >>%s -> %s\n", fsmOpNames[pending->operation], pendingName);
            }
            else
            {
                offset = appendf(buffer, (s32)sizeof(buffer), offset, "pending: >>%s\n", fsmOpNames[pending->operation]);
            }
            break;
        default:
            break;
        }
    }

    wcheatp_setDebugText(context, wsv_fromCString(buffer));
}
