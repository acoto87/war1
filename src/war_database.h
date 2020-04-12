#pragma once

typedef enum
{
    DB_ENTRY_TYPE_UNKNOWN,
    DB_ENTRY_TYPE_IMAGE,
    DB_ENTRY_TYPE_PALETTE,
    DB_ENTRY_TYPE_XMID,
    DB_ENTRY_TYPE_CURSOR,
    DB_ENTRY_TYPE_UI,
    DB_ENTRY_TYPE_TEXT,
    DB_ENTRY_TYPE_LEVEL_INFO,
    DB_ENTRY_TYPE_LEVEL_VISUAL,
    DB_ENTRY_TYPE_LEVEL_PASSABLE,
    DB_ENTRY_TYPE_SPRITE,
    DB_ENTRY_TYPE_WAVE,
    DB_ENTRY_TYPE_VOC,
    DB_ENTRY_TYPE_TILESET,
    DB_ENTRY_TYPE_TILES
} DatabaseEntryType;

typedef struct
{
    s32 index;
    DatabaseEntryType type;
    char *name;
    u16 param1;
    u16 param2;
    u16 param3;
} DatabaseEntry;

DatabaseEntry assets[] =
{
    // Music
    {   0, DB_ENTRY_TYPE_XMID,              "00",                                           0, 0, 0 },
    {   1, DB_ENTRY_TYPE_XMID,              "01",                                           0, 0, 0 },
    {   2, DB_ENTRY_TYPE_XMID,              "02",                                           0, 0, 0 }, // demo
    {   3, DB_ENTRY_TYPE_XMID,              "03",                                           0, 0, 0 },
    {   4, DB_ENTRY_TYPE_XMID,              "04",                                           0, 0, 0 }, // demo
    {   5, DB_ENTRY_TYPE_XMID,              "05",                                           0, 0, 0 },
    {   6, DB_ENTRY_TYPE_XMID,              "06",                                           0, 0, 0 },
    {   7, DB_ENTRY_TYPE_XMID,              "07",                                           0, 0, 0 },
    {   8, DB_ENTRY_TYPE_XMID,              "08",                                           0, 0, 0 },
    {   9, DB_ENTRY_TYPE_XMID,              "09",                                           0, 0, 0 },
    {  10, DB_ENTRY_TYPE_XMID,              "10",                                           0, 0, 0 },
    {  11, DB_ENTRY_TYPE_XMID,              "11",                                           0, 0, 0 },
    {  12, DB_ENTRY_TYPE_XMID,              "12",                                           0, 0, 0 },
    {  13, DB_ENTRY_TYPE_XMID,              "13",                                           0, 0, 0 },
    {  14, DB_ENTRY_TYPE_XMID,              "14",                                           0, 0, 0 },
    {  15, DB_ENTRY_TYPE_XMID,              "15",                                           0, 0, 0 },
    {  16, DB_ENTRY_TYPE_XMID,              "16",                                           0, 0, 0 },
    {  17, DB_ENTRY_TYPE_XMID,              "17",                                           0, 0, 0 }, // demo
    {  18, DB_ENTRY_TYPE_XMID,              "18",                                           0, 0, 0 },
    {  19, DB_ENTRY_TYPE_XMID,              "19",                                           0, 0, 0 }, // demo
    {  20, DB_ENTRY_TYPE_XMID,              "20",                                           0, 0, 0 },
    {  21, DB_ENTRY_TYPE_XMID,              "21",                                           0, 0, 0 },
    {  22, DB_ENTRY_TYPE_XMID,              "22",                                           0, 0, 0 },
    {  23, DB_ENTRY_TYPE_XMID,              "23",                                           0, 0, 0 },
    {  24, DB_ENTRY_TYPE_XMID,              "24",                                           0, 0, 0 },
    {  25, DB_ENTRY_TYPE_XMID,              "25",                                           0, 0, 0 },
    {  26, DB_ENTRY_TYPE_XMID,              "26",                                           0, 0, 0 },
    {  27, DB_ENTRY_TYPE_XMID,              "27",                                           0, 0, 0 },
    {  28, DB_ENTRY_TYPE_XMID,              "28",                                           0, 0, 0 },
    {  29, DB_ENTRY_TYPE_XMID,              "29",                                           0, 0, 0 },
    {  30, DB_ENTRY_TYPE_XMID,              "30",                                           0, 0, 0 },
    {  31, DB_ENTRY_TYPE_XMID,              "31",                                           0, 0, 0 },
    {  32, DB_ENTRY_TYPE_XMID,              "32",                                           0, 0, 0 }, // demo
    {  33, DB_ENTRY_TYPE_XMID,              "33",                                           0, 0, 0 },
    {  34, DB_ENTRY_TYPE_XMID,              "34",                                           0, 0, 0 }, // demo
    {  35, DB_ENTRY_TYPE_XMID,              "35",                                           0, 0, 0 },
    {  36, DB_ENTRY_TYPE_XMID,              "36",                                           0, 0, 0 },
    {  37, DB_ENTRY_TYPE_XMID,              "37",                                           0, 0, 0 },
    {  38, DB_ENTRY_TYPE_XMID,              "38",                                           0, 0, 0 },
    {  39, DB_ENTRY_TYPE_XMID,              "39",                                           0, 0, 0 },
    {  40, DB_ENTRY_TYPE_XMID,              "40",                                           0, 0, 0 },
    {  41, DB_ENTRY_TYPE_XMID,              "41",                                           0, 0, 0 },
    {  42, DB_ENTRY_TYPE_XMID,              "42",                                           0, 0, 0 },
    {  43, DB_ENTRY_TYPE_XMID,              "43",                                           0, 0, 0 },
    {  44, DB_ENTRY_TYPE_XMID,              "44",                                           0, 0, 0 },

    // Levels info
    {  45, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  46, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  47, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  48, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  49, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 }, // demo
    {  50, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 }, // demo
    {  51, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 }, // demo
    {  52, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 }, // demo
    {  53, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  54, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  55, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 }, // demo
    {  56, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 }, // demo
    {  57, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  58, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  59, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  60, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  61, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  62, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  63, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 }, // demo
    {  64, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 }, // demo
    {  65, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  66, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  67, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  68, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  69, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 }, // demo
    {  70, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 }, // demo
    {  71, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  72, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  73, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  74, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  75, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  76, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  77, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  78, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  79, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 }, // demo
    {  80, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 }, // demo
    {  81, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 }, // demo
    {  82, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 }, // demo
    {  83, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  84, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  85, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  86, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  87, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  88, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  89, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 }, // demo
    {  90, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 }, // demo
    {  91, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  92, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  93, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 }, // demo
    {  94, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 }, // demo
    {  95, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  96, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  97, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    {  98, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    {  99, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    { 100, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    { 101, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    { 102, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    { 103, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    { 104, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    { 105, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    { 106, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    { 107, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    { 108, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    { 109, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    { 110, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    { 111, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    { 112, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    { 113, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    { 114, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },
    { 115, DB_ENTRY_TYPE_LEVEL_VISUAL,      "FileLevelVisual",                              0, 0, 0 },
    { 116, DB_ENTRY_TYPE_LEVEL_PASSABLE,    "FileLevelPassable",                            0, 0, 0 },

    // Levels
    { 117, DB_ENTRY_TYPE_LEVEL_INFO,        "humans_01",                                    0, 0, 0 }, // demo
    { 118, DB_ENTRY_TYPE_LEVEL_INFO,        "orcs_01",                                      1, 0, 0 }, // demo
    { 119, DB_ENTRY_TYPE_LEVEL_INFO,        "humans_02",                                    0, 0, 0 }, // demo
    { 120, DB_ENTRY_TYPE_LEVEL_INFO,        "orcs_02",                                      1, 0, 0 }, // demo
    { 121, DB_ENTRY_TYPE_LEVEL_INFO,        "humans_03",                                    1, 0, 0 }, // demo
    { 122, DB_ENTRY_TYPE_LEVEL_INFO,        "orcs_03",                                      0, 0, 0 }, // demo
    { 123, DB_ENTRY_TYPE_LEVEL_INFO,        "humans_04",                                    2, 0, 0 },
    { 124, DB_ENTRY_TYPE_LEVEL_INFO,        "orcs_04",                                      2, 0, 0 },
    { 125, DB_ENTRY_TYPE_LEVEL_INFO,        "humans_05",                                    0, 0, 0 },
    { 126, DB_ENTRY_TYPE_LEVEL_INFO,        "orcs_05",                                      1, 0, 0 },
    { 127, DB_ENTRY_TYPE_LEVEL_INFO,        "humans_06",                                    0, 0, 0 },
    { 128, DB_ENTRY_TYPE_LEVEL_INFO,        "orcs_06",                                      0, 0, 0 },
    { 129, DB_ENTRY_TYPE_LEVEL_INFO,        "humans_07",                                    0, 0, 0 },
    { 130, DB_ENTRY_TYPE_LEVEL_INFO,        "orcs_07",                                      1, 0, 0 },
    { 131, DB_ENTRY_TYPE_LEVEL_INFO,        "humans_08",                                    2, 0, 0 },
    { 132, DB_ENTRY_TYPE_LEVEL_INFO,        "orcs_08",                                      2, 0, 0 },
    { 133, DB_ENTRY_TYPE_LEVEL_INFO,        "humans_09",                                    1, 0, 0 },
    { 134, DB_ENTRY_TYPE_LEVEL_INFO,        "orcs_09",                                      1, 0, 0 },
    { 135, DB_ENTRY_TYPE_LEVEL_INFO,        "humans_10",                                    1, 0, 0 },
    { 136, DB_ENTRY_TYPE_LEVEL_INFO,        "orcs_10",                                      0, 0, 0 },
    { 137, DB_ENTRY_TYPE_LEVEL_INFO,        "humans_11",                                    1, 0, 0 },
    { 138, DB_ENTRY_TYPE_LEVEL_INFO,        "orcs_11",                                      0, 0, 0 },
    { 139, DB_ENTRY_TYPE_LEVEL_INFO,        "humans_12",                                    1, 0, 0 },
    { 140, DB_ENTRY_TYPE_LEVEL_INFO,        "orcs_12",                                      0, 0, 0 },

    // Custom
    { 141, DB_ENTRY_TYPE_UNKNOWN,           "battle_test_map_1",                            1, 0, 0 }, // test map, just several footmans and archers against grunts and spearmans, player control humans
    { 142, DB_ENTRY_TYPE_UNKNOWN,           "battle_test_map_2",                            1, 0, 0 }, // test map, several footmans and archers attacking an orc camp with few grunts and spearmans, player controls humans
    { 143, DB_ENTRY_TYPE_UNKNOWN,           "battle_test_map_3",                            1, 0, 0 }, // test map, several footmans and archers attacking an orc camp with few grunts and spearmans, player controls orcs
    { 144, DB_ENTRY_TYPE_UNKNOWN,           "battle_test_map_4",                            1, 0, 0 }, // test map, several grunts and spearmans attacking a human camp with few footmans and archers, player controls orcs
    { 147, DB_ENTRY_TYPE_LEVEL_INFO,        "forest_1",                                     0, 1, 0 },
    { 148, DB_ENTRY_TYPE_LEVEL_INFO,        "forest_2",                                     0, 1, 0 },
    { 149, DB_ENTRY_TYPE_LEVEL_INFO,        "forest_3",                                     0, 1, 0 },
    { 150, DB_ENTRY_TYPE_LEVEL_INFO,        "forest_4",                                     0, 1, 0 },
    { 151, DB_ENTRY_TYPE_LEVEL_INFO,        "forest_5",                                     0, 1, 0 },
    { 152, DB_ENTRY_TYPE_LEVEL_INFO,        "forest_6",                                     0, 1, 0 },
    { 153, DB_ENTRY_TYPE_LEVEL_INFO,        "forest_7",                                     0, 1, 0 },
    { 154, DB_ENTRY_TYPE_LEVEL_INFO,        "swamp_6",                                      1, 1, 0 },
    { 155, DB_ENTRY_TYPE_LEVEL_INFO,        "swamp_7",                                      1, 1, 0 },
    { 156, DB_ENTRY_TYPE_LEVEL_INFO,        "swamp_1",                                      1, 1, 0 },
    { 157, DB_ENTRY_TYPE_LEVEL_INFO,        "swamp_2",                                      1, 1, 0 },
    { 158, DB_ENTRY_TYPE_LEVEL_INFO,        "swamp_3",                                      1, 1, 0 },
    { 159, DB_ENTRY_TYPE_LEVEL_INFO,        "swamp_4",                                      1, 1, 0 },
    { 160, DB_ENTRY_TYPE_LEVEL_INFO,        "swamp_5",                                      1, 1, 0 },
    { 161, DB_ENTRY_TYPE_LEVEL_INFO,        "dungeon_1",                                    2, 1, 0 },
    { 162, DB_ENTRY_TYPE_LEVEL_INFO,        "dungeon_2",                                    2, 1, 0 },
    { 163, DB_ENTRY_TYPE_LEVEL_INFO,        "dungeon_3",                                    2, 1, 0 },
    { 164, DB_ENTRY_TYPE_LEVEL_INFO,        "dungeon_4",                                    2, 1, 0 },
    { 165, DB_ENTRY_TYPE_LEVEL_INFO,        "dungeon_5",                                    2, 1, 0 },
    { 166, DB_ENTRY_TYPE_LEVEL_INFO,        "dungeon_6",                                    2, 1, 0 },
    { 167, DB_ENTRY_TYPE_LEVEL_INFO,        "dungeon_7",                                    2, 1, 0 },

    { 168, DB_ENTRY_TYPE_LEVEL_INFO,        "forest_1_big_enemy",                           0, 1, 0 },
    { 169, DB_ENTRY_TYPE_LEVEL_INFO,        "forest_2_big_enemy",                           0, 1, 0 },
    { 170, DB_ENTRY_TYPE_LEVEL_INFO,        "forest_3_big_enemy",                           0, 1, 0 },
    { 171, DB_ENTRY_TYPE_LEVEL_INFO,        "forest_4_big_enemy",                           0, 1, 0 },
    { 172, DB_ENTRY_TYPE_LEVEL_INFO,        "forest_5_big_enemy",                           0, 1, 0 },
    { 173, DB_ENTRY_TYPE_LEVEL_INFO,        "forest_6_big_enemy",                           0, 1, 0 },
    { 174, DB_ENTRY_TYPE_LEVEL_INFO,        "forest_7_big_enemy",                           0, 1, 0 },
    { 175, DB_ENTRY_TYPE_LEVEL_INFO,        "swamp_6_big_enemy",                            1, 1, 0 },
    { 176, DB_ENTRY_TYPE_LEVEL_INFO,        "swamp_7_big_enemy",                            1, 1, 0 },
    { 177, DB_ENTRY_TYPE_LEVEL_INFO,        "swamp_1_big_enemy",                            1, 1, 0 },
    { 178, DB_ENTRY_TYPE_LEVEL_INFO,        "swamp_2_big_enemy",                            1, 1, 0 },
    { 179, DB_ENTRY_TYPE_LEVEL_INFO,        "swamp_3_big_enemy",                            1, 1, 0 },
    { 180, DB_ENTRY_TYPE_LEVEL_INFO,        "swamp_4_big_enemy",                            1, 1, 0 },
    { 181, DB_ENTRY_TYPE_LEVEL_INFO,        "swamp_5_big_enemy",                            1, 1, 0 },
    { 182, DB_ENTRY_TYPE_LEVEL_INFO,        "dungeon_1_big_enemy",                          2, 1, 0 },
    { 183, DB_ENTRY_TYPE_LEVEL_INFO,        "dungeon_2_big_enemy",                          2, 1, 0 },
    { 184, DB_ENTRY_TYPE_LEVEL_INFO,        "dungeon_3_big_enemy",                          2, 1, 0 },
    { 185, DB_ENTRY_TYPE_LEVEL_INFO,        "dungeon_4_big_enemy",                          2, 1, 0 },
    { 186, DB_ENTRY_TYPE_LEVEL_INFO,        "dungeon_5_big_enemy",                          2, 1, 0 },
    { 187, DB_ENTRY_TYPE_LEVEL_INFO,        "dungeon_6_big_enemy",                          2, 1, 0 },
    { 188, DB_ENTRY_TYPE_LEVEL_INFO,        "dungeon_7_big_enemy",                          2, 1, 0 },

    // Palettes
    { 191, DB_ENTRY_TYPE_PALETTE,           "forest",                                       0, 0, 0 }, // demo
    { 194, DB_ENTRY_TYPE_PALETTE,           "swamp",                                        0, 0, 0 }, // demo
    { 197, DB_ENTRY_TYPE_PALETTE,           "dungeon",                                      0, 0, 0 },
    { 217, DB_ENTRY_TYPE_PALETTE,           "background",                                   0, 0, 0 }, // demo
    { 255, DB_ENTRY_TYPE_PALETTE,           "humans",                                       0, 0, 0 }, // demo
    { 260, DB_ENTRY_TYPE_PALETTE,           "ui",                                           0, 0, 0 }, // demo
    { 262, DB_ENTRY_TYPE_PALETTE,           "pointer",                                      0, 0, 0 }, // demo
    { 413, DB_ENTRY_TYPE_PALETTE,           "summary",                                      0, 0, 0 }, // demo
    { 416, DB_ENTRY_TYPE_PALETTE,           "for 415",                                      0, 0, 0 }, // demo
    { 418, DB_ENTRY_TYPE_PALETTE,           "for 417",                                      0, 0, 0 }, // demo
    { 423, DB_ENTRY_TYPE_PALETTE,           "briefing",                                     0, 0, 0 }, // demo
    { 424, DB_ENTRY_TYPE_PALETTE,           "briefing",                                     0, 0, 0 }, // demo
    { 457, DB_ENTRY_TYPE_PALETTE,           "for 456",                                      0, 0, 0 },
    { 459, DB_ENTRY_TYPE_PALETTE,           "for 458",                                      0, 0, 0 },

    // Tiles
    { 190, DB_ENTRY_TYPE_TILES,             "forest",                                       191, 217, 0 }, // demo
    { 193, DB_ENTRY_TYPE_TILES,             "swamp",                                        194, 217, 0 }, // demo
    { 196, DB_ENTRY_TYPE_TILES,             "dungeon",                                      197, 217, 0 },

    // Tilesets
    { 189, DB_ENTRY_TYPE_TILESET,           "forest",                                       190, 0, 0 }, // demo
    { 192, DB_ENTRY_TYPE_TILESET,           "swamp",                                        193, 0, 0 }, // demo
    { 195, DB_ENTRY_TYPE_TILESET,           "dungeon",                                      196, 0, 0 },

    // Cursors
    { 263, DB_ENTRY_TYPE_CURSOR,            "arrow",                                        262, 0, 0 }, // demo
    { 264, DB_ENTRY_TYPE_CURSOR,            "invalid_command",                              262, 0, 0 }, // demo
    { 265, DB_ENTRY_TYPE_CURSOR,            "yellow_crosshair",                             262, 0, 0 }, // demo
    { 266, DB_ENTRY_TYPE_CURSOR,            "red_crosshair",                                262, 0, 0 }, // demo
    { 267, DB_ENTRY_TYPE_CURSOR,            "yellow_crosshair_2",                           262, 0, 0 }, // demo
    { 268, DB_ENTRY_TYPE_CURSOR,            "magnifying_glass",                             262, 0, 0 }, // demo
    { 269, DB_ENTRY_TYPE_CURSOR,            "small_green_crosshair",                        262, 0, 0 }, // demo
    { 270, DB_ENTRY_TYPE_CURSOR,            "watch",                                        262, 0, 0 }, // demo
    { 271, DB_ENTRY_TYPE_CURSOR,            "up_arrow",                                     262, 0, 0 }, // demo
    { 272, DB_ENTRY_TYPE_CURSOR,            "upper_right_arrow",                            262, 0, 0 }, // demo
    { 273, DB_ENTRY_TYPE_CURSOR,            "right_arrow",                                  262, 0, 0 }, // demo
    { 274, DB_ENTRY_TYPE_CURSOR,            "lower_right_arrow",                            262, 0, 0 }, // demo
    { 275, DB_ENTRY_TYPE_CURSOR,            "down_arrow",                                   262, 0, 0 }, // demo
    { 276, DB_ENTRY_TYPE_CURSOR,            "lower_left_arrow",                             262, 0, 0 }, // demo
    { 277, DB_ENTRY_TYPE_CURSOR,            "left_arrow",                                   262, 0, 0 }, // demo
    { 278, DB_ENTRY_TYPE_CURSOR,            "upper_left_arrow",                             262, 0, 0 }, // demo

    // Sprites
    { 279, DB_ENTRY_TYPE_SPRITE,            "footman_humans",                               191, 217, 0 }, // demo
    { 280, DB_ENTRY_TYPE_SPRITE,            "grunt_orcs",                                   191, 217, 0 }, // demo
    { 281, DB_ENTRY_TYPE_SPRITE,            "peasant_humans",                               191, 217, 0 }, // demo
    { 282, DB_ENTRY_TYPE_SPRITE,            "peon_orcs",                                    191, 217, 0 }, // demo
    { 283, DB_ENTRY_TYPE_SPRITE,            "catapult_humans",                              191, 217, 0 },
    { 284, DB_ENTRY_TYPE_SPRITE,            "catapult_orcs",                                191, 217, 0 },
    { 285, DB_ENTRY_TYPE_SPRITE,            "knight_humans",                                191, 217, 0 },
    { 286, DB_ENTRY_TYPE_SPRITE,            "raider_orcs",                                  191, 217, 0 },
    { 287, DB_ENTRY_TYPE_SPRITE,            "archer_humans",                                191, 217, 0 }, // demo
    { 288, DB_ENTRY_TYPE_SPRITE,            "axethrower_orcs",                              191, 217, 0 }, // demo
    { 289, DB_ENTRY_TYPE_SPRITE,            "wizard_humans",                                191, 217, 0 },
    { 290, DB_ENTRY_TYPE_SPRITE,            "wizard_orcs",                                  191, 217, 0 },
    { 291, DB_ENTRY_TYPE_SPRITE,            "priest_humans",                                191, 217, 0 }, // demo
    { 292, DB_ENTRY_TYPE_SPRITE,            "necrolyte_orcs",                               191, 217, 0 }, // demo
    { 293, DB_ENTRY_TYPE_SPRITE,            "medivh",                                       191, 217, 0 },
    { 294, DB_ENTRY_TYPE_SPRITE,            "lothar",                                       191, 217, 0 },
    { 295, DB_ENTRY_TYPE_SPRITE,            "wounded",                                      191, 217, 0 },
    { 296, DB_ENTRY_TYPE_SPRITE,            "garona",                                       191, 217, 0 },
    { 297, DB_ENTRY_TYPE_SPRITE,            "ogre",                                         191, 217, 0 },
    { 298, DB_ENTRY_TYPE_SPRITE,            "spider",                                       191, 217, 0 },
    { 299, DB_ENTRY_TYPE_SPRITE,            "slime",                                        191, 217, 0 },
    { 300, DB_ENTRY_TYPE_SPRITE,            "fire_elemental",                               191, 217, 0 },
    { 301, DB_ENTRY_TYPE_SPRITE,            "scorpion",                                     191, 217, 0 },
    { 302, DB_ENTRY_TYPE_SPRITE,            "brigand",                                      191, 217, 0 },
    { 303, DB_ENTRY_TYPE_SPRITE,            "the_dead",                                     191, 217, 0 }, // demo
    { 304, DB_ENTRY_TYPE_SPRITE,            "skeleton_02",                                  191, 217, 0 },
    { 305, DB_ENTRY_TYPE_SPRITE,            "demon",                                        191, 217, 0 },
    { 306, DB_ENTRY_TYPE_SPRITE,            "water_elemental",                              191, 217, 0 },
    { 307, DB_ENTRY_TYPE_SPRITE,            "farm_humans",                                  191, 217, 0 }, // demo
    { 308, DB_ENTRY_TYPE_SPRITE,            "farm_orcs",                                    191, 217, 0 }, // demo
    { 309, DB_ENTRY_TYPE_SPRITE,            "barracks_humans",                              191, 217, 0 }, // demo
    { 310, DB_ENTRY_TYPE_SPRITE,            "barracks_orcs",                                191, 217, 0 }, // demo
    { 311, DB_ENTRY_TYPE_SPRITE,            "church_humans",                                191, 217, 0 }, // demo
    { 312, DB_ENTRY_TYPE_SPRITE,            "temple_orcs",                                  191, 217, 0 }, // demo
    { 313, DB_ENTRY_TYPE_SPRITE,            "tower_humns",                                  191, 217, 0 },
    { 314, DB_ENTRY_TYPE_SPRITE,            "skull_orcs",                                   191, 217, 0 },
    { 315, DB_ENTRY_TYPE_SPRITE,            "town_hall_humans",                             191, 217, 0 }, // demo
    { 316, DB_ENTRY_TYPE_SPRITE,            "town_hall_orcs",                               191, 217, 0 }, // demo
    { 317, DB_ENTRY_TYPE_SPRITE,            "lumbermill_humans",                            191, 217, 0 }, // demo
    { 318, DB_ENTRY_TYPE_SPRITE,            "lumbermill_orcs",                              191, 217, 0 }, // demo
    { 319, DB_ENTRY_TYPE_SPRITE,            "stable_humans",                                191, 217, 0 },
    { 320, DB_ENTRY_TYPE_SPRITE,            "stable_orcs",                                  191, 217, 0 },
    { 321, DB_ENTRY_TYPE_SPRITE,            "blacksmith_humans",                            191, 217, 0 },
    { 322, DB_ENTRY_TYPE_SPRITE,            "blacksmith_orcs",                              191, 217, 0 },
    { 323, DB_ENTRY_TYPE_SPRITE,            "stormwind_humans",                             191, 217, 0 },
    { 324, DB_ENTRY_TYPE_SPRITE,            "black_spire_orcs",                             191, 217, 0 },
    { 325, DB_ENTRY_TYPE_SPRITE,            "goldmine",                                     191, 217, 0 }, // demo
    { 326, DB_ENTRY_TYPE_SPRITE,            "corpse",                                       191, 217, 0 }, // demo
    { 327, DB_ENTRY_TYPE_SPRITE,            "peasant_with_lumber_humans",                   191, 217, 0 }, // demo
    { 328, DB_ENTRY_TYPE_SPRITE,            "peon_with_lumber_orcs",                        191, 217, 0 }, // demo
    { 329, DB_ENTRY_TYPE_SPRITE,            "peasant_with_gold_humans",                     191, 217, 0 }, // demo
    { 330, DB_ENTRY_TYPE_SPRITE,            "peon_with_gold_orcs",                          191, 217, 0 }, // demo
    { 331, DB_ENTRY_TYPE_SPRITE,            "farm_construction_humans",                     191, 217, 0 }, // demo
    { 332, DB_ENTRY_TYPE_SPRITE,            "farm_construction_orcs",                       191, 217, 0 }, // demo
    { 333, DB_ENTRY_TYPE_SPRITE,            "barracks_construction_humans",                 191, 217, 0 }, // demo
    { 334, DB_ENTRY_TYPE_SPRITE,            "barracks_construction_orcs",                   191, 217, 0 }, // demo
    { 335, DB_ENTRY_TYPE_SPRITE,            "church_construction_humans",                   191, 217, 0 }, // demo
    { 336, DB_ENTRY_TYPE_SPRITE,            "temple_construction_orcs",                     191, 217, 0 }, // demo
    { 337, DB_ENTRY_TYPE_SPRITE,            "tower_construction_humans",                    191, 217, 0 },
    { 338, DB_ENTRY_TYPE_SPRITE,            "tower_construction_orcs",                      191, 217, 0 },
    { 339, DB_ENTRY_TYPE_SPRITE,            "town_hall_construction_humans",                191, 217, 0 }, // demo
    { 340, DB_ENTRY_TYPE_SPRITE,            "town_hall_construction_orcs",                  191, 217, 0 }, // demo
    { 341, DB_ENTRY_TYPE_SPRITE,            "lumber_mill_construction_humans",              191, 217, 0 }, // demo
    { 342, DB_ENTRY_TYPE_SPRITE,            "lumber_mill_construction_orcs",                191, 217, 0 }, // demo
    { 343, DB_ENTRY_TYPE_SPRITE,            "stable_construction_humans",                   191, 217, 0 },
    { 344, DB_ENTRY_TYPE_SPRITE,            "stable_construction_orcs",                     191, 217, 0 },
    { 345, DB_ENTRY_TYPE_SPRITE,            "blacksmith_construction_humans",               191, 217, 0 },
    { 346, DB_ENTRY_TYPE_SPRITE,            "blacksmith_construction_orcs",                 191, 217, 0 },
    { 350, DB_ENTRY_TYPE_SPRITE,            "poison_cloud",                                 191, 217, 0 },
    { 352, DB_ENTRY_TYPE_SPRITE,            "small_fire",                                   191, 217, 0 }, // demo
    { 353, DB_ENTRY_TYPE_SPRITE,            "large_fire",                                   191, 217, 0 }, // demo
    { 354, DB_ENTRY_TYPE_SPRITE,            "explosion",                                    191, 217, 0 },
    { 355, DB_ENTRY_TYPE_SPRITE,            "healing",                                      191, 217, 0 }, // demo
    { 356, DB_ENTRY_TYPE_SPRITE,            "building_collapse",                            191, 217, 0 }, // demo
    { 359, DB_ENTRY_TYPE_SPRITE,            "ui_pictures_orcs",                             191, 217, 0 }, // demo
    { 360, DB_ENTRY_TYPE_SPRITE,            "ui_pictures_humans",                           255,   0, 0 }, // demo
    { 361, DB_ENTRY_TYPE_SPRITE,            "ui_pictures_icons",                            191, 217, 0 }, // demo
    { 425, DB_ENTRY_TYPE_SPRITE,            "burning_fire",                                 424,   0, 0 },
    { 426, DB_ENTRY_TYPE_SPRITE,            "talking_orc_01",                               424,   0, 0 }, // demo
    { 427, DB_ENTRY_TYPE_SPRITE,            "talking_orc_02",                               424,   0, 0 }, // demo
    { 428, DB_ENTRY_TYPE_SPRITE,            "eating_human",                                 423,   0, 0 }, // demo
    { 429, DB_ENTRY_TYPE_SPRITE,            "human_wizard",                                 423,   0, 0 }, // demo
    { 430, DB_ENTRY_TYPE_SPRITE,            "anim_01",                                      423,   0, 0 }, // demo
    { 431, DB_ENTRY_TYPE_SPRITE,            "anim_02",                                      423,   0, 0 }, // demo
    { 460, DB_ENTRY_TYPE_SPRITE,            "anim_03",                                      459,   0, 0 },

    // Projectiles
    { 347, DB_ENTRY_TYPE_SPRITE,            "missiles_fireball",                            191, 217, 0 }, // demo
    { 348, DB_ENTRY_TYPE_SPRITE,            "missiles_catapult_projectile",                 191, 217, 0 },
    { 349, DB_ENTRY_TYPE_SPRITE,            "missiles_arrow",                               191, 217, 0 }, // demo
    { 351, DB_ENTRY_TYPE_SPRITE,            "missiles_rain_of_fire",                        191, 217, 0 },
    { 357, DB_ENTRY_TYPE_SPRITE,            "missiles_water_elemental_projectile",          191, 217, 0 },
    { 358, DB_ENTRY_TYPE_SPRITE,            "missiles_fireball_2",                          191, 217, 0 },

    // Images
    { 216, DB_ENTRY_TYPE_IMAGE,             "background 'Blizzard'",                        217,   0, 0 }, // demo
    { 218, DB_ENTRY_TYPE_IMAGE,             "topbar_humans",                                255,   0, 0 }, // demo
    { 219, DB_ENTRY_TYPE_IMAGE,             "topbar_orcs",                                  191, 217, 0 }, // demo
    { 220, DB_ENTRY_TYPE_IMAGE,             "sidebar_right_humans",                         255,   0, 0 }, // demo
    { 221, DB_ENTRY_TYPE_IMAGE,             "sidebar_right_orcs",                           217,   0, 0 }, // demo
    { 222, DB_ENTRY_TYPE_IMAGE,             "lower_bar_humans",                             255,   0, 0 }, // demo
    { 223, DB_ENTRY_TYPE_IMAGE,             "lower_bar_orcs",                               217,   0, 0 }, // demo
    { 224, DB_ENTRY_TYPE_IMAGE,             "sidebar_left_minimap_empty_humans",            255,   0, 0 }, // demo
    { 225, DB_ENTRY_TYPE_IMAGE,             "sidebar_left_minimap_empty_orcs",              217,   0, 0 }, // demo
    { 226, DB_ENTRY_TYPE_IMAGE,             "sidebar_left_humans",                          255,   0, 0 }, // demo
    { 227, DB_ENTRY_TYPE_IMAGE,             "sidebar_left_orcs",                            217,   0, 0 }, // demo
    { 228, DB_ENTRY_TYPE_IMAGE,             "sidebar_left_minimap_black_humans",            255,   0, 0 }, // demo
    { 229, DB_ENTRY_TYPE_IMAGE,             "sidebar_left_minimap_black_orcs",              217,   0, 0 }, // demo
    { 233, DB_ENTRY_TYPE_IMAGE,             "large_box_humans",                             255,   0, 0 }, // demo
    { 234, DB_ENTRY_TYPE_IMAGE,             "large_box_orcs",                               217,   0, 0 }, // demo
    { 235, DB_ENTRY_TYPE_IMAGE,             "small_box_humans",                             255,   0, 0 }, // demo
    { 236, DB_ENTRY_TYPE_IMAGE,             "smal_box_orcs",                                217,   0, 0 }, // demo
    { 237, DB_ENTRY_TYPE_IMAGE,             "large_button",                                 260,   0, 0 }, // demo
    { 238, DB_ENTRY_TYPE_IMAGE,             "large_button_pressed",                         260,   0, 0 }, // demo
    { 239, DB_ENTRY_TYPE_IMAGE,             "medium_button",                                260,   0, 0 }, // demo
    { 240, DB_ENTRY_TYPE_IMAGE,             "medium_button_pressed",                        260,   0, 0 }, // demo
    { 241, DB_ENTRY_TYPE_IMAGE,             "small_button",                                 260,   0, 0 }, // demo
    { 242, DB_ENTRY_TYPE_IMAGE,             "small_button_pressed",                         260,   0, 0 }, // demo
    { 243, DB_ENTRY_TYPE_IMAGE,             "menu_background_lower",                        260,   0, 0 }, // demo
    { 244, DB_ENTRY_TYPE_IMAGE,             "arrow_left_button",                            255,   0, 0 }, // demo
    { 245, DB_ENTRY_TYPE_IMAGE,             "arrow_left_button_grey",                       255,   0, 0 }, // demo
    { 246, DB_ENTRY_TYPE_IMAGE,             "arrow_right_button",                           255,   0, 0 }, // demo
    { 247, DB_ENTRY_TYPE_IMAGE,             "arrow_right_button_grey",                      255,   0, 0 }, // demo
    { 248, DB_ENTRY_TYPE_IMAGE,             "window_border",                                255,   0, 0 }, // demo
    { 249, DB_ENTRY_TYPE_IMAGE,             "saving_loading_humans",                        255,   0, 0 }, // demo
    { 250, DB_ENTRY_TYPE_IMAGE,             "saving_loading_orcs",                          217,   0, 0 }, // demo
    { 254, DB_ENTRY_TYPE_IMAGE,             "hot_keys",                                     255,   0, 0 }, // demo
    { 256, DB_ENTRY_TYPE_IMAGE,             "ok1_button",                                   255,   0, 0 }, // demo
    { 257, DB_ENTRY_TYPE_IMAGE,             "ok2_button",                                   255,   0, 0 }, // demo
    { 258, DB_ENTRY_TYPE_IMAGE,             "warcraft_text",                                260,   0, 0 }, // demo
    { 261, DB_ENTRY_TYPE_IMAGE,             "menu_background",                              260,   0, 0 }, // demo
    { 362, DB_ENTRY_TYPE_IMAGE,             "menu_button",                                  217,   0, 0 }, // demo
    { 363, DB_ENTRY_TYPE_IMAGE,             "menu_button_pressed",                          217,   0, 0 }, // demo
    { 364, DB_ENTRY_TYPE_IMAGE,             "empty_button",                                 255,   0, 0 }, // demo
    { 365, DB_ENTRY_TYPE_IMAGE,             "empty_button_pressed",                         217,   0, 0 }, // demo
    { 406, DB_ENTRY_TYPE_IMAGE,             "gold_1",                                       191, 217, 0 }, // demo
    { 407, DB_ENTRY_TYPE_IMAGE,             "lumber_1",                                     217,   0, 0 }, // demo
    { 408, DB_ENTRY_TYPE_IMAGE,             "gold_2",                                       191, 217, 0 }, // demo
    { 409, DB_ENTRY_TYPE_IMAGE,             "lumber_2",                                     217,   0, 0 }, // demo
    { 410, DB_ENTRY_TYPE_IMAGE,             "percent_complete",                             191, 217, 0 }, // demo
    { 411, DB_ENTRY_TYPE_IMAGE,             "points_summary_humans",                        413,   0, 0 }, // demo
    { 412, DB_ENTRY_TYPE_IMAGE,             "points_summary_orcs",                          413,   0, 0 }, // demo
    { 415, DB_ENTRY_TYPE_IMAGE,             "victory_background",                           416,   0, 0 }, // demo
    { 417, DB_ENTRY_TYPE_IMAGE,             "defeat_background",                            418,   0, 0 }, // demo
    { 419, DB_ENTRY_TYPE_IMAGE,             "victory_text",                                 423,   0, 0 }, // demo
    { 420, DB_ENTRY_TYPE_IMAGE,             "defeat_text",                                  423,   0, 0 }, // demo
    { 421, DB_ENTRY_TYPE_IMAGE,             "briefing_humans",                              423,   0, 0 }, // demo
    { 422, DB_ENTRY_TYPE_IMAGE,             "briefing_orcs",                                424,   0, 0 }, // demo
    { 456, DB_ENTRY_TYPE_IMAGE,             "win_humans",                                   457,   0, 0 },
    { 458, DB_ENTRY_TYPE_IMAGE,             "win_orcs",                                     459,   0, 0 },
    { 470, DB_ENTRY_TYPE_IMAGE,             "win_anim_humans",                              457,   0, 0 },
    { 471, DB_ENTRY_TYPE_IMAGE,             "win_anim_orcs",                                260,   0, 0 },

    // Text
    { 432, DB_ENTRY_TYPE_TEXT,              "01_intro_orcs",                                0, 0, 0 }, // demo
    { 433, DB_ENTRY_TYPE_TEXT,              "02_intro_orcs",                                0, 0, 0 }, // demo
    { 434, DB_ENTRY_TYPE_TEXT,              "03_intro_orcs",                                0, 0, 0 }, // demo
    { 435, DB_ENTRY_TYPE_TEXT,              "04_intro_orcs",                                0, 0, 0 }, // demo
    { 436, DB_ENTRY_TYPE_TEXT,              "05_intro_orcs",                                0, 0, 0 },
    { 437, DB_ENTRY_TYPE_TEXT,              "06_intro_orcs",                                0, 0, 0 },
    { 438, DB_ENTRY_TYPE_TEXT,              "07_intro_orcs",                                0, 0, 0 },
    { 439, DB_ENTRY_TYPE_TEXT,              "08_intro_orcs",                                0, 0, 0 },
    { 440, DB_ENTRY_TYPE_TEXT,              "09_intro_orcs",                                0, 0, 0 },
    { 441, DB_ENTRY_TYPE_TEXT,              "10_intro_orcs",                                0, 0, 0 },
    { 442, DB_ENTRY_TYPE_TEXT,              "11_intro_orcs",                                0, 0, 0 },
    { 443, DB_ENTRY_TYPE_TEXT,              "12_intro_orcs",                                0, 0, 0 },
    { 444, DB_ENTRY_TYPE_TEXT,              "01_intro_humans",                              0, 0, 0 }, // demo
    { 445, DB_ENTRY_TYPE_TEXT,              "02_intro_humans",                              0, 0, 0 }, // demo
    { 446, DB_ENTRY_TYPE_TEXT,              "03_intro_humans",                              0, 0, 0 }, // demo
    { 447, DB_ENTRY_TYPE_TEXT,              "04_intro_humans",                              0, 0, 0 }, // demo
    { 448, DB_ENTRY_TYPE_TEXT,              "05_intro_humans",                              0, 0, 0 },
    { 449, DB_ENTRY_TYPE_TEXT,              "06_intro_humans",                              0, 0, 0 },
    { 450, DB_ENTRY_TYPE_TEXT,              "07_intro_humans",                              0, 0, 0 },
    { 451, DB_ENTRY_TYPE_TEXT,              "08_intro_humans",                              0, 0, 0 },
    { 452, DB_ENTRY_TYPE_TEXT,              "09_intro_humans",                              0, 0, 0 },
    { 453, DB_ENTRY_TYPE_TEXT,              "10_intro_humans",                              0, 0, 0 },
    { 454, DB_ENTRY_TYPE_TEXT,              "11_intro_humans",                              0, 0, 0 },
    { 455, DB_ENTRY_TYPE_TEXT,              "12_intro_humans",                              0, 0, 0 },
    { 461, DB_ENTRY_TYPE_TEXT,              "ending_1_humans",                              0, 0, 0 },
    { 462, DB_ENTRY_TYPE_TEXT,              "ending_1_orcs",                                0, 0, 0 },
    { 463, DB_ENTRY_TYPE_TEXT,              "ending_2_humans",                              0, 0, 0 },
    { 464, DB_ENTRY_TYPE_TEXT,              "ending_2_orcs",                                0, 0, 0 },
    { 465, DB_ENTRY_TYPE_TEXT,              "credits",                                      0, 0, 0 },
    { 466, DB_ENTRY_TYPE_TEXT,              "victory_dialog_1",                             0, 0, 0 }, // demo
    { 467, DB_ENTRY_TYPE_TEXT,              "victory_dialog_2",                             0, 0, 0 }, // demo
    { 468, DB_ENTRY_TYPE_TEXT,              "defeat_dialog_1",                              0, 0, 0 }, // demo
    { 469, DB_ENTRY_TYPE_TEXT,              "defeat_dialog_2",                              0, 0, 0 }, // demo

    // Sounds
    { 472, DB_ENTRY_TYPE_WAVE,              "logo",                                         0, 0, 0 }, // demo
    { 473, DB_ENTRY_TYPE_WAVE,              "intro_door",                                   0, 0, 0 },
    { 474, DB_ENTRY_TYPE_VOC,               "misc_building",                                0, 0, 0 }, // demo
    { 475, DB_ENTRY_TYPE_VOC,               "misc_explosion",                               0, 0, 0 }, // demo
    { 476, DB_ENTRY_TYPE_VOC,               "missiles_catapult_rock_fired",                 0, 0, 0 },
    { 477, DB_ENTRY_TYPE_VOC,               "misc_tree_chopping_1",                         0, 0, 0 }, // demo
    { 478, DB_ENTRY_TYPE_VOC,               "misc_tree_chopping_2",                         0, 0, 0 }, // demo
    { 479, DB_ENTRY_TYPE_VOC,               "misc_tree_chopping_3",                         0, 0, 0 }, // demo
    { 480, DB_ENTRY_TYPE_VOC,               "misc_tree_chopping_4",                         0, 0, 0 }, // demo
    { 481, DB_ENTRY_TYPE_VOC,               "misc_building_collapse_1",                     0, 0, 0 },
    { 482, DB_ENTRY_TYPE_VOC,               "misc_building_collapse_2",                     0, 0, 0 },
    { 483, DB_ENTRY_TYPE_VOC,               "misc_building_collapse_3",                     0, 0, 0 },
    { 484, DB_ENTRY_TYPE_VOC,               "ui_chime",                                     0, 0, 0 }, // demo
    { 485, DB_ENTRY_TYPE_WAVE,              "ui_click",                                     0, 0, 0 }, // demo
    { 486, DB_ENTRY_TYPE_VOC,               "ui_cancel",                                    0, 0, 0 }, // demo
    { 487, DB_ENTRY_TYPE_VOC,               "missiles_sword_attack_1",                      0, 0, 0 }, // demo
    { 488, DB_ENTRY_TYPE_VOC,               "missiles_sword_attack_2",                      0, 0, 0 }, // demo
    { 489, DB_ENTRY_TYPE_VOC,               "missiles_sword_attack_3",                      0, 0, 0 }, // demo
    { 490, DB_ENTRY_TYPE_VOC,               "missiles_fist_attack",                         0, 0, 0 },
    { 491, DB_ENTRY_TYPE_VOC,               "missiles_catapult_fire_explosion",             0, 0, 0 }, // demo
    { 492, DB_ENTRY_TYPE_VOC,               "missiles_fireball",                            0, 0, 0 }, // demo
    { 493, DB_ENTRY_TYPE_VOC,               "missiles_arrow_spear",                         0, 0, 0 }, // demo
    { 494, DB_ENTRY_TYPE_VOC,               "missiles_arrow_spear_hit",                     0, 0, 0 }, // demo
    { 495, DB_ENTRY_TYPE_VOC,               "orc_help_1",                                   0, 0, 0 }, // demo
    { 496, DB_ENTRY_TYPE_VOC,               "orc_help_2",                                   0, 0, 0 },
    { 497, DB_ENTRY_TYPE_WAVE,              "human_help_2",                                 0, 0, 0 }, // demo
    { 498, DB_ENTRY_TYPE_WAVE,              "human_help_1",                                 0, 0, 0 },
    { 499, DB_ENTRY_TYPE_VOC,               "orc_dead",                                     0, 0, 0 }, // demo
    { 500, DB_ENTRY_TYPE_VOC,               "human_dead",                                   0, 0, 0 }, // demo
    { 501, DB_ENTRY_TYPE_VOC,               "orc_work_complete",                            0, 0, 0 }, // demo
    { 502, DB_ENTRY_TYPE_WAVE,              "human_work_complete",                          0, 0, 0 }, // demo
    { 503, DB_ENTRY_TYPE_VOC,               "orc_help_3",                                   0, 0, 0 }, // demo
    { 504, DB_ENTRY_TYPE_WAVE,              "orc_help_4",                                   0, 0, 0 },
    { 505, DB_ENTRY_TYPE_WAVE,              "human_help_3",                                 0, 0, 0 }, // demo
    { 506, DB_ENTRY_TYPE_WAVE,              "human_help_4",                                 0, 0, 0 }, // demo
    { 507, DB_ENTRY_TYPE_VOC,               "orc_ready",                                    0, 0, 0 }, // demo
    { 508, DB_ENTRY_TYPE_WAVE,              "human_ready",                                  0, 0, 0 }, // demo
    { 509, DB_ENTRY_TYPE_VOC,               "orc_acknowledgement_1",                        0, 0, 0 }, // demo
    { 510, DB_ENTRY_TYPE_VOC,               "orc_acknowledgement_2",                        0, 0, 0 }, // demo
    { 511, DB_ENTRY_TYPE_VOC,               "orc_acknowledgement_3",                        0, 0, 0 }, // demo
    { 512, DB_ENTRY_TYPE_VOC,               "orc_acknowledgement_4",                        0, 0, 0 }, // demo
    { 513, DB_ENTRY_TYPE_WAVE,              "human_acknowledgement_1",                      0, 0, 0 }, // demo
    { 514, DB_ENTRY_TYPE_WAVE,              "human_acknowledgement_2",                      0, 0, 0 }, // demo
    { 515, DB_ENTRY_TYPE_VOC,               "orc_selected_1",                               0, 0, 0 }, // demo
    { 516, DB_ENTRY_TYPE_VOC,               "orc_selected_2",                               0, 0, 0 }, // demo
    { 517, DB_ENTRY_TYPE_VOC,               "orc_selected_3",                               0, 0, 0 }, // demo
    { 518, DB_ENTRY_TYPE_VOC,               "orc_selected_4",                               0, 0, 0 }, // demo
    { 519, DB_ENTRY_TYPE_VOC,               "orc_selected_5",                               0, 0, 0 }, // demo
    { 520, DB_ENTRY_TYPE_WAVE,              "human_selected_1",                             0, 0, 0 }, // demo
    { 521, DB_ENTRY_TYPE_WAVE,              "human_selected_2",                             0, 0, 0 }, // demo
    { 522, DB_ENTRY_TYPE_WAVE,              "human_selected_3",                             0, 0, 0 }, // demo
    { 523, DB_ENTRY_TYPE_WAVE,              "human_selected_4",                             0, 0, 0 }, // demo
    { 524, DB_ENTRY_TYPE_WAVE,              "human_selected_5",                             0, 0, 0 }, // demo
    { 525, DB_ENTRY_TYPE_VOC,               "orc_annoyed_1",                                0, 0, 0 }, // demo
    { 526, DB_ENTRY_TYPE_VOC,               "orc_annoyed_2",                                0, 0, 0 }, // demo
    { 527, DB_ENTRY_TYPE_WAVE,              "orc_annoyed_3",                                0, 0, 0 },
    { 528, DB_ENTRY_TYPE_WAVE,              "human_annoyed_1",                              0, 0, 0 }, // demo
    { 529, DB_ENTRY_TYPE_WAVE,              "human_annoyed_2",                              0, 0, 0 }, // demo
    { 530, DB_ENTRY_TYPE_WAVE,              "human_annoyed_3",                              0, 0, 0 },
    { 531, DB_ENTRY_TYPE_WAVE,              "dead_spider_scorpion",                         0, 0, 0 },
    { 532, DB_ENTRY_TYPE_WAVE,              "normal_spell",                                 0, 0, 0 }, // demo
    { 533, DB_ENTRY_TYPE_WAVE,              "misc_build_road",                              0, 0, 0 }, // demo
    { 534, DB_ENTRY_TYPE_WAVE,              "orc_temple",                                   0, 0, 0 },
    { 535, DB_ENTRY_TYPE_WAVE,              "human_church",                                 0, 0, 0 },
    { 536, DB_ENTRY_TYPE_WAVE,              "orc_kennel",                                   0, 0, 0 },
    { 537, DB_ENTRY_TYPE_WAVE,              "human_stable",                                 0, 0, 0 },
    { 538, DB_ENTRY_TYPE_WAVE,              "blacksmith",                                   0, 0, 0 },
    { 539, DB_ENTRY_TYPE_WAVE,              "misc_fire_crackling",                          0, 0, 0 },
    { 540, DB_ENTRY_TYPE_WAVE,              "cannon",                                       0, 0, 0 },
    { 541, DB_ENTRY_TYPE_WAVE,              "cannon2",                                      0, 0, 0 },
    { 542, DB_ENTRY_TYPE_WAVE,              "campaigns_human_ending_1",                     0, 0, 0 },
    { 543, DB_ENTRY_TYPE_WAVE,              "campaigns_human_ending_2",                     0, 0, 0 },
    { 544, DB_ENTRY_TYPE_WAVE,              "campaigns_orc_ending_1",                       0, 0, 0 },
    { 545, DB_ENTRY_TYPE_WAVE,              "campaigns_orc_ending_2",                       0, 0, 0 },
    { 546, DB_ENTRY_TYPE_WAVE,              "intro_1",                                      0, 0, 0 },
    { 547, DB_ENTRY_TYPE_WAVE,              "intro_2",                                      0, 0, 0 },
    { 548, DB_ENTRY_TYPE_WAVE,              "intro_3",                                      0, 0, 0 },
    { 549, DB_ENTRY_TYPE_WAVE,              "intro_4",                                      0, 0, 0 },
    { 550, DB_ENTRY_TYPE_WAVE,              "intro_5",                                      0, 0, 0 },
    { 551, DB_ENTRY_TYPE_WAVE,              "campaigns_human_01_intro",                     0, 0, 0 },
    { 552, DB_ENTRY_TYPE_WAVE,              "campaigns_human_02_intro",                     0, 0, 0 },
    { 553, DB_ENTRY_TYPE_WAVE,              "campaigns_human_03_intro",                     0, 0, 0 },
    { 554, DB_ENTRY_TYPE_WAVE,              "campaigns_human_04_intro",                     0, 0, 0 },
    { 555, DB_ENTRY_TYPE_WAVE,              "campaigns_human_05_intro",                     0, 0, 0 },
    { 556, DB_ENTRY_TYPE_WAVE,              "campaigns_human_06_intro",                     0, 0, 0 },
    { 557, DB_ENTRY_TYPE_WAVE,              "campaigns_human_07_intro",                     0, 0, 0 },
    { 558, DB_ENTRY_TYPE_WAVE,              "campaigns_human_08_intro",                     0, 0, 0 },
    { 559, DB_ENTRY_TYPE_WAVE,              "campaigns_human_09_intro",                     0, 0, 0 },
    { 560, DB_ENTRY_TYPE_WAVE,              "campaigns_human_10_intro",                     0, 0, 0 },
    { 561, DB_ENTRY_TYPE_WAVE,              "campaigns_human_11_intro",                     0, 0, 0 },
    { 562, DB_ENTRY_TYPE_WAVE,              "campaigns_human_12_intro",                     0, 0, 0 },
    { 563, DB_ENTRY_TYPE_WAVE,              "campaigns_orc_01_intro",                       0, 0, 0 },
    { 564, DB_ENTRY_TYPE_WAVE,              "campaigns_orc_02_intro",                       0, 0, 0 },
    { 565, DB_ENTRY_TYPE_WAVE,              "campaigns_orc_03_intro",                       0, 0, 0 },
    { 566, DB_ENTRY_TYPE_WAVE,              "campaigns_orc_04_intro",                       0, 0, 0 },
    { 567, DB_ENTRY_TYPE_WAVE,              "campaigns_orc_05_intro",                       0, 0, 0 },
    { 568, DB_ENTRY_TYPE_WAVE,              "campaigns_orc_06_intro",                       0, 0, 0 },
    { 569, DB_ENTRY_TYPE_WAVE,              "campaigns_orc_07_intro",                       0, 0, 0 },
    { 570, DB_ENTRY_TYPE_WAVE,              "campaigns_orc_08_intro",                       0, 0, 0 },
    { 571, DB_ENTRY_TYPE_WAVE,              "campaigns_orc_09_intro",                       0, 0, 0 },
    { 572, DB_ENTRY_TYPE_WAVE,              "campaigns_orc_10_intro",                       0, 0, 0 },
    { 573, DB_ENTRY_TYPE_WAVE,              "campaigns_orc_11_intro",                       0, 0, 0 },
    { 574, DB_ENTRY_TYPE_WAVE,              "campaigns_orc_12_intro",                       0, 0, 0 },
    { 575, DB_ENTRY_TYPE_WAVE,              "human_defeat",                                 0, 0, 0 },
    { 576, DB_ENTRY_TYPE_WAVE,              "orc_defeat",                                   0, 0, 0 },
    { 577, DB_ENTRY_TYPE_WAVE,              "orc_victory_1",                                0, 0, 0 },
    { 578, DB_ENTRY_TYPE_WAVE,              "orc_victory_2",                                0, 0, 0 },
    { 579, DB_ENTRY_TYPE_WAVE,              "orc_victory_3",                                0, 0, 0 },
    { 580, DB_ENTRY_TYPE_WAVE,              "human_victory_1",                              0, 0, 0 },
    { 581, DB_ENTRY_TYPE_WAVE,              "human_victory_2",                              0, 0, 0 },
    { 582, DB_ENTRY_TYPE_WAVE,              "human_victory_3",                              0, 0, 0 },

    // NOTE: these are resources included in the demo version
    // not sure what are they, a little investigation is required
    //

    { 198, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 199, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 200, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 201, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 202, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 203, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 204, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 205, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 206, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 207, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 208, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 209, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 210, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 211, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 212, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 213, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 214, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 215, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 230, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 251, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 252, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 253, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 259, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 366, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 367, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 368, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 369, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 370, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 371, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 372, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 373, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 374, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 375, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 376, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 377, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 378, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 379, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 380, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 381, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 382, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 383, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 384, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 385, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 386, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 387, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 388, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 389, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 390, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 391, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 392, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 393, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 394, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 395, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 396, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 397, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 398, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 399, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 400, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 401, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 402, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 403, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 404, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 405, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
    { 414, DB_ENTRY_TYPE_UNKNOWN,           "",                                             0, 0, 0 },
};
