#pragma once

#include <stdint.h>
#include "war_enums.h"

struct _WarInput;
typedef struct _WarInput WarInput;

struct _WarInputState;
typedef struct _WarInputState WarInputState;

struct _WarFontData;
typedef struct _WarFontData WarFontData;

struct _WarRawResource;
typedef struct _WarRawResource WarRawResource;

struct _WarFile;
typedef struct _WarFile WarFile;

struct _WarSpriteFrame;
typedef struct _WarSpriteFrame WarSpriteFrame;

struct _WarSprite;
typedef struct _WarSprite WarSprite;

struct _WarSpriteResourceRef;
typedef struct _WarSpriteResourceRef WarSpriteResourceRef;

struct _WarSpriteAnimation;
typedef struct _WarSpriteAnimation WarSpriteAnimation;

struct _WarLevelUnit;
typedef struct _WarLevelUnit WarLevelUnit;

struct _WarLevelConstruct;
typedef struct _WarLevelConstruct WarLevelConstruct;

struct _WarTilesetTile;
typedef struct _WarTilesetTile WarTilesetTile;

struct _WarCustomMapConfiguration;
typedef struct _WarCustomMapConfiguration WarCustomMapConfiguration;

struct _WarResource;
typedef struct _WarResource WarResource;

struct _WarUnitCommand;
typedef struct _WarUnitCommand WarUnitCommand;

struct _WarRoadPiece;
typedef struct _WarRoadPiece WarRoadPiece;

struct _WarWallPiece;
typedef struct _WarWallPiece WarWallPiece;

struct _WarRuinPiece;
typedef struct _WarRuinPiece WarRuinPiece;

struct _WarTree;
typedef struct _WarTree WarTree;

struct _WarUnitActionStep;
typedef struct _WarUnitActionStep WarUnitActionStep;

struct _WarUnitActionDef;
typedef struct _WarUnitActionDef WarUnitActionDef;

struct _WarUnitAction;
typedef struct _WarUnitAction WarUnitAction;

struct _WarUnitData;
typedef struct _WarUnitData WarUnitData;

struct _WarWorkerData;
typedef struct _WarWorkerData WarWorkerData;

struct _WarBuildingData;
typedef struct _WarBuildingData WarBuildingData;

struct _WarRoadData;
typedef struct _WarRoadData WarRoadData;

struct _WarWallData;
typedef struct _WarWallData WarWallData;

struct _WarRuinData;
typedef struct _WarRuinData WarRuinData;

struct _WarTreeData;
typedef struct _WarTreeData WarTreeData;

struct _WarUnitStats;
typedef struct _WarUnitStats WarUnitStats;

struct _WarBuildingStats;
typedef struct _WarBuildingStats WarBuildingStats;

struct _WarUpgradeData;
typedef struct _WarUpgradeData WarUpgradeData;

struct _WarUpgradeStats;
typedef struct _WarUpgradeStats WarUpgradeStats;

struct _WarSpellData;
typedef struct _WarSpellData WarSpellData;

struct _WarSpellStats;
typedef struct _WarSpellStats WarSpellStats;

struct _WarSpellMapping;
typedef struct _WarSpellMapping WarSpellMapping;

struct _WarUnitCommandBaseData;
typedef struct _WarUnitCommandBaseData WarUnitCommandBaseData;

struct _WarUnitCommandMapping;
typedef struct _WarUnitCommandMapping WarUnitCommandMapping;

struct _WarUnitCommandData;
typedef struct _WarUnitCommandData WarUnitCommandData;

struct _WarMapNode;
typedef struct _WarMapNode WarMapNode;

struct _WarMapPath;
typedef struct _WarMapPath WarMapPath;

struct _WarPathFinder;
typedef struct _WarPathFinder WarPathFinder;

struct _WarState;
typedef struct _WarState WarState;

struct _WarCampaignMapData;
typedef struct _WarCampaignMapData WarCampaignMapData;

struct _WarSpriteComponent;
typedef struct _WarSpriteComponent WarSpriteComponent;

struct _WarUnitComponent;
typedef struct _WarUnitComponent WarUnitComponent;

struct _WarAnimationsComponent;
typedef struct _WarAnimationsComponent WarAnimationsComponent;

struct _WarRoadComponent;
typedef struct _WarRoadComponent WarRoadComponent;

struct _WarWallComponent;
typedef struct _WarWallComponent WarWallComponent;

struct _WarRuinComponent;
typedef struct _WarRuinComponent WarRuinComponent;

struct _WarForestComponent;
typedef struct _WarForestComponent WarForestComponent;

struct _WarStateMachineComponent;
typedef struct _WarStateMachineComponent WarStateMachineComponent;

struct _WarUIComponent;
typedef struct _WarUIComponent WarUIComponent;

struct _WarTextComponent;
typedef struct _WarTextComponent WarTextComponent;

struct _WarRectComponent;
typedef struct _WarRectComponent WarRectComponent;

struct _WarButtonComponent;
typedef struct _WarButtonComponent WarButtonComponent;

struct _WarAudioComponent;
typedef struct _WarAudioComponent WarAudioComponent;

struct _WarCursorComponent;
typedef struct _WarCursorComponent WarCursorComponent;

struct _WarProjectileComponent;
typedef struct _WarProjectileComponent WarProjectileComponent;

struct _WarPoisonCloudComponent;
typedef struct _WarPoisonCloudComponent WarPoisonCloudComponent;

struct _WarSightComponent;
typedef struct _WarSightComponent WarSightComponent;

struct _WarEntity;
typedef struct _WarEntity WarEntity;

struct _WarMapTile;
typedef struct _WarMapTile WarMapTile;

struct _WarUpgrade;
typedef struct _WarUpgrade WarUpgrade;

struct _WarAICommand;
typedef struct _WarAICommand WarAICommand;

struct _WarAI;
typedef struct _WarAI WarAI;

struct _WarPlayerInfo;
typedef struct _WarPlayerInfo WarPlayerInfo;

struct _WarFlashStatus;
typedef struct _WarFlashStatus WarFlashStatus;

struct _WarCheatStatus;
typedef struct _WarCheatStatus WarCheatStatus;

struct _WarMapSettings;
typedef struct _WarMapSettings WarMapSettings;

struct _WarEntityManager;
typedef struct _WarEntityManager WarEntityManager;

struct _WarMap;
typedef struct _WarMap WarMap;

struct _WarScene;
typedef struct _WarScene WarScene;

struct _WarCheatDescriptor;
typedef struct _WarCheatDescriptor WarCheatDescriptor;

struct _WarSceneDescriptor;
typedef struct _WarSceneDescriptor WarSceneDescriptor;

struct _WarTransformComponent;
typedef struct _WarTransformComponent WarTransformComponent;

struct _WarRenderState;
typedef struct _WarRenderState WarRenderState;

struct _WarContext;
typedef struct _WarContext WarContext;

typedef uint16_t WarEntityId;

typedef void (*WarClickHandler)(struct _WarContext* context, struct _WarEntity* entity);
typedef void (*WarRenderFunc)(struct _WarContext* context, struct _WarEntity* entity);
typedef int32_t (*WarRenderCompareFunc)(const struct _WarEntity* e1, const struct _WarEntity* e2);
typedef WarLevelResult (*WarCheckObjectivesFunc)(struct _WarContext* context);
typedef void (*WarCheatFunc)(WarContext* context, StringView argument);
typedef void (*WarRenderFunc)(WarContext* context, WarEntity* entity);
typedef int32_t (*WarRenderCompareFunc)(const WarEntity* e1, const WarEntity* e2);
typedef void (*WarSceneFunc)(WarContext* context);
