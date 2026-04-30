#include "war_ai.h"

#include <assert.h>

bool wai_equalsAICommand(const WarAICommand* command1, const WarAICommand* command2)
{
    return command1->id == command2->id;
}

void wai_freeAICommand(WarAICommand* command)
{
    wm_free((void*)command);
}

shlDefineQueue(WarAICommandQueue, WarAICommand*)
shlDefineList(WarAICommandList, WarAICommand*)

WarAI* wai_createAI(WarContext* context)
{
    NOT_USED(context);

    WarAI* ai = (WarAI*)wm_alloc(sizeof(WarAI));
    ai->staticCommandId = 0;
    ai->customData = NULL;

    WarAICommandListInit(&ai->currentCommands, WarAICommandListDefaultOptions);
    WarAICommandQueueInit(&ai->nextCommands, WarAICommandQueueDefaultOptions);

    return ai;
}

WarAICommand* wai_createAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommandType type)
{
    NOT_USED(context);

    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICommand* command = (WarAICommand*)wm_alloc(sizeof(WarAICommand));
    command->id = ++ai->staticCommandId;
    command->type = type;
    command->status = WAR_AI_COMMAND_STATUS_CREATED;

    return command;
}

WarAICommand* wai_createUnitRequest(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, s32 count)
{
    WarAICommand* request = wai_createAICommand(context, aiPlayer, WAR_AI_COMMAND_REQUEST);
    request->request.unitType = unitType;
    request->request.count = count;
    return request;
}

WarAICommand* wai_createWaitForUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType, s32 count)
{
    WarAICommand* wait = wai_createAICommand(context, aiPlayer, WAR_AI_COMMAND_WAIT);
    wait->wait.unitType = unitType;
    wait->wait.count = count;
    return wait;
}

WarAICommand* wai_createSleepForTime(WarContext* context, WarPlayerInfo* aiPlayer, f32 time)
{
    WarAICommand* sleep = wai_createAICommand(context, aiPlayer, WAR_AI_COMMAND_SLEEP);
    sleep->sleep.time = time;
    return sleep;
}

typedef struct
{
    s32 index;
    WarAICommandList commands;
} WarAICustomData;

void wai_initAI(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICustomData* customData = (WarAICustomData*)wm_alloc(sizeof(WarAICustomData));
    customData->index = 0;

    WarAICommandList* commands = &customData->commands;
    WarAICommandListInit(commands, WarAICommandListDefaultOptions);

    WarUnitType townHall = wu_getUnitTypeForRace(WAR_UNIT_TOWNHALL_HUMANS, aiPlayer->race);
    WarAICommandListAdd(commands, wai_createUnitRequest(context, aiPlayer, townHall, 1));
    WarAICommandListAdd(commands, wai_createWaitForUnit(context, aiPlayer, townHall, 1));

    WarUnitType worker = wu_getUnitTypeForRace(WAR_UNIT_PEASANT, aiPlayer->race);
    WarAICommandListAdd(commands, wai_createUnitRequest(context, aiPlayer, worker, 2));
    WarAICommandListAdd(commands, wai_createWaitForUnit(context, aiPlayer, worker, 2));
    WarAICommandListAdd(commands, wai_createUnitRequest(context, aiPlayer, worker, 2));
    WarAICommandListAdd(commands, wai_createWaitForUnit(context, aiPlayer, worker, 4));

    ai->customData = customData;
}

WarAICommand* wai_getNextAICommand(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICustomData* customData = (WarAICustomData*)ai->customData;
    assert(customData);

    if (customData->index < customData->commands.count)
    {
        return customData->commands.items[customData->index++];
    }

    return wai_createSleepForTime(context, aiPlayer, 10.0f);
}

void wai_initAIPlayer(WarContext* context, WarPlayerInfo* aiPlayer)
{
    aiPlayer->ai = wai_createAI(context);
    wai_initAI(context, aiPlayer);
}

void wai_initAIPlayers(WarContext* context)
{
    WarMap* map = context->map;

    // for now assume player 1 is the only AI
    wai_initAIPlayer(context, &map->players[1]);
}

bool wai_tryCreateUnit(WarContext* context, WarPlayerInfo* aiPlayer, WarUnitType unitType)
{
    if (wu_isDudeUnitType(unitType))
    {
        WarUnitStats stats = wu_getUnitStats(unitType);
        if (!we_enoughPlayerResources(context, aiPlayer, stats.goldCost, stats.woodCost))
        {
            // there is not enough resources to create the unit
            return false;
        }

        // check if there is enough food
        if (!we_enoughFarmFood(context, aiPlayer))
        {
            // set a request for supply because there is not enough food
            return false;
        }

        WarUnitType producerType = wu_getProducerUnitOfType(unitType);
        if (isValidUnitType(producerType))
        {
            WarEntityList* units = we_getUnitsOfType(context, producerType);
            for (s32 i = 0; i < units->count; i++)
            {
                WarEntity* entity = units->items[i];
                if (entity && entity->unit.player == aiPlayer->index)
                {
                    if (!isTraining(entity) && !isUpgrading(entity))
                    {
                        if (we_decreasePlayerResources(context, aiPlayer, stats.goldCost, stats.woodCost))
                        {
                            WarState* trainState = wst_createTrainState(context, entity, unitType, (f32)stats.buildTime);
                            wst_changeNextState(context, entity, trainState, true, true);
                        }

                        return true;
                    }
                }
            }
        }

        // if there is no building to train the unit or all of them are busy
        // let the request live to try again later
        return false;
    }

    if (wu_isBuildingUnitType(unitType))
    {
        // find a free worker
        // find a place to build
        // send worker to build the building

        // if there is no free worker, try to get one assigned to a resource

        // at the worst case, the building can't be built right now, so
        // let the request live to try again later
        return true;
    }

    logWarning("Unkown unit type %d to be built by player %d", unitType, aiPlayer->index);
    return false;
}

bool wai_executeRequestAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    command->status = WAR_AI_COMMAND_STATUS_STARTED;

    if (wai_tryCreateUnit(context, aiPlayer, command->request.unitType))
    {
        command->request.count--;

        if (command->request.count == 0)
        {
            command->status = WAR_AI_COMMAND_STATUS_COMPLETED;
        }
    }

    return true;
}

bool wai_executeWaitAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    command->status = WAR_AI_COMMAND_STATUS_STARTED;

    s32 numberOfUnits = wu_getNumberOfUnitsOfType(context, aiPlayer->index, command->wait.unitType);
    if (numberOfUnits >= command->wait.count)
    {
        command->status = WAR_AI_COMMAND_STATUS_COMPLETED;
        return true;
    }

    return false;
}

bool wai_executeSleepAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    NOT_USED(aiPlayer);

    command->status = WAR_AI_COMMAND_STATUS_STARTED;
    command->sleep.time -= context->deltaTime;

    if (command->sleep.time <= 0)
    {
        command->status = WAR_AI_COMMAND_STATUS_COMPLETED;
        return true;
    }

    return false;
}

bool wai_executeAICommand(WarContext* context, WarPlayerInfo* aiPlayer, WarAICommand* command)
{
    if (command->status == WAR_AI_COMMAND_STATUS_COMPLETED)
    {
        return true;
    }

    switch (command->type)
    {
        case WAR_AI_COMMAND_REQUEST:
        {
            return wai_executeRequestAICommand(context, aiPlayer, command);
        }

        case WAR_AI_COMMAND_WAIT:
        {
            return wai_executeWaitAICommand(context, aiPlayer, command);
        }

        case WAR_AI_COMMAND_SLEEP:
        {
            return wai_executeSleepAICommand(context, aiPlayer, command);
        }

        default:
        {
            logWarning("AI commands of type %d are NOT handled yet", command->type);
            return true;
        }
    }
}

bool wai_updateAICurrentCommands(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    bool updateNext = true;

    WarAICommandList* commands = &ai->currentCommands;
    for (s32 i = 0; i < commands->count; i++)
    {
        WarAICommand* command = commands->items[i];
        if (!wai_executeAICommand(context, aiPlayer, command))
        {
            updateNext = false;
        }
    }

    return updateNext;
}

void wai_updateAINextCommands(WarContext* context, WarPlayerInfo* aiPlayer)
{
    NOT_USED(context);

    WarAI* ai = aiPlayer->ai;
    assert(ai);

#define isWaitOrSleep(command) ((command)->type == WAR_AI_COMMAND_WAIT || (command)->type == WAR_AI_COMMAND_SLEEP)

    WarAICommandList* currentCommands = &ai->currentCommands;
    WarAICommandQueue* nextCommands = &ai->nextCommands;
    if (nextCommands->count > 0)
    {
        WarAICommand* command = NULL;
        do
        {
            command = WarAICommandQueuePop(nextCommands);
            WarAICommandListAdd(currentCommands, command);
        } while (nextCommands->count > 0 && !isWaitOrSleep(command));
    }

#undef isWaitOrSleep
}

void wai_removeCompletedAICommands(WarContext* context, WarPlayerInfo* aiPlayer)
{
    NOT_USED(context);

    WarAI* ai = aiPlayer->ai;
    assert(ai);

    WarAICommandList* commands = &ai->currentCommands;
    for (s32 i = commands->count - 1; i >= 0; i--)
    {
        WarAICommand* command = commands->items[i];
        if (command->status == WAR_AI_COMMAND_STATUS_COMPLETED)
        {
            WarAICommandListRemoveAt(commands, i);
        }
    }
}

void wai_updateAIPlayer(WarContext* context, WarPlayerInfo* aiPlayer)
{
    WarAI* ai = aiPlayer->ai;
    assert(ai);

    if (wai_updateAICurrentCommands(context, aiPlayer))
    {
        WarAICommand* command = wai_getNextAICommand(context, aiPlayer);
        if (command)
        {
            WarAICommandQueuePush(&ai->nextCommands, command);
        }

        wai_updateAINextCommands(context, aiPlayer);
    }

    wai_removeCompletedAICommands(context, aiPlayer);
}


void wai_updateAIPlayers(WarContext* context)
{
    TracyCZoneN(ctx, "UpdateAI", 1);
    WarMap* map = context->map;

    // for now assume player 1 is the only AI
    wai_updateAIPlayer(context, &map->players[1]);
    TracyCZoneEnd(ctx);
}
