#pragma once

typedef struct
{
    char name[20];
    WarAIInitFunc initFunc;
    WarAINextCommandFunc getCommandFunc;
} WarAIData;

WarAIData getAIData(const char* name);

WarAI* createAI(WarContext* context, const char* name);

void initAIPlayers(WarContext* context);
void updateAIPlayers(WarContext* context);

shlDeclareList(WarUnitTypeList, WarUnitType)
shlDefineList(WarUnitTypeList, WarUnitType)

typedef enum
{
    WAR_AI_SYSTEM_RESOURCE,
    WAR_AI_SYSTEM_TRAIN,
    WAR_AI_SYSTEM_UPGRADE,
    WAR_AI_SYSTEM_BUILD,
    WAR_AI_SYSTEM_PLANNING,
    WAR_AI_SYSTEM_SQUAD,
    WAR_AI_SYSTEM_SCRIPT
} WarAISystemType;

// typedef struct
// {
//     WarAISystemType type;

//     union
//     {
//         struct
//         {
//             WarUnitTypeList requests;
//         } train;
//     };
// } WarAISystem;



// void updateTrainSystem(WarContext* context, WarPlayerInfo* aiPlayer)
// {
//     WarAI* ai = aiPlayer->ai;
//     assert(ai);

//     WarAISystem* system = WarAISystemsMapGet(&ai->systems, WAR_AI_SYSTEM_TRAIN);
//     assert(system);

//     s32List requestsToRemove;
//     s32ListInit(&requestsToRemove, s32ListDefaultOptions);

//     WarUnitTypeList* requests = &system->train.requests;
//     for (s32 i = 0; i < requests->count; i++)
//     {
//         WarUnitType requestedUnitType = requests->items[i];

//         WarUnitType producerType = getUnitTypeProducer(requestedUnitType);
//         WarEntityList* producers = getUnitsOfTypeOfPlayer(context, producerType, aiPlayer->index);
//         for (s32 k = 0; k < producers->count; k++)
//         {
//             WarEntity* producer = producers->items[k];
//             if (producer)
//             {
//                 WarCommand* command = createTrainCommand(context, aiPlayer, requestedUnitType, producer->id);
//                 WarCommandStatus status = executeCommand(context, aiPlayer, command);

//                 if (status < WAR_COMMAND_STATUS_FAILED)
//                 {
//                     s32ListAdd(&requestsToRemove, i);
//                 }
//             }
//         }
//         WarEntityListFree(producers);
//     }

//     for (s32 i = requestsToRemove.count - 1; i >= 0; i--)
//     {
//         WarUnitTypeList(requests, requestsToRemove.items[i]);
//     }

//     s32ListFree(&requestsToRemove);
// }

// void updatePlanningSystems(WarContext* context);

// void updateResourceSystems(WarContext* context);
// void updateTrainSystems(WarContext* context);
// void updateUpgradeSystems(WarContext* context);
// void updateBuildSystems(WarContext* context);
// void updateSquadSystems(WarContext* context);
// void updateScriptSystems(WarContext* context);


// Planning System: This system will come up with a plan, like what is the build order to achieve specific goal.
//                  This will materialize in a structure list-like that represent the build order.
//                  It's up to the other system to execute that build order.
//
// Resource System: This will watch the resources needed for specific points in the game. The resources that matter here
//                  are gold, wood and farms.
// Train System:
