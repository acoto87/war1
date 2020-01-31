const WarCheatDescriptor cheatDescriptors[] =
{
    // custom cheats
    { WAR_CHEAT_MUSIC_VOL,      "Music vol",                true,  applyMusicVolCheat  },
    { WAR_CHEAT_SOUND_VOL,      "Sound vol",                true,  applySoundVolCheat  },
    { WAR_CHEAT_MUSIC,          "Music",                    true,  applyMusicCheat     },
    { WAR_CHEAT_SOUND,          "Sound",                    true,  applySoundCheat     },

    // original cheats
    { WAR_CHEAT_GOLD,           "Pot of gold",              false, applyGoldCheat      },
    { WAR_CHEAT_SPELLS,         "Eye of newt",              false, applySpellsCheat    },
    { WAR_CHEAT_UPGRADES,       "Iron forge",               false, applyUpgradesCheat  },
    { WAR_CHEAT_END,            "Ides of march",            false, applyEndCheat       },
    { WAR_CHEAT_ENABLE,         "Corwin of Amber",          false, applyEnableCheat    },
    { WAR_CHEAT_GOD_MODE,       "There can be only one",    false, applyGodModeCheat   },
    { WAR_CHEAT_LOSS,           "Crushing defeat",          false, applyLossCheat      },
    { WAR_CHEAT_FOG,            "Sally Shears",             false, applyFogOfWarCheat  },
    { WAR_CHEAT_SKIP_HUMAN,     "Human",                    true,  applySkipHumanCheat },
    { WAR_CHEAT_SKIP_ORC,       "Orc",                      true,  applySkipOrcCheat   },
    { WAR_CHEAT_SPEED,          "Hurry up guys",            false, applySpeedCheat     },
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

void applyMusicCheat(WarContext* context, const char* argument)
{
    WarMap* map = context->map;
    assert(map);

    if (!map->cheatsEnabled)
        return;

    if (strCaseEquals(argument, "on", true))
    {
        map->settings.musicEnabled = true;
    }
    else if (strCaseEquals(argument, "off", true))
    {
        map->settings.musicEnabled = false;
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
                WarEntityIdList toRemove;
                WarEntityIdListInit(&toRemove, WarEntityIdListDefaultOptions);

                WarEntityList* audios = getEntitiesOfType(context, WAR_ENTITY_TYPE_AUDIO);
                for (s32 i = 0; i < audios->count; i++)
                {
                    WarEntity* entity = audios->items[i];
                    if (entity)
                    {
                        WarAudioComponent* audio = &entity->audio;
                        if (audio->type == WAR_AUDIO_MIDI)
                        {
                            WarEntityIdListAdd(&toRemove, entity->id);
                        }
                    }
                }

                for (s32 i = 0; i < toRemove.count; i++)
                {
                    WarEntityId entityId = toRemove.items[i];
                    removeEntityById(context, entityId);
                }

                WarEntityIdListFree(&toRemove);

                createAudio(context, musicId, true);
            }
        }
    }
}

void applySoundCheat(WarContext* context, const char* argument)
{
    WarMap* map = context->map;
    assert(map);

    if (!map->cheatsEnabled)
        return;

    if (strCaseEquals(argument, "on", true))
    {
        map->settings.sfxEnabled = true;
    }
    else if (strCaseEquals(argument, "off", true))
    {
        map->settings.sfxEnabled = false;
    }
}

void applyMusicVolCheat(WarContext* context, const char* argument)
{
    WarMap* map = context->map;
    assert(map);

    if (!map->cheatsEnabled)
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

        map->settings.musicEnabled = true;
        map->settings.musicVol = musicVol;
    }
}

void applySoundVolCheat(WarContext* context, const char* argument)
{
    WarMap* map = context->map;
    assert(map);

    if (!map->cheatsEnabled)
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

        map->settings.sfxEnabled = true;
        map->settings.sfxVol = sfxVol;
    }
}

void applyGoldCheat(WarContext* context, const char* argument)
{
    WarMap* map = context->map;
    assert(map);

    if (!map->cheatsEnabled)
        return;

    increasePlayerResources(context, &map->players[0], CHEAT_GOLD_INCREASE, CHEAT_WOOD_INCREASE);
}

void applySpellsCheat(WarContext* context, const char* argument)
{
    WarMap* map = context->map;
    assert(map);

    if (!map->cheatsEnabled)
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
        while (hasRemainingUpgrade(player, spells[i]))
        {
            increaseUpgradeLevel(context, player, spells[i]);
        }
    }
}

void applyUpgradesCheat(WarContext* context, const char* argument)
{
    WarMap* map = context->map;
    assert(map);

    if (!map->cheatsEnabled)
        return;

    WarPlayerInfo* player = &map->players[0];

    WarUpgradeType upgrades[] =
    {
        WAR_UPGRADE_ARROWS,
        WAR_UPGRADE_SPEARS,
        WAR_UPGRADE_SWORDS,
        WAR_UPGRADE_AXES,
        WAR_UPGRADE_HORSES,
        WAR_UPGRADE_WOLVES,
        WAR_UPGRADE_SHIELD
    };

    for (s32 i = 0; i < arrayLength(upgrades); i++)
    {
        while (hasRemainingUpgrade(player, upgrades[i]))
        {
            increaseUpgradeLevel(context, player, upgrades[i]);
        }
    }
}

void applyEndCheat(WarContext* context, const char* argument)
{
    WarMap* map = context->map;
    assert(map);

    if (!map->cheatsEnabled)
        return;

    NOT_IMPLEMENTED();
}

void applyEnableCheat(WarContext* context, const char* argument)
{
    WarMap* map = context->map;
    assert(map);

    map->cheatsEnabled = !map->cheatsEnabled;
}

void applyGodModeCheat(WarContext* context, const char* argument)
{
    WarMap* map = context->map;
    assert(map);

    if (!map->cheatsEnabled)
        return;

    NOT_IMPLEMENTED();
}

void applyLossCheat(WarContext* context, const char* argument)
{
    WarMap* map = context->map;
    assert(map);

    if (!map->cheatsEnabled)
        return;

    NOT_IMPLEMENTED();
}

void applyFogOfWarCheat(WarContext* context, const char* argument)
{
    WarMap* map = context->map;
    assert(map);

    if (!map->cheatsEnabled)
        return;

    NOT_IMPLEMENTED();
}

void applySkipHumanCheat(WarContext* context, const char* argument)
{
    WarMap* map = context->map;
    assert(map);

    if (!map->cheatsEnabled)
        return;

    NOT_IMPLEMENTED();
}

void applySkipOrcCheat(WarContext* context, const char* argument)
{
    WarMap* map = context->map;
    assert(map);

    if (!map->cheatsEnabled)
        return;

    NOT_IMPLEMENTED();
}

void applySpeedCheat(WarContext* context, const char* argument)
{
    WarMap* map = context->map;
    assert(map);

    if (!map->cheatsEnabled)
        return;

    NOT_IMPLEMENTED();
}
