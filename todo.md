# TODO

List of thing to do in no particular order

* General
  * Create and entity for each tile that represent wood in order to keep track of the amount of wood in each area and been able to replace with the appropiated sprite when the wood in that tile is over
  * Manage the gold in the mines and explode it when the gold is over
  * Add explosion animations for the buildings
  * Add animations data for each entity type
  * Add functionalities about players and player infos, gold and wood amount, upgrades, unit count, race, etc.
  * Manage components with a dictionary and not each entity having all the components
  * Map scroll with the mouse cursor at the edge
* Animations
  * (done) Switch animations without reseting the new animation to the start. This will allow have one animation for each orientation of then switch to the correct one depending of the orientation but conserving the state.
  * (done) Have a giant table with animations data for each entity type.
    * Instead an `addAnimations` function was written that assign the animations to the entities. This was because multiple entity types have the same animations data.
  * Change the concept of animations by a sequence of frames, to a more complex but powerful system of actions. Each unit can have several actions, which can have steps and the steps of the actions describe what the unit does. For example, this is the `Attack` action of the footman:
  ```
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
  
* State machine
  * Idle behaviour
  * Move behaviour
  * Move attack behaviour
  * Build behaviour
  * Patrol behaviour?
  * Gathering resources behaviour
* Path finding
* Collision system
* UI system
  * Buttons
  * Dialogues
  * Cut scenes
  * Minimap
    * The minimap has to consider: base layer, chopped wood layer, entities layer. The base layer doesn't change once the map is created, so each frame only add the tiles about chopped wood and entities (position and types).