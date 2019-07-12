# TODO

List of thing to do in no particular order

* Bugs
  * ~~Corpses can be selected, it shouldn't be.~~
  * ~~Black pixels in little tree and gold icons.~~
  * ~~Black pixels in the "% COMPLETE" text.~~
  * ~~If a building is damaged then it can't start a train of a unit because it can't have the two states running at the same time.~~
    * ~~The Damaged state of buildings was removed, the damage animations are set in the takeDamage function now the building can build/train while is damaged.~~
  * ~~When a building is being built dont show command buttons or command texts.~~
  * ~~When different buildings are collapsed in near positions, the ruins doesn't merge well.~~
  * ~~Check why is failing when trying to determine if the midi is finish playing. Check that the entity removes correctly after the midi finish playing.~~
  * ~~Arrows for scrolling on the windows edges instead of viewport edges.~~
  * ~~Cursor have a some pixels without transparency.~~
  * ~~Right click when a command that needs a target should cancel the command.~~
  * ~~~Sounds are freed 3 times!, what's up with that?~~
    This was the result of deleting the entity and the engine trying to free the sprites associated with the sprite and button components.
  * ~~When executing a command with a multiple selection all units play the corresponding sound, it saturate the channel and it sounds horrible.~~
  * ~~The lastActionStep for the harvest action is the same for multiples frames. Investigate what is happening there to correctly chop the tree and play the chop sound. Do the same for the repairing state.~~
  * ~~Sounds of the goldmine?~~
    There is no sound for the goldmine.
  * ~~Selection rect green for friendly units, red for enemies and white for neutral.~~
  * ~~Editing trees, walls, roads and ruins doesn't check that the click was inside the map panel.~~
  * ~~In the minimap corpses are shown.~~
  * ~~WATER ELEMENTAL name go outside of portrait area. Change to WATER ELEM or W. ELEMENTAL~~
  * Sounds should be dependent if the source is inside the viewport bounds. For example, the swords sounds shouldn't interrupt other sounds if the battle is far away from the viewport bounds.
  * Cursor should stay at the edges of the window. Should I capture the mouse from the OS!? That would allow scrolling when the cursor is at the edge and the player keep moving the mouse in the direction of that edge. Right the OS cursor shows up when the user move the game cursor outside the window. That's no good.
  * When a unit is selected, say a warrior, and it cursor is over an enemy unit, it shows the magnifying glass because there is no active command. The same occurs when a worker is selected and the mouse is over a goldmine. Maybe make a check about possible commands, and show the corresponding cursor, for these cases.
  * When building roads the player needs to go back and click on the build road button again to build the next one.
  * When the last position of a segment is occupied and there is more segments, what should be the behavior? continue to next segment from the current position? stop?
  * Changing global speed doesn't change ongoing trainings.
  * Select footman -> right click on a tree, and assertion is hit.
  * Check for memory leaks in the removing animations functionality.
  * Check why the changing of the global scale renders with the previous global scale after a change (only on Linux, on Windows it doesn't happen).
  * Corpses are rendered above units.
  * Damage animations are rendered below units.
  * Click in a button, drag to the map panel, it start the selection rect. This shouldn't be.
  * Make a call to `sleep` instead of waiting in a cycle until the frame end. This will probably increase effiency and decrease CPU usage. I need a portable sleep function, maybe something like:
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
  * Search for files: HMAP01-12.war, LOSE1.WAR, OMAP01-12.WAR, WIN1.WAR
  * ~~Death animation of scorpions loops.~~
  * ~~Spell animation isn't shown.~~
  * ~~Mana of magic units don't increase over time.~~
  * Fix walking animation with follow behavior. Make it continuous intead of reseting piece of the path.
  * When a unit attacks a unit that is attacking a building, the second unit should stop the attack on the building and attack the first unit.
  * Use `entityTilePositionInRange` to calculate near units? used in splash damage.
  * When attacking a wall, if the units isn't within range, it will walk to the piece's position without considering its range of attack. That's because move state is being used in this case, instead of follow when attacking an unit.
  * ~~Fix colors of Rain of Fire portrait~~
  * ~~Fix colors of Poison Cloud~~
  * ~~Fix orc maps tile displacements~~
  * Check the uses of `context->deltaTime` when the speed of the game is not `1`.

* General
  * ~~Test the new implementation for lists.~~
  * ~~Make some or macros or inline functions to create options for lists initializations.~~
  * ~~Map scroll with the mouse cursor at the edge.~~
  * ~~Factorize state_machine.c in files (maybe `state_machine_update.c`, `state_machine_enter.c`, etc.).~~
  * ~~Walls (same system like roads).~~
  * ~~Make a better input system~~
    * ~~Map scrolling and positioning by clicking in the minimap is now all under `updateViewport` function.~~
    * ~~Selection drag is now all under `updateDragRect` function.~~
    * ~~Refactor right click code into a more robust order system.~~
  * ~~Make trees behavior~~
    * ~~Test different configurations and update the excel and the array of tree index to tile mappings.~~
  * ~~Make roads behavior like the trees.~~
  * ~~Make walls behavior like the trees.~~
  * ~~Make ruins behavior like the trees.~~
  * ~~Update minimap with chopped trees.~~
  * ~~Make a `setUITextFormat` method that takes a format with arguments `printf` style.~~
  * ~~Rename `WarUnitCommandBaseData` to something like `WarUnitCommandBaseData`.~~
  * Write a detailed description of the actions system, maybe as comments in the `war_actions.c` file?
  * Manage components with a dictionary and not each entity having all the components.
  * Create EntityManager to manage entities.
  * Make a profiler system.
  * Sort the units by `y` position to render and the units with greater `y` render on top of the ones with less `y`.
  * Draw text system (to debug and other texts).
  * Remove global __log__ and move it to WarContext.
  * Make so that entities can have multiple sprites.
  * Add a `renderAnimations` function to render the animations above everything else and move the corresponding code in `renderUnit` to the new function.
  * Add animation for the gold and lumber numbers when they change.
  * Make commands/cheats system.
  * Make the blue water/green water animated.
  * Make the move state to consider range distance to stop.

* Gameplay
  * ~~Add functionalities about players and player infos, gold and wood amount, upgrades, unit count, race, etc.~~
    * ~~Fix black areas in human unit portraits images.~~
    * ~~Add selected units back images.~~
    * ~~Add portraits to selected units.~~
    * ~~Remove pink pixels from unit portraits images.~~
    * ~~Add name to the unit portraits.~~
    * ~~Add life bar to the unit portraits.~~
    * ~~Add magic to the unit portraits.~~
    * ~~Add complete % bar only when there is something building.~~
    * ~~Add command images based on selected(s) unit types.~~
    * ~~Add upgrades info to player info.~~
  * Change the palette of the unit portraits depending on the tileset of the current map (building portraits looks different).
  * ~~Manage the gold in the mines and explode it when the gold is over.~~
  * ~~Add ruins to collapsed buildings.~~
  * Add explosion animation to living npc units when clicking to many times.
  * ~~Create and entity for each tile that represent wood in order to keep track of the amount of wood in each area and been able to replace with the appropiated sprite when the wood in that tile is over.~~
  * ~~Fix the selection of units to select only dude units or one building.~~
  * Make Ctrl+click select all units of the same type on the screen.
  * ~~Making health system for units.~~
  * ~~Change the behavior of chopping trees when multiple units are chopping the same tree. With each hack of the axe the tree should loose wood, until it ran out of it. Each unit go back to the townhall when has max amount of wood or there is no more trees to chop.~~
  * ~~If each tree island is considered a forest, then the workers naturally will chop the entire forest/island and stop there, but the editing trees functionality doesn't work for multiple forests/island. If there is a single forest in the map, then the unit will chop all the wood in the map, which is not desired. Figure out which way is better, and make the necessary changes.~~
    * ~~The solution implemented was that tree islands are forests, and in editing, just determine the tree tiles for each forest, because the edition a less used and slower funcionality and it doesn't matter too much if every tree is checked in each operation.~~
  * ~~Add functionality of training units.~~
    * ~~Fix upgrade build times.~~
    * ~~Make so that train units withdraw gold and lumber.~~
    * ~~Handle the case when there is no sufficient gold or lumber (put a message in the status bar).~~
    * ~~Add keyboard shortcuts to train units.~~
    * ~~Handle the case when there is no sufficient farm food for the new training (put a message in the status bar).~~
  * ~~Add functionality of building upgrades.~~
    * ~~Fix upgrade build times.~~
    * ~~Make so that build upgrades withdraw gold and lumber.~~
    * ~~Handle the case when there is no sufficient gold or lumber (put a message in the status bar).~~
    * ~~Make so that the upgrades increase the corresponding values for the damage, armor and so.~~
    * ~~Add keyboard shortcuts to build upgrades.~~
  * ~~Add functionality of building buildings.~~
    * ~~Make so that build buildings withdraw gold and lumber.~~
    * ~~Handle the case when there is no sufficient gold or lumber (put a message in the status bar).~~
    * ~~Add keyboard shortcuts to build buildings.~~
  * ~~Add functionality of basic commands.~~
    * ~~Move command~~
    * ~~Stop command~~
    * ~~Attack command~~
      * ~~Check that the selected units can attack the target unit.~~
      * ~~Attack on the ground.~~
    * ~~Harvest command~~
    * ~~Repair command~~
  * Add functionality of spell commands.
    * ~~Summoning~~
    * Rain of fire
    * Cloud of poison
    * Far seeing / Dark vision
    * Invincibility / Holy armor
    * Healing
    * Raise dead

* Animations/actions
  * ~~Add animations data for each unit type.~~
  * ~~Add actions for the peasant and peon when carrying gold or wood.~~
  * ~~Add actions for the buildings.~~
  * ~~Add little damage, huge damage and collapse animations to buildings.~~
  * ~~Add animation system, again.~~
  * ~~Switch animations without reseting the new animation to the start. This will allow have one animation for each orientation of then switch to the correct one depending of the orientation but conserving the state.~~
  * ~~Change the concept of animations by a sequence of frames, to a more complex but powerful system of actions. Each unit can have several actions, which can have steps and the steps of the actions describe what the unit does. For example, this is the `Attack` action of the footman:~~
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
  * Move actions system to animations, again? :| 
    The problem is, for example in the move action, that the state machine does the moving, the wait between action steps are almost the same within the actions, and what is needed in reality is the changing frame, maybe the unbreakable markers and the sounds. I don't know maybe keep it, but removing the moving steps only.
  * ~~Projectiles~~
    * ~~Arrows~~
    * ~~Fireballs~~
    * ~~Rain of fire~~
  
* State machine
  * ~~Idle state~~
  * ~~Move state~~
    * ~~When the last position of a path is occupied and the unit waits is period but then continue to the last position when isn't clear.~~
    * ~~Mark the pathfinding data with id of the units, so they can clear it appropiately when leaving states.~~
    * ~~Moving with multiple waypoints.~~
    * ~~When the last few positions in a path are occupied and the final position is no longer reachable, the unit should get to the closest position posible.~~
  * ~~Patrol state~~
    * ~~Patrol is a combination of move behaviors.~~
    * ~~Patrol with multiple waypoints.~~
  * ~~Follow state~~
    * ~~Follow other units.~~
  * Attack state
    * ~~Attack the unit next to~~
    * ~~Move to attack the unit that is far~~
    * ~~Consider ranged and melee attacks~~
    * Do damage.
      * ~~Do damage to units.~~
      * ~~Do damage to buildings.~~
      * Do damage with splash
        * ~~Catapults~~
        * Rain of fire
        * Cloud of poison
      * Do damage with magic.
  * ~~Ground-attack state~~
    * ~~Ground-attack is a combination of move and look around behaviors to attack anyone in range while the unit is moving to the target.~~
  * ~~Damaged state (for buildings)~~
  * ~~Collapse state (for buildings)~~
    * ~~Spawn ruins after the collapse of a building.~~
    * ~~Check if the new spawning ruins could merge with a previous one. This occurs when a building is built above a ruins.~~
  * ~~Build state~~
  * ~~Gathering resources state~~
    * ~~Gathering gold.~~
    * ~~Gathering wood~~
  * ~~Make a Leave function when the states are leaving, and not just free them. Let that responsibility to the state itself.~~
  * Make the state switching system can return values when going back to the previous state. This will allow follow and move to return to previous state (such attack) that there is no path to the target, so the unit can go idle.
    * When an unit is going to attack another one, but can't reach it because of is blocked but other units, there is a loop between attack and follow states, because there is not mechanism to allow the follow state to tell the attack state that the target unit can't be reached, so the unit must go idle.
  * Remove the interval for state, each state machine updates every frame. I'm going to worry about performance issues that this may cause later.

* Pathfinding
  * ~~Add path finding algorithm to move the entities.~~
    * ~~For now BFS is implemented.~~
    * ~~Implement A* algorithm for pathfinding.~~
    * ~~Test the implementation of the A* algorithm for pathfinding.~~
  * ~~Update the map in each interation of the state machines to support updating the paths when moving units.~~
  * ~~Include the current position of the unit in the path to support the patrol behaviour.~~
  * ~~Manage the case when there is no path to a position. It should stay or should go the closest position?~~
  * ~~Optimize the path finding when the destination is unreachable (e.g. when the unit is moving to another unit's position)~~
  * ~~When right click in the minimap, the selected unit should go there.~~
  * Give greater cost to diagonal movements than to straight movements.
  * Remove BFS implementation.
  * Check again the path finding stuff, because now the units will go for side edges of buildings when a corner is closest.
  * Workers don't collide well while harvesting wood or gold when near the townhall.

* UI system
  * ~~Buttons~~
  * ~~Text~~
  * ~~Images~~
  * Dialogues
  * Cut scenes
  * Minimap
    * The minimap has to consider: base layer, chopped wood layer, entities layer. The base layer doesn't change once the map is created, so each frame only add the tiles about chopped wood and entities (position and types).
  * ~~Make font of original warcraft.~~
    * ~~Make a bunch of screenshots and draw the font in pixel art.~~
    * ~~Make the system to draw text from a sprite of characters.~~
    * ~~Make font of menus~~