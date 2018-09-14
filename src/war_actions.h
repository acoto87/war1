#define MAX_ACTIONS 6
#define MAX_ACTION_STEPS 32

typedef struct
{
    s32 walkFramesCount;
    s32 walkFrames[5];

    s32 attackFramesCount;
    s32 attackFrames[5];

    s32 deathFramesCount;
    s32 deathFrames[5];
} FrameNumbers;

/*
 * 
*/
FrameNumbers getFrameNumbers(s32 nbdir, s32 initCounter[])
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

    FrameNumbers frameNumbers = (FrameNumbers){0};

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
    WarUnitAction* action = malloc(sizeof(WarUnitAction));
    action->type = type;
    action->currentIndex = 0;
    WarUnitActionStepListInit(&action->steps);
    return action;
}

void addActionStep(WarUnitAction* action, WarUnitActionStepType type, s32 param)
{
    WarUnitActionStepListAdd(&action->steps, (WarUnitActionStep){type, param});
}

WarUnitAction* buildWalkAnimation(s32 nframes, s32 frames[], s32 waitTime)
{
    WarUnitAction* action = createUnitAction(WAR_ACTION_TYPE_ATTACK);

    s32 halfIndex = nframes % 2 == 0 ? nframes / 2 : (nframes + 1) / 2;
    s32 halfIndex2 = halfIndex - 1;

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);

    // This code convert the frames sequence in this WAR_ACTION_STEP_FRAME sequence.
    // (this code is extracted from the War1gus project, built with the Stratagus engine)
    // 
    // 1 frame  a        => a 0 a 0 a 0 a 0
    // 2 frames a b      => a 0 b 0 a 0 b 0
    // 3 frames a b c    => a b a 0 c b c 0
    // 4 frames a b c d  => a b a 0 d c d 0
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
            addActionStep(action, WAR_ACTION_STEP_WAIT, waitTime);
            actionFrames--;
        }
        
        for(s32 i = halfIndex - 2; i >= 0; i--)
        {
            addActionStep(action, WAR_ACTION_STEP_FRAME, frames[i]);
            addActionStep(action, WAR_ACTION_STEP_MOVE, 4);
            addActionStep(action, WAR_ACTION_STEP_WAIT, waitTime);
            actionFrames--;
        }
        
        addActionStep(action, WAR_ACTION_STEP_FRAME, 0);
        addActionStep(action, WAR_ACTION_STEP_MOVE, 4);
        addActionStep(action, WAR_ACTION_STEP_WAIT, waitTime);
        actionFrames--;
        
        for(s32 i = 0; i < halfIndex; i++)
        {
            addActionStep(action, WAR_ACTION_STEP_FRAME, frames[nframes - 1 - i]);
            addActionStep(action, WAR_ACTION_STEP_MOVE, 4);
            addActionStep(action, WAR_ACTION_STEP_WAIT, waitTime);
            actionFrames--;
        }
        
        for(s32 i = 1 + nframes - halfIndex; i < nframes; i++)
        {
            addActionStep(action, WAR_ACTION_STEP_FRAME, frames[i]);
            addActionStep(action, WAR_ACTION_STEP_MOVE, 4);
            addActionStep(action, WAR_ACTION_STEP_WAIT, waitTime);
            actionFrames--;
        }
        
        addActionStep(action, WAR_ACTION_STEP_FRAME, 0);
        addActionStep(action, WAR_ACTION_STEP_MOVE, 4);
        addActionStep(action, WAR_ACTION_STEP_WAIT, waitTime);
        actionFrames--;
    }


    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addActionStep(action, WAR_ACTION_STEP_FRAME, 0);
    addActionStep(action, WAR_ACTION_STEP_WAIT, 1);

    assert(!actionFrames);

    return action;
}

WarUnitAction* buildAttackAnimation(s32 nframes, s32* frames, s32 waitTime, s32 coolOffTime, WarUnitActionStepType sound)
{
    WarUnitAction* action = createUnitAction(WAR_ACTION_TYPE_ATTACK);

    s32 halfIndex = nframes%2 == 0 ? nframes/2 : (nframes+1)/2;

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);
    
    for(s32 i = 0; i < nframes; i++)
    {
        addActionStep(action, WAR_ACTION_STEP_FRAME, frames[i]);

        if (i == halfIndex)
        {
            addActionStep(action, WAR_ACTION_STEP_ATTACK, 0);
            addActionStep(action, WAR_ACTION_STEP_SOUND_SWORD, 0);
        }

        addActionStep(action, WAR_ACTION_STEP_WAIT, waitTime);
    }

    // make sure we don't attack faster just because we have fewer frames
    addActionStep(action, WAR_ACTION_STEP_WAIT, (5 - nframes) * waitTime);
    addActionStep(action, WAR_ACTION_STEP_FRAME, 0);
    addActionStep(action, WAR_ACTION_STEP_WAIT, coolOffTime);

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addActionStep(action, WAR_ACTION_STEP_FRAME, 0);
    addActionStep(action, WAR_ACTION_STEP_WAIT, 1);

    return action;
}

WarUnitAction* buildHarvestAnimation(s32 nframes, s32* frames, s32 waitTime, WarUnitActionStepType sound)
{
    WarUnitAction* action = createUnitAction(WAR_ACTION_TYPE_HARVEST);

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);
    addActionStep(action, WAR_ACTION_STEP_SOUND_CHOPPING, 0);
    addActionStep(action, WAR_ACTION_STEP_WAIT, 5);
    
    for(s32 i = 0; i < nframes; i++)
    {
        addActionStep(action, WAR_ACTION_STEP_FRAME, frames[i]);

        if (i == nframes/2)
            addActionStep(action, sound, 0);

        addActionStep(action, WAR_ACTION_STEP_WAIT, waitTime);
    }

    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addActionStep(action, WAR_ACTION_STEP_FRAME, 0);
    addActionStep(action, WAR_ACTION_STEP_WAIT, 1);

    return action;
}

WarUnitAction* buildDeathAnimation(s32 nframes, s32* frames)
{   
    WarUnitAction* action = createUnitAction(WAR_ACTION_TYPE_DEATH);
    
    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_BEGIN);
    
    for(s32 i = 0; i < nframes; i++)
    {
        addActionStep(action, WAR_ACTION_STEP_FRAME, frames[i]);
        addActionStep(action, WAR_ACTION_STEP_WAIT, 3);
    }

    addActionStep(action, WAR_ACTION_STEP_WAIT, 101);
    addActionStep(action, WAR_ACTION_STEP_UNBREAKABLE, WAR_UNBREAKABLE_END);
    addActionStep(action, WAR_ACTION_STEP_WAIT, 1);

    return action;
}

WarUnitAction* buildIdleAnimation()
{
    WarUnitAction* action = createUnitAction(WAR_ACTION_TYPE_DEATH);
    addActionStep(action, WAR_ACTION_STEP_FRAME, 0);
    addActionStep(action, WAR_ACTION_STEP_WAIT, 4);
    return action;
}
