#include "war_actions.h"

#include <assert.h>
#include <stdlib.h>

#include "war.h"
#include "war_units.h"

#define __frameCountToSeconds(f) ((f32)(f)/FRAMES_PER_SECONDS)

static WarUnitActionDef gUnitActionDefs[WAR_UNIT_COUNT][WAR_ACTION_TYPE_COUNT];

typedef struct
{
    s32 walkFramesCount;
    s32 walkFrames[5];

    s32 attackFramesCount;
    s32 attackFrames[5];

    s32 deathFramesCount;
    s32 deathFrames[5];
} WarUnitFrameNumbers;

static WarUnitFrameNumbers wact_getFrameNumbers(s32 nbdir, s32 initCounter[])
{
    initCounter[0]--;

    s32 total = initCounter[0] + initCounter[1] + initCounter[2];
    s32 counter[] = {initCounter[0] + 1, initCounter[1], initCounter[2]};
    s32 itype = 0;

    s32 res[3][5] = {0};

    for(s32 i = 0; i < total; i++)
    {
        counter[itype]--;

        do
        {
            itype++;
            if (itype == 3) itype = 0;
        } while (counter[itype] <= 0);

        res[itype][initCounter[itype] - counter[itype]] = (i + 1) * nbdir;
    }

    WarUnitFrameNumbers frameNumbers = (WarUnitFrameNumbers){0};

    frameNumbers.walkFramesCount = initCounter[0];
    memcpy(frameNumbers.walkFrames, res[0], initCounter[0] * sizeof(s32));

    frameNumbers.attackFramesCount = initCounter[1];
    memcpy(frameNumbers.attackFrames, res[1], initCounter[1] * sizeof(s32));

    frameNumbers.deathFramesCount = initCounter[2];
    memcpy(frameNumbers.deathFrames, res[2], initCounter[2] * sizeof(s32));

    return frameNumbers;
}

static WarUnitActionDef createUnitActionDef(WarUnitActionType type, bool directional, bool loop)
{
    WarUnitActionDef def = {0};
    def.type = type;
    def.directional = directional;
    def.loop = loop;
    WarUnitActionStepListInit(&def.steps, WarUnitActionStepListDefaultOptions);
    return def;
}

static void addUnitActionDefStep(WarUnitActionDef* def, WarUnitActionStepType type, s32 param)
{
    WarUnitActionStepListAdd(&def->steps, (WarUnitActionStep){type, param});
}

static WarUnitActionDef createWalkActionDef(s32 nframes, s32 frames[], s32 walkSpeed, bool directional)
{
    WarUnitActionDef def = createUnitActionDef(WAR_ACTION_TYPE_WALK, directional, true);

    s32 halfIndex = nframes % 2 == 0 ? nframes / 2 : (nframes + 1) / 2;

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

    // This code convert the frames sequence in this WAR_ACTION_STEP_FRAME sequence.
    // (this code is ported from the War1gus project, built with the Stratagus engine)
    //
    // 1 frame  a         => a 0 a 0 a 0 a 0
    // 2 frames a b       => a 0 b 0 a 0 b 0
    // 3 frames a b c     => a b a 0 c b c 0
    // 4 frames a b c d   => a b a 0 d c d 0
    // 5 frames a b c d e => a b c b a e d c d e
    //
    // This code is used with a previous steps calling 'wact_getFrameNumbers'
    // and construct the base frame sequences for walk, attack and death animations.
    //
    // Is this way because of the layout of the spritesheets of the units:
    // (in reality is 5 frames per row, because of the directions of the units,
    //  to keep it simple asume that there is only one direction NORTH)
    //
    // footman spritesheet:
    // 1 frame of walk
    // 1 frame of attack
    // 1 frame of death
    // 1 frame of walk
    // 1 frame of attack
    // 1 frame of death
    // 1 frame of walk
    // 1 frame of attack
    // 1 frame of death (death anim is only 3 frames for the footman)
    // 1 frame of walk
    // 1 frame of attack
    // 1 frame of walk
    // 1 frame of attack
    //
    // other spritesheets are similar, just with different frames count per animations

    s32 actionFrames = 8;
    while (actionFrames > 0)
    {
        for(s32 i = 0; i < halfIndex; i++)
        {
            addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, frames[i]);
            addUnitActionDefStep(&def, WAR_ACTION_STEP_MOVE, 4);
            addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, walkSpeed);
            actionFrames--;
        }

        for(s32 i = halfIndex - 2; i >= 0; i--)
        {
            addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, frames[i]);
            addUnitActionDefStep(&def, WAR_ACTION_STEP_MOVE, 4);
            addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, walkSpeed);
            actionFrames--;
        }

        addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, 0);
        addUnitActionDefStep(&def, WAR_ACTION_STEP_MOVE, 4);
        addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, walkSpeed);
        actionFrames--;

        for(s32 i = 0; i < halfIndex; i++)
        {
            addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, frames[nframes - 1 - i]);
            addUnitActionDefStep(&def, WAR_ACTION_STEP_MOVE, 4);
            addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, walkSpeed);
            actionFrames--;
        }

        for(s32 i = 1 + nframes - halfIndex; i < nframes; i++)
        {
            addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, frames[i]);
            addUnitActionDefStep(&def, WAR_ACTION_STEP_MOVE, 4);
            addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, walkSpeed);
            actionFrames--;
        }

        addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, 0);
        addUnitActionDefStep(&def, WAR_ACTION_STEP_MOVE, 4);
        addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, walkSpeed);
        actionFrames--;
    }

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, 0);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, 1);

    assert(!actionFrames);
    return def;
}

static WarUnitActionDef createLinearWalkActionDef(s32 framesCount, s32 frames[], bool directional, s32 walkSpeed)
{
    WarUnitActionDef def = createUnitActionDef(WAR_ACTION_TYPE_WALK, directional, true);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

    for(s32 i = 0; i < framesCount; i++)
    {
        addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, frames[i]);
        addUnitActionDefStep(&def, WAR_ACTION_STEP_MOVE, 4);
        addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, walkSpeed);
    }

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, 0);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, 1);

    return def;
}

static WarUnitActionDef createAttackActionDef(s32 nframes, s32 frames[], s32 attackSpeed, s32 attackSound, s32 coolOffTime, bool directional)
{
    WarUnitActionDef def = createUnitActionDef(WAR_ACTION_TYPE_ATTACK, directional, true);

    s32 halfIndex = nframes%2 == 0 ? nframes/2 : (nframes+1)/2;

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

    for(s32 i = 0; i < nframes; i++)
    {
        addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, frames[i]);

        if (i == halfIndex)
        {
            addUnitActionDefStep(&def, WAR_ACTION_STEP_ATTACK, 0);
            addUnitActionDefStep(&def, attackSound, 0);
        }

        addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, attackSpeed);
    }

    // make sure we don't attack faster just because we have fewer frames
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, (5 - nframes) * attackSpeed);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, 0);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, coolOffTime);

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, 0);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, 1);

    return def;
}

static WarUnitActionDef createRepairActionDef(s32 nframes, s32 frames[], s32 attackSpeed, s32 attackSound, s32 coolOffTime, bool directional)
{
    WarUnitActionDef def = createUnitActionDef(WAR_ACTION_TYPE_REPAIR, directional, true);

    s32 halfIndex = nframes%2 == 0 ? nframes/2 : (nframes+1)/2;

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

    for(s32 i = 0; i < nframes; i++)
    {
        addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, frames[i]);

        if (i == halfIndex)
        {
            addUnitActionDefStep(&def, WAR_ACTION_STEP_ATTACK, 0);
            addUnitActionDefStep(&def, attackSound, 0);
        }

        addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, attackSpeed);
    }

    // make sure we don't attack faster just because we have fewer frames
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, (5 - nframes) * attackSpeed);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, 0);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, coolOffTime);

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, 0);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, 1);

    return def;
}

static WarUnitActionDef createHarvestActionDef(s32 nframes, s32 frames[], s32 harvestSpeed, s32 harvestSound, s32 coolOffTime, bool directional)
{
    WarUnitActionDef def = createUnitActionDef(WAR_ACTION_TYPE_HARVEST, directional, true);

    s32 halfIndex = nframes%2 == 0 ? nframes/2 : (nframes+1)/2;

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, 5);

    for(s32 i = 0; i < nframes; i++)
    {
        addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, frames[i]);

        if (i == halfIndex)
        {
            addUnitActionDefStep(&def, WAR_ACTION_STEP_ATTACK, 0);
            addUnitActionDefStep(&def, harvestSound, 0);
        }

        addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, harvestSpeed);
    }

    // make sure we don't attack faster just because we have fewer frames
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, (5 - nframes) * harvestSpeed);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, 0);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, coolOffTime);

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, 0);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, 1);

    return def;
}

static WarUnitActionDef createDeathActionDef(s32 nframes, s32 frames[], s32 waitTime, bool directional, bool doWait101Step)
{
    WarUnitActionDef def = createUnitActionDef(WAR_ACTION_TYPE_DEATH, directional, true);

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

    for(s32 i = 0; i < nframes; i++)
    {
        addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, frames[i]);
        addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, waitTime);
    }

    if (doWait101Step)
        addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, 101);

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, 1);

    return def;
}

static WarUnitActionDef createBuildActionDef(s32 nframes, s32 frames[], s32 waitTime)
{
    WarUnitActionDef def = createUnitActionDef(WAR_ACTION_TYPE_BUILD, false, false);

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

    for(s32 i = 0; i < nframes; i++)
    {
        addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, frames[i]);
        addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, waitTime);
    }

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, 1);

    return def;
}

static WarUnitActionDef createIdleActionDef(s32 nframes, s32 frames[], s32 waitTime, bool directional)
{
    WarUnitActionDef def = createUnitActionDef(WAR_ACTION_TYPE_IDLE, directional, true);

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

    for(s32 i = 0; i < nframes; i++)
    {
        addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, frames[i]);
        addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, waitTime);
    }

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, 1);

    return def;
}

static WarUnitActionDef createDefaultIdleActionDef(s32 waitTime, bool directional)
{
    s32 idleFrames[] = {0};
    return createIdleActionDef(1, idleFrames, waitTime, directional);
}

static WarUnitActionDef createSpiderScorpionDeathActionDef(s32 framesCount, s32 frames[], bool directional, s32 waitTime)
{
    // Scorpions and Spiders have distinct wait times than other units,
    // so a custom death action is built for those cases
    WarUnitActionDef deathDef = createUnitActionDef(WAR_ACTION_TYPE_DEATH, directional, true);
    addUnitActionDefStep(&deathDef, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);
    addUnitActionDefStep(&deathDef, WAR_ACTION_STEP_FRAME, frames[0]);
    addUnitActionDefStep(&deathDef, WAR_ACTION_STEP_WAIT, waitTime);
    addUnitActionDefStep(&deathDef, WAR_ACTION_STEP_FRAME, frames[1]);
    addUnitActionDefStep(&deathDef, WAR_ACTION_STEP_WAIT, waitTime);
    addUnitActionDefStep(&deathDef, WAR_ACTION_STEP_FRAME, frames[2]);
    addUnitActionDefStep(&deathDef, WAR_ACTION_STEP_WAIT, waitTime * 100);
    addUnitActionDefStep(&deathDef, WAR_ACTION_STEP_FRAME, frames[3]);
    addUnitActionDefStep(&deathDef, WAR_ACTION_STEP_WAIT, waitTime);
    addUnitActionDefStep(&deathDef, WAR_ACTION_STEP_FRAME, frames[4]);
    addUnitActionDefStep(&deathDef, WAR_ACTION_STEP_WAIT, waitTime);
    addUnitActionDefStep(&deathDef, WAR_ACTION_STEP_WAIT, 101);
    addUnitActionDefStep(&deathDef, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addUnitActionDefStep(&deathDef, WAR_ACTION_STEP_WAIT, 1);
    return deathDef;
}

static WarUnitActionDef createSlimeWalkActionDef(bool directional, s32 walkSpeed)
{
    WarUnitActionDef def = createUnitActionDef(WAR_ACTION_TYPE_WALK, directional, true);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

    s32 walkFrames[] = {15, 30, 45, 55, 65, 0};
    for(s32 i = 0; i < arrayLength(walkFrames); i++)
    {
        addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, walkFrames[i]);
        addUnitActionDefStep(&def, WAR_ACTION_STEP_MOVE, 4);
        addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, walkSpeed);
    }

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, 0);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, 15);

    return def;
}

static WarUnitActionDef createFireElementalWalkActionDef(bool directional, s32 walkSpeed)
{
    WarUnitActionDef def = createUnitActionDef(WAR_ACTION_TYPE_WALK, directional, true);

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

    s32 walkFrames[] = {10, 20, 30, 40, 50, 0};
    for(s32 i = 0; i < arrayLength(walkFrames); i++)
    {
        addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, walkFrames[i]);
        addUnitActionDefStep(&def, WAR_ACTION_STEP_MOVE, 4);
        addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, walkSpeed);
    }

    addUnitActionDefStep(&def, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_FRAME, 0);
    addUnitActionDefStep(&def, WAR_ACTION_STEP_WAIT, 1);

    return def;
}

static void initFootmanGruntActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 6;
    s32 attackSpeed = 6;
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;
    WarUnitType unitTypes[] = {WAR_UNIT_FOOTMAN, WAR_UNIT_GRUNT};

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_SWORD, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

    for (s32 i = 0; i < arrayLength(unitTypes); i++)
    {
        WarUnitType unitType = unitTypes[i];

        gUnitActionDefs[unitType][WAR_ACTION_TYPE_IDLE] = idleDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_WALK] = walkDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_ATTACK] = attackDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_DEATH] = deathDef;
    }
}

static void initPeasantPeonActionDefs(WarUnitFrameNumbers frames, WarUnitFrameNumbers framesHarvest)
{
    s32 walkSpeed = 6;
    s32 attackSpeed = 6;
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;
    WarUnitType unitTypes[] = {WAR_UNIT_PEASANT, WAR_UNIT_PEON};

    WarUnitActionDef defaultIdleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_CHOPPING, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);
    WarUnitActionDef repairDef = createRepairActionDef(framesHarvest.attackFramesCount, framesHarvest.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_CHOPPING, coolOffTime, directional);
    WarUnitActionDef harvestDef = createHarvestActionDef(framesHarvest.attackFramesCount, framesHarvest.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_CHOPPING, coolOffTime, directional);

    for (s32 i = 0; i < arrayLength(unitTypes); i++)
    {
        WarUnitType unitType = unitTypes[i];

        gUnitActionDefs[unitType][WAR_ACTION_TYPE_IDLE] = defaultIdleDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_WALK] = walkDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_ATTACK] = attackDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_DEATH] = deathDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_REPAIR] = repairDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_HARVEST] = harvestDef;
    }
}

static void initCatapultActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 10;
    s32 attackSpeed = 25;
    s32 coolOffTime = 49;
    s32 waitTime = 3;
    bool directional = true;
    WarUnitType unitTypes[] = {WAR_UNIT_CATAPULT_HUMANS, WAR_UNIT_CATAPULT_ORCS};

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_CATAPULT, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

    for (s32 i = 0; i < arrayLength(unitTypes); i++)
    {
        WarUnitType unitType = unitTypes[i];

        gUnitActionDefs[unitType][WAR_ACTION_TYPE_IDLE] = idleDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_WALK] = walkDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_ATTACK] = attackDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_DEATH] = deathDef;
    }
}

static void initKnightRaiderActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 10;
    s32 attackSpeed = 6;
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;
    WarUnitType unitTypes[] = {WAR_UNIT_KNIGHT, WAR_UNIT_RAIDER};

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    s32 walkFrames[] = {15, 30, 45, 60, 0};
    WarUnitActionDef walkDef = createLinearWalkActionDef(arrayLength(walkFrames), walkFrames, directional, walkSpeed);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_SWORD, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

    for (s32 i = 0; i < arrayLength(unitTypes); i++)
    {
        WarUnitType unitType = unitTypes[i];

        gUnitActionDefs[unitType][WAR_ACTION_TYPE_IDLE] = idleDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_WALK] = walkDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_ATTACK] = attackDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_DEATH] = deathDef;
    }

    // These are probably when the knight and rider are upgraded?
    //
    // options.speed = 2;
    // buildActions(entity, frameNumbers_5_5_5_5, options);

    // options.speed = 1;
    // buildActions(entity, frameNumbers_5_5_5_5, options);
}

static void initArcherSpearmanActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 6;
    s32 attackSpeed = 10;
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;
    WarUnitType unitTypes[] = {WAR_UNIT_ARCHER, WAR_UNIT_SPEARMAN};

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_ARROW, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

    for (s32 i = 0; i < arrayLength(unitTypes); i++)
    {
        WarUnitType unitType = unitTypes[i];

        gUnitActionDefs[unitType][WAR_ACTION_TYPE_IDLE] = idleDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_WALK] = walkDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_ATTACK] = attackDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_DEATH] = deathDef;
    }
}

static void initConjurerActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 7;
    s32 attackSpeed = 8;
    s32 coolOffTime = 0;
    s32 waitTime = 3;
    bool directional = true;
    s32 attackFrames[] = {5, 20, 35, 50};

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(arrayLength(attackFrames), attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_FIREBALL, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

    gUnitActionDefs[WAR_UNIT_CONJURER][WAR_ACTION_TYPE_IDLE] = idleDef;
    gUnitActionDefs[WAR_UNIT_CONJURER][WAR_ACTION_TYPE_WALK] = walkDef;
    gUnitActionDefs[WAR_UNIT_CONJURER][WAR_ACTION_TYPE_ATTACK] = attackDef;
    gUnitActionDefs[WAR_UNIT_CONJURER][WAR_ACTION_TYPE_DEATH] = deathDef;
}

static void initWarlockActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 7;
    s32 attackSpeed = 8;
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;
    s32 attackFrames[] = {5, 20, 35, 50, 60};

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(arrayLength(attackFrames), attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_FIREBALL, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

    gUnitActionDefs[WAR_UNIT_WARLOCK][WAR_ACTION_TYPE_IDLE] = idleDef;
    gUnitActionDefs[WAR_UNIT_WARLOCK][WAR_ACTION_TYPE_WALK] = walkDef;
    gUnitActionDefs[WAR_UNIT_WARLOCK][WAR_ACTION_TYPE_ATTACK] = attackDef;
    gUnitActionDefs[WAR_UNIT_WARLOCK][WAR_ACTION_TYPE_DEATH] = deathDef;
}

static void initClericActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 7;
    s32 attackSpeed = 6;
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_FIREBALL, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

    gUnitActionDefs[WAR_UNIT_CLERIC][WAR_ACTION_TYPE_IDLE] = idleDef;
    gUnitActionDefs[WAR_UNIT_CLERIC][WAR_ACTION_TYPE_WALK] = walkDef;
    gUnitActionDefs[WAR_UNIT_CLERIC][WAR_ACTION_TYPE_ATTACK] = attackDef;
    gUnitActionDefs[WAR_UNIT_CLERIC][WAR_ACTION_TYPE_DEATH] = deathDef;
}

static void initNecrolyteActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 7;
    s32 attackSpeed = 6;
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_FIREBALL, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

    gUnitActionDefs[WAR_UNIT_NECROLYTE][WAR_ACTION_TYPE_IDLE] = idleDef;
    gUnitActionDefs[WAR_UNIT_NECROLYTE][WAR_ACTION_TYPE_WALK] = walkDef;
    gUnitActionDefs[WAR_UNIT_NECROLYTE][WAR_ACTION_TYPE_ATTACK] = attackDef;
    gUnitActionDefs[WAR_UNIT_NECROLYTE][WAR_ACTION_TYPE_DEATH] = deathDef;
}

static void initMedivhActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 6;
    s32 attackSpeed = 6;
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_LIGHTNING, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

    gUnitActionDefs[WAR_UNIT_MEDIVH][WAR_ACTION_TYPE_IDLE] = idleDef;
    gUnitActionDefs[WAR_UNIT_MEDIVH][WAR_ACTION_TYPE_WALK] = walkDef;
    gUnitActionDefs[WAR_UNIT_MEDIVH][WAR_ACTION_TYPE_ATTACK] = attackDef;
    gUnitActionDefs[WAR_UNIT_MEDIVH][WAR_ACTION_TYPE_DEATH] = deathDef;
}

static void initLotharActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 6;
    s32 attackSpeed = 6;
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_SWORD, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

    gUnitActionDefs[WAR_UNIT_LOTHAR][WAR_ACTION_TYPE_IDLE] = idleDef;
    gUnitActionDefs[WAR_UNIT_LOTHAR][WAR_ACTION_TYPE_WALK] = walkDef;
    gUnitActionDefs[WAR_UNIT_LOTHAR][WAR_ACTION_TYPE_ATTACK] = attackDef;
    gUnitActionDefs[WAR_UNIT_LOTHAR][WAR_ACTION_TYPE_DEATH] = deathDef;
}

static void initGrizeldaGaronaActionDefs(void)
{
    s32 walkSpeed = 6;
    s32 waitTime = 4;
    bool directional = true;
    s32 walkFrames[] = {10, 20, 30, 35};
    s32 deathFrames[] = {5, 15, 25};
    WarUnitType unitTypes[] = {WAR_UNIT_GRIZELDA, WAR_UNIT_GARONA};

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(arrayLength(walkFrames), walkFrames, walkSpeed, directional);
    WarUnitActionDef deathDef = createDeathActionDef(arrayLength(deathFrames), deathFrames, waitTime, directional, true);

    for (s32 i = 0; i < arrayLength(unitTypes); i++)
    {
        WarUnitType unitType = unitTypes[i];

        gUnitActionDefs[unitType][WAR_ACTION_TYPE_IDLE] = idleDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_WALK] = walkDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_DEATH] = deathDef;
    }
}

static void initOgreActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 6;
    s32 attackSpeed = 6;
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_FIST, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

    gUnitActionDefs[WAR_UNIT_OGRE][WAR_ACTION_TYPE_IDLE] = idleDef;
    gUnitActionDefs[WAR_UNIT_OGRE][WAR_ACTION_TYPE_WALK] = walkDef;
    gUnitActionDefs[WAR_UNIT_OGRE][WAR_ACTION_TYPE_ATTACK] = attackDef;
    gUnitActionDefs[WAR_UNIT_OGRE][WAR_ACTION_TYPE_DEATH] = deathDef;
}

static void initSpiderActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 4;
    s32 attackSpeed = 6;
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;
    s32 walkFrames[] = {15, 30, 45, 60, 0};

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createLinearWalkActionDef(arrayLength(walkFrames), walkFrames, directional, walkSpeed);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_FIST, coolOffTime, directional);
    WarUnitActionDef deathDef = createSpiderScorpionDeathActionDef(frames.deathFramesCount, frames.deathFrames, directional, waitTime);

    gUnitActionDefs[WAR_UNIT_SPIDER][WAR_ACTION_TYPE_IDLE] = idleDef;
    gUnitActionDefs[WAR_UNIT_SPIDER][WAR_ACTION_TYPE_WALK] = walkDef;
    gUnitActionDefs[WAR_UNIT_SPIDER][WAR_ACTION_TYPE_ATTACK] = attackDef;
    gUnitActionDefs[WAR_UNIT_SPIDER][WAR_ACTION_TYPE_DEATH] = deathDef;
}

static void initSlimeActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 6;
    s32 attackSpeed = 15;
    s32 coolOffTime = 15;
    s32 waitTime = 8;
    bool directional = true;
    s32 idleFrames[] = {0, 70, 75, 80, 85, 90};

    WarUnitActionDef idleDef = createIdleActionDef(arrayLength(idleFrames), idleFrames, waitTime, directional);
    WarUnitActionDef walkDef = createSlimeWalkActionDef(directional, walkSpeed);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_FIST, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, 3, directional, true);

    gUnitActionDefs[WAR_UNIT_SLIME][WAR_ACTION_TYPE_IDLE] = idleDef;
    gUnitActionDefs[WAR_UNIT_SLIME][WAR_ACTION_TYPE_WALK] = walkDef;
    gUnitActionDefs[WAR_UNIT_SLIME][WAR_ACTION_TYPE_ATTACK] = attackDef;
    gUnitActionDefs[WAR_UNIT_SLIME][WAR_ACTION_TYPE_DEATH] = deathDef;
}

static void initFireElementalActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 5;
    s32 attackSpeed = 6;
    s32 coolOffTime = 24;
    s32 waitTime = 8;
    bool directional = true;
    s32 idleFrames[] = {5, 15, 25, 35, 50};

    WarUnitActionDef idleDef = createIdleActionDef(arrayLength(idleFrames), idleFrames, waitTime, directional);
    WarUnitActionDef walkDef = createFireElementalWalkActionDef(directional, walkSpeed);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_FIREBALL, coolOffTime, directional);

    gUnitActionDefs[WAR_UNIT_FIRE_ELEMENTAL][WAR_ACTION_TYPE_IDLE] = idleDef;
    gUnitActionDefs[WAR_UNIT_FIRE_ELEMENTAL][WAR_ACTION_TYPE_WALK] = walkDef;
    gUnitActionDefs[WAR_UNIT_FIRE_ELEMENTAL][WAR_ACTION_TYPE_ATTACK] = attackDef;
}

static void initWaterElementalActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 4;
    s32 attackSpeed = 6;
    s32 coolOffTime = 24;
    s32 waitTime = 8;
    bool directional = true;

    s32 idleFrames[] = {0, 5, 15, 30};
    WarUnitActionDef idleDef = createIdleActionDef(arrayLength(idleFrames), idleFrames, waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_FIREBALL, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, 3, directional, true);

    gUnitActionDefs[WAR_UNIT_WATER_ELEMENTAL][WAR_ACTION_TYPE_IDLE] = idleDef;
    gUnitActionDefs[WAR_UNIT_WATER_ELEMENTAL][WAR_ACTION_TYPE_WALK] = walkDef;
    gUnitActionDefs[WAR_UNIT_WATER_ELEMENTAL][WAR_ACTION_TYPE_ATTACK] = attackDef;
    gUnitActionDefs[WAR_UNIT_WATER_ELEMENTAL][WAR_ACTION_TYPE_DEATH] = deathDef;
}

static void initScorpionActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 4;
    s32 attackSpeed = 6;
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;
    s32 walkFrames[] = {15, 30, 45, 60, 0};

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createLinearWalkActionDef(arrayLength(walkFrames), walkFrames, directional, walkSpeed);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_FIST, coolOffTime, directional);
    WarUnitActionDef deathDef = createSpiderScorpionDeathActionDef(frames.deathFramesCount, frames.deathFrames, directional, waitTime);

    gUnitActionDefs[WAR_UNIT_SCORPION][WAR_ACTION_TYPE_IDLE] = idleDef;
    gUnitActionDefs[WAR_UNIT_SCORPION][WAR_ACTION_TYPE_WALK] = walkDef;
    gUnitActionDefs[WAR_UNIT_SCORPION][WAR_ACTION_TYPE_ATTACK] = attackDef;
    gUnitActionDefs[WAR_UNIT_SCORPION][WAR_ACTION_TYPE_DEATH] = deathDef;
}

static void initBrigandActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 4;
    s32 attackSpeed = 6;
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_SWORD, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

    gUnitActionDefs[WAR_UNIT_BRIGAND][WAR_ACTION_TYPE_IDLE] = idleDef;
    gUnitActionDefs[WAR_UNIT_BRIGAND][WAR_ACTION_TYPE_WALK] = walkDef;
    gUnitActionDefs[WAR_UNIT_BRIGAND][WAR_ACTION_TYPE_ATTACK] = attackDef;
    gUnitActionDefs[WAR_UNIT_BRIGAND][WAR_ACTION_TYPE_DEATH] = deathDef;
}

static void initSkeletonActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 4;
    s32 attackSpeed = 6;
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_SWORD, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

    gUnitActionDefs[WAR_UNIT_SKELETON][WAR_ACTION_TYPE_IDLE] = idleDef;
    gUnitActionDefs[WAR_UNIT_SKELETON][WAR_ACTION_TYPE_WALK] = walkDef;
    gUnitActionDefs[WAR_UNIT_SKELETON][WAR_ACTION_TYPE_ATTACK] = attackDef;
    gUnitActionDefs[WAR_UNIT_SKELETON][WAR_ACTION_TYPE_DEATH] = deathDef;
}

static void initDaemonActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 6;
    s32 attackSpeed = 6;
    s32 coolOffTime = 24;
    s32 waitTime = 3;
    bool directional = true;

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_SWORD, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

    gUnitActionDefs[WAR_UNIT_DAEMON][WAR_ACTION_TYPE_IDLE] = idleDef;
    gUnitActionDefs[WAR_UNIT_DAEMON][WAR_ACTION_TYPE_WALK] = walkDef;
    gUnitActionDefs[WAR_UNIT_DAEMON][WAR_ACTION_TYPE_ATTACK] = attackDef;
    gUnitActionDefs[WAR_UNIT_DAEMON][WAR_ACTION_TYPE_DEATH] = deathDef;
}

static void initTheDeadActionDefs(WarUnitFrameNumbers frames)
{
    s32 walkSpeed = 6;
    s32 attackSpeed = 6;
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;

    WarUnitActionDef idleDef = createDefaultIdleActionDef(waitTime, directional);
    WarUnitActionDef walkDef = createWalkActionDef(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
    WarUnitActionDef attackDef = createAttackActionDef(frames.attackFramesCount, frames.attackFrames, attackSpeed, WAR_ACTION_STEP_SOUND_SWORD, coolOffTime, directional);
    WarUnitActionDef deathDef = createDeathActionDef(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

    gUnitActionDefs[WAR_UNIT_THE_DEAD][WAR_ACTION_TYPE_IDLE] = idleDef;
    gUnitActionDefs[WAR_UNIT_THE_DEAD][WAR_ACTION_TYPE_WALK] = walkDef;
    gUnitActionDefs[WAR_UNIT_THE_DEAD][WAR_ACTION_TYPE_ATTACK] = attackDef;
    gUnitActionDefs[WAR_UNIT_THE_DEAD][WAR_ACTION_TYPE_DEATH] = deathDef;
}

static void initWoundedActionDefs(void)
{
    s32 idleFrames[] = {0};
    WarUnitActionDef idleDef = createIdleActionDef(arrayLength(idleFrames), idleFrames, 5, true);

    gUnitActionDefs[WAR_UNIT_WOUNDED][WAR_ACTION_TYPE_IDLE] = idleDef;
}

static void initBuildableActionDefs(void)
{
    s32 idleFrames[]  = {0};
    s32 buildFrames[] = {0, 1, 2};
    WarUnitType unitTypes[] = {
        WAR_UNIT_FARM_HUMANS,       WAR_UNIT_FARM_ORCS,
        WAR_UNIT_BARRACKS_HUMANS,   WAR_UNIT_BARRACKS_ORCS,
        WAR_UNIT_CHURCH,            WAR_UNIT_TEMPLE,
        WAR_UNIT_TOWER_HUMANS,      WAR_UNIT_TOWER_ORCS,
        WAR_UNIT_TOWNHALL_HUMANS,   WAR_UNIT_TOWNHALL_ORCS,
        WAR_UNIT_LUMBERMILL_HUMANS, WAR_UNIT_LUMBERMILL_ORCS,
        WAR_UNIT_STABLE,            WAR_UNIT_KENNEL,
        WAR_UNIT_BLACKSMITH_HUMANS, WAR_UNIT_BLACKSMITH_ORCS,
    };

    WarUnitActionDef idleDef = createIdleActionDef(arrayLength(idleFrames), idleFrames, 5, false);
    WarUnitActionDef buildDef = createBuildActionDef(arrayLength(buildFrames), buildFrames, 50);

    for (s32 i = 0; i < arrayLength(unitTypes); i++)
    {
        WarUnitType unitType = unitTypes[i];

        gUnitActionDefs[unitType][WAR_ACTION_TYPE_IDLE] = idleDef;
        gUnitActionDefs[unitType][WAR_ACTION_TYPE_BUILD] = buildDef;
    }
}

static void initNonBuildableActionDefs(void)
{
    s32 idleFrames[] = {0};
    WarUnitType unitTypes[] = { WAR_UNIT_STORMWIND, WAR_UNIT_BLACKROCK, WAR_UNIT_GOLDMINE };

    WarUnitActionDef idleDef = createIdleActionDef(arrayLength(idleFrames), idleFrames, 5, false);

    for (s32 i = 0; i < arrayLength(unitTypes); i++)
    {
        WarUnitType unitType = unitTypes[i];

        gUnitActionDefs[unitType][WAR_ACTION_TYPE_IDLE] = idleDef;
    }
}

static void initHumanCorpseActionDefs(void)
{
    s32 deathFrames[] = {0, 10, 15, 20};
    WarUnitActionDef deathDef = createDeathActionDef(arrayLength(deathFrames), deathFrames, 300, true, false);

    gUnitActionDefs[WAR_UNIT_HUMAN_CORPSE][WAR_ACTION_TYPE_DEATH] = deathDef;
}

static void initOrcCorpseActionDefs(void)
{
    s32 deathFrames[] = {5, 10, 15, 20};
    WarUnitActionDef deathDef = createDeathActionDef(arrayLength(deathFrames), deathFrames, 300, true, false);

    gUnitActionDefs[WAR_UNIT_ORC_CORPSE][WAR_ACTION_TYPE_DEATH] = deathDef;
}

void wact_initUnitActionDefs(void)
{
    WarUnitFrameNumbers frameNumbers_5_5_5_5 = wact_getFrameNumbers(5, arrayArg(s32, 5, 5, 5));
    WarUnitFrameNumbers frameNumbers_5_5_5_4 = wact_getFrameNumbers(5, arrayArg(s32, 5, 5, 4));
    WarUnitFrameNumbers frameNumbers_5_5_5_3 = wact_getFrameNumbers(5, arrayArg(s32, 5, 5, 3));
    WarUnitFrameNumbers frameNumbers_5_5_4_5 = wact_getFrameNumbers(5, arrayArg(s32, 5, 4, 5));
    WarUnitFrameNumbers frameNumbers_5_5_4_4 = wact_getFrameNumbers(5, arrayArg(s32, 5, 4, 4));
    WarUnitFrameNumbers frameNumbers_5_5_4_3 = wact_getFrameNumbers(5, arrayArg(s32, 5, 4, 3));
    WarUnitFrameNumbers frameNumbers_5_5_3_2 = wact_getFrameNumbers(5, arrayArg(s32, 5, 3, 2));
    WarUnitFrameNumbers frameNumbers_5_5_2_3 = wact_getFrameNumbers(5, arrayArg(s32, 5, 2, 3));
    WarUnitFrameNumbers frameNumbers_5_3_5_3 = wact_getFrameNumbers(5, arrayArg(s32, 3, 5, 3));
    WarUnitFrameNumbers frameNumbers_5_2_5_3 = wact_getFrameNumbers(5, arrayArg(s32, 2, 5, 3));

    WarUnitFrameNumbers frameNumbers_5_4_3_3 = wact_getFrameNumbers(5, arrayArg(s32, 4, 3, 3));
    NOT_USED(frameNumbers_5_4_3_3);

    WarUnitFrameNumbers frameNumbers_5_5_5_0 = wact_getFrameNumbers(5, arrayArg(s32, 5, 5, 0));

    for (s32 i = 0; i < WAR_UNIT_COUNT; i++)
    {
        for (s32 j = 0; j < WAR_ACTION_TYPE_COUNT; j++)
        {
            gUnitActionDefs[i][j].type = WAR_ACTION_TYPE_NONE;
        }
    }

    initFootmanGruntActionDefs(frameNumbers_5_5_5_3);
    initPeasantPeonActionDefs(frameNumbers_5_5_5_3, frameNumbers_5_5_4_3);
    initCatapultActionDefs(frameNumbers_5_2_5_3);
    initKnightRaiderActionDefs(frameNumbers_5_5_5_5);
    initArcherSpearmanActionDefs(frameNumbers_5_5_2_3);
    initConjurerActionDefs(frameNumbers_5_5_4_4);
    initWarlockActionDefs(frameNumbers_5_5_5_3);
    initClericActionDefs(frameNumbers_5_5_4_3);
    initNecrolyteActionDefs(frameNumbers_5_5_5_4);
    initMedivhActionDefs(frameNumbers_5_5_5_3);
    initLotharActionDefs(frameNumbers_5_5_5_3);
    initGrizeldaGaronaActionDefs();
    initOgreActionDefs(frameNumbers_5_5_5_5);
    initSpiderActionDefs(frameNumbers_5_5_4_5);
    initSlimeActionDefs(frameNumbers_5_5_5_3);
    initFireElementalActionDefs(frameNumbers_5_5_5_0);
    initWaterElementalActionDefs(frameNumbers_5_3_5_3);
    initScorpionActionDefs(frameNumbers_5_5_5_5);
    initBrigandActionDefs(frameNumbers_5_5_3_2);
    initSkeletonActionDefs(frameNumbers_5_5_5_5);
    initDaemonActionDefs(frameNumbers_5_5_5_5);
    initTheDeadActionDefs(frameNumbers_5_5_5_5);
    initWoundedActionDefs();
    initBuildableActionDefs();
    initNonBuildableActionDefs();
    initHumanCorpseActionDefs();
    initOrcCorpseActionDefs();
}

 void wact_addUnitActions(WarEntity* entity)
{
    WarUnitComponent* unit = &entity->unit;

    for (s32 i = 0; i < WAR_ACTION_TYPE_COUNT; i++)
    {
        WarUnitAction* action = &unit->actions[i];
        action->status = WAR_ACTION_NOT_STARTED;
        action->unbreakable = false;
        action->scale = 1.0f;
        action->waitCount = 0;
        action->stepIndex = -1;
        action->lastActionStep = WAR_ACTION_STEP_NONE;
        action->lastSoundStep = WAR_ACTION_STEP_NONE;
    }
}

s32 wact_getActionDuration(WarEntity* entity, WarUnitActionType type)
{
    assert(isUnit(entity));

    if (type == WAR_ACTION_TYPE_NONE)
    {
        return 0;
    }

    WarUnitComponent* unit = &entity->unit;
    WarUnitActionDef* actionDef = &gUnitActionDefs[unit->type][type];

    s32 duration = 0;

    for (s32 i = 0; i < actionDef->steps.count; i++)
    {
        WarUnitActionStep step = actionDef->steps.items[i];
        if (step.type == WAR_ACTION_STEP_WAIT)
            duration += step.param;
    }

    return duration;
}

void wact_resetAction(WarUnitAction* action)
{
    action->stepIndex = 0;
    action->status = WAR_ACTION_NOT_STARTED;
}

void wact_setAction(WarContext* context, WarEntity* entity, WarUnitActionType type, bool reset, f32 scale)
{
    NOT_USED(context);

    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    if (type != WAR_ACTION_TYPE_NONE)
    {
        WarUnitActionDef* actionDef = &gUnitActionDefs[unit->type][type];
        assert(actionDef->type != WAR_ACTION_TYPE_NONE);

        unit->actionType = type;

        WarUnitAction* action = &unit->actions[unit->actionType];

        if (scale >= 0)
        {
            action->scale = scale;
        }

        if (reset)
        {
            wact_resetAction(action);
        }
    }
    else
    {
        unit->actionType = type;
    }
}

void wact_updateAction(WarContext* context, WarEntity* entity)
{
    WarTransformComponent* transform = &entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarUnitComponent* unit = &entity->unit;

    if (!unit || unit->actionType == WAR_ACTION_TYPE_NONE)
    {
        return;
    }

    WarUnitAction* action = &unit->actions[unit->actionType];
    if (!action || action->status == WAR_ACTION_FINISHED)
    {
        return;
    }

    if (action->stepIndex < 0)
    {
        wact_resetAction(action);
    }

    action->status = WAR_ACTION_RUNNING;

    WarUnitActionDef* actionDef = &gUnitActionDefs[unit->type][unit->actionType];
    WarUnitActionStep step = actionDef->steps.items[action->stepIndex];
    if (step.type == WAR_ACTION_STEP_WAIT)
    {
        action->waitCount -= wmap_getMapScaledSpeed(context, context->deltaTime);
        if (action->waitCount > 0)
        {
            return;
        }

        action->waitCount = 0;

        action->stepIndex++;
        if (action->stepIndex >= actionDef->steps.count)
        {
            if (!actionDef->loop)
            {
                action->status = WAR_ACTION_FINISHED;
                return;
            }

            action->stepIndex = 0;
        }

        step = actionDef->steps.items[action->stepIndex];
    }

    action->lastActionStep = WAR_ACTION_STEP_NONE;
    action->lastSoundStep = WAR_ACTION_STEP_NONE;

    while (step.type != WAR_ACTION_STEP_WAIT)
    {
        switch (step.type)
        {
            case WAR_ACTION_STEP_UNBREAKABLE:
            {
                if (step.param == WAR_UNBREAKABLE_BEGIN)
                    action->unbreakable = true;
                else if(step.param == WAR_UNBREAKABLE_END)
                    action->unbreakable = false;

                break;
            }

            case WAR_ACTION_STEP_FRAME:
            {
                s32 frameIndex = step.param;

                if (actionDef->directional)
                {
                    // super complicated math here:
                    //
                    // d               d
                    // N:  4 - abs(4 - 0) = 4 - 4 = 0
                    // NE: 4 - abs(4 - 1) = 4 - 3 = 1
                    // E:  4 - abs(4 - 2) = 4 - 2 = 2
                    // SW: 4 - abs(4 - 3) = 4 - 1 = 3
                    // S:  4 - abs(4 - 4) = 4 - 0 = 4
                    // SW: 4 - abs(4 - 5) = 4 - 1 = 3
                    // W:  4 - abs(4 - 6) = 4 - 2 = 2
                    // NW: 4 - abs(4 - 7) = 4 - 3 = 1
                    //
                    // ... 4 - abs(4 - d)

                    frameIndex += (4 - abs(4 - (s32)unit->direction));

                    transform->scale.x = inRange(unit->direction, WAR_DIRECTION_SOUTH_WEST, WAR_DIRECTION_COUNT) ? -1.0f : 1.0f;
                }

                sprite->frameIndex = frameIndex;
                break;
            }

            case WAR_ACTION_STEP_MOVE:
            {
                action->lastActionStep = WAR_ACTION_STEP_MOVE;
                break;
            }

            case WAR_ACTION_STEP_ATTACK:
            {
                action->lastActionStep = WAR_ACTION_STEP_ATTACK;
                break;
            }

            case WAR_ACTION_STEP_SOUND_SWORD:
            case WAR_ACTION_STEP_SOUND_FIST:
            case WAR_ACTION_STEP_SOUND_FIREBALL:
            case WAR_ACTION_STEP_SOUND_CHOPPING:
            case WAR_ACTION_STEP_SOUND_CATAPULT:
            case WAR_ACTION_STEP_SOUND_ARROW:
            case WAR_ACTION_STEP_SOUND_LIGHTNING:
            {
                action->lastSoundStep = step.type;
                break;
            }

            default:
            {
                break;
            }
        }

        action->stepIndex++;
        if (action->stepIndex >= actionDef->steps.count)
        {
            if (!actionDef->loop)
            {
                action->status = WAR_ACTION_FINISHED;
                return;
            }

            action->stepIndex = 0;
        }

        step = actionDef->steps.items[action->stepIndex];
    }

    action->waitCount = __frameCountToSeconds(step.param) * action->scale;
}
