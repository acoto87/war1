# TODO

List of thing to do in no particular order

## Bugs
* [x] Corpses can be selected, it shouldn't be.
* [x] Black pixels in little tree and gold icons.
* [x] Black pixels in the "% COMPLETE" text.
* [x] If a building is damaged then it can't start a train of a unit because it can't have the two states running at the same time.
* [x] The Damaged state of buildings was removed, the damage animations are set in the takeDamage function now the building can build/train while is damaged.
* [x] When a building is being built dont show command buttons or command texts.
* [x] When different buildings are collapsed in near positions, the ruins doesn't merge well.
* [x] Check why is failing when trying to determine if the midi is finish playing. Check that the entity removes correctly after the midi finish playing.
* [x] Arrows for scrolling on the windows edges instead of viewport edges.
* [x] Cursor have a some pixels without transparency.
* [x] Right click when a command that needs a target should cancel the command.
* [x] Sounds are freed 3 times!, what's up with that?
This was the result of deleting the entity and the engine trying to free the sprites associated with the sprite and button components.
* [x] When executing a command with a multiple selection all units play the corresponding sound, it saturate the channel and it sounds horrible.
* [x] The lastActionStep for the harvest action is the same for multiples frames. Investigate what is happening there to correctly chop the tree and play the chop sound. Do the same for the repairing state.
* [x] Sounds of the goldmine? There is no sound for the goldmine.
* [x] Selection rect green for friendly units, red for enemies and white for neutral.
* [x] Editing trees, walls, roads and ruins doesn't check that the click was inside the map panel.
* [x] In the minimap corpses are shown.
* [x] WATER ELEMENTAL name go outside of portrait area. Change to WATER ELEM or W. ELEMENTAL
* [x] Death animation of scorpions loops.
* [x] Spell animation isn't shown.
* [x] Mana of magic units don't increase over time.
* [x] Use `tileInRange` to calculate near units? used in splash damage.
* [x] Fix colors of Rain of Fire portrait
* [x] Fix colors of Poison Cloud
* [x] Fix orc maps tile displacements
* [x] Deliver command (when click on button) is not working when unit have wood.
* [x] Summoned units have mana and when its mana runs out they die.
* [x] Check harvest right click vs command button when clicking on a dark area that is partially covering a goldmine.
* [x] Assertion in determining ruin pieces hit when goldmine ran out of gold.
* [x] Holy Sight doesn't mark the buildings as hasBeenSeen.
* [x] When a menu is showing in the map scene, the player can still select units and buildings.
* [x] Orcs UI is still the Humans.
* [x] In the first level there shouldn't be a Lumbermill to build.
* [x] Revisit the FEATURES enum, it should be like this (maybe do a solution like the `getUpgradeLevel` macro):
* [x] Check clipping of audios, `value = clamp(value, INT16_MIN, INT16_MAX);` this line doesn't make much sense because value is a `s16` already.
* [x] Fix decaying music when changing scenes.
* [x] Fix fucsia color in the border of the map of the orcs
* [x] Damage animations are rendered below units.
* [x] Corpses are rendered above units.
* [x] Select footman -> right click on a tree, and assertion is hit.
* [x] Fix walking animation with follow behavior. Make it continuous intead of reseting piece of the path.
* [x] Make highlights in text be a span of text instead of just one character.
* [x] When changing scenes, it seems that part of the music of the previous scene keeps playing. This was due to a feature on TinySoundFont that makes a fade of the music. The solution I gave was to make the time of that fade short enough so there is not an issue.
* [x] If a worker is selected and enter a mine, you can still give orders to him.
* [x] Enemy units are shown in fog.
* [x] Buildings like mines aren't show in the minimap in fog, but they are in unkwon when have one tile on fog.
* [x] Attack on ground on fog, the units doesn't move.
* [x] Attack on units on fog, the minimap and the unit goes out of fog for a frame.
* [x] When selected and enemy that goes into fog, the unit keeps selected.
* [x] Fix clicking buttons will flick the tooltip text on and off.
* [x] Make a call to `sleep` instead of waiting in a cycle until the frame end. This will probably increase effiency and decrease CPU usage. I need a portable sleep function, maybe something like:
    ```c
    void msleep(s32 milliseconds) // cross-platform sleep function
    {
      #ifdef WIN32
        // windows.h need to be include for this
        Sleep(milliseconds);
      #elif _POSIX_C_SOURCE >= 199309L
        // this is the posix call, _POSIX_C_SOURCE need to be defined
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        nanosleep(&ts, NULL);
      #else
        // unistd.h need to be include for this
        usleep(milliseconds * 1000);
      #endif
    }
    ```
* [x] Search for files: HMAP01-12.war, LOSE1.WAR, OMAP01-12.WAR, WIN1.WAR. Recovered from https://www.gog.com/game/warcraft_orcs_and_humans.
* [x] When a unit is selected before dispear in the fog, the unit remains selected. The unit should be removed from the selection.
* [x] When rendering multiline texts, if the last character of a line doesn't fit and it's a space, then the space will be rendered in the next line which causes missalignment in the left border of the text.
* [x] When building roads the player needs to go back and click on the build road button again to build the next one.
* [x] When attacking a wall, if the units isn't within range, it will walk to the piece's position without considering its range of attack. That's because move state is being used in this case, instead of follow when attacking an unit.
* [x] Sounds should be dependent if the source is inside the viewport bounds. For example, the swords sounds shouldn't interrupt other sounds if the battle is far away from the viewport bounds.
* [x] Check why this appear when trying to spell the Dark Vision: "This upgrade type 15 doesn't increase any value of the units (Not reproducible)".
* [x] Summon spells summon as many units as mana allows.
* [x] Check death animations of scorpions and spiders.
* [ ] Cursor should stay at the edges of the window. Should I capture the mouse from the OS!? That would allow scrolling when the cursor is at the edge and the player keep moving the mouse in the direction of that edge. Right now the OS cursor shows up when the user move the game cursor outside the window. That's no good.
* [x] When a unit is selected, say a warrior, and the cursor is over an enemy unit, it shows the magnifying glass because there is no active command and is expecting to select the foe unit when maybe it would be better to show a possible command like attack. The same occurs when a worker is selected and the mouse is over a goldmine. Maybe make a check about possible commands, and show the corresponding cursor, for these cases.
* [ ] When the last position of a segment is occupied and there is more segments, what should be the behavior? continue to next segment from the current position? stop?
* [ ] Changing global speed doesn't change ongoing trainings.
* [ ] Check for memory leaks in the removing animations functionality.
* [ ] Check why the changing of the global scale renders with the previous global scale after a change (only on Linux, on Windows it doesn't happen).
* [ ] Click in a button, drag to the map panel, it start the selection rect. This shouldn't be.
* [ ] When a unit attacks a unit that is attacking a building, the second unit should stop the attack on the building and attack the first unit.
* [ ] Check the uses of `context->deltaTime` when the speed of the game is not `1`.
* [ ] If an unit is attacked when idle, the unit respond the attack.
* [ ] Instead of Holy Sight/Dark Vision create an object, make the fog of war cells have more states like MAP_STATE_ALWAYS_VISIBLE and MAP_STATE_TIMED_VISIBLE.
* [ ] Check if the `changeSampleRate` introduces the tiny pop bug at the end of short sounds.
* [ ] Sometimes you order a worker to mine, and it will enter the mine (dissapear) but it doesn't perform the mining. You can also give other orders like move, and the invisible worker will go there and do other stuff.... on the bright side, I have invisible units! :D... But wait.. I already have invisible units with Invisibility spells..., damn! :(
* [ ] Some lines in text appear on Windows and between sprites, antialiasing maybe on the OpenGL ES driver in Windows?
* [ ] If a unit is selected and the player give several orders in a row quickly, the audio gets saturated. Maybe I can restrict how much audios are generated by units, maybe keep track of the last time the unit spoke or a sound is emitted from a unit, to not do it again in short periods of time.
* [ ] Check this: https://tcrf.net/Warcraft:_Orcs_%26_Humans, https://tcrf.net/Proto:Warcraft:_Orcs_%26_Humans

## General
* [x] Test the new implementation for lists.
* [x] Make some or macros or inline functions to create options for lists initializations.
* [x] Map scroll with the mouse cursor at the edge.
* [x] Factorize state_machine.c in files (maybe `state_machine_update.c`, `state_machine_enter.c`, etc.).
* [x] Walls (same system like roads).
* [x] Make a better input system
* [x] Map scrolling and positioning by clicking in the minimap is now all under `updateViewport` function.
* [x] Selection drag is now all under `updateDragRect` function.
* [x] Refactor right click code into a more robust order system.
* [x] Make trees behavior
* [x] Test different configurations and update the excel and the array of tree index to tile mappings.
* [x] Make roads behavior like the trees.
* [x] Make walls behavior like the trees.
* [x] Make ruins behavior like the trees.
* [x] Update minimap with chopped trees.
* [x] Make a `setUITextFormat` method that takes a format with arguments `printf` style.
* [x] Rename `WarUnitCommandBaseData` to something like `WarUnitCommandBaseData`.
* [x] Show corresponding WIN or LOSE messages in game over menu.
* [x] Skip briefing with click.
* [x] Create EntityManager to manage entities.
* [x] Draw text system (to debug and other texts).
* [x] Sort the units by `y` position to render and the units with greater `y` render on top of the ones with less `y`.
* [x] Make screen to download demo DATA.WAR file if it doesn't exists (the app should download it and install it itself). Download it from here: https://archive.org/download/WarcraftOrcsHumansDemo/WCRFT.ZIP/DEMODATA%2FDATA.WAR. To get there go to https://archive.org/details/WarcraftOrcsHumansDemo, then SHOW ALL in DOWNLOAD OPTIONS -> WCRFT.ZIP (View Contents).
* [ ] Write a detailed description of the actions system, maybe as comments in the `war_actions.c` file?
* [ ] Manage components with a dictionary and not each entity having all the components.
* [ ] Make a profiler system.
* [ ] Remove global __log__ and move it to WarContext.
* [ ] Make so that entities can have multiple sprites.
* [ ] Add a `renderAnimations` function to render the animations above everything else and move the corresponding code in `renderUnit` to the new function.
* [ ] Add animation for the gold and lumber numbers when they change.
* [ ] Make commands/cheats system.
* [ ] Make the blue water/green water animated.
* [ ] Make the move state to consider range distance to stop.
* [ ] Units like raised skeletons have a decay, that's that after a certain time, the unit dies. Check if summoned units have the same behavior.
* [ ] Check behavior of invisible units when is under attack (it maybe work with workers, to stop the attack on it)
* [ ] Zoom feature

## Gameplay
* [x] Add functionalities about players and player infos, gold and wood amount, upgrades, unit count, race, etc.
    * [x] Fix black areas in human unit portraits images.
    * [x] Add selected units back images.
    * [x] Add portraits to selected units.
    * [x] Remove pink pixels from unit portraits images.
    * [x] Add name to the unit portraits.
    * [x] Add life bar to the unit portraits.
    * [x] Add magic to the unit portraits.
    * [x] Add complete % bar only when there is something building.
    * [x] Add command images based on selected(s) unit types.
    * [x] Add upgrades info to player info.
* [ ] Change the palette of the unit portraits depending on the tileset of the current map (building portraits looks different).
* [x] Manage the gold in the mines and explode it when the gold is over.
* [x] Add ruins to collapsed buildings.
* [ ] Add explosion animation to living npc units when clicking to many times.
* [x] Create and entity for each tile that represent wood in order to keep track of the amount of wood in each area and been able to replace with the appropiated sprite when the wood in that tile is over.
* [x] Fix the selection of units to select only dude units or one building.
* [ ] Make Ctrl+click select all units of the same type on the screen.
* [x] Making health system for units.
* [x] Change the behavior of chopping trees when multiple units are chopping the same tree. With each hack of the axe the tree should loose wood, until it ran out of it. Each unit go back to the townhall when has max amount of wood or there is no more trees to chop.
* [x] If each tree island is considered a forest, then the workers naturally will chop the entire forest/island and stop there, but the editing trees functionality doesn't work for multiple forests/island. If there is a single forest in the map, then the unit will chop all the wood in the map, which is not desired. Figure out which way is better, and make the necessary changes.
    * [x] The solution implemented was that tree islands are forests, and in editing, just determine the tree tiles for each forest, because the edition a less used and slower funcionality and it doesn't matter too much if every tree is checked in each operation.
* [x] Add functionality of training units.
    * [x] Fix upgrade build times.
    * [x] Make so that train units withdraw gold and lumber.
    * [x] Handle the case when there is no sufficient gold or lumber (put a message in the status bar).
    * [x] Add keyboard shortcuts to train units.
    * [x] Handle the case when there is no sufficient farm food for the new training (put a message in the status bar).
* [x] Add functionality of building upgrades.
    * [x] Fix upgrade build times.
    * [x] Make so that build upgrades withdraw gold and lumber.
    * [x] Handle the case when there is no sufficient gold or lumber (put a message in the status bar).
    * [x] Make so that the upgrades increase the corresponding values for the damage, armor and so.
    * [x] Add keyboard shortcuts to build upgrades.
* [x] Add functionality of building buildings.
    * [x] Make so that build buildings withdraw gold and lumber.
    * [x] Handle the case when there is no sufficient gold or lumber (put a message in the status bar).
    * [x] Add keyboard shortcuts to build buildings.
* [x] Add functionality of basic commands.
    * [x] Move command
    * [x] Stop command
    * [x] Attack command
        * [x] Check that the selected units can attack the target unit.
        * [x] Attack on the ground.
    * [x] Harvest command
    * [x] Repair command
* [x] Add functionality of spell commands.
    * [x] Summoning
    * [x] Rain of fire
    * [x] Cloud of poison
    * [x] Far seeing / Dark vision
    * [x] Invisibility
    * [x] UnHoly armor
    * [x] Healing
    * [x] Raise dead
* [x] Fog of war
    * [x] Update fog of war once each second? Yes.
    * [x] Block selection clicks in fog region
    * [x] Block build in fog region
    * [x] Cursor changing when hover a foe unit that is in the fog
    * [x] Moving unit clears fog
    * [x] Attack at distance clears fog? Yes.
    * [x] Spells at distance clears fog? Yes.
    * [x] Holy Vision/Dark Vision spells clears fog
    * [x] Buildings can't be built on unknown areas
    * [x] Attacks on unknown areas always produce the unit to move to the attacking point, even ranges ones.
    * [x] Attackers reveals themselves when attacking a player's unit.
    * [x] Reveal attacked units? Yes.
    * [x] After a number of seconds (3 maybe?) an area without friendly units remain with fog.
    * [x] That is, buildings, roads, ruins, walls, trees are shown in the map, with a fade of fog.
    * [x] Moving units are not shown in areas with fog.

## Animations/actions
* [x] Add animations data for each unit type.
* [x] Add actions for the peasant and peon when carrying gold or wood.
* [x] Add actions for the buildings.
* [x] Add little damage, huge damage and collapse animations to buildings.
* [x] Add animation system, again.
* [x] Switch animations without reseting the new animation to the start. This will allow have one animation for each orientation of then switch to the correct one depending of the orientation but conserving the state.
* [x] Change the concept of animations by a sequence of frames, to a more complex but powerful system of actions. Each unit can have several actions, which can have steps and the steps of the actions describe what the unit does. For example, this is the `Attack` action of the footman:
    ```c
    Attack={
    "unbreakable begin",
    "frame 5",
    "wait 5",
    "frame 20",
    "wait 5",
    "frame 35",
    "attack",
    "sound sword attack",
    "wait 5",
    "frame 50",
    "wait 5",
    "frame 60",
    "wait 5",
    "wait 0",
    "frame 0",
    "wait 1",
    "unbreakable end",
    "frame 0",
    "wait 1"
    }
    ```
* [ ] Move actions system to animations, again? :| The problem is, for example in the move action, that the state machine does the moving, the wait between action steps are almost the same within the actions, and what is needed in reality is the changing frame, maybe the unbreakable markers and the sounds. I don't know maybe keep it, but removing the moving steps only.
* [x] Projectiles
    * [x] Arrows
    * [x] Fireballs
    * [x] Rain of fire

## State machine
* [x] Idle state
* [x] Move state
    * [x] When the last position of a path is occupied and the unit waits is period but then continue to the last position when isn't clear.
    * [x] Mark the pathfinding data with id of the units, so they can clear it appropiately when leaving states.
    * [x] Moving with multiple waypoints.
    * [x] When the last few positions in a path are occupied and the final position is no longer reachable, the unit should get to the closest position posible.
* [x] Patrol state
    * [x] Patrol is a combination of move behaviors.
    * [x] Patrol with multiple waypoints.
* [x] Follow state
    * [x] Follow other units.
* [ ] Attack state
    * [x] Attack the unit next to
    * [x] Move to attack the unit that is far
    * [x] Consider ranged and melee attacks
    * [x] Do damage
        * [x] Do damage to units.
        * [x] Do damage to buildings.
        * [x] Do damage with splash
            * [x] Catapults
            * [x] Rain of fire
            * [x] Cloud of poison
        * [x] Do damage with magic.
* [x] Ground-attack state
    * [x] Ground-attack is a combination of move and look around behaviors to attack anyone in range while the unit is moving to the target.
* [x] Damaged state (for buildings)
* [x] Collapse state (for buildings)
    * [x] Spawn ruins after the collapse of a building.
    * [x] Check if the new spawning ruins could merge with a previous one. This occurs when a building is built above a ruins.
* [x] Build state
* [x] Gathering resources state
    * [x] Gathering gold.
    * [x] Gathering wood
* [x] Make a Leave function when the states are leaving, and not just free them. Let that responsibility to the state itself.
* [ ] Make the state switching system can return values when going back to the previous state. This will allow follow and move to return to previous state (such attack) that there is no path to the target, so the unit can go idle.
    * [ ] When an unit is going to attack another one, but can't reach it because of is blocked but other units, there is a loop between attack and follow states, because there is not mechanism to allow the follow state to tell the attack state that the target unit can't be reached, so the unit must go idle.
* [ ] Remove the interval for state, each state machine updates every frame. I'm going to worry about performance issues that this may cause later.

## Pathfinding
* [x] Add path finding algorithm to move the entities.
    * [x] For now BFS is implemented.
    * [x] Implement A* algorithm for pathfinding.
    * [x] Test the implementation of the A* algorithm for pathfinding.
* [x] Update the map in each interation of the state machines to support updating the paths when moving units.
* [x] Include the current position of the unit in the path to support the patrol behaviour.
* [x] Manage the case when there is no path to a position. It should stay or should go the closest position?
* [x] Optimize the path finding when the destination is unreachable (e.g. when the unit is moving to another unit's position)
* [x] When right click in the minimap, the selected unit should go there.
* [ ] Give greater cost to diagonal movements than to straight movements. Check these links for a heuristic function that considers diagonal cost:
  http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html
  http://theory.stanford.edu/~amitp/GameProgramming/Variations.html#bidirectional-search
* [ ] Remove BFS implementation.
* [ ] Check again the path finding stuff, because now the units will go for side edges of buildings when a corner is closest.
* [ ] Workers don't collide well while harvesting wood or gold when near the townhall.

## UI system
* [x] Buttons
* [x] Text
* [x] Images
* [ ] Dialogues
* [ ] Cut scenes
* [ ] Minimap
    * [ ] The minimap has to consider: base layer, chopped wood layer, entities layer. The base layer doesn't change once the map is created, so each frame only add the tiles about chopped wood and entities (position and types).
* [x] Make font of original warcraft.
    * [x] Make a bunch of screenshots and draw the font in pixel art.
    * [x] Make the system to draw text from a sprite of characters.
    * [x] Make font of menus.