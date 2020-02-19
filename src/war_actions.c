#define MAX_ACTIONS 6
#define MAX_ACTION_STEPS 32

#define __frameCountToSeconds(f) ((f32)(f)/FRAMES_PER_SECONDS)

typedef struct
{
    s32 walkFramesCount;
    s32 walkFrames[5];

    s32 attackFramesCount;
    s32 attackFrames[5];

    s32 deathFramesCount;
    s32 deathFrames[5];
} WarUnitFrameNumbers;

WarUnitFrameNumbers getFrameNumbers(s32 nbdir, s32 initCounter[])
{
    initCounter[0]--;

    s32 total = initCounter[0] + initCounter[1] + initCounter[2];
    s32 counter[] = {initCounter[0] + 1, initCounter[1], initCounter[2]};
    s32 itype = 0;

    s32** res = s32CreateArray(3, 5);

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

    s32FreeArray(res);

    return frameNumbers;
}

WarUnitAction* createUnitAction(WarUnitActionType type)
{
    WarUnitAction* action = (WarUnitAction*)xmalloc(sizeof(WarUnitAction));
    action->type = type;
    action->loop = true;
    action->scale = 1;
    action->status = WAR_ACTION_NOT_STARTED;

    WarUnitActionStepListInit(&action->steps, WarUnitActionStepListDefaultOptions);

    action->waitCount = 0;
    action->stepIndex = -1;
    return action;
}

void addActionStep(WarUnitAction* action, WarUnitActionStepType type, s32 param)
{
    WarUnitActionStepListAdd(&action->steps, (WarUnitActionStep){type, param});
}

WarUnitAction* buildWalkAction(s32 nframes, s32 frames[], s32 walkSpeed, bool directional)
{
    WarUnitAction* action = createUnitAction(WAR_ACTION_TYPE_WALK);
    action->directional = directional;

    s32 halfIndex = nframes % 2 == 0 ? nframes / 2 : (nframes + 1) / 2;

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

    // This code convert the frames sequence in this WAR_ACTION_STEP_FRAME sequence.
    // (this code is ported from the War1gus project, built with the Stratagus engine)
    //
    // 1 frame  a         => a 0 a 0 a 0 a 0
    // 2 frames a b       => a 0 b 0 a 0 b 0
    // 3 frames a b c     => a b a 0 c b c 0
    // 4 frames a b c d   => a b a 0 d c d 0
    // 5 frames a b c d e => a b c b a e d c d e
    //
    // This code is used with a previous steps calling 'getFrameNumbers'
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
            addActionStep(action, WAR_ACTION_STEP_FRAME, frames[i]);
            addActionStep(action, WAR_ACTION_STEP_MOVE, 4);
            addActionStep(action, WAR_ACTION_STEP_WAIT, walkSpeed);
            actionFrames--;
        }

        for(s32 i = halfIndex - 2; i >= 0; i--)
        {
            addActionStep(action, WAR_ACTION_STEP_FRAME, frames[i]);
            addActionStep(action, WAR_ACTION_STEP_MOVE, 4);
            addActionStep(action, WAR_ACTION_STEP_WAIT, walkSpeed);
            actionFrames--;
        }

        addActionStep(action, WAR_ACTION_STEP_FRAME, 0);
        addActionStep(action, WAR_ACTION_STEP_MOVE, 4);
        addActionStep(action, WAR_ACTION_STEP_WAIT, walkSpeed);
        actionFrames--;

        for(s32 i = 0; i < halfIndex; i++)
        {
            addActionStep(action, WAR_ACTION_STEP_FRAME, frames[nframes - 1 - i]);
            addActionStep(action, WAR_ACTION_STEP_MOVE, 4);
            addActionStep(action, WAR_ACTION_STEP_WAIT, walkSpeed);
            actionFrames--;
        }

        for(s32 i = 1 + nframes - halfIndex; i < nframes; i++)
        {
            addActionStep(action, WAR_ACTION_STEP_FRAME, frames[i]);
            addActionStep(action, WAR_ACTION_STEP_MOVE, 4);
            addActionStep(action, WAR_ACTION_STEP_WAIT, walkSpeed);
            actionFrames--;
        }

        addActionStep(action, WAR_ACTION_STEP_FRAME, 0);
        addActionStep(action, WAR_ACTION_STEP_MOVE, 4);
        addActionStep(action, WAR_ACTION_STEP_WAIT, walkSpeed);
        actionFrames--;
    }

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addActionStep(action, WAR_ACTION_STEP_FRAME, 0);
    addActionStep(action, WAR_ACTION_STEP_WAIT, 1);

    assert(!actionFrames);

    return action;
}

WarUnitAction* buildAttackAction(s32 nframes, s32 frames[], s32 attackSpeed, s32 attackSound, s32 coolOffTime, bool directional)
{
    WarUnitAction* action = createUnitAction(WAR_ACTION_TYPE_ATTACK);
    action->directional = directional;

    s32 halfIndex = nframes%2 == 0 ? nframes/2 : (nframes+1)/2;

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

    for(s32 i = 0; i < nframes; i++)
    {
        addActionStep(action, WAR_ACTION_STEP_FRAME, frames[i]);

        if (i == halfIndex)
        {
            addActionStep(action, WAR_ACTION_STEP_ATTACK, 0);
            addActionStep(action, attackSound, 0);
        }

        addActionStep(action, WAR_ACTION_STEP_WAIT, attackSpeed);
    }

    // make sure we don't attack faster just because we have fewer frames
    addActionStep(action, WAR_ACTION_STEP_WAIT, (5 - nframes) * attackSpeed);
    addActionStep(action, WAR_ACTION_STEP_FRAME, 0);
    addActionStep(action, WAR_ACTION_STEP_WAIT, coolOffTime);

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addActionStep(action, WAR_ACTION_STEP_FRAME, 0);
    addActionStep(action, WAR_ACTION_STEP_WAIT, 1);

    return action;
}

WarUnitAction* buildRepairAction(s32 nframes, s32 frames[], s32 attackSpeed, s32 attackSound, s32 coolOffTime, bool directional)
{
    WarUnitAction* action = createUnitAction(WAR_ACTION_TYPE_REPAIR);
    action->directional = directional;

    s32 halfIndex = nframes%2 == 0 ? nframes/2 : (nframes+1)/2;

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

    for(s32 i = 0; i < nframes; i++)
    {
        addActionStep(action, WAR_ACTION_STEP_FRAME, frames[i]);

        if (i == halfIndex)
        {
            addActionStep(action, WAR_ACTION_STEP_ATTACK, 0);
            addActionStep(action, attackSound, 0);
        }

        addActionStep(action, WAR_ACTION_STEP_WAIT, attackSpeed);
    }

    // make sure we don't attack faster just because we have fewer frames
    addActionStep(action, WAR_ACTION_STEP_WAIT, (5 - nframes) * attackSpeed);
    addActionStep(action, WAR_ACTION_STEP_FRAME, 0);
    addActionStep(action, WAR_ACTION_STEP_WAIT, coolOffTime);

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addActionStep(action, WAR_ACTION_STEP_FRAME, 0);
    addActionStep(action, WAR_ACTION_STEP_WAIT, 1);

    return action;
}

WarUnitAction* buildHarvestAction(s32 nframes, s32 frames[], s32 harvestSpeed, s32 harvestSound, s32 coolOffTime, bool directional)
{
    WarUnitAction* action = createUnitAction(WAR_ACTION_TYPE_HARVEST);
    action->directional = directional;

    s32 halfIndex = nframes%2 == 0 ? nframes/2 : (nframes+1)/2;

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);
    addActionStep(action, WAR_ACTION_STEP_WAIT, 5);

    for(s32 i = 0; i < nframes; i++)
    {
        addActionStep(action, WAR_ACTION_STEP_FRAME, frames[i]);

        if (i == halfIndex)
        {
            addActionStep(action, WAR_ACTION_STEP_ATTACK, 0);
            addActionStep(action, harvestSound, 0);
        }

        addActionStep(action, WAR_ACTION_STEP_WAIT, harvestSpeed);
    }

    // make sure we don't attack faster just because we have fewer frames
    addActionStep(action, WAR_ACTION_STEP_WAIT, (5 - nframes) * harvestSpeed);
    addActionStep(action, WAR_ACTION_STEP_FRAME, 0);
    addActionStep(action, WAR_ACTION_STEP_WAIT, coolOffTime);

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addActionStep(action, WAR_ACTION_STEP_FRAME, 0);
    addActionStep(action, WAR_ACTION_STEP_WAIT, 1);

    return action;
}

WarUnitAction* buildDeathAction(s32 nframes, s32 frames[], s32 waitTime, bool directional, bool doWait101Step)
{
    WarUnitAction* action = createUnitAction(WAR_ACTION_TYPE_DEATH);
    action->directional = directional;

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

    for(s32 i = 0; i < nframes; i++)
    {
        addActionStep(action, WAR_ACTION_STEP_FRAME, frames[i]);
        addActionStep(action, WAR_ACTION_STEP_WAIT, waitTime);
    }

    if (doWait101Step)
        addActionStep(action, WAR_ACTION_STEP_WAIT, 101);

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addActionStep(action, WAR_ACTION_STEP_WAIT, 1);

    return action;
}

WarUnitAction* buildBuildAction(s32 nframes, s32 frames[], s32 waitTime)
{
    WarUnitAction* action = createUnitAction(WAR_ACTION_TYPE_BUILD);
    action->loop = false;

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

    for(s32 i = 0; i < nframes; i++)
    {
        addActionStep(action, WAR_ACTION_STEP_FRAME, frames[i]);
        addActionStep(action, WAR_ACTION_STEP_WAIT, waitTime);
    }

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addActionStep(action, WAR_ACTION_STEP_WAIT, 1);

    return action;
}

WarUnitAction* buildIdleAction(s32 nframes, s32 frames[], s32 waitTime, bool directional)
{
    WarUnitAction* action = createUnitAction(WAR_ACTION_TYPE_IDLE);
    action->directional = directional;

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

    for(s32 i = 0; i < nframes; i++)
    {
        addActionStep(action, WAR_ACTION_STEP_FRAME, frames[i]);
        addActionStep(action, WAR_ACTION_STEP_WAIT, waitTime);
    }

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addActionStep(action, WAR_ACTION_STEP_WAIT, 1);

    return action;
}

WarUnitAction* buildDefaultIdleAction(s32 waitTime, bool directional)
{
    s32 idleFrames[] = {0};
    return buildIdleAction(1, idleFrames, waitTime, directional);
}

void addActions(WarEntity* entity, s32 count, ...)
{
    WarUnitComponent* unit = &entity->unit;

    va_list actions;
    va_start(actions, count);

    for(s32 i = 0; i < count; i++)
    {
        WarUnitAction* action = va_arg(actions, WarUnitAction*);
        WarUnitActionListAdd(&unit->actions, action);
    }

    va_end(actions);
}

void addUnitActions(WarEntity* entity)
{
    WarUnitFrameNumbers frameNumbers_5_5_5_5 = getFrameNumbers(5, arrayArg(s32, 5, 5, 5));
    WarUnitFrameNumbers frameNumbers_5_5_5_4 = getFrameNumbers(5, arrayArg(s32, 5, 5, 4));
    WarUnitFrameNumbers frameNumbers_5_5_5_3 = getFrameNumbers(5, arrayArg(s32, 5, 5, 3));
    WarUnitFrameNumbers frameNumbers_5_5_4_5 = getFrameNumbers(5, arrayArg(s32, 5, 4, 5));
    WarUnitFrameNumbers frameNumbers_5_5_4_4 = getFrameNumbers(5, arrayArg(s32, 5, 4, 4));
    WarUnitFrameNumbers frameNumbers_5_5_4_3 = getFrameNumbers(5, arrayArg(s32, 5, 4, 3));
    WarUnitFrameNumbers frameNumbers_5_5_3_2 = getFrameNumbers(5, arrayArg(s32, 5, 3, 2));
    WarUnitFrameNumbers frameNumbers_5_5_2_3 = getFrameNumbers(5, arrayArg(s32, 5, 2, 3));
    WarUnitFrameNumbers frameNumbers_5_3_5_3 = getFrameNumbers(5, arrayArg(s32, 3, 5, 3));
    WarUnitFrameNumbers frameNumbers_5_2_5_3 = getFrameNumbers(5, arrayArg(s32, 2, 5, 3));

    WarUnitFrameNumbers frameNumbers_5_4_3_3 = getFrameNumbers(5, arrayArg(s32, 4, 3, 3));
    NOT_USED(frameNumbers_5_4_3_3);

    WarUnitFrameNumbers frameNumbers_5_5_5_0 = getFrameNumbers(5, arrayArg(s32, 5, 5, 0));

    s32 walkSpeed = 6;
    s32 attackSpeed = 6;
    s32 coolOffTime = 1;
    s32 waitTime = 3;
    bool directional = true;
    WarUnitActionStepType attackSound = WAR_ACTION_STEP_SOUND_SWORD;

    WarUnitComponent* unit = &entity->unit;

    switch (unit->type)
    {
        // units
        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_GRUNT:
        {
            WarUnitFrameNumbers frames = frameNumbers_5_5_5_3;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);
            WarUnitAction* walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_PEASANT:
        case WAR_UNIT_PEON:
        {
            attackSound = WAR_ACTION_STEP_SOUND_CHOPPING;

            WarUnitFrameNumbers frames = frameNumbers_5_5_5_3;
            WarUnitFrameNumbers framesHarvest = frameNumbers_5_5_4_3;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);
            WarUnitAction* walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);
            WarUnitAction* repairAction = buildRepairAction(framesHarvest.attackFramesCount, framesHarvest.attackFrames, attackSpeed, attackSound, coolOffTime, directional);
            WarUnitAction* harvestAction = buildHarvestAction(framesHarvest.attackFramesCount, framesHarvest.attackFrames, attackSpeed, attackSound, coolOffTime, directional);

            addActions(entity, 6, idleAction, walkAction, attackAction, deathAction, repairAction, harvestAction);
            break;
        }

        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_CATAPULT_ORCS:
        {
            walkSpeed = 10;
            attackSpeed = 25;
            coolOffTime = 49;
            attackSound = WAR_ACTION_STEP_SOUND_CATAPULT;

            WarUnitFrameNumbers frames = frameNumbers_5_2_5_3;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);
            WarUnitAction* walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_RAIDER:
        {
            walkSpeed = 10;
            WarUnitFrameNumbers frames = frameNumbers_5_5_5_5;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);

            WarUnitAction* walkAction = createUnitAction(WAR_ACTION_TYPE_WALK);
            walkAction->directional = directional;

            addActionStep(walkAction, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

            s32 walkFrames[] = {15, 30, 45, 60, 0};
            for(s32 i = 0; i < arrayLength(walkFrames); i++)
            {
                addActionStep(walkAction, WAR_ACTION_STEP_FRAME, walkFrames[i]);
                addActionStep(walkAction, WAR_ACTION_STEP_MOVE, 4);
                addActionStep(walkAction, WAR_ACTION_STEP_WAIT, walkSpeed);
            }

            addActionStep(walkAction, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
            addActionStep(walkAction, WAR_ACTION_STEP_FRAME, 0);
            addActionStep(walkAction, WAR_ACTION_STEP_WAIT, 1);

            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);

            // These are probably when the knight and rider are upgraded?
            //
            // options.speed = 2;
            // buildActions(entity, frameNumbers_5_5_5_5, options);

            // options.speed = 1;
            // buildActions(entity, frameNumbers_5_5_5_5, options);
            break;
        }

        case WAR_UNIT_ARCHER:
        case WAR_UNIT_SPEARMAN:
        {
            attackSpeed = 10;
            attackSound = WAR_ACTION_STEP_SOUND_ARROW;
            WarUnitFrameNumbers frames = frameNumbers_5_5_2_3;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);
            WarUnitAction* walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_CONJURER:
        {
            walkSpeed = 7;
            attackSpeed = 8;
            attackSound = WAR_ACTION_STEP_SOUND_FIREBALL;
            coolOffTime = 0;
            WarUnitFrameNumbers frames = frameNumbers_5_5_4_4;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);
            WarUnitAction* walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            s32 attackFrames[] = {5, 20, 35, 50};
            WarUnitAction* attackAction = buildAttackAction(arrayLength(attackFrames), attackFrames, attackSpeed, attackSound, coolOffTime, directional);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_WARLOCK:
        {
            walkSpeed = 7;
            attackSpeed = 8;
            attackSound = WAR_ACTION_STEP_SOUND_FIREBALL;
            coolOffTime = 1;
            WarUnitFrameNumbers frames = frameNumbers_5_5_5_3;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);
            WarUnitAction* walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            s32 attackFrames[] = {5, 20, 35, 50, 60};
            WarUnitAction* attackAction = buildAttackAction(arrayLength(attackFrames), attackFrames, attackSpeed, attackSound, coolOffTime, directional);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_CLERIC:
        {
            walkSpeed = 7;
            attackSound = WAR_ACTION_STEP_SOUND_FIREBALL;
            WarUnitFrameNumbers frames = frameNumbers_5_5_4_3;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);
            WarUnitAction* walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_NECROLYTE:
        {
            walkSpeed = 7;
            attackSound = WAR_ACTION_STEP_SOUND_FIREBALL;
            WarUnitFrameNumbers frames = frameNumbers_5_5_5_4;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);
            WarUnitAction* walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_MEDIVH:
        {
            attackSound = WAR_ACTION_STEP_SOUND_LIGHTNING;
            WarUnitFrameNumbers frames = frameNumbers_5_5_5_3;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);
            WarUnitAction* walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_LOTHAR:
        {
            WarUnitFrameNumbers frames = frameNumbers_5_5_5_3;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);
            WarUnitAction* walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_GRIZELDA:
        case WAR_UNIT_GARONA:
        {
            waitTime = 4;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);

            s32 walkFrames[] = {10, 20, 30, 35};
            WarUnitAction* walkAction = buildWalkAction(arrayLength(walkFrames), walkFrames, walkSpeed, directional);

            s32 deathFrames[] = {5, 15, 25};
            waitTime = 3;
            WarUnitAction* deathAction = buildDeathAction(arrayLength(deathFrames), deathFrames, waitTime, directional, true);

            addActions(entity, 3, idleAction, walkAction, deathAction);
            break;
        }

        case WAR_UNIT_OGRE:
        {
            attackSound = WAR_ACTION_STEP_SOUND_FIST;
            WarUnitFrameNumbers frames = frameNumbers_5_5_5_5;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);
            WarUnitAction* walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_SPIDER:
        {
            walkSpeed = 4;
            attackSound = WAR_ACTION_STEP_SOUND_FIST;
            WarUnitFrameNumbers frames = frameNumbers_5_5_4_5;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);

            WarUnitAction* walkAction = createUnitAction(WAR_ACTION_TYPE_WALK);
            walkAction->directional = directional;

            addActionStep(walkAction, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

            s32 walkFrames[] = {15, 30, 45, 60, 0};
            for(s32 i = 0; i < arrayLength(walkFrames); i++)
            {
                addActionStep(walkAction, WAR_ACTION_STEP_FRAME, walkFrames[i]);
                addActionStep(walkAction, WAR_ACTION_STEP_MOVE, 4);
                addActionStep(walkAction, WAR_ACTION_STEP_WAIT, walkSpeed);
            }

            addActionStep(walkAction, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
            addActionStep(walkAction, WAR_ACTION_STEP_FRAME, 0);
            addActionStep(walkAction, WAR_ACTION_STEP_WAIT, 1);

            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);

            // Scorpions and Spiders have distinct wait times than other units,
            // so a custom death action is built for those cases
            WarUnitAction* deathAction = createUnitAction(WAR_ACTION_TYPE_DEATH);
            deathAction->directional = true;
            addActionStep(deathAction, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);
            addActionStep(deathAction, WAR_ACTION_STEP_FRAME, frames.deathFrames[0]);
            addActionStep(deathAction, WAR_ACTION_STEP_WAIT, waitTime);
            addActionStep(deathAction, WAR_ACTION_STEP_FRAME, frames.deathFrames[1]);
            addActionStep(deathAction, WAR_ACTION_STEP_WAIT, waitTime);
            addActionStep(deathAction, WAR_ACTION_STEP_FRAME, frames.deathFrames[2]);
            addActionStep(deathAction, WAR_ACTION_STEP_WAIT, waitTime * 100);
            addActionStep(deathAction, WAR_ACTION_STEP_FRAME, frames.deathFrames[3]);
            addActionStep(deathAction, WAR_ACTION_STEP_WAIT, waitTime);
            addActionStep(deathAction, WAR_ACTION_STEP_FRAME, frames.deathFrames[4]);
            addActionStep(deathAction, WAR_ACTION_STEP_WAIT, waitTime);
            addActionStep(deathAction, WAR_ACTION_STEP_WAIT, 101);
            addActionStep(deathAction, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
            addActionStep(deathAction, WAR_ACTION_STEP_WAIT, 1);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_SLIME:
        {
            attackSpeed = 15;
            attackSound = WAR_ACTION_STEP_SOUND_FIST;
            coolOffTime = 15;
            waitTime = 8;
            WarUnitFrameNumbers frames = frameNumbers_5_5_5_3;

            s32 idleFrames[] = {0, 70, 75, 80, 85, 90};
            WarUnitAction* idleAction = buildIdleAction(arrayLength(idleFrames), idleFrames, waitTime, directional);

            WarUnitAction* walkAction = createUnitAction(WAR_ACTION_TYPE_WALK);
            walkAction->directional = directional;

            addActionStep(walkAction, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

            s32 walkFrames[] = {15, 30, 45, 55, 65, 0};
            for(s32 i = 0; i < arrayLength(walkFrames); i++)
            {
                addActionStep(walkAction, WAR_ACTION_STEP_FRAME, walkFrames[i]);
                addActionStep(walkAction, WAR_ACTION_STEP_MOVE, 4);
                addActionStep(walkAction, WAR_ACTION_STEP_WAIT, walkSpeed);
            }

            addActionStep(walkAction, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
            addActionStep(walkAction, WAR_ACTION_STEP_FRAME, 0);
            addActionStep(walkAction, WAR_ACTION_STEP_WAIT, 15);

            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);

            waitTime = 3;
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_FIRE_ELEMENTAL:
        {
            walkSpeed = 5;
            attackSound = WAR_ACTION_STEP_SOUND_FIREBALL;
            coolOffTime = 24;
            waitTime = 8;
            WarUnitFrameNumbers frames = frameNumbers_5_5_5_0;

            s32 idleFrames[] = {5, 15, 25, 35, 50};
            WarUnitAction* idleAction = buildIdleAction(arrayLength(idleFrames), idleFrames, waitTime, directional);

            WarUnitAction* walkAction = createUnitAction(WAR_ACTION_TYPE_WALK);
            walkAction->directional = directional;

            addActionStep(walkAction, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

            s32 walkFrames[] = {10, 20, 30, 40, 50, 0};
            for(s32 i = 0; i < arrayLength(walkFrames); i++)
            {
                addActionStep(walkAction, WAR_ACTION_STEP_FRAME, walkFrames[i]);
                addActionStep(walkAction, WAR_ACTION_STEP_MOVE, 4);
                addActionStep(walkAction, WAR_ACTION_STEP_WAIT, walkSpeed);
            }

            addActionStep(walkAction, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
            addActionStep(walkAction, WAR_ACTION_STEP_FRAME, 0);
            addActionStep(walkAction, WAR_ACTION_STEP_WAIT, 1);

            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);

            addActions(entity, 3, idleAction, walkAction, attackAction);
            break;
        }

        case WAR_UNIT_WATER_ELEMENTAL:
        {
            walkSpeed = 4;
            attackSound = WAR_ACTION_STEP_SOUND_FIREBALL;
            coolOffTime = 24;
            waitTime = 8;
            WarUnitFrameNumbers frames = frameNumbers_5_3_5_3;

            s32 idleFrames[] = {0, 5, 15, 30};
            WarUnitAction* idleAction = buildIdleAction(arrayLength(idleFrames), idleFrames, waitTime, directional);

            WarUnitAction* walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);

            waitTime = 3;
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_SCORPION:
        {
            walkSpeed = 4;
            attackSound = WAR_ACTION_STEP_SOUND_FIST;
            WarUnitFrameNumbers frames = frameNumbers_5_5_5_5;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);

            WarUnitAction* walkAction = createUnitAction(WAR_ACTION_TYPE_WALK);
            walkAction->directional = directional;

            addActionStep(walkAction, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

            s32 walkFrames[] = {15, 30, 45, 60, 0};
            for(s32 i = 0; i < arrayLength(walkFrames); i++)
            {
                addActionStep(walkAction, WAR_ACTION_STEP_FRAME, walkFrames[i]);
                addActionStep(walkAction, WAR_ACTION_STEP_MOVE, 4);
                addActionStep(walkAction, WAR_ACTION_STEP_WAIT, walkSpeed);
            }

            addActionStep(walkAction, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
            addActionStep(walkAction, WAR_ACTION_STEP_FRAME, 0);
            addActionStep(walkAction, WAR_ACTION_STEP_WAIT, 1);

            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);

            // Scorpions and Spiders have distinct wait times than other units,
            // so a custom death action is built for those cases
            WarUnitAction* deathAction = createUnitAction(WAR_ACTION_TYPE_DEATH);
            deathAction->directional = true;
            addActionStep(deathAction, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);
            addActionStep(deathAction, WAR_ACTION_STEP_FRAME, frames.deathFrames[0]);
            addActionStep(deathAction, WAR_ACTION_STEP_WAIT, waitTime);
            addActionStep(deathAction, WAR_ACTION_STEP_FRAME, frames.deathFrames[1]);
            addActionStep(deathAction, WAR_ACTION_STEP_WAIT, waitTime);
            addActionStep(deathAction, WAR_ACTION_STEP_FRAME, frames.deathFrames[2]);
            addActionStep(deathAction, WAR_ACTION_STEP_WAIT, waitTime * 100);
            addActionStep(deathAction, WAR_ACTION_STEP_FRAME, frames.deathFrames[3]);
            addActionStep(deathAction, WAR_ACTION_STEP_WAIT, waitTime);
            addActionStep(deathAction, WAR_ACTION_STEP_FRAME, frames.deathFrames[4]);
            addActionStep(deathAction, WAR_ACTION_STEP_WAIT, waitTime);
            addActionStep(deathAction, WAR_ACTION_STEP_WAIT, 101);
            addActionStep(deathAction, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
            addActionStep(deathAction, WAR_ACTION_STEP_WAIT, 1);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_BRIGAND:
        {
            walkSpeed = 4;
            WarUnitFrameNumbers frames = frameNumbers_5_5_3_2;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);
            WarUnitAction* walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_SKELETON:
        {
            walkSpeed = 4;
            WarUnitFrameNumbers frames = frameNumbers_5_5_5_5;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);
            WarUnitAction* walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_DAEMON:
        {
            coolOffTime = 24;
            WarUnitFrameNumbers frames = frameNumbers_5_5_5_5;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);
            WarUnitAction* walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_THE_DEAD:
        {
            WarUnitFrameNumbers frames = frameNumbers_5_5_5_5;

            WarUnitAction* idleAction = buildDefaultIdleAction(waitTime, directional);
            WarUnitAction* walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, walkSpeed, directional);
            WarUnitAction* attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, attackSpeed, attackSound, coolOffTime, directional);
            WarUnitAction* deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames, waitTime, directional, true);

            addActions(entity, 4, idleAction, walkAction, attackAction, deathAction);
            break;
        }

        case WAR_UNIT_WOUNDED:
        {
            waitTime = 5;

            s32 idleFrames[] = {0};
            WarUnitAction* idleAction = buildIdleAction(arrayLength(idleFrames), idleFrames, waitTime, directional);

            addActions(entity, 1, idleAction);
            break;
        }

        // buildings
        case WAR_UNIT_FARM_HUMANS:
        case WAR_UNIT_FARM_ORCS:
        case WAR_UNIT_BARRACKS_HUMANS:
        case WAR_UNIT_BARRACKS_ORCS:
        case WAR_UNIT_CHURCH:
        case WAR_UNIT_TEMPLE:
        case WAR_UNIT_TOWER_HUMANS:
        case WAR_UNIT_TOWER_ORCS:
        case WAR_UNIT_TOWNHALL_HUMANS:
        case WAR_UNIT_TOWNHALL_ORCS:
        case WAR_UNIT_LUMBERMILL_HUMANS:
        case WAR_UNIT_LUMBERMILL_ORCS:
        case WAR_UNIT_STABLE:
        case WAR_UNIT_KENNEL:
        case WAR_UNIT_BLACKSMITH_HUMANS:
        case WAR_UNIT_BLACKSMITH_ORCS:
        {
            s32 idleFrames[] = {0};
            WarUnitAction* idleAction = buildIdleAction(arrayLength(idleFrames), idleFrames, 5, false);

            s32 buildFrames[] = {0, 1, 2};
            WarUnitAction* buildAction = buildBuildAction(arrayLength(buildFrames), buildFrames, 50);

            addActions(entity, 2, idleAction, buildAction);
            break;
        }

        case WAR_UNIT_STORMWIND:
        case WAR_UNIT_BLACKROCK:
        case WAR_UNIT_GOLDMINE:
        {
            s32 idleFrames[] = {0};
            WarUnitAction* idleAction = buildIdleAction(arrayLength(idleFrames), idleFrames, 5, false);

            addActions(entity, 1, idleAction);
            break;
        }

        // others
        case WAR_UNIT_HUMAN_CORPSE:
        {
            s32 deathFrames[] = {0, 10, 15, 20};
            WarUnitAction* deathAction = buildDeathAction(arrayLength(deathFrames), deathFrames, 300, true, false);

            addActions(entity, 1, deathAction);
            break;
        }

        case WAR_UNIT_ORC_CORPSE:
        {
            s32 deathFrames[] = {5, 10, 15, 20};
            WarUnitAction* deathAction = buildDeathAction(arrayLength(deathFrames), deathFrames, 300, true, false);

            addActions(entity, 1, deathAction);
            break;
        }

        default:
        {
            logError("Type doesn't have any defined actions: %d\n", unit->type);
            break;
        }
    }
}

s32 findActionIndex(WarEntity* entity, WarUnitActionType type)
{
    WarUnitComponent* unit = &entity->unit;
    if (unit)
    {
        for(s32 i = 0; i < unit->actions.count; i++)
        {
            WarUnitAction* action = unit->actions.items[i];
            if (action->type == type)
            {
                return i;
            }
        }
    }

    return -1;
}

s32 getActionDuration(WarEntity* entity, WarUnitActionType type)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    if (type == WAR_ACTION_TYPE_NONE)
    {
        return 0;
    }

    s32 actionIndex = findActionIndex(entity, type);
    if (actionIndex < 0)
    {
        logError("Entity of type %d doesn't have a %d action\n", entity->type, type);
        return 0;
    }

    s32 duration = 0;

    WarUnitAction* action = unit->actions.items[actionIndex];
    for (s32 i = 0; i < action->steps.count; i++)
    {
        WarUnitActionStep step = action->steps.items[i];
        if (step.type == WAR_ACTION_STEP_WAIT)
            duration += step.param;
    }

    return duration;
}

void resetAction(WarUnitAction* action)
{
    action->stepIndex = 0;
    action->status = WAR_ACTION_NOT_STARTED;
}

void setAction(WarContext* context, WarEntity* entity, WarUnitActionType type, bool reset, f32 scale)
{
    assert(isUnit(entity));

    WarUnitComponent* unit = &entity->unit;

    if (type == WAR_ACTION_TYPE_NONE)
    {
        unit->actionIndex = -1;
        return;
    }

    unit->actionIndex = findActionIndex(entity, type);

    if (unit->actionIndex < 0)
    {
        logError("Entity of type %d doesn't have a %d action\n", entity->type, type);
        return;
    }

    if (scale >= 0)
    {
        unit->actions.items[unit->actionIndex]->scale = scale;
    }

    if (reset)
    {
        resetAction(unit->actions.items[unit->actionIndex]);
    }
}

void updateAction(WarContext* context, WarEntity* entity)
{
    WarTransformComponent* transform = &entity->transform;
    WarSpriteComponent* sprite = &entity->sprite;
    WarUnitComponent* unit = &entity->unit;

    if (!unit || unit->actions.count == 0 || unit->actionIndex < 0)
    {
        return;
    }

    WarUnitAction* action = unit->actions.items[unit->actionIndex];
    if (!action || action->status == WAR_ACTION_FINISHED)
    {
        return;
    }

    if (action->stepIndex < 0)
    {
        resetAction(action);
    }

    action->status = WAR_ACTION_RUNNING;

    WarUnitActionStep step = action->steps.items[action->stepIndex];
    if (step.type == WAR_ACTION_STEP_WAIT)
    {
        action->waitCount -= getMapScaledSpeed(context, context->deltaTime);
        if (action->waitCount > 0)
        {
            return;
        }

        action->waitCount = 0;

        action->stepIndex++;
        if (action->stepIndex >= action->steps.count)
        {
            if (!action->loop)
            {
                action->status = WAR_ACTION_FINISHED;
                return;
            }

            action->stepIndex = 0;
        }

        step = action->steps.items[action->stepIndex];
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

                if (action->directional)
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

                    transform->scale.x = inRange(unit->direction, WAR_DIRECTION_SOUTH_WEST, WAR_DIRECTION_COUNT) ? -1 : 1;
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
        if (action->stepIndex >= action->steps.count)
        {
            if (!action->loop)
            {
                action->status = WAR_ACTION_FINISHED;
                return;
            }

            action->stepIndex = 0;
        }

        step = action->steps.items[action->stepIndex];
    }

    action->waitCount = __frameCountToSeconds(step.param) * action->scale;
}

// peasant: 40s
// farm: 30s


// farm 1000 => 80s
// peasant 750  => 60s
// footman 600  => 48s