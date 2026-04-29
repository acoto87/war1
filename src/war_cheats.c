#include "war_cheats.h"

#include "shl/wstr.h"

#include "war_audio.h"
#include "war_entities.h"
#include "war_game.h"
#include "war_scenes.h"
#include "war_units.h"

const WarCheatDescriptor cheatDescriptors[] =
{
    // original cheats
    { WAR_CHEAT_GOLD,           "Pot of gold",              false,  wcheat_applyGoldCheat          },
    { WAR_CHEAT_SPELLS,         "Eye of newt",              false,  wcheat_applySpellsCheat        },
    { WAR_CHEAT_UPGRADES,       "Iron forge",               false,  wcheat_applyUpgradesCheat      },
    { WAR_CHEAT_END,            "Ides of march",            false,  wcheat_applyEndCheat           },
    { WAR_CHEAT_ENABLE,         "Corwin of Amber",          false,  wcheat_applyEnableCheat        },
    { WAR_CHEAT_GOD_MODE,       "There can be only one",    false,  wcheat_applyGodModeCheat       },
    { WAR_CHEAT_WIN,            "Yours truly",              false,  wcheat_applyWinCheat           },
    { WAR_CHEAT_LOSS,           "Crushing defeat",          false,  wcheat_applyLossCheat          },
    { WAR_CHEAT_FOG,            "Sally Shears",             false,  wcheat_applyFogOfWarCheat      },
    { WAR_CHEAT_SKIP_HUMAN,     "Human",                    true,   wcheat_applySkipHumanCheat     },
    { WAR_CHEAT_SKIP_ORC,       "Orc",                      true,   wcheat_applySkipOrcCheat       },
    { WAR_CHEAT_SPEED,          "Hurry up guys",            false,  wcheat_applySpeedCheat         },

    // custom cheats
    { WAR_CHEAT_MUSIC_VOL,      "Music vol",                true,   wcheat_applyMusicVolCheat      },
    { WAR_CHEAT_SOUND_VOL,      "Sound vol",                true,   wcheat_applySoundVolCheat      },
    { WAR_CHEAT_MUSIC,          "Music",                    true,   wcheat_applyMusicCheat         },
    { WAR_CHEAT_SOUND,          "Sound",                    true,   wcheat_applySoundCheat         },
    { WAR_CHEAT_GLOBAL_SCALE,   "Scale",                    true,   wcheat_applyGlobalScaleCheat   },
    { WAR_CHEAT_GLOBAL_SPEED,   "Speed",                    true,   wcheat_applyGlobalSpeedCheat   },
    { WAR_CHEAT_EDIT,           "Edit",                     true,   wcheat_applyEditCheat          },
    { WAR_CHEAT_ADD_UNIT,       "Add unit",                 true,   wcheat_applyAddUnitCheat       },
    { WAR_CHEAT_RAIN_OF_FIRE,   "Rain of fire",             false,  wcheat_applyRainOfFireCheat    },
};

void wcheat_applyCheat(WarContext* context, StringView text)
{
    for (s32 i = 0; i < arrayLength(cheatDescriptors); i++)
    {
        WarCheatDescriptor desc = cheatDescriptors[i];
        StringView descText = wsv_fromCString(desc.text);

        if (!desc.argument)
        {
            if (wsv_equalsIgnoreCase(text, descText))
            {
                desc.cheatFunc(context, wsv_empty());
                return;
            }
        }
        else
        {
            if (wsv_startsWithIgnoreCase(text, descText))
            {
                StringView argument = wsv_subview(text, descText.length);
                argument = wsv_trimLeft(argument); // skip whitespace characters

                desc.cheatFunc(context, argument);
                return;
            }
        }
    }

    // if we reach here no cheat was applied
    logInfo("Unknown cheat: %.*s", (int)text.length, text.data);
}

void wcheat_applyGoldCheat(WarContext* context, StringView argument)
{
    NOT_USED(argument);

    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    increasePlayerResources(context, &map->players[0], CHEAT_GOLD_INCREASE, CHEAT_WOOD_INCREASE);
    setCheatsFeedback(context, wstr_fromCString(CHEAT_FEEDBACK_WASCALLY_WABBIT));
}

void wcheat_applySpellsCheat(WarContext* context, StringView argument)
{
    NOT_USED(argument);

    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    WarPlayerInfo* player = &map->players[0];

    WarFeatureType spellFeatures[] =
    {
        WAR_FEATURE_SPELL_HEALING,
        WAR_FEATURE_SPELL_RAISE_DEAD,
        WAR_FEATURE_SPELL_FAR_SIGHT,
        WAR_FEATURE_SPELL_DARK_VISION,
        WAR_FEATURE_SPELL_INVISIBILITY,
        WAR_FEATURE_SPELL_UNHOLY_ARMOR,
        WAR_FEATURE_SPELL_SCORPION,
        WAR_FEATURE_SPELL_SPIDER,
        WAR_FEATURE_SPELL_RAIN_OF_FIRE,
        WAR_FEATURE_SPELL_POISON_CLOUD,
        WAR_FEATURE_SPELL_WATER_ELEMENTAL,
        WAR_FEATURE_SPELL_DAEMON,
    };

    for (s32 i = 0; i < arrayLength(spellFeatures); i++)
    {
        setFeatureAllowed(player, spellFeatures[i], true);
    }

    WarUpgradeType upgradeFeatures[] =
    {
        WAR_UPGRADE_SCORPIONS,
        WAR_UPGRADE_SPIDERS,
        WAR_UPGRADE_RAIN_OF_FIRE,
        WAR_UPGRADE_POISON_CLOUD,
        WAR_UPGRADE_WATER_ELEMENTAL,
        WAR_UPGRADE_DAEMON,
        WAR_UPGRADE_HEALING,
        WAR_UPGRADE_RAISE_DEAD,
        WAR_UPGRADE_FAR_SIGHT,
        WAR_UPGRADE_DARK_VISION,
        WAR_UPGRADE_INVISIBILITY,
        WAR_UPGRADE_UNHOLY_ARMOR
    };

    for (s32 i = 0; i < arrayLength(upgradeFeatures); i++)
    {
        WarUpgradeData upgradeData = getUpgradeData(upgradeFeatures[i]);
        setUpgradeAllowed(player, upgradeFeatures[i], upgradeData.maxLevelAllowed);
        while (hasRemainingUpgrade(player, upgradeFeatures[i]))
        {
            increaseUpgradeLevel(context, player, upgradeFeatures[i]);
        }
    }

    setCheatsFeedback(context, wstr_fromCString(CHEAT_FEEDBACK_WASCALLY_WABBIT));
}

void wcheat_applyUpgradesCheat(WarContext* context, StringView argument)
{
    NOT_USED(argument);

    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    WarPlayerInfo* player = &map->players[0];

    WarUpgradeType upgrades[] =
    {
        WAR_UPGRADE_ARROWS,
        WAR_UPGRADE_SPEARS,
        WAR_UPGRADE_SWORDS,
        WAR_UPGRADE_AXES,
        WAR_UPGRADE_SHIELD
    };

    for (s32 i = 0; i < arrayLength(upgrades); i++)
    {
        WarUpgradeData upgradeData = getUpgradeData(upgrades[i]);
        setUpgradeAllowed(player, upgrades[i], upgradeData.maxLevelAllowed);
        while (hasRemainingUpgrade(player, upgrades[i]))
        {
            increaseUpgradeLevel(context, player, upgrades[i]);
        }
    }

    setCheatsFeedback(context, wstr_fromCString(CHEAT_FEEDBACK_WASCALLY_WABBIT));
}

void wcheat_applyEndCheat(WarContext* context, StringView argument)
{
    NOT_USED(argument);

    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    map->playing = false;

    showDemoEndMenu(context, true);
}

void wcheat_applyEnableCheat(WarContext* context, StringView argument)
{
    NOT_USED(argument);

    if (!context->cheatsEnabled)
    {
        context->cheatsEnabled = true;
        setCheatsFeedback(context, wstr_fromCString("cheats enabled"));
    }
    else
    {
        context->cheatsEnabled = false;
        setCheatsFeedback(context, wstr_fromCString("cheats disabled"));
    }
}

void wcheat_applyGodModeCheat(WarContext* context, StringView argument)
{
    NOT_USED(argument);

    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    WarPlayerInfo* player = &map->players[0];
    player->godMode = !player->godMode;

    setCheatsFeedback(context, wstr_fromCString(CHEAT_FEEDBACK_WASCALLY_WABBIT));
}

void wcheat_applyWinCheat(WarContext* context, StringView argument)
{
    NOT_USED(argument);

    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    map->result = WAR_LEVEL_RESULT_WIN;

    setCheatsFeedback(context, wstr_fromCString(CHEAT_FEEDBACK_WASCALLY_WABBIT));
}

void wcheat_applyLossCheat(WarContext* context, StringView argument)
{
    NOT_USED(argument);

    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    map->result = WAR_LEVEL_RESULT_LOSE;

    setCheatsFeedback(context, wstr_fromCString(CHEAT_FEEDBACK_WASCALLY_WABBIT));
}

void wcheat_applyFogOfWarCheat(WarContext* context, StringView argument)
{
    NOT_USED(argument);

    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    map->fowEnabled = !map->fowEnabled;

    setCheatsFeedback(context, wstr_fromCString(CHEAT_FEEDBACK_WASCALLY_WABBIT));
}

void wcheat_applySkipHumanCheat(WarContext* context, StringView argument)
{
    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    s32 level;
    if (wsv_tryParseS32(argument, &level))
    {
        // TODO: remove this check when more levels are allowed
        if (level <= 0 || level > 2)
            return;

        WarScene* scene = createScene(context, WAR_SCENE_BRIEFING);
        scene->briefing.race = WAR_RACE_HUMANS;
        scene->briefing.mapType = WAR_CAMPAIGN_HUMANS_01 + 2 * (level - 1);
        setNextScene(context, scene, 1.0f);
    }
}

void wcheat_applySkipOrcCheat(WarContext* context, StringView argument)
{
    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    s32 level;
    if (wsv_tryParseS32(argument, &level))
    {
        // TODO: remove this check when more levels are allowed
        if (level <= 0 || level > 2)
            return;

        WarScene* scene = createScene(context, WAR_SCENE_BRIEFING);
        scene->briefing.race = WAR_RACE_ORCS;
        scene->briefing.mapType = WAR_CAMPAIGN_ORCS_01 + 2 * (level - 1);
        setNextScene(context, scene, 1.0f);
    }
}

void wcheat_applySpeedCheat(WarContext* context, StringView argument)
{
    NOT_USED(argument);

    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    map->hurryUp = !map->hurryUp;

    setCheatsFeedback(context, wstr_fromCString(CHEAT_FEEDBACK_WASCALLY_WABBIT));
}

void wcheat_applyMusicCheat(WarContext* context, StringView argument)
{
    if (!context->cheatsEnabled)
        return;

    if (wsv_equalsIgnoreCase(argument, wsv_fromCString("on")))
    {
        context->musicEnabled = true;
        setCheatsFeedback(context, wstr_fromCString("Music on"));
    }
    else if (wsv_equalsIgnoreCase(argument, wsv_fromCString("off")))
    {
        context->musicEnabled = false;
        setCheatsFeedback(context, wstr_fromCString("Music off"));
    }
    else if (!isDemo(context))
    {
        s32 musicId;
        if (wsv_tryParseS32(argument, &musicId))
        {
            // argument is expected in the range 1-45, so convert it to the range 0-44
            musicId--;

            if (musicId >= WAR_MUSIC_00 && musicId <= WAR_MUSIC_44)
            {
                // before changing the music, remove the current one
                // almost all the time there should only one active
                // but I really don't if that will hold true in the future
                //
                // for now remove all the active music (audios of type WAR_AUDIO_MIDI)
                // and the create the new one
                waud_removeAudiosOfType(context, WAR_AUDIO_MIDI);
                waud_createAudio(context, musicId, true);
                setCheatsFeedback(context, wstr_fromCStringFormat("Music %d set", musicId + 1));
            }
        }
    }
}

void wcheat_applySoundCheat(WarContext* context, StringView argument)
{
    if (!context->cheatsEnabled)
        return;

    if (wsv_equalsIgnoreCase(argument, wsv_fromCString("on")))
    {
        context->soundEnabled = true;
        setCheatsFeedback(context, wstr_fromCString("Sounds on"));
    }
    else if (wsv_equalsIgnoreCase(argument, wsv_fromCString("off")))
    {
        context->soundEnabled = false;
        setCheatsFeedback(context, wstr_fromCString("Sounds off"));
    }
}

void wcheat_applyMusicVolCheat(WarContext* context, StringView argument)
{
    if (!context->cheatsEnabled)
        return;

    s32 musicVol;
    if (wsv_tryParseS32(argument, &musicVol))
    {
        musicVol = clamp(musicVol, 0, 100);

        // round the argument to a value multiple of 5
        switch (musicVol % 5)
        {
            case 1: { musicVol -= 1; break; }
            case 2: { musicVol -= 2; break; }
            case 3: { musicVol += 2; break; }
            case 4: { musicVol += 1; break; }
            default:
            {
                // do nothing, it's already a multiple of 5
                break;
            }
        }

        context->audioEnabled = true;
        context->musicEnabled = true;

        setMusicVolume(context, (f32)musicVol / 100.0f);
        setCheatsFeedback(context, wstr_fromCStringFormat("Music volume set to %d", musicVol));
    }
}

void wcheat_applySoundVolCheat(WarContext* context, StringView argument)
{
    if (!context->cheatsEnabled)
        return;

    s32 sfxVol;
    if (wsv_tryParseS32(argument, &sfxVol))
    {
        sfxVol = clamp(sfxVol, 0, 100);

        // round the argument to a value multiple of 5
        switch (sfxVol % 5)
        {
            case 1: { sfxVol -= 1; break; }
            case 2: { sfxVol -= 2; break; }
            case 3: { sfxVol += 2; break; }
            case 4: { sfxVol += 1; break; }
            default:
            {
                // do nothing, it's already a multiple of 5
                break;
            }
        }

        context->audioEnabled = true;
        context->soundEnabled = true;

        setSoundVolume(context, (f32)sfxVol / 100.0f);
        setCheatsFeedback(context, wstr_fromCStringFormat("Sounds volume set to %d", sfxVol));
    }
}

void wcheat_applyGlobalScaleCheat(WarContext* context, StringView argument)
{
    if (!context->cheatsEnabled)
        return;

    s32 scale;
    if (wsv_tryParseS32(argument, &scale))
    {
        scale = clamp(scale, 1, 5);
        setGlobalScale(context, (f32)scale);
        setCheatsFeedback(context, wstr_fromCStringFormat("Global scale set to %d", scale));
    }
}

void wcheat_applyGlobalSpeedCheat(WarContext* context, StringView argument)
{
    if (!context->cheatsEnabled)
        return;

    s32 speed;
    if (wsv_tryParseS32(argument, &speed))
    {
        speed = clamp(speed, 1, 5);
        setGlobalSpeed(context, (f32)speed);
        setCheatsFeedback(context, wstr_fromCStringFormat("Global speed set to %d", speed));
    }
}

void wcheat_applyEditCheat(WarContext* context, StringView argument)
{
    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    map->editingTrees = false;
    map->editingWalls = false;
    map->editingRoads = false;
    map->editingRuins = false;
    map->editingRainOfFire = false;
    map->addingUnit = false;

    if (wsv_equalsIgnoreCase(argument, wsv_fromCString("off")))
    {
        setCheatsFeedback(context, wstr_fromCString("Edit off"));
        return;
    }

    if (wsv_equalsIgnoreCase(argument, wsv_fromCString("trees")))
    {
        map->editingTrees = true;
        setCheatsFeedback(context, wstr_fromCString("Edit trees on"));
    }
    else if (wsv_equalsIgnoreCase(argument, wsv_fromCString("walls")))
    {
        map->editingWalls = true;
        setCheatsFeedback(context, wstr_fromCString("Edit walls on"));
    }
    else if (wsv_equalsIgnoreCase(argument, wsv_fromCString("roads")))
    {
        map->editingRoads = true;
        setCheatsFeedback(context, wstr_fromCString("Edit roads on"));
    }
    else if (wsv_equalsIgnoreCase(argument, wsv_fromCString("ruins")))
    {
        map->editingRuins = true;
        setCheatsFeedback(context, wstr_fromCString("Edit ruins on"));
    }
}

void wcheat_applyRainOfFireCheat(WarContext* context, StringView argument)
{
    NOT_USED(argument);

    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    map->editingTrees = false;
    map->editingWalls = false;
    map->editingRoads = false;
    map->editingRuins = false;
    map->editingRainOfFire = !map->editingRainOfFire;
    map->addingUnit = false;

    if (map->editingRainOfFire)
        setCheatsFeedback(context, wstr_fromCString("Rain of fire on"));
    else
        setCheatsFeedback(context, wstr_fromCString("Rain of fire off"));
}

void wcheat_applyAddUnitCheat(WarContext* context, StringView argument)
{
    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    map->editingTrees = false;
    map->editingWalls = false;
    map->editingRoads = false;
    map->editingRuins = false;
    map->editingRainOfFire = false;

    map->addingUnit = false;

    if (wsv_startsWithIgnoreCase(argument, wsv_fromCString("off")))
    {
        setCheatsFeedback(context, wstr_fromCString("Add unit off"));
        return;
    }

    StringView part1, part2;
    if (!wsv_nextToken(&argument, wsv_fromCString(" "), &part1))
    {
        part1 = argument;
    }
    wsv_nextToken(&argument, wsv_fromCString(" "), &part2);

    if (wsv_equalsIgnoreCase(part1, wsv_fromCString("CATAPULT")))
    {
        if (wsv_startsWithIgnoreCase(part2, wsv_fromCString("HUMANS")))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_CATAPULT_HUMANS;
        }
        else if (wsv_startsWithIgnoreCase(part2, wsv_fromCString("ORCS")))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_CATAPULT_ORCS;
        }
    }
    else if (wsv_equalsIgnoreCase(part1, wsv_fromCString("FARM")))
    {
        if (wsv_startsWithIgnoreCase(part2, wsv_fromCString("HUMANS")))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_FARM_HUMANS;
        }
        else if (wsv_startsWithIgnoreCase(part2, wsv_fromCString("ORCS")))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_FARM_ORCS;
        }
    }
    else if (wsv_equalsIgnoreCase(part1, wsv_fromCString("BARRACKS")))
    {
        if (wsv_startsWithIgnoreCase(part2, wsv_fromCString("HUMANS")))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_BARRACKS_HUMANS;
        }
        else if (wsv_startsWithIgnoreCase(part2, wsv_fromCString("ORCS")))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_BARRACKS_ORCS;
        }
    }
    else if (wsv_equalsIgnoreCase(part1, wsv_fromCString("TOWER")))
    {
        if (wsv_startsWithIgnoreCase(part2, wsv_fromCString("HUMANS")))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_TOWER_HUMANS;
        }
        else if (wsv_startsWithIgnoreCase(part2, wsv_fromCString("ORCS")))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_TOWER_ORCS;
        }
    }
    else if (wsv_equalsIgnoreCase(part1, wsv_fromCString("TOWN")) && wsv_equalsIgnoreCase(part2, wsv_fromCString("HALL")))
    {
        StringView part3;
        wsv_nextToken(&argument, wsv_fromCString(" "), &part3);

        if (wsv_startsWithIgnoreCase(part3, wsv_fromCString("HUMANS")))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_TOWNHALL_HUMANS;
        }
        else if (wsv_startsWithIgnoreCase(part3, wsv_fromCString("ORCS")))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_TOWNHALL_ORCS;
        }
    }
    else if (wsv_equalsIgnoreCase(part1, wsv_fromCString("MILL")))
    {
        if (wsv_startsWithIgnoreCase(part2, wsv_fromCString("HUMANS")))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_LUMBERMILL_HUMANS;
        }
        else if (wsv_startsWithIgnoreCase(part2, wsv_fromCString("ORCS")))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_LUMBERMILL_ORCS;
        }
    }
    else if (wsv_equalsIgnoreCase(part1, wsv_fromCString("BLACKSMITH")))
    {
        if (wsv_startsWithIgnoreCase(part2, wsv_fromCString("HUMANS")))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_BLACKSMITH_HUMANS;
        }
        else if (wsv_startsWithIgnoreCase(part2, wsv_fromCString("ORCS")))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_BLACKSMITH_ORCS;
        }
    }
    else if (wsv_equalsIgnoreCase(part1, wsv_fromCString("CORPSE")))
    {
        if (wsv_startsWithIgnoreCase(part2, wsv_fromCString("HUMANS")))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_HUMAN_CORPSE;
        }
        else if (wsv_startsWithIgnoreCase(part2, wsv_fromCString("ORCS")))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_ORC_CORPSE;
        }
    }
    else
    {
        for (s32 i = 0; i < arrayLength(unitsData); i++)
        {
            if (wsv_equalsIgnoreCase(part1, unitsData[i].name))
            {
                map->addingUnit = true;
                map->addingUnitType = (WarUnitType)i;
                break;
            }
        }
    }

    if (map->addingUnit)
    {
        setCheatsFeedback(context, wstr_fromCStringFormat("Add unit %.*s", (int)argument.length, argument.data));
    }
}
