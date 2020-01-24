const WarCheatDescriptor cheatDescriptors[] =
{
    // custom cheats
    { WAR_CHEAT_MUSIC,          "Music",                    false,  applyMusicCheat     },
    { WAR_CHEAT_MUSIC_VOL,      "Music vol",                false,  applyMusicVolCheat  },
    { WAR_CHEAT_SOUND_VOL,      "Mound vol",                false,  applySoundVolCheat  },

    // original cheats
    { WAR_CHEAT_GOLD,           "Pot of gold",              true,   applyGoldCheat      },
    { WAR_CHEAT_SPELLS,         "Eye of newt",              true,   applySpellsCheat    },
    { WAR_CHEAT_UPGRADES,       "Iron forge",               true,   applyUpgradesCheat  },
    { WAR_CHEAT_END,            "Ides of march",            true,   applyEndCheat       },
    { WAR_CHEAT_ENABLE,         "Corwin of Amber",          true,   applyEnableCheat    },
    { WAR_CHEAT_GOD_MODE,       "There can be only one",    true,   applyGodModeCheat   },
    { WAR_CHEAT_LOSS,           "Crushing defeat",          true,   applyLossCheat      },
    { WAR_CHEAT_FOG,            "Sally Shears",             true,   applyFogOfWarCheat  },
    { WAR_CHEAT_SKIP_HUMAN,     "Human",                    false,  applySkipHumanCheat },
    { WAR_CHEAT_SKIP_ORC,       "Orc",                      false,  applySkipOrcCheat   },
    { WAR_CHEAT_SPEED,          "Hurry up guys",            true,   applySpeedCheat     },
};

void applyCheat(WarContext* context, const char* text)
{
    for (s32 i = 0; i < arrayLength(cheatDescriptors); i++)
    {
        s32 argument = 0;

        WarCheatDescriptor desc = cheatDescriptors[i];
        if (!desc.argument)
        {
            if (strCaseEquals(text, desc.text, true))
            {
                desc.cheatFunc(context, 0);
                return;
            }
        }
        else
        {
            if (strCaseStartsWith(text, desc.text, true))
            {
                int argument = strParseS32(text + strlen(desc.text));
                desc.cheatFunc(context, argument);
                return;
            }
        }
    }

    // if we reach here no cheat was applied
    logInfo("Unknown cheat: %s\n", text);
}

void applyMusicCheat(WarContext* context, s32 argument)
{
    WarMap* map = context->map;
    assert(map);

    // argument is expected in the range 1-45, so convert it to the range 0-44
    argument--;

    if (argument >= WAR_MUSIC_00 && argument <= WAR_MUSIC_44)
    {
        if (!isDemo(context))
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

            createAudio(context, argument, true);
        }
    }
}

void applyMusicVolCheat(WarContext* context, s32 argument)
{
    WarMap* map = context->map;
    assert(map);

    f32 volume = clamp((f32)argument / 100, 0, 1);
    setMusicVolume(context, volume);
}

void applySoundVolCheat(WarContext* context, s32 argument)
{
    WarMap* map = context->map;
    assert(map);

    f32 volume = clamp((f32)argument / 100, 0, 1);
    setSoundVolume(context, volume);
}

void applyGoldCheat(WarContext* context, s32 argument)
{
    WarMap* map = context->map;
    assert(map);

    increasePlayerResources(context, &map->players[0], 10000, 5000);
}

void applySpellsCheat(WarContext* context, s32 argument)
{
    WarMap* map = context->map;
    assert(map);

    NOT_IMPLEMENTED();
}

void applyUpgradesCheat(WarContext* context, s32 argument)
{
    WarMap* map = context->map;
    assert(map);

    NOT_IMPLEMENTED();
}

void applyEndCheat(WarContext* context, s32 argument)
{
    WarMap* map = context->map;
    assert(map);

    NOT_IMPLEMENTED();
}

void applyEnableCheat(WarContext* context, s32 argument)
{
    WarMap* map = context->map;
    assert(map);

    NOT_IMPLEMENTED();
}

void applyGodModeCheat(WarContext* context, s32 argument)
{
    WarMap* map = context->map;
    assert(map);

    NOT_IMPLEMENTED();
}

void applyLossCheat(WarContext* context, s32 argument)
{
    WarMap* map = context->map;
    assert(map);

    NOT_IMPLEMENTED();
}

void applyFogOfWarCheat(WarContext* context, s32 argument)
{
    WarMap* map = context->map;
    assert(map);

    NOT_IMPLEMENTED();
}

void applySkipHumanCheat(WarContext* context, s32 argument)
{
    WarMap* map = context->map;
    assert(map);

    NOT_IMPLEMENTED();
}

void applySkipOrcCheat(WarContext* context, s32 argument)
{
    WarMap* map = context->map;
    assert(map);

    NOT_IMPLEMENTED();
}

void applySpeedCheat(WarContext* context, s32 argument)
{
    WarMap* map = context->map;
    assert(map);

    NOT_IMPLEMENTED();
}
