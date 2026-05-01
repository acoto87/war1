# War1-C Commands System Documentation

## Overview

The Commands System is the **interface between player input and game state changes**. It translates player actions (clicking on units, right-clicking the map, pressing ability hotkeys) into high-level commands that affect units, buildings, and gameplay progression.

Commands are **asynchronous and batched**: the player issues commands through the UI, commands are queued, and they're executed when ready (respecting cooldowns, resource costs, and prerequisites). This design enables:

- **Multi-unit operations** - "Select 5 footmen, move to point X" applies to all selected
- **Deferred execution** - Commands respect prerequisites (enough resources, mana, valid targets)
- **Sound feedback** - Acknowledgement sounds and error handling
- **Formation preservation** - Moving multiple units maintains their formation
- **State transitions** - Commands drive the FSM to appropriate unit behaviors

Examples:
- **Movement**: Player clicks terrain → all selected units move there in formation
- **Training**: Player clicks "Train Footman" → barracks queues unit if resources permit
- **Casting**: Player clicks "Rain of Fire" + target tile → conjurer casts spell
- **Building**: Player clicks "Build Farm" + target tile → peasant walks there and constructs

---

## Architecture

### Two-Tier Command Processing

The system has two processing levels:

1. **Command Registration** (`wcmd_train*`, `wcmd_cast*`, etc.)
   - Called by UI buttons/hotkeys
   - Stores command type in `map->command`
   - Lightweight, no validation yet

2. **Command Execution** (`wcmd_executeCommand`)
   - Called from game loop
   - Validates command feasibility
   - Applies state changes
   - Affects all selected units
   - Plays audio feedback

### Command Flow

```
User Input (UI button click)
  ↓
wcmd_trainFootman(context, entity)  [or similar wcmd_* function]
  ↓
map->command.type = WAR_COMMAND_TRAIN_FOOTMAN
  ↓
[Next game frame]
  ↓
wcmd_executeCommand(context)  [main game loop]
  ↓
Validate command (check resources, mana, unit state, etc.)
  ↓
For each selected unit:
  - Create appropriate FSM state
  - Queue or execute the action
  - Play acknowledgement sound
```

---

## Key Types

### `WarUnitCommandType` - Command Categories

```c
typedef enum _WarUnitCommandType
{
    // Basic unit actions
    WAR_COMMAND_NONE,
    WAR_COMMAND_MOVE,        // Move to position
    WAR_COMMAND_STOP,        // Stop current action
    WAR_COMMAND_HARVEST,     // Gather resources (gold/wood)
    WAR_COMMAND_DELIVER,     // Deliver resources to town hall
    WAR_COMMAND_REPAIR,      // Repair building
    WAR_COMMAND_ATTACK,      // Attack unit/building

    // Unit training
    WAR_COMMAND_TRAIN_FOOTMAN,
    WAR_COMMAND_TRAIN_GRUNT,
    WAR_COMMAND_TRAIN_PEASANT,
    WAR_COMMAND_TRAIN_PEON,
    WAR_COMMAND_TRAIN_CATAPULT_HUMANS,
    WAR_COMMAND_TRAIN_CATAPULT_ORCS,
    // ... 8 more unit types ...

    // Spell casting
    WAR_COMMAND_SPELL_HEALING,
    WAR_COMMAND_SPELL_FAR_SIGHT,
    WAR_COMMAND_SPELL_INVISIBILITY,
    WAR_COMMAND_SPELL_RAIN_OF_FIRE,
    WAR_COMMAND_SPELL_POISON_CLOUD,
    WAR_COMMAND_SPELL_RAISE_DEAD,
    WAR_COMMAND_SPELL_DARK_VISION,
    WAR_COMMAND_SPELL_UNHOLY_ARMOR,

    // Summoning
    WAR_COMMAND_SUMMON_SPIDER,
    WAR_COMMAND_SUMMON_SCORPION,
    WAR_COMMAND_SUMMON_DAEMON,
    WAR_COMMAND_SUMMON_WATER_ELEMENTAL,

    // Building construction
    WAR_COMMAND_BUILD_BASIC,
    WAR_COMMAND_BUILD_ADVANCED,
    WAR_COMMAND_BUILD_FARM_HUMANS,
    WAR_COMMAND_BUILD_FARM_ORCS,
    // ... 16 more building types ...
    WAR_COMMAND_BUILD_ROAD,
    WAR_COMMAND_BUILD_WALL,

    // Unit/spell upgrades
    WAR_COMMAND_UPGRADE_SWORDS,
    WAR_COMMAND_UPGRADE_AXES,
    // ... 18 more upgrades ...

    // Special
    WAR_COMMAND_CANCEL,

    WAR_COMMAND_COUNT
} WarUnitCommandType;
```

### `WarUnitCommand` - Command Payload

```c
struct _WarUnitCommand
{
    WarUnitCommandType type;  // What to do

    union                     // Type-specific parameters
    {
        struct
        {
            WarUnitType unitToTrain;   // Which unit to create
            WarUnitType buildingUnit;  // Where to create it (building type)
        } train;

        struct
        {
            WarUpgradeType upgradeToBuild;  // Which upgrade to research
            WarUnitType buildingUnit;       // Which building researches it
        } upgrade;

        struct
        {
            WarUnitType buildingToBuild;    // Which building to construct
        } build;
    };
};
```

---

## API Reference

### High-Level Command Functions

These are called by the UI when the player clicks a button or presses a hotkey. They simply store the command type (and metadata) for later execution.

#### Unit Movement

```c
void move(WarContext* context, WarEntity* entity)
```
**Purpose:** Queue a move command to a specific position.

**How it's used:**
```c
// Player right-clicks the map
wcmd_executeMoveCommand(context, clickedPosition);
```

**What happens:**
- All selected units move to the position
- Formation is preserved (relative positions maintained)
- With SHIFT held, adds waypoint to current move queue
- Plays acknowledgement sound

---

#### Unit Actions

```c
void wcmd_stop(WarContext* context, WarEntity* entity)
```
**Purpose:** Stop all current actions.

```c
void wcmd_harvest(WarContext* context, WarEntity* entity)
```
**Purpose:** Harvest resources (gold/wood).

**What happens:**
- Workers go to resource location
- If carrying resources, deliver first
- Loops until manually stopped

```c
void attack(WarContext* context, WarEntity* entity)
```
**Purpose:** Attack target unit/building.

**What happens:**
- Selected units engage target
- Combat until target dies or command cancelled
- Applies to friendly, hostile detection

```c
void repair(WarContext* context, WarEntity* entity)
```
**Purpose:** Repair damaged building.

**What happens:**
- Workers walk to building
- Repair animation plays
- Building health increases
- Workers can be interrupted

---

#### Training Units

```c
void wcmd_trainFootman(WarContext* context, WarEntity* entity)
void wcmd_trainGrunt(WarContext* context, WarEntity* entity)
void wcmd_trainPeasant(WarContext* context, WarEntity* entity)
// ... and 11 more unit types
```

**Purpose:** Queue unit training in a barracks/training building.

**Parameters:**
- `context` - Game context
- `entity` - (unused, for API consistency)

**What happens:**
1. Check if barracks has required resources
2. Deduct resources from player pool
3. Start training timer
4. Create unit when complete
5. Play acknowledgement sound if successful

**Example:**
```c
// Player clicks "Train Footman" button
wcmd_trainFootman(context, NULL);

// Implementation sets:
// map->command.type = WAR_COMMAND_TRAIN_FOOTMAN
// Later execution validates and creates unit
```

**Training is tracked per building:**
- Barracks can queue multiple units
- Each unit takes time and resources
- Units are created one at a time

---

#### Building Construction

```c
void wcmd_buildFarmHumans(WarContext* context, WarEntity* entity)
void wcmd_buildBarracksHumans(WarContext* context, WarEntity* entity)
// ... and 16 more building types
```

**Purpose:** Queue building placement mode.

**What happens:**
1. Stores build command in map state
2. UI enters "building placement" mode
3. Player clicks terrain to place
4. Peasant/Peon walks to location
5. Construction begins
6. Building health increases during construction

**Construction by workers:**
- Workers carry construction materials
- Multiple workers can work on same building
- Accelerates construction
- Blocks workers from other tasks

```c
void wcmd_buildWall(WarContext* context, WarEntity* entity)
void wcmd_buildRoad(WarContext* context, WarEntity* entity)
```

**Purpose:** Begin wall/road placement.

**What happens:**
- UI enters placement mode
- Player clicks tiles to place wall/road pieces
- Pieces automatically connect (T-junctions, crosses, etc.)
- Completed instantly (no construction time)

---

#### Spell Casting

```c
void wcmd_castRainOfFire(WarContext* context, WarEntity* entity)
void wcmd_castPoisonCloud(WarContext* context, WarEntity* entity)
void wcmd_castHeal(WarContext* context, WarEntity* entity)
void wcmd_castFarSight(WarContext* context, WarEntity* entity)
// ... more spells
```

**Purpose:** Queue spell casting.

**What happens:**
1. Validates casting unit has mana
2. Creates CAST state for selected mages
3. Mage walks toward target (if out of range)
4. Animates casting
5. Spell effect triggers at full cast
6. Deducts mana
7. Cooldown period

**Target vs Area spells:**
- **Targeted** (Heal, Invisibility): Click on unit
- **Area** (Rain of Fire, Poison Cloud): Click on terrain
- **Instant** (Raise Dead): Cast at location

```c
void wcmd_castInvisibility(WarContext* context, WarEntity* entity)
```
Makes selected units invisible (if Cleric/Necrolyte selected).

```c
void wcmd_castUnHolyArmor(WarContext* context, WarEntity* entity)
```
Buffs selected units (damage reduction).

```c
void wcmd_castFarSight(WarContext* context, WarEntity* entity)
```
Temporarily reveals distant map area.

---

#### Summoning

```c
void wcmd_summonSpider(WarContext* context, WarEntity* entity)
void wcmd_summonScorpion(WarContext* context, WarEntity* entity)
void wcmd_summonDaemon(WarContext* context, WarEntity* entity)
void wcmd_summonWaterElemental(WarContext* context, WarEntity* entity)
```

**Purpose:** Summon creature as Conjurer/Warlock.

**What happens:**
1. Validates unit is Conjurer/Warlock
2. Checks mana for each summon
3. Spawns creature at mage position (randomized empty spot)
4. Creature can be controlled like normal unit
5. Creature disappears when killed
6. Plays spell cast effect

**Cost:** Each summon costs mana (can summon multiple per cast)

---

#### Upgrades

```c
void wcmd_upgradeSwords(WarContext* context, WarEntity* entity)
void wcmd_upgradeHumanShields(WarContext* context, WarEntity* entity)
void wcmd_upgradeHorses(WarContext* context, WarEntity* entity)
// ... 17 more upgrades
```

**Purpose:** Research technology in appropriate building.

**What happens:**
1. Validates building exists and is idle
2. Deducts resources
3. Plays "research" animation
4. After duration, upgrade complete
5. All matching units gain bonus (damage, armor, speed, etc.)
6. UI updates to reflect new capabilities

**Upgrade types:**
- **Weapon upgrades** (Swords, Axes, Arrows, Spears) → More damage
- **Armor upgrades** (Shields) → More armor
- **Unit upgrades** (Horses, Wolves, Scorpions, Spiders) → Better mounts/summons
- **Spell upgrades** (Rain of Fire, Poison Cloud, Water Elemental, Daemon) → More powerful spells
- **Ability upgrades** (Healing, Raise Dead, Invisibility, Unholy Armor, Far Sight, Dark Vision)

---

### Execution Functions

These low-level functions handle the actual command execution. Called from the main game loop.

#### `bool wcmd_executeCommand(WarContext* context)`

**Purpose:** Process the queued command.

**Parameters:**
- `context` - Game context

**Returns:** true if command was processed, false if no command pending

**What it does:**
1. Checks if a command is queued
2. Validates all preconditions (resources, mana, valid targets, unit states)
3. For each selected unit, executes command
4. Plays audio feedback (success or error)
5. Clears command after execution

**Called every frame** from the main game loop.

**Example flow:**
```c
// Game loop
void wg_updateGame(WarContext* context)
{
    // ... other updates ...
    wcmd_executeCommand(context);  // Execute queued command
    // ... continue ...
}
```

---

#### Execute Unit Commands

```c
void wcmd_executeMoveCommand(WarContext* context, vec2 targetPoint)
void wcmd_executeFollowCommand(WarContext* context, WarEntity* targetEntity)
void wcmd_executeStopCommand(WarContext* context)
void wcmd_executeHarvestCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
void wcmd_executeDeliverCommand(WarContext* context, WarEntity* targetEntity)
void wcmd_executeRepairCommand(WarContext* context, WarEntity* targetEntity)
void wcmd_executeAttackCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
```

**Purpose:** Execute low-level unit actions.

**Parameters vary by command:**
- Movement: target position
- Attack/Repair: target entity
- Harvesting: target resource and position

**What they do:**
- Create appropriate FSM state for each selected unit
- Queue the state change
- Filter units (only friendly, only valid types)
- Play acknowledgement sound

**Formation Handling:**
Movement preserves formation:
```c
// Calculate bounding box of selected units
rect bbox = ...;

// For each unit, calculate offset relative to bbox
// Move unit to target position + offset
```

---

#### Execute Spell Commands

```c
void wcmd_executeSummonCommand(WarContext* context, WarUnitCommandType summonType)
void wcmd_executeRainOfFireCommand(WarContext* context, vec2 targetTile)
void wcmd_executePoisonCloudCommand(WarContext* context, vec2 targetTile)
void wcmd_executeHealingCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
void wcmd_executeInvisiblityCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
void wcmd_executeUnholyArmorCommand(WarContext* context, WarEntity* targetEntity, vec2 targetTile)
void wcmd_executeRaiseDeadCommand(WarContext* context, vec2 targetTile)
void wcmd_executeSightCommand(WarContext* context, vec2 targetTile)
```

**Purpose:** Execute spell casting.

**Parameters:**
- Caster selection (operates on selected units)
- Target entity (for targeted spells) or target tile (for area spells)

**What they do:**
- Create CAST state for each selected mage
- Validate mana pool
- Set target (entity ID or tile position)
- Animation plays and damage/effect applies

---

## Integration Points

### UI Integration

The command system is driven by UI interactions:

```c
// User clicks "Move" button or right-clicks map
// UI layer calls:
wcmd_executeMoveCommand(context, clickPosition);

// User clicks "Attack" button, selects target
// UI layer calls:
wcmd_executeAttackCommand(context, targetEntity, targetTile);

// User presses 'F' for train (or clicks button)
// UI layer calls:
wcmd_trainFootman(context, NULL);
```

### Game Loop Integration

```c
void wg_updateGame(WarContext* context)
{
    // ... update game state ...
    
    // Process any queued commands
    wcmd_executeCommand(context);
    
    // ... render ...
}
```

### State Machine Integration

Commands create FSM states that drive unit behavior:

```c
// Move command creates MOVE state
WarState* moveState = wst_createMoveState(context, entity, ...);
wst_changeNextState(context, entity, moveState, true, true);

// Attack command creates ATTACK state
WarState* attackState = wst_createAttackState(context, entity, ...);
wst_changeNextState(context, entity, attackState, true, true);

// Build command creates BUILD state
WarState* buildState = wst_createBuildState(context, entity, ...);
wst_changeNextState(context, entity, buildState, true, true);
```

The command is ephemeral; the FSM state persists and controls unit behavior until completion.

---

## Command Execution Flow Examples

### Example 1: Move Command

```
Player right-clicks map at (100, 200)
  ↓
wcmd_executeMoveCommand(context, {100, 200})
  ↓
For each selected unit:
  - Calculate bounding box
  - Compute relative offset
  - Create MOVE state with target position + offset
  - Change unit state to MOVE
  ↓
Play acknowledgement sound
  ↓
[Game loop continues]
  ↓
Unit FSM executes MOVE state
  ↓
Pathfinding → Movement → Arrival
```

### Example 2: Train Unit

```
Player clicks "Train Footman" button
  ↓
wcmd_trainFootman(context, NULL)
  ↓
map->command.type = WAR_COMMAND_TRAIN_FOOTMAN
  ↓
[Next frame: wcmd_executeCommand(context)]
  ↓
Check: Does barracks have 600 gold? Yes
  ↓
Deduct 600 gold from player
  ↓
Start training timer (10 seconds)
  ↓
Play acknowledgement sound
  ↓
[After 10 seconds]
  ↓
Footman spawns at barracks
  ↓
Can now be selected and commanded
```

### Example 3: Cast Spell

```
Player clicks "Rain of Fire" button
  ↓
wcmd_castRainOfFire(context, NULL)
  ↓
map->command.type = WAR_COMMAND_SPELL_RAIN_OF_FIRE
UI enters "spell targeting" mode
  ↓
Player clicks target tile
  ↓
wcmd_executeRainOfFireCommand(context, targetTile)
  ↓
For each selected Conjurer/Warlock:
  - Check: Mana >= 50? Yes
  - Create CAST state (spell type = RAIN_OF_FIRE)
  - Set target position
  - Change state to CAST
  ↓
[FSM executes CAST state]
  ↓
Mage walks toward target (if needed)
  ↓
Animation plays
  ↓
At cast completion:
  - Spell effect triggers
  - Rain of fire appears at target
  - Damage applied to units in area
  - Mana deducted
```

### Example 4: Repair Building

```
Player selects 3 peasants
Player right-clicks damaged barracks
  ↓
wcmd_executeRepairCommand(context, barracks)
  ↓
For each selected peasant:
  - Check: Is peasant friendly? Yes
  - Check: Is target a building? Yes
  - Create REPAIR state (target = barracks)
  - Change state to REPAIR
  ↓
[FSM executes REPAIR for each peasant]
  ↓
Peasant 1 walks to barracks
Peasant 2 walks to barracks
Peasant 3 walks to barracks
  ↓
Each plays repair animation at building
  ↓
Building health increases
  ↓
Repair continues until full health or interrupted
```

---

## Command Queueing & Shift-Click

**Without SHIFT:**
- Command replaces current unit orders
- Unit stops and executes new command

**With SHIFT (Shift+Right-Click):**
- Command is queued after current action
- Unit completes current action, then executes queued command
- Can queue multiple waypoints or actions

```c
// Without SHIFT: Replace current order
WarState* moveState = wst_createMoveState(context, entity, ...);
wst_changeNextState(context, entity, moveState, true, true);  // Replace state

// With SHIFT: Queue waypoint
if (isKeyPressed(input, WAR_KEY_SHIFT))
{
    WarState* moveState = getMoveState(entity);
    vec2ListAdd(&moveState->move.positions, targetWaypoint);  // Add to queue
}
```

---

## Command Validation

Before executing, commands validate:

1. **Unit validity**
   - Is unit friendly (owned by player)?
   - Is unit capable of command (e.g., workers for harvest, mages for spells)?

2. **Target validity**
   - Does target exist?
   - Is target appropriate for command (e.g., can't attack own units)?
   - Is target in range?

3. **Resource validation**
   - Player has gold/wood for training
   - Unit has mana for spells
   - Building can be repaired (has health < max)

4. **State validation**
   - Unit is not dead
   - Unit is not currently in incompatible state (e.g., can't train while under attack)
   - Building is constructed (not mid-construction)

5. **Feasibility**
   - Pathfinding can find route to target
   - Target location is reachable
   - Building location is valid (no overlap, terrain passable)

---

## Common Patterns

### Pattern 1: Simple Move/Attack

```c
// User selects units and right-clicks
// If terrain clicked: move
wcmd_executeMoveCommand(context, terrainPos);

// If enemy clicked: attack
wcmd_executeAttackCommand(context, enemyUnit, enemyPos);
```

### Pattern 2: Multi-Selection Actions

```c
// Works on all selected units
WarMap* map = context->map;
for (s32 i = 0; i < map->selectedEntities.count; i++)
{
    WarEntityId id = map->selectedEntities.items[i];
    WarEntity* unit = we_findEntity(context, id);
    
    if (wu_isFriendlyUnit(context, unit))
    {
        // Apply command to this unit
        WarState* newState = wst_createXxxState(...);
        wst_changeNextState(context, unit, newState, true, true);
    }
}
```

### Pattern 3: Formation Preservation

```c
// Calculate bounding box of selected units
rect bbox = getSelectionBBox(map->selectedEntities);

// For each unit, maintain relative offset
for (each unit in selection)
{
    vec2 relativeOffset = unit->position - bbox.center;
    vec2 newTarget = targetPos + relativeOffset;
    // Move unit to newTarget
}
```

### Pattern 4: Resource Validation

```c
// Before executing train command
WarPlayerInfo* player = &map->players[player_id];
WarUnitStats* stats = wu_getUnitStats(unitToTrain);

if (player->gold >= stats->cost.gold && 
    player->wood >= stats->cost.wood)
{
    // Deduct and create unit
    player->gold -= stats->cost.gold;
    player->wood -= stats->cost.wood;
    we_createUnit(context, unitToTrain, ...);
}
else
{
    // Play error sound, show message
    wa_playErrorSound(context, player);
}
```

---

## Performance Considerations

### Efficient Multi-Unit Operations

- Single loop over selected units (not nested searches)
- Batch pathfinding queries
- Cache unit capabilities (is worker? is mage?)
- Defer state creation to dedicated state machine

### Command Queue Depth

- Typically 1 command queued at a time
- With Shift+Click, up to 10 waypoints per move state
- No global command queue (per-unit FSM manages order)

### Validation Overhead

- Validation is O(selected units count)
- Typical selection: 1-20 units
- Pathfinding: O(map size), done once per move
- Negligible performance impact

---

## Summary

The Commands System provides:
- ✅ **Player input translation** - UI clicks → game state changes
- ✅ **Batched multi-unit operations** - Select 10 units, command all simultaneously
- ✅ **Formation preservation** - Units maintain relative positions
- ✅ **Validation & error handling** - Checks resources, mana, targets, feasibility
- ✅ **Audio feedback** - Acknowledgement or error sounds
- ✅ **State machine integration** - Commands drive FSM behaviors
- ✅ **Queuing support** - Shift+Click for waypoint chains
- ✅ **Resource management** - Gold, wood, mana deduction with verification

All commands flow through two APIs:
1. **Registration** (`wcmd_trainFootman()`, `wcmd_castRainOfFire()`, etc.) - Lightweight
2. **Execution** (`wcmd_executeCommand()`, `wcmd_execute*Command()`) - Validation & application

The system is extensible: new commands add one registration function and one execution function.
