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
} WarUnitFrameNumbers;

typedef struct
{
    s32 speed;
    s32 attackSpeed;
    s32 coolOffTime;
    s32 attackSound;
    WarUnitAction* walkAction;
    WarUnitAction* attackAction;
    WarUnitAction* deathAction;
    WarUnitAction* harvestAction;
    WarUnitAction* repairAction;
    WarUnitAction* idleAction;
} WarUnitActionOptions;

#define DEFAULT_WAR_ACTION_OPTIONS (WarUnitActionOptions){0}

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
    action->currentIndex = 0;
    WarUnitActionStepListInit(&action->steps);
    return action;
}

void addActionStep(WarUnitAction* action, WarUnitActionStepType type, s32 param)
{
    WarUnitActionStepListAdd(&action->steps, (WarUnitActionStep){type, param});
}

WarUnitAction* buildWalkAction(s32 nframes, s32 frames[], s32 waitTime)
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

WarUnitAction* buildAttackAction(s32 nframes, s32* frames, s32 waitTime, s32 coolOffTime, WarUnitActionStepType sound)
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

WarUnitAction* buildHarvestAction(s32 nframes, s32* frames, s32 waitTime, WarUnitActionStepType sound)
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

WarUnitAction* buildDeathAction(s32 nframes, s32* frames)
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

WarUnitAction* buildIdleAction(s32 nframes, s32* frames, s32 waitTime)
{
    WarUnitAction* action = createUnitAction(WAR_ACTION_TYPE_DEATH);
    
    for(s32 i = 0; i < nframes; i++)
    {
        addActionStep(action, WAR_ACTION_STEP_FRAME, frames[i]);
        addActionStep(action, WAR_ACTION_STEP_WAIT, waitTime);
    }
    
    return action;
}

void buildActions(WarEntity* entity, WarUnitFrameNumbers frames, WarUnitActionOptions options)
{
    WarUnitComponent* unit = &entity->unit;

    if (!options.speed)
        options.speed = 6;

    if (!options.attackSpeed)
        options.attackSpeed = 6;

    if (!options.coolOffTime)
        options.coolOffTime = 1;

    if (!options.attackSound)
        options.attackSound = WAR_ACTION_STEP_SOUND_SWORD;

    if (!options.walkAction)
        options.walkAction = buildWalkAction(frames.walkFramesCount, frames.walkFrames, options.speed);

    if (!options.attackAction)
        options.attackAction = buildAttackAction(frames.attackFramesCount, frames.attackFrames, options.attackSpeed, options.coolOffTime, options.attackSound);

    if (!options.deathAction)
        options.deathAction = buildDeathAction(frames.deathFramesCount, frames.deathFrames);

    if (!options.idleAction)
    {
        s32 idleFrames[] = {0};
        options.idleAction = buildIdleAction(1, idleFrames, 4);
    }

    WarUnitActionListAdd(&unit->actions, options.walkAction);
    WarUnitActionListAdd(&unit->actions, options.attackAction);
    WarUnitActionListAdd(&unit->actions, options.deathAction);
    WarUnitActionListAdd(&unit->actions, options.idleAction);

    if (options.harvestAction)
        WarUnitActionListAdd(&unit->actions, options.harvestAction);

    if (options.repairAction)
        WarUnitActionListAdd(&unit->actions, options.repairAction);
}

void buildUnitActions(WarEntity* entity)
{
    local s32 arr_5_5_5[] = {5, 5, 5};
    local s32 arr_5_5_4[] = {5, 5, 4};
    local s32 arr_5_5_3[] = {5, 5, 3};
    local s32 arr_5_4_5[] = {5, 4, 5};
    local s32 arr_5_4_4[] = {5, 4, 4};
    local s32 arr_5_4_3[] = {5, 4, 3};
    local s32 arr_5_3_2[] = {5, 3, 2};
    local s32 arr_5_2_3[] = {5, 2, 3};
    local s32 arr_3_5_3[] = {3, 5, 3};
    local s32 arr_2_5_3[] = {2, 5, 3};
    local s32 arr_4_3_3[] = {4, 3, 3};
    local s32 arr_3_5_0[] = {3, 5, 0};

    WarUnitFrameNumbers frameNumbers_5_5_5_5 = getFrameNumbers(5, arr_5_5_5);
    WarUnitFrameNumbers frameNumbers_5_5_5_4 = getFrameNumbers(5, arr_5_5_4);
    WarUnitFrameNumbers frameNumbers_5_5_5_3 = getFrameNumbers(5, arr_5_5_3);
    WarUnitFrameNumbers frameNumbers_5_5_4_5 = getFrameNumbers(5, arr_5_4_5);
    WarUnitFrameNumbers frameNumbers_5_5_4_4 = getFrameNumbers(5, arr_5_4_4);
    WarUnitFrameNumbers frameNumbers_5_5_4_3 = getFrameNumbers(5, arr_5_4_3);
    WarUnitFrameNumbers frameNumbers_5_5_3_2 = getFrameNumbers(5, arr_5_3_2);
    WarUnitFrameNumbers frameNumbers_5_5_2_3 = getFrameNumbers(5, arr_5_2_3);
    WarUnitFrameNumbers frameNumbers_5_3_5_3 = getFrameNumbers(5, arr_3_5_3);
    WarUnitFrameNumbers frameNumbers_5_2_5_3 = getFrameNumbers(5, arr_2_5_3);
    WarUnitFrameNumbers frameNumbers_5_4_3_3 = getFrameNumbers(5, arr_4_3_3);
    WarUnitFrameNumbers frameNumbers_5_3_5_0 = getFrameNumbers(5, arr_3_5_0);

    WarUnitComponent* unit = &entity->unit;
    
    switch (unit->type)
    {
        // units
        case WAR_UNIT_FOOTMAN:
        case WAR_UNIT_GRUNT:
        {
            buildActions(entity, frameNumbers_5_5_5_3, DEFAULT_WAR_ACTION_OPTIONS);
            break;
        }

        case WAR_UNIT_PEASANT:
        case WAR_UNIT_PEON:
        {
            WarUnitActionOptions options = DEFAULT_WAR_ACTION_OPTIONS;
            options.attackSound = WAR_ACTION_STEP_SOUND_CHOPPING;
            options.harvestAction = buildHarvestAction(frameNumbers_5_5_4_3.attackFrames, frameNumbers_5_5_4_3.attackFrames, 5, WAR_ACTION_STEP_SOUND_CHOPPING);
            options.repairAction = buildAttackAction(frameNumbers_5_5_5_3.attackFrames, frameNumbers_5_5_5_3.attackFrames, 6, 5, WAR_ACTION_STEP_SOUND_CHOPPING);
            buildActions(entity, frameNumbers_5_5_5_3, options);
            break;
        }

        case WAR_UNIT_CATAPULT_HUMANS:
        case WAR_UNIT_CATAPULT_ORCS:
        {
            WarUnitActionOptions options = DEFAULT_WAR_ACTION_OPTIONS;
            options.speed = 10;
            options.attackSpeed = 25;
            options.coolOffTime= 49;
            options.attackSound = WAR_ACTION_STEP_SOUND_CATAPULT;
            buildActions(entity, frameNumbers_5_2_5_3, options);
            break;
        }

        case WAR_UNIT_KNIGHT:
        case WAR_UNIT_RAIDER:
        {
            WarUnitActionOptions options = DEFAULT_WAR_ACTION_OPTIONS;
            options.speed = 3;
            buildActions(entity, frameNumbers_5_5_5_5, options);

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
            WarUnitActionOptions options = DEFAULT_WAR_ACTION_OPTIONS;
            options.attackSpeed = 10;
            options.attackSound = WAR_ACTION_STEP_SOUND_ARROW;
            buildActions(entity, frameNumbers_5_5_2_3, options);
            break;
        }

        case WAR_UNIT_CONJURER:
        {
            WarUnitActionOptions options = DEFAULT_WAR_ACTION_OPTIONS;
            options.speed = 7;

            s32 attackFrames[] = {5, 20, 35, 50};
            options.attackAction = buildAttackAction(4, attackFrames, 8, 0, WAR_ACTION_STEP_SOUND_FIREBALL);
            buildActions(entity, frameNumbers_5_5_4_4, options);
            break;
        }

        case WAR_UNIT_WARLOCK:
        {
            WarUnitActionOptions options = DEFAULT_WAR_ACTION_OPTIONS;
            options.speed = 7;

            s32 attackFrames[] = {5, 20, 35, 50, 60};
            options.attackAction = buildAttackAction(5, attackFrames, 8, 1, WAR_ACTION_STEP_SOUND_FIREBALL);
            buildActions(entity, frameNumbers_5_5_5_3, options);
            break;
        }

        case WAR_UNIT_CLERIC:
        {
            WarUnitActionOptions options = DEFAULT_WAR_ACTION_OPTIONS;
            options.speed = 7;
            options.attackSound = WAR_ACTION_STEP_SOUND_FIREBALL;
            buildActions(entity, frameNumbers_5_5_4_3, options);
            break;
        }

        case WAR_UNIT_NECROLYTE:
        {
            WarUnitActionOptions options = DEFAULT_WAR_ACTION_OPTIONS;
            options.speed = 7;
            options.attackSound = WAR_ACTION_STEP_SOUND_FIREBALL;
            buildActions(entity, frameNumbers_5_5_5_4, options);
            break;
        }

        case WAR_UNIT_MEDIVH:
        {
            WarUnitActionOptions options = DEFAULT_WAR_ACTION_OPTIONS;
            options.attackSound = WAR_ACTION_STEP_SOUND_LIGHTNING;
            buildActions(entity, frameNumbers_5_5_5_3, options);
            break;
        }

        case WAR_UNIT_LOTHAR:
        {
            buildActions(entity, frameNumbers_5_5_5_3, DEFAULT_WAR_ACTION_OPTIONS);
            break;
        }

        case WAR_UNIT_GRIZELDA:
        case WAR_UNIT_GARONA:
        {
            s32 walkFrames[] = {10, 20, 30, 35};
            WarUnitAction* walkAction = buildWalkAction(4, walkFrames, 6);

            s32 deathFrames[] = {5, 15, 25};
            WarUnitAction* deathAction = buildDeathAction(3, deathFrames, 6, 1);

            s32 idleFrames[] = {0};
            WarUnitAction* idleAction = buildIdleAction(1, idleFrames, 4);

            WarUnitActionListAdd(&unit->actions, walkAction);
            WarUnitActionListAdd(&unit->actions, deathAction);
            WarUnitActionListAdd(&unit->actions, idleAction);
            break;
        }

        case WAR_UNIT_OGRE:
        {
            WarUnitActionOptions options = DEFAULT_WAR_ACTION_OPTIONS;
            options.attackSound = WAR_ACTION_STEP_SOUND_FIST;
            buildActions(entity, frameNumbers_5_5_5_5, options);
            break;
        }

        case WAR_UNIT_SPIDER:
        {
            WarUnitActionOptions options = DEFAULT_WAR_ACTION_OPTIONS;
            options.speed = 4;
            options.attackSound = WAR_ACTION_STEP_SOUND_FIST;
            buildActions(entity, frameNumbers_5_5_4_5, options);
            break;
        }

        case WAR_UNIT_SLIME:
        {
            WarUnitActionOptions options = DEFAULT_WAR_ACTION_OPTIONS;
            options.attackSpeed = 15;
            options.attackSound = WAR_ACTION_STEP_SOUND_FIST;
            options.coolOffTime = 15;

            s32 idleFrames[] = {0, 65, 70, 75, 80, 85, 90};
            options.idleAction = buildIdleAction(7, idleFrames, 8);
            buildActions(entity, frameNumbers_5_5_5_3, options);
            break;
        }

        case WAR_UNIT_FIREELEMENTAL:
        {
            WarUnitActionOptions options = DEFAULT_WAR_ACTION_OPTIONS;
            options.speed = 5;
            options.attackSound = WAR_ACTION_STEP_SOUND_FIREBALL;
            options.coolOffTime = 24;

            s32 idleFrames[] = {5, 15, 25, 35, 50};
            options.idleAction = buildIdleAction(4, idleFrames, 8);
            buildActions(entity, frameNumbers_5_3_5_0, options);
            break;
        }

        case WAR_UNIT_WATERELEMENTAL:
        {
            WarUnitActionOptions options = DEFAULT_WAR_ACTION_OPTIONS;
            options.speed = 4;
            options.attackSound = WAR_ACTION_STEP_SOUND_FIREBALL;
            options.coolOffTime = 24;

            s32 idleFrames[] = {1, 5, 15, 30};
            options.idleAction = buildIdleAction(4, idleFrames, 8);
            buildActions(entity, frameNumbers_5_3_5_3, options);
            break;
        }

        case WAR_UNIT_SCORPION:
        {
            WarUnitActionOptions options = DEFAULT_WAR_ACTION_OPTIONS;
            options.speed = 4;
            options.attackSound = WAR_ACTION_STEP_SOUND_FIST;
            buildActions(entity, frameNumbers_5_5_5_5, options);
            break;
        }

        case WAR_UNIT_BRIGAND:
        {
            buildActions(entity, frameNumbers_5_5_3_2, DEFAULT_WAR_ACTION_OPTIONS);
            break;
        }

        case WAR_UNIT_SKELETON:
        {
            buildActions(entity, frameNumbers_5_5_5_5, DEFAULT_WAR_ACTION_OPTIONS);
            break;
        }

        case WAR_UNIT_DAEMON:
        {
            WarUnitActionOptions options = DEFAULT_WAR_ACTION_OPTIONS;
            options.coolOffTime = 24;
            buildActions(entity, frameNumbers_5_5_5_5, options);
            break;
        }

        case WAR_UNIT_WOUNDED:
        {
            break;
        }

        case WAR_UNIT_20:
        {
            break;
        }

        case WAR_UNIT_26:
        {
            break;
        }

        case WAR_UNIT_DRAGON_CYCLOPS_GIANT:
        {
            break;
        }

        case WAR_UNIT_30:
        {
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
        case WAR_UNIT_STABLES:
        case WAR_UNIT_KENNEL:
        case WAR_UNIT_BLACKSMITH_HUMANS:
        case WAR_UNIT_BLACKSMITH_ORCS:
        case WAR_UNIT_STORMWIND:
        case WAR_UNIT_BLACKROCK:
        {
            break;
        }

        // neutral
        case WAR_UNIT_GOLDMINE:
        {
            break;
        }

        case WAR_UNIT_51: break;
        case WAR_UNIT_PEASANT_WITH_WOOD: break;
        case WAR_UNIT_PEON_WITH_WOOD: break;
        case WAR_UNIT_PEASANT_WITH_GOLD: break;
        case WAR_UNIT_PEON_WITH_GOLD: break;

        // others
        case WAR_UNIT_HUMAN_CORPSE: break;
        case WAR_UNIT_ORC_CORPSE: break;

        default:
        {
            break;
        }
    }
}