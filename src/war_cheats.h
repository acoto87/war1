#pragma once

#define CHEAT_TEXT_MAX_LENGTH 32

#define CHEAT_GOLD_INCREASE 10000
#define CHEAT_WOOD_INCREASE 5000

typedef void (*WarCheatFunc)(WarContext* context, const char* argument);

typedef enum
{
    WAR_CHEAT_NONE,

    // custom cheats
    WAR_CHEAT_MUSIC,        // Music #: Set volume music (enter 1-45 for #)
                            // Music {on|off}: Enable or disable music
    WAR_CHEAT_SOUND,        // Sound {on|off}: Enable or disable sounds
    WAR_CHEAT_MUSIC_VOL,    // Music volume #: Set volume of music (enter 0-100 for #)
    WAR_CHEAT_SOUND_VOL,    // Sound volume #: Set volume of sounds (enter 0-100 for #)

    // original cheats
    WAR_CHEAT_GOLD,         // Pot of Gold: Gives 10000 gold and 5000 lumber to the player
    WAR_CHEAT_SPELLS,       // Eye of newt: All spells are now be able to be casted
    WAR_CHEAT_UPGRADES,     // Iron forge: Research all upgrades
    WAR_CHEAT_END,          // Ides of March: Brings player to final campaign sequence
    WAR_CHEAT_ENABLE,       // Corwin of Amber: Enables cheats
    WAR_CHEAT_GOD_MODE,     // There can be only one: Your units stop taking damage and deal 255 Damage
    WAR_CHEAT_LOSS,         // Crushing defeat: Instant loss
    WAR_CHEAT_FOG,          // Sally Shears: Disables fog of war
    WAR_CHEAT_SKIP_HUMAN,   // Human #: Skip to human level (enter 1-12 for #)
    WAR_CHEAT_SKIP_ORC,     // Orc #: Skip to orc level (enter 1-12 for #)
    WAR_CHEAT_SPEED,        // Hurry up guys: Speeds up building/unit production,

    WAR_CHEAT_COUNT
} WarCheat;

typedef struct
{
    WarCheat cheat;
    char text[CHEAT_TEXT_MAX_LENGTH];
    bool argument;
    WarCheatFunc cheatFunc;
} WarCheatDescriptor;

void applyCheat(WarContext* context, const char* text);

void applyMusicCheat(WarContext* context, const char* argument);
void applySoundCheat(WarContext* context, const char* argument);
void applyMusicVolCheat(WarContext* context, const char* argument);
void applySoundVolCheat(WarContext* context, const char* argument);
void applyGoldCheat(WarContext* context, const char* argument);
void applySpellsCheat(WarContext* context, const char* argument);
void applyUpgradesCheat(WarContext* context, const char* argument);
void applyEndCheat(WarContext* context, const char* argument);
void applyEnableCheat(WarContext* context, const char* argument);
void applyGodModeCheat(WarContext* context, const char* argument);
void applyLossCheat(WarContext* context, const char* argument);
void applyFogOfWarCheat(WarContext* context, const char* argument);
void applySkipHumanCheat(WarContext* context, const char* argument);
void applySkipOrcCheat(WarContext* context, const char* argument);
void applySpeedCheat(WarContext* context, const char* argument);
