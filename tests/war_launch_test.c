#include "unity/unity.h"

#include "war_alloc.h"
#include "war_campaigns.h"
#include "war_game.h"
#include "war_map.h"
#include "war_map_menu.h"
#include "war_resources.h"
#include "war_test_context.h"

extern WarTestContext g_test;

static void configureSyntheticCustomMap(WarContext* context, s32 resourceIndex)
{
    WarResource* resource = &context->resources[resourceIndex];
    resource->type = WAR_RESOURCE_TYPE_LEVEL_INFO;
    resource->levelInfo.customMap = true;

    resource->levelInfo.startGoldminesCount = 1;
    resource->levelInfo.startGoldmines = (WarLevelUnit*)wm_alloc(sizeof(WarLevelUnit));
    resource->levelInfo.startGoldmines[0] = (WarLevelUnit){
        .x = 20,
        .y = 20,
        .type = WAR_UNIT_GOLDMINE
    };

    resource->levelInfo.startConfigurationsCount = 2;
    for (s32 i = 0; i < 2; i++)
    {
        WarCustomMapConfiguration* configuration = &resource->levelInfo.startConfigurations[i];
        configuration->startEntitiesCount = 2;
        configuration->startEntities = (WarLevelUnit*)wm_alloc(2 * sizeof(WarLevelUnit));
        configuration->startEntities[0] = (WarLevelUnit){
            .x = (u8)(4 + i * 30),
            .y = (u8)(6 + i * 30),
            .type = WAR_UNIT_TOWNHALL_HUMANS,
            .player = 0
        };
        configuration->startEntities[1] = (WarLevelUnit){
            .x = (u8)(40 - i * 30),
            .y = (u8)(42 - i * 30),
            .type = WAR_UNIT_TOWNHALL_HUMANS,
            .player = 1
        };
    }
}

static void test_campaign_launch_defaults_race_to_random(void)
{
    WarContext context = { 0 };
    char* argv[] = { "war1", "--mission", "3" };

    TEST_ASSERT_TRUE(wg_parseCommandLine(&context, (int)arrayLength(argv), argv));
    TEST_ASSERT_EQUAL_INT(WAR_LAUNCH_CAMPAIGN, context.launch.mode);
    TEST_ASSERT_EQUAL_INT(3, context.launch.campaignMission);
    TEST_ASSERT_EQUAL_INT(WAR_RACE_NEUTRAL, context.launch.campaignRace);
}

static void test_custom_launch_defaults_both_races_to_random(void)
{
    WarContext context = { 0 };
    char* argv[] = { "war1", "--mission", "custom", "--map", "forest_1" };

    TEST_ASSERT_TRUE(wg_parseCommandLine(&context, (int)arrayLength(argv), argv));
    TEST_ASSERT_EQUAL_INT(WAR_LAUNCH_PREDEFINED_CUSTOM, context.launch.mode);
    TEST_ASSERT_EQUAL_INT(147, context.launch.customMapIndex);
    TEST_ASSERT_EQUAL_INT(WAR_RACE_NEUTRAL, context.launch.customGame.playerRace);
    TEST_ASSERT_EQUAL_INT(WAR_RACE_NEUTRAL, context.launch.customGame.enemyRace);
}

static void test_custom_launch_parses_shared_resources_and_seed(void)
{
    WarContext context = { 0 };
    char* argv[] = {
        "war1", "--race", "human", "--enemy-race", "orc",
        "--mission", "custom", "--map", "dungeon_7_big_enemy",
        "--gold", "1000", "--wood", "750", "--seed", "42"
    };

    TEST_ASSERT_TRUE(wg_parseCommandLine(&context, (int)arrayLength(argv), argv));
    TEST_ASSERT_EQUAL_INT(188, context.launch.customMapIndex);
    TEST_ASSERT_EQUAL_INT(WAR_RACE_HUMANS, context.launch.customGame.playerRace);
    TEST_ASSERT_EQUAL_INT(WAR_RACE_ORCS, context.launch.customGame.enemyRace);
    TEST_ASSERT_TRUE(context.launch.customGame.hasGold);
    TEST_ASSERT_EQUAL_INT(1000, context.launch.customGame.gold);
    TEST_ASSERT_TRUE(context.launch.customGame.hasWood);
    TEST_ASSERT_EQUAL_INT(750, context.launch.customGame.wood);
    TEST_ASSERT_TRUE(context.launch.customGame.hasSeed);
    TEST_ASSERT_EQUAL_UINT64(42, context.launch.customGame.seed);
}

static void test_numeric_options_are_decimal_and_seed_accepts_u64(void)
{
    WarContext context = { 0 };
    char* argv[] = {
        "war1", "--mission", "custom", "--map", "forest_1",
        "--gold", "01000", "--wood", "00750", "--seed", "18446744073709551615"
    };

    TEST_ASSERT_TRUE(wg_parseCommandLine(&context, (int)arrayLength(argv), argv));
    TEST_ASSERT_EQUAL_INT(1000, context.launch.customGame.gold);
    TEST_ASSERT_EQUAL_INT(750, context.launch.customGame.wood);
    TEST_ASSERT_EQUAL_UINT64(UINT64_MAX, context.launch.customGame.seed);
}

static void test_external_map_keeps_legacy_map_option(void)
{
    WarContext context = { 0 };
    char* argv[] = { "war1", "--map", "maps/example.w1m" };

    TEST_ASSERT_TRUE(wg_parseCommandLine(&context, (int)arrayLength(argv), argv));
    TEST_ASSERT_EQUAL_INT(WAR_LAUNCH_MAP_FILE, context.launch.mode);
    TEST_ASSERT_EQUAL_STRING("maps/example.w1m", context.launch.mapValue);
}

static void test_custom_launch_rejects_unknown_map(void)
{
    WarContext context = { 0 };
    char* argv[] = { "war1", "--mission", "custom", "--map", "missing_map" };

    TEST_ASSERT_FALSE(wg_parseCommandLine(&context, (int)arrayLength(argv), argv));
}

static void test_campaign_launch_rejects_custom_only_options(void)
{
    WarContext context = { 0 };
    char* argv[] = { "war1", "--mission", "2", "--gold", "1000" };

    TEST_ASSERT_FALSE(wg_parseCommandLine(&context, (int)arrayLength(argv), argv));
}

static void test_campaign_map_type_mapping_covers_both_campaigns(void)
{
    WarCampaignMapType mapType = WAR_CAMPAIGN_CUSTOM;

    TEST_ASSERT_TRUE(wcamp_tryGetMapType(WAR_RACE_HUMANS, 1, &mapType));
    TEST_ASSERT_EQUAL_INT(WAR_CAMPAIGN_HUMANS_01, mapType);
    TEST_ASSERT_TRUE(wcamp_tryGetMapType(WAR_RACE_ORCS, 12, &mapType));
    TEST_ASSERT_EQUAL_INT(WAR_CAMPAIGN_ORCS_12, mapType);
    TEST_ASSERT_FALSE(wcamp_tryGetMapType(WAR_RACE_NEUTRAL, 1, &mapType));
    TEST_ASSERT_FALSE(wcamp_tryGetMapType(WAR_RACE_HUMANS, 13, &mapType));
}

static void test_predefined_custom_catalog_names_are_resolvable(void)
{
    s32 customMapCount = 0;
    for (s32 i = 0; i < (s32)arrayLength(assets); i++)
    {
        DatabaseEntry* entry = &assets[i];
        if (entry->type != DB_ENTRY_TYPE_LEVEL_INFO || entry->param2 != 1)
        {
            continue;
        }

        s32 index = 0;
        TEST_ASSERT_TRUE(wres_tryGetPredefinedCustomMapIndex(wsv_fromCString(entry->name), &index));
        TEST_ASSERT_EQUAL_INT(entry->index, index);
        customMapCount++;
    }
    TEST_ASSERT_EQUAL_INT(42, customMapCount);
}

static void test_seeded_custom_startup_is_reproducible(void)
{
    const s32 resourceIndex = 145;
    WarContext* context = g_test.context;
    configureSyntheticCustomMap(context, resourceIndex);

    WarCustomGameOptions options = {
        .enabled = true,
        .playerRace = WAR_RACE_NEUTRAL,
        .enemyRace = WAR_RACE_NEUTRAL,
        .hasGold = true,
        .gold = 1000,
        .hasWood = true,
        .wood = 750,
        .hasSeed = true,
        .seed = 42,
        .startConfigurationIndex = -1
    };
    context->launch.mode = WAR_LAUNCH_PREDEFINED_CUSTOM;
    context->launch.customMapIndex = resourceIndex;
    context->launch.customGame = options;
    SDL_strlcpy(context->launch.mapValue, "synthetic", sizeof(context->launch.mapValue));

    TEST_ASSERT_TRUE(wg_setStartupDestination(context));
    WarMap* firstMap = context->nextMap;
    TEST_ASSERT_NOT_NULL(firstMap);
    TEST_ASSERT_NOT_EQUAL(WAR_RACE_NEUTRAL, firstMap->customGame.playerRace);
    TEST_ASSERT_NOT_EQUAL(WAR_RACE_NEUTRAL, firstMap->customGame.enemyRace);
    TEST_ASSERT_TRUE(firstMap->hasCustomCameraStart);
    TEST_ASSERT_EQUAL_INT(1000, firstMap->customGame.gold);
    TEST_ASSERT_EQUAL_INT(750, firstMap->customGame.wood);

    WarResource* resource = &context->resources[resourceIndex];
    initPlayersInfo(firstMap, resource);
    TEST_ASSERT_EQUAL_INT(1000, firstMap->players[0].gold);
    TEST_ASSERT_EQUAL_INT(1000, firstMap->players[1].gold);
    TEST_ASSERT_EQUAL_INT(750, firstMap->players[0].wood);
    TEST_ASSERT_EQUAL_INT(750, firstMap->players[1].wood);

    initCamera(firstMap, resource);
    TEST_ASSERT_EQUAL_INT(CLAMP(firstMap->customCameraCenterX - MAP_VIEWPORT_WIDTH / 2,
                                0, MAP_WIDTH - MAP_VIEWPORT_WIDTH), firstMap->camera.viewport.x);
    TEST_ASSERT_EQUAL_INT(CLAMP(firstMap->customCameraCenterY - MAP_VIEWPORT_HEIGHT / 2,
                                0, MAP_HEIGHT - MAP_VIEWPORT_HEIGHT), firstMap->camera.viewport.y);

    WarMap* activeMap = context->map;
    context->map = firstMap;
    wmm_handleRestartRestart(context, NULL);
    WarMap* restartMap = context->nextMap;
    context->map = activeMap;
    TEST_ASSERT_EQUAL_INT(firstMap->customGame.playerRace, restartMap->customGame.playerRace);
    TEST_ASSERT_EQUAL_INT(firstMap->customGame.enemyRace, restartMap->customGame.enemyRace);
    TEST_ASSERT_EQUAL_INT(firstMap->customGame.gold, restartMap->customGame.gold);
    TEST_ASSERT_EQUAL_INT(firstMap->customGame.wood, restartMap->customGame.wood);
    TEST_ASSERT_EQUAL_UINT64(firstMap->customGame.seed, restartMap->customGame.seed);
    TEST_ASSERT_EQUAL_INT(firstMap->customGame.startConfigurationIndex,
                          restartMap->customGame.startConfigurationIndex);
    TEST_ASSERT_EQUAL_INT(firstMap->hasCustomCameraStart, restartMap->hasCustomCameraStart);
    TEST_ASSERT_EQUAL_INT(firstMap->customCameraCenterX, restartMap->customCameraCenterX);
    TEST_ASSERT_EQUAL_INT(firstMap->customCameraCenterY, restartMap->customCameraCenterY);

    initPlayersInfo(restartMap, resource);
    initCamera(restartMap, resource);
    TEST_ASSERT_EQUAL_INT(1000, restartMap->players[0].gold);
    TEST_ASSERT_EQUAL_INT(1000, restartMap->players[1].gold);
    TEST_ASSERT_EQUAL_INT(750, restartMap->players[0].wood);
    TEST_ASSERT_EQUAL_INT(750, restartMap->players[1].wood);
    TEST_ASSERT_EQUAL_INT(firstMap->camera.viewport.x, restartMap->camera.viewport.x);
    TEST_ASSERT_EQUAL_INT(firstMap->camera.viewport.y, restartMap->camera.viewport.y);

    u16 firstMineAmount = resource->levelInfo.startEntities[0].amount;
    WarMap* secondMap = wmap_createCustomMap(context, resourceIndex, &options);

    TEST_ASSERT_EQUAL_INT(firstMap->customGame.playerRace, secondMap->customGame.playerRace);
    TEST_ASSERT_EQUAL_INT(firstMap->customGame.enemyRace, secondMap->customGame.enemyRace);
    TEST_ASSERT_EQUAL_INT(firstMap->customGame.startConfigurationIndex,
                          secondMap->customGame.startConfigurationIndex);
    TEST_ASSERT_EQUAL_UINT16(firstMineAmount, resource->levelInfo.startEntities[0].amount);
    TEST_ASSERT_EQUAL_INT(firstMap->customCameraCenterX, secondMap->customCameraCenterX);
    TEST_ASSERT_EQUAL_INT(firstMap->customCameraCenterY, secondMap->customCameraCenterY);
}

static void run_launch_tests(void)
{
    Unity.TestFile = __FILE__;
    WAR_TEST_FILTER(test_campaign_launch_defaults_race_to_random);
    WAR_TEST_FILTER(test_custom_launch_defaults_both_races_to_random);
    WAR_TEST_FILTER(test_custom_launch_parses_shared_resources_and_seed);
    WAR_TEST_FILTER(test_numeric_options_are_decimal_and_seed_accepts_u64);
    WAR_TEST_FILTER(test_external_map_keeps_legacy_map_option);
    WAR_TEST_FILTER(test_custom_launch_rejects_unknown_map);
    WAR_TEST_FILTER(test_campaign_launch_rejects_custom_only_options);
    WAR_TEST_FILTER(test_campaign_map_type_mapping_covers_both_campaigns);
    WAR_TEST_FILTER(test_predefined_custom_catalog_names_are_resolvable);
    WAR_TEST_FILTER(test_seeded_custom_startup_is_reproducible);
}
