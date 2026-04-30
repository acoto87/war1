#pragma once

#include <stddef.h>

#include "war_math.h"
#include "war_actions.h"

typedef struct tsf tsf;
typedef struct tml_message tml_message;

enum _WarAudioId
{
    WAR_MUSIC_00 = 0,
    WAR_MUSIC_01 = 1,
    WAR_MUSIC_02 = 2,
    WAR_MUSIC_03 = 3,
    WAR_MUSIC_04 = 4,
    WAR_MUSIC_05 = 5,
    WAR_MUSIC_06 = 6,
    WAR_MUSIC_07 = 7,
    WAR_MUSIC_08 = 8,
    WAR_MUSIC_09 = 9,
    WAR_MUSIC_10 = 10,
    WAR_MUSIC_11 = 11,
    WAR_MUSIC_12 = 12,
    WAR_MUSIC_13 = 13,
    WAR_MUSIC_14 = 14,
    WAR_MUSIC_15 = 15,
    WAR_MUSIC_16 = 16,
    WAR_MUSIC_17 = 17,
    WAR_MUSIC_18 = 18,
    WAR_MUSIC_19 = 19,
    WAR_MUSIC_20 = 20,
    WAR_MUSIC_21 = 21,
    WAR_MUSIC_22 = 22,
    WAR_MUSIC_23 = 23,
    WAR_MUSIC_24 = 24,
    WAR_MUSIC_25 = 25,
    WAR_MUSIC_26 = 26,
    WAR_MUSIC_27 = 27,
    WAR_MUSIC_28 = 28,
    WAR_MUSIC_29 = 29,
    WAR_MUSIC_30 = 30,
    WAR_MUSIC_31 = 31,
    WAR_MUSIC_32 = 32,
    WAR_MUSIC_33 = 33,
    WAR_MUSIC_34 = 34,
    WAR_MUSIC_35 = 35,
    WAR_MUSIC_36 = 36,
    WAR_MUSIC_37 = 37,
    WAR_MUSIC_38 = 38,
    WAR_MUSIC_39 = 39,
    WAR_MUSIC_40 = 40,
    WAR_MUSIC_41 = 41,
    WAR_MUSIC_42 = 42,
    WAR_MUSIC_43 = 43,
    WAR_MUSIC_44 = 44,
    WAR_LOGO = 472,
    WAR_INTRO_DOOR = 473,
    WAR_BUILDING = 474,
    WAR_EXPLOSION = 475,
    WAR_CATAPULT_ROCK_FIRED = 476,
    WAR_TREE_CHOPPING_1 = 477,
    WAR_TREE_CHOPPING_2 = 478,
    WAR_TREE_CHOPPING_3 = 479,
    WAR_TREE_CHOPPING_4 = 480,
    WAR_BUILDING_COLLAPSE_1 = 481,
    WAR_BUILDING_COLLAPSE_2 = 482,
    WAR_BUILDING_COLLAPSE_3 = 483,
    WAR_UI_CHIME = 484,
    WAR_UI_CLICK = 485,
    WAR_UI_CANCEL = 486,
    WAR_SWORD_ATTACK_1 = 487,
    WAR_SWORD_ATTACK_2 = 488,
    WAR_SWORD_ATTACK_3 = 489,
    WAR_FIST_ATTACK = 490,
    WAR_CATAPULT_FIRE_EXPLOSION = 491,
    WAR_FIREBALL = 492,
    WAR_ARROW_SPEAR = 493,
    WAR_ARROW_SPEAR_HIT = 494,
    WAR_ORC_HELP_1 = 495,                           // "The humans draw near"
    WAR_ORC_HELP_2 = 496,                           // "The pale dogs approach"
    WAR_HUMAN_HELP_1 = 497,                         // "The Orcs are approaching"
    WAR_HUMAN_HELP_2 = 498,                         // "There are enemies nearby"
    WAR_ORC_DEAD = 499,
    WAR_HUMAN_DEAD = 500,
    WAR_ORC_WORK_COMPLETE = 501,                    // "Work completed"
    WAR_HUMAN_WORK_COMPLETE = 502,                  // "Work completed"
    WAR_ORC_HELP_3 = 503,                           // "We are being attacked"
    WAR_ORC_HELP_4 = 504,                           // "They're destroying our city"
    WAR_HUMAN_HELP_3 = 505,                         // "We are under attack"
    WAR_HUMAN_HELP_4 = 506,                         // "The town is under attack"
    WAR_ORC_READY = 507,                            // "Your command, master"
    WAR_HUMAN_READY = 508,                          // "Your command"
    WAR_ORC_ACKNOWLEDGEMENT_1 = 509,
    WAR_ORC_ACKNOWLEDGEMENT_2 = 510,
    WAR_ORC_ACKNOWLEDGEMENT_3 = 511,
    WAR_ORC_ACKNOWLEDGEMENT_4 = 512,
    WAR_HUMAN_ACKNOWLEDGEMENT_1 = 513,              // "Yes"
    WAR_HUMAN_ACKNOWLEDGEMENT_2 = 514,              // "Yes, mylord"
    WAR_ORC_SELECTED_1 = 515,
    WAR_ORC_SELECTED_2 = 516,
    WAR_ORC_SELECTED_3 = 517,
    WAR_ORC_SELECTED_4 = 518,
    WAR_ORC_SELECTED_5 = 519,
    WAR_HUMAN_SELECTED_1 = 520,                     // "Yes?"
    WAR_HUMAN_SELECTED_2 = 521,                     // "Your will, sire?"
    WAR_HUMAN_SELECTED_3 = 522,                     // "Mylord?"
    WAR_HUMAN_SELECTED_4 = 523,                     // "My liege?"
    WAR_HUMAN_SELECTED_5 = 524,                     // "Your bidding?"
    WAR_ORC_ANNOYED_1 = 525,
    WAR_ORC_ANNOYED_2 = 526,
    WAR_ORC_ANNOYED_3 = 527,                        // "Stop poking me"
    WAR_HUMAN_ANNOYED_1 = 528,                      // "What?!"
    WAR_HUMAN_ANNOYED_2 = 529,                      // "What do you want?!"
    WAR_HUMAN_ANNOYED_3 = 530,                      // "Why do you keep touching me?!"
    WAR_DEAD_SPIDER_SCORPION = 531,
    WAR_NORMAL_SPELL = 532,
    WAR_BUILD_ROAD = 533,
    WAR_ORC_TEMPLE = 534,
    WAR_HUMAN_CHURCH = 535,
    WAR_ORC_KENNEL = 536,
    WAR_HUMAN_STABLE = 537,
    WAR_BLACKSMITH = 538,
    WAR_FIRE_CRACKLING = 539,
    WAR_CANNON = 540,
    WAR_CANNON2 = 541,
    WAR_CAMPAIGNS_HUMAN_ENDING_1 = 542,
    WAR_CAMPAIGNS_HUMAN_ENDING_2 = 543,
    WAR_CAMPAIGNS_ORC_ENDING_1 = 544,
    WAR_CAMPAIGNS_ORC_ENDING_2 = 545,
    WAR_INTRO_1 = 546,                              // "In the age of chaos..."
    WAR_INTRO_2 = 547,                              // "The kingdom of Azeroth was..."
    WAR_INTRO_3 = 548,                              // "No one knew where these..."
    WAR_INTRO_4 = 549,                              // "With an ingenious..."
    WAR_INTRO_5 = 550,                              // "Welcome to the World of Warcraft"
    WAR_CAMPAIGNS_HUMAN_01_INTRO = 551,
    WAR_CAMPAIGNS_HUMAN_02_INTRO = 552,
    WAR_CAMPAIGNS_HUMAN_03_INTRO = 553,
    WAR_CAMPAIGNS_HUMAN_04_INTRO = 554,
    WAR_CAMPAIGNS_HUMAN_05_INTRO = 555,
    WAR_CAMPAIGNS_HUMAN_06_INTRO = 556,
    WAR_CAMPAIGNS_HUMAN_07_INTRO = 557,
    WAR_CAMPAIGNS_HUMAN_08_INTRO = 558,
    WAR_CAMPAIGNS_HUMAN_09_INTRO = 559,
    WAR_CAMPAIGNS_HUMAN_10_INTRO = 560,
    WAR_CAMPAIGNS_HUMAN_11_INTRO = 561,
    WAR_CAMPAIGNS_HUMAN_12_INTRO = 562,
    WAR_CAMPAIGNS_ORC_01_INTRO = 563,
    WAR_CAMPAIGNS_ORC_02_INTRO = 564,
    WAR_CAMPAIGNS_ORC_03_INTRO = 565,
    WAR_CAMPAIGNS_ORC_04_INTRO = 566,
    WAR_CAMPAIGNS_ORC_05_INTRO = 567,
    WAR_CAMPAIGNS_ORC_06_INTRO = 568,
    WAR_CAMPAIGNS_ORC_07_INTRO = 569,
    WAR_CAMPAIGNS_ORC_08_INTRO = 570,
    WAR_CAMPAIGNS_ORC_09_INTRO = 571,
    WAR_CAMPAIGNS_ORC_10_INTRO = 572,
    WAR_CAMPAIGNS_ORC_11_INTRO = 573,
    WAR_CAMPAIGNS_ORC_12_INTRO = 574,
    WAR_HUMAN_DEFEAT = 575,                         // "Your failure in battle"
    WAR_ORC_DEFEAT = 576,                           // "You pitiful"
    WAR_ORC_VICTORY_1 = 577,                        // "The feel of bones"
    WAR_ORC_VICTORY_2 = 578,                        // "If only the worthless"
    WAR_ORC_VICTORY_3 = 579,                        // "Gaze upon the destruction"
    WAR_HUMAN_VICTORY_1 = 580,                      // "The forces of darkness"
    WAR_HUMAN_VICTORY_2 = 581,                      // "Even these children"
    WAR_HUMAN_VICTORY_3 = 582,                      // "Cheers of victory"
};

enum _WarAudioType
{
    WAR_AUDIO_MIDI,
    WAR_AUDIO_WAVE
};

struct _WarAudioComponent
{
    bool enabled;
    WarAudioType type;
    s32 resourceIndex;
    bool loop;
    f32 playbackTime;
    s32 sampleIndex;
    tml_message* firstMessage;
    tml_message* currentMessage;
};

bool wa_initAudio(WarContext* context);
void wa_removeAudiosOfType(WarContext* context, WarAudioType type);
WarEntity* wa_createAudio(WarContext* context, WarAudioId audioId, bool loop);
WarEntity* wa_createAudioWithPosition(WarContext* context, WarAudioId audioId, vec2 position, bool loop);
WarEntity* wa_createAudioRandom(WarContext* context, WarAudioId fromId, WarAudioId toId, bool loop);
WarEntity* wa_createAudioRandomWithPosition(WarContext* context, WarAudioId fromId, WarAudioId toId, vec2 position, bool loop);
WarEntity* wa_playAttackSound(WarContext* context, vec2 position, WarUnitActionStepType soundStep);
WarEntity* wa_playDudeSelectionSound(WarContext* context, WarEntity* entity);
WarEntity* wa_playBuildingSelectionSound(WarContext* context, WarEntity* entity);
WarEntity* wa_playAcknowledgementSound(WarContext* context, WarPlayerInfo* player);
u8* wa_transcodeXmiToMid(WarContext* context, u8* xmiData, size_t xmiLength, size_t* midLength);
u8* wa_changeSampleRate(WarContext* context, u8* samplesIn, s32 length, s32 factor);
