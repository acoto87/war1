const WarCheatDescriptor cheatDescriptors[] =
{
    // original cheats
    { WAR_CHEAT_GOLD,           "Pot of gold",              false,  applyGoldCheat          },
    { WAR_CHEAT_SPELLS,         "Eye of newt",              false,  applySpellsCheat        },
    { WAR_CHEAT_UPGRADES,       "Iron forge",               false,  applyUpgradesCheat      },
    { WAR_CHEAT_END,            "Ides of march",            false,  applyEndCheat           },
    { WAR_CHEAT_ENABLE,         "Corwin of Amber",          false,  applyEnableCheat        },
    { WAR_CHEAT_GOD_MODE,       "There can be only one",    false,  applyGodModeCheat       },
    { WAR_CHEAT_WIN,            "Yours truly",              false,  applyWinCheat           },
    { WAR_CHEAT_LOSS,           "Crushing defeat",          false,  applyLossCheat          },
    { WAR_CHEAT_FOG,            "Sally Shears",             false,  applyFogOfWarCheat      },
    { WAR_CHEAT_SKIP_HUMAN,     "Human",                    true,   applySkipHumanCheat     },
    { WAR_CHEAT_SKIP_ORC,       "Orc",                      true,   applySkipOrcCheat       },
    { WAR_CHEAT_SPEED,          "Hurry up guys",            false,  applySpeedCheat         },

    // custom cheats
    { WAR_CHEAT_MUSIC_VOL,      "Music vol",                true,   applyMusicVolCheat      },
    { WAR_CHEAT_SOUND_VOL,      "Sound vol",                true,   applySoundVolCheat      },
    { WAR_CHEAT_MUSIC,          "Music",                    true,   applyMusicCheat         },
    { WAR_CHEAT_SOUND,          "Sound",                    true,   applySoundCheat         },
    { WAR_CHEAT_GLOBAL_SCALE,   "Scale",                    true,   applyGlobalScaleCheat   },
    { WAR_CHEAT_GLOBAL_SPEED,   "Speed",                    true,   applyGlobalSpeedCheat   },
    { WAR_CHEAT_EDIT,           "Edit",                     true,   applyEditCheat          },
    { WAR_CHEAT_ADD_UNIT,       "Add unit",                 true,   applyAddUnitCheat       },
    { WAR_CHEAT_RAIN_OF_FIRE,   "Rain of fire",             false,  applyRainOfFireCheat    },
};

void applyCheat(WarContext* context, const char* text)
{
    for (s32 i = 0; i < arrayLength(cheatDescriptors); i++)
    {
        WarCheatDescriptor desc = cheatDescriptors[i];
        if (!desc.argument)
        {
            if (strCaseEquals(text, desc.text, true))
            {
                desc.cheatFunc(context, NULL);
                return;
            }
        }
        else
        {
            if (strCaseStartsWith(text, desc.text, true))
            {
                // skip the command text and the whitespace characters
                s32 skip = strlen(desc.text);
                while (text[skip] == ' ' || text[skip] == '\t')
                    skip++;

                const char* argument = text + skip;
                desc.cheatFunc(context, argument);
                return;
            }
        }
    }

    // if we reach here no cheat was applied
    logInfo("Unknown cheat: %s\n", text);
}

void applyGoldCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    increasePlayerResources(context, &map->players[0], CHEAT_GOLD_INCREASE, CHEAT_WOOD_INCREASE);
    setCheatsFeedback(context, CHEAT_FEEDBACK_WASCALLY_WABBIT);
}

void applySpellsCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    WarPlayerInfo* player = &map->players[0];

    WarFeatureType features[] =
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

    for (s32 i = 0; i < arrayLength(features); i++)
    {
        setFeatureAllowed(player, features[i], true);
    }

    WarUpgradeType spells[] =
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

    for (s32 i = 0; i < arrayLength(spells); i++)
    {
        WarUpgradeData upgradeData = getUpgradeData(spells[i]);
        setUpgradeAllowed(player, spells[i], upgradeData.maxLevelAllowed);
        while (hasRemainingUpgrade(player, spells[i]))
        {
            increaseUpgradeLevel(context, player, spells[i]);
        }
    }

    setCheatsFeedback(context, CHEAT_FEEDBACK_WASCALLY_WABBIT);
}

void applyUpgradesCheat(WarContext* context, const char* argument)
{
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

    setCheatsFeedback(context, CHEAT_FEEDBACK_WASCALLY_WABBIT);
}

void applyEndCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    map->playing = false;

    showDemoEndMenu(context, true);
}

void applyEnableCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
    {
        context->cheatsEnabled = true;
        setCheatsFeedback(context, "cheats enabled");
    }
    else
    {
        context->cheatsEnabled = false;
        setCheatsFeedback(context, "cheats disabled");
    }
}

void applyGodModeCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    WarPlayerInfo* player = &map->players[0];
    player->godMode = !player->godMode;

    setCheatsFeedback(context, CHEAT_FEEDBACK_WASCALLY_WABBIT);
}

void applyWinCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    map->result = WAR_LEVEL_RESULT_WIN;

    setCheatsFeedback(context, CHEAT_FEEDBACK_WASCALLY_WABBIT);
}

void applyLossCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    map->result = WAR_LEVEL_RESULT_LOSE;

    setCheatsFeedback(context, CHEAT_FEEDBACK_WASCALLY_WABBIT);
}

void applyFogOfWarCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    map->fowEnabled = !map->fowEnabled;

    setCheatsFeedback(context, CHEAT_FEEDBACK_WASCALLY_WABBIT);
}

void applySkipHumanCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    s32 level;
    if (strTryParseS32(argument, &level))
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

void applySkipOrcCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    s32 level;
    if (strTryParseS32(argument, &level))
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

void applySpeedCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
        return;

    WarMap* map = context->map;
    if (!map)
        return;

    map->hurryUp = !map->hurryUp;

    setCheatsFeedback(context, CHEAT_FEEDBACK_WASCALLY_WABBIT);
}

void applyMusicCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
        return;

    if (strCaseEquals(argument, "on", true))
    {
        context->musicEnabled = true;
        setCheatsFeedback(context, "Music on");
    }
    else if (strCaseEquals(argument, "off", true))
    {
        context->musicEnabled = false;
        setCheatsFeedback(context, "Music off");
    }
    else if (!isDemo(context))
    {
        s32 musicId;
        if (strTryParseS32(argument, &musicId))
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
                removeAudiosOfType(context, WAR_AUDIO_MIDI);
                createAudio(context, musicId, true);
                setCheatsFeedbackFormat(context, "Music %d set", musicId + 1);
            }
        }
    }
}

void applySoundCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
        return;

    if (strCaseEquals(argument, "on", true))
    {
        context->soundEnabled = true;
        setCheatsFeedback(context, "Sounds on");
    }
    else if (strCaseEquals(argument, "off", true))
    {
        context->soundEnabled = false;
        setCheatsFeedback(context, "Sounds off");
    }
}

void applyMusicVolCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
        return;

    s32 musicVol;
    if (strTryParseS32(argument, &musicVol))
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

        setMusicVolume(context, (f32)musicVol / 100);
        setCheatsFeedbackFormat(context, "Music volume set to %d", musicVol);
    }
}

void applySoundVolCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
        return;

    s32 sfxVol;
    if (strTryParseS32(argument, &sfxVol))
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

        setSoundVolume(context, (f32)sfxVol / 100);
        setCheatsFeedbackFormat(context, "Sounds volume set to %d", sfxVol);
    }
}

void applyGlobalScaleCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
        return;

    s32 scale;
    if (strTryParseS32(argument, &scale))
    {
        scale = clamp(scale, 1, 5);
        setGlobalScale(context, scale);
        setCheatsFeedbackFormat(context, "Global scale set to %d", scale);
    }
}

void applyGlobalSpeedCheat(WarContext* context, const char* argument)
{
    if (!context->cheatsEnabled)
        return;

    s32 speed;
    if (strTryParseS32(argument, &speed))
    {
        speed = clamp(speed, 1, 5);
        setGlobalSpeed(context, speed);
        setCheatsFeedbackFormat(context, "Global speed set to %d", speed);
    }
}

void applyEditCheat(WarContext* context, const char* argument)
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

    if (strCaseEquals(argument, "off", true))
    {
        setCheatsFeedback(context, "Edit off");
        return;
    }

    if (strCaseEquals(argument, "trees", true))
    {
        map->editingTrees = true;
        setCheatsFeedback(context, "Edit trees on");
    }
    else if (strCaseEquals(argument, "walls", true))
    {
        map->editingWalls = true;
        setCheatsFeedback(context, "Edit walls on");
    }
    else if (strCaseEquals(argument, "roads", true))
    {
        map->editingRoads = true;
        setCheatsFeedback(context, "Edit roads on");
    }
    else if (strCaseEquals(argument, "ruins", true))
    {
        map->editingRuins = true;
        setCheatsFeedback(context, "Edit ruins on");
    }
}

void applyRainOfFireCheat(WarContext* context, const char* argument)
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
    map->editingRainOfFire = !map->editingRainOfFire;
    map->addingUnit = false;

    if (map->editingRainOfFire)
        setCheatsFeedback(context, "Rain of fire on");
    else
        setCheatsFeedback(context, "Rain of fire off");
}

void applyAddUnitCheat(WarContext* context, const char* argument)
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

    if (strCaseStartsWith(argument, "off", true))
    {
        setCheatsFeedback(context, "Add unit off");
        return;
    }

    if (strCaseStartsWith(argument, "CATAPULT", true))
    {
        const char* part2 = strSkipUntil(argument + strlen("CATAPULT"), " ");
        if (strCaseStartsWith(part2, "HUMANS", true))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_CATAPULT_HUMANS;
        }
        else if (strCaseStartsWith(part2, "ORCS", true))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_CATAPULT_ORCS;
        }
    }
    else if (strCaseStartsWith(argument, "FARM", true))
    {
        const char* part2 = strSkipUntil(argument + strlen("FARM"), " ");
        if (strCaseStartsWith(part2, "HUMANS", true))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_FARM_HUMANS;
        }
        else if (strCaseStartsWith(part2, "ORCS", true))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_FARM_ORCS;
        }
    }
    else if (strCaseStartsWith(argument, "BARRACkS", true))
    {
        const char* part2 = strSkipUntil(argument + strlen("BARRACkS"), " ");
        if (strCaseStartsWith(part2, "HUMANS", true))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_BARRACKS_HUMANS;
        }
        else if (strCaseStartsWith(part2, "ORCS", true))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_BARRACKS_ORCS;
        }
    }
    else if (strCaseStartsWith(argument, "TOWER", true))
    {
        const char* part2 = strSkipUntil(argument + strlen("TOWER"), " ");
        if (strCaseStartsWith(part2, "HUMANS", true))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_TOWER_HUMANS;
        }
        else if (strCaseStartsWith(part2, "ORCS", true))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_TOWER_ORCS;
        }
    }
    else if (strCaseStartsWith(argument, "TOWN HALL", true))
    {
        const char* part2 = strSkipUntil(argument + strlen("TOWN HALL"), " ");
        if (strCaseStartsWith(part2, "HUMANS", true))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_TOWNHALL_HUMANS;
        }
        else if (strCaseStartsWith(part2, "ORCS", true))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_TOWNHALL_ORCS;
        }
    }
    else if (strCaseStartsWith(argument, "MILL", true))
    {
        const char* part2 = strSkipUntil(argument + strlen("MILL"), " ");
        if (strCaseStartsWith(part2, "HUMANS", true))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_LUMBERMILL_HUMANS;
        }
        else if (strCaseStartsWith(part2, "ORCS", true))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_LUMBERMILL_ORCS;
        }
    }
    else if (strCaseStartsWith(argument, "BLACKSMITH", true))
    {
        const char* part2 = strSkipUntil(argument + strlen("BLACKSMITH"), " ");
        if (strCaseStartsWith(part2, "HUMANS", true))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_BLACKSMITH_HUMANS;
        }
        else if (strCaseStartsWith(part2, "ORCS", true))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_BLACKSMITH_ORCS;
        }
    }
    else if (strCaseStartsWith(argument, "CORPSE", true))
    {
        const char* part2 = strSkipUntil(argument + strlen("CORPSE"), " ");
        if (strCaseStartsWith(part2, "HUMANS", true))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_HUMAN_CORPSE;
        }
        else if (strCaseStartsWith(part2, "ORCS", true))
        {
            map->addingUnit = true;
            map->addingUnitType = WAR_UNIT_ORC_CORPSE;
        }
    }
    else
    {
        for (s32 i = 0; i < arrayLength(unitsData); i++)
        {
            if (strCaseEquals(argument, unitsData[i].name, true))
            {
                map->addingUnit = true;
                map->addingUnitType = unitsData[i].type;
                break;
            }
        }
    }

    if (map->addingUnit)
    {
        setCheatsFeedbackFormat(context, "Add unit %s", argument);
    }
}
