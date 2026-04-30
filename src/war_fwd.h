#pragma once

#include <stdint.h>

enum _WarMouseButtons;
typedef enum _WarMouseButtons WarMouseButtons;

enum _WarKeys;
typedef enum _WarKeys WarKeys;

enum _WarFileType;
typedef enum _WarFileType WarFileType;

enum _WarResourceType;
typedef enum _WarResourceType WarResourceType;

enum _WarLevelInfoType;
typedef enum _WarLevelInfoType WarLevelInfoType;

enum _WarRace;
typedef enum _WarRace WarRace;

enum _WarUnitDirection;
typedef enum _WarUnitDirection WarUnitDirection;

enum _WarUnitType;
typedef enum _WarUnitType WarUnitType;

enum _WarAnimationStatus;
typedef enum _WarAnimationStatus WarAnimationStatus;

enum _WarResourceKind;
typedef enum _WarResourceKind WarResourceKind;

enum _WarConstructType;
typedef enum _WarConstructType WarConstructType;

enum _WarCampaignMapType;
typedef enum _WarCampaignMapType WarCampaignMapType;

enum _WarLevelResult;
typedef enum _WarLevelResult WarLevelResult;

enum _WarFeatureType;
typedef enum _WarFeatureType WarFeatureType;

enum _WarUpgradeType;
typedef enum _WarUpgradeType WarUpgradeType;

enum _WarSpellType;
typedef enum _WarSpellType WarSpellType;

enum _WarUnitCommandType;
typedef enum _WarUnitCommandType WarUnitCommandType;

enum _WarEntityType;
typedef enum _WarEntityType WarEntityType;

enum _WarRoadPieceType;
typedef enum _WarRoadPieceType WarRoadPieceType;

enum _WarWallPieceType;
typedef enum _WarWallPieceType WarWallPieceType;

enum _WarRuinPieceType;
typedef enum _WarRuinPieceType WarRuinPieceType;

enum _WarTreeTileType;
typedef enum _WarTreeTileType WarTreeTileType;

enum _WarUnitActionStepType;
typedef enum _WarUnitActionStepType WarUnitActionStepType;

enum _WarUnbreakableParam;
typedef enum _WarUnbreakableParam WarUnbreakableParam;

enum _WarUnitActionType;
typedef enum _WarUnitActionType WarUnitActionType;

enum _WarUnitActionStatus;
typedef enum _WarUnitActionStatus WarUnitActionStatus;

enum _PathFindingType;
typedef enum _PathFindingType PathFindingType;

enum _WarPathFinderDataType;
typedef enum _WarPathFinderDataType WarPathFinderDataType;

enum _WarStateType;
typedef enum _WarStateType WarStateType;

enum _WarTextAlignment;
typedef enum _WarTextAlignment WarTextAlignment;

enum _WarTextWrapping;
typedef enum _WarTextWrapping WarTextWrapping;

enum _WarTextTrimming;
typedef enum _WarTextTrimming WarTextTrimming;

enum _WarAudioId;
typedef enum _WarAudioId WarAudioId;

enum _WarAudioType;
typedef enum _WarAudioType WarAudioType;

enum _WarCursorType;
typedef enum _WarCursorType WarCursorType;

enum _WarProjectileType;
typedef enum _WarProjectileType WarProjectileType;

enum _WarMapTilesetType;
typedef enum _WarMapTilesetType WarMapTilesetType;

enum _WarFogPieceType;
typedef enum _WarFogPieceType WarFogPieceType;

enum _WarFogBoundaryType;
typedef enum _WarFogBoundaryType WarFogBoundaryType;

enum _WarUnitPortraits;
typedef enum _WarUnitPortraits WarUnitPortraits;

enum _WarMapTileState;
typedef enum _WarMapTileState WarMapTileState;

enum _WarAICommandStatus;
typedef enum _WarAICommandStatus WarAICommandStatus;

enum _WarAICommandType;
typedef enum _WarAICommandType WarAICommandType;

enum _WarCheat;
typedef enum _WarCheat WarCheat;

enum _WarMapSpeed;
typedef enum _WarMapSpeed WarMapSpeed;

enum _WarSceneType;
typedef enum _WarSceneType WarSceneType;

enum _WarSceneDownloadState;
typedef enum _WarSceneDownloadState WarSceneDownloadState;

struct _WarKeyButtonState;
typedef struct _WarKeyButtonState WarKeyButtonState;

struct _WarInput;
typedef struct _WarInput WarInput;

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

struct _WarMapPath;
typedef struct _WarMapPath WarMapPath;

struct _WarPathFinder;
typedef struct _WarPathFinder WarPathFinder;

struct _WarState;
typedef struct _WarState WarState;

struct _WarTransformComponent;
typedef struct _WarTransformComponent WarTransformComponent;

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

struct _WarRenderState;
typedef struct _WarRenderState WarRenderState;

struct _WarContext;
typedef struct _WarContext WarContext;

typedef uint16_t WarEntityId;

typedef void (*WarClickHandler)(struct _WarContext* context, struct _WarEntity* entity);
typedef void (*WarRenderFunc)(struct _WarContext* context, struct _WarEntity* entity);
typedef int32_t (*WarRenderCompareFunc)(const struct _WarEntity* e1, const struct _WarEntity* e2);
typedef WarLevelResult (*WarCheckObjectivesFunc)(struct _WarContext* context);
