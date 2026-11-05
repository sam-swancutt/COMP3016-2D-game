Comp3016 CW1

Hunter Game 
Gameplay description 
A hunter enters a dungeon to slay the boss. the  hunter must clear waves of minions to get to the next level, whilst collection loot along the way.
after the last of the minions are defeated the boss appears, defeat it to win. if the hunters heath reaches 0 the run restarts from level 1. 

Controls

Input	Action
W A S D	Move
Left mouse button	Attack towards the mouse (0.4 s cooldown)
Space	Dodge roll (1 s cooldown, brief invincibility)
1 to 4	Use a potion or equip a weapon from that inventory slot
Q or the DROP button	Turn drop mode on or off, then press 1 to 4 or click a slot to drop that item
Right-click a slot	Drop that item straight away
Walk over an item	Pick it up
Esc	Return to the menu (quits from the menu)
Enter	Start from the menu, or return to the menu after winning

Start menu: pick Easy, Normal or Hard. The difficulty adds 0, 2 or 4 extra enemies to every room.

Loot: enemies have a 35% chance to drop an item. There are two item types in three rarities.

Item	Common (white)	Rare (blue)	Legendary (gold)
Potion	Heals 1	Heals 2	Full heal
Sword	+1 damage	+2 damage	+3 damage

Rarity is shown by border colour and thickness, so it does not rely on colour alone.

Boss: it chases the hunter, then stops and flashes yellow as a warning before attacking. It has three phases that begin at 66% and 33% health. Each phase is faster and fires a bigger ring of projectiles. In phase 3 it also charges at the player and is stunned afterwards, which gives the player a window to hit it. Rolling through projectiles avoids the damage.


 
COMP3016 CW1 Proposal
Genre 
Top down 2D action boss hunting structure.
Synopsis 
A hunter enters the cave/dungeon to slay the beast that resides inside. Each room has the beast's minions the hunter must slay before entering the next room/level. Minions will also drop loot the hunter can use to make them stronger until the final boss room where the hunter will have to defeat the boss or risk restarting the game.
Core gameplay 
Hunter fights waves of minions, collects and equip/use loot, then moves to the next level, slightly harder with more minions. Final round boss fight, win = the boss dies, lose player health reaches 0 and the run restarts.
Mechanics 
moving / wasd, Attacking with cooldown, dodge / roll with cooldown and brief invincibility, Small inventory, Pick up, use and equip,Rarity in items, Health for player and enemies, Level manger that controls the spawns of the minions. 
Dynamics and Aesthetics
Risk against reward when looting mid fight and saving heals for boss fight, also learning the boss fighting pattern. Loot colours vary depending on the rarity (green, common, gold legendary). The health bar changes colour based on the remaining health(turns red when getting low and green for full health on player).    
 
 
What was delivered

Top-down 2D action Delivered	
positions are random each run.
Hunter vs beast, rooms of minions	Delivered
Minions drop loot	Delivered	35% drop chance, three rarities
Each level harder, more minions	Delivered	base_enemies + enemies_per_room * room, plus the difficulty bonus
Final boss, win by killing it	Delivered	Boss with three phases
Lose at 0 health, run restarts	Delivered	
WASD movement	Delivered	
Attack with cooldown	Delivered	
Dodge roll with cooldown and invincibility	Delivered	
Small inventory, pick up, use, equip	Delivered	Four slots, plus a drop feature
Rarity in items	Delivered	
Health for player and enemies	Delivered	Also the boss
Level manager controls spawns	Delivered	LevelManager
Risk against reward	Delivered	Small inventory, potions saved for the boss
Learning the boss pattern	Delivered	Yellow telegraph, three phases
Health bar changes colour	Delivered	Player, enemies and boss

Deviations and justification

Rarity colours changed from my proposal. from green/ gold to white/ blue/ gold, as using green coloured items would clash with the background and items. I also made the item borders thicker depending on the rarity to help show change, this can also be helpful for visually impaired users play.

Additions I made was to add a start menu with difficulty settings, a boss with projectiles and phases and knocking enemy's back when hit. these deepen the risk vs reward and boss learning dynamics from the proposal.

Dependencies
SDL3 This is the only external library. It is used for the window, rendering, input, BMP(sprites and background) loading and timing.

Game mechanics and how they are coded

Mechanic:	How it works:	Where.

Game loop and delta time	Each frame computes dt in seconds, then calls handleEvents, update(dt) and render. All movement and timers use dt, so speed doesn't depend on frame rate.	Game::run

Movement	Reads the keyboard state each frame, builds a direction from W A S D and normalises it so diagonals aren't faster.	Player::update

Attack	A left click starts an attack if attackCooldown is ready. It places a short-lived hitbox 40 pixels from the player towards the mouse. Game checks that rectangle against enemies and the boss.	Player::update, Game::update

Dodge roll	Space starts a 0.2 s dash in the last movement direction. Damage is ignored while rolling. A separate 1 s cooldown limits it.	Player::update, Player::takeDamage

Cooldowns	A small reusable class. tryUse() returns true only when the timer has run out, then restarts it. Used for attack, dodge, hurt invincibility and boss hit frames.	Cooldown.h

Health	A small class held by composition. The player, enemies and boss all use it. fraction() drives the colour-changing health bars.	Health.h

Enemy AI	Each enemy moves towards the player's centre every frame. While knockback is strong it stops chasing, which gives the player a short opening.	Enemy::update

Knockback	A hit sets a velocity away from the player. Each frame it moves the enemy and is reduced by a friction factor, so the push starts fast and eases to a stop.	Enemy::takeDamage, Enemy::update

Collision	Axis-aligned rectangle overlap tests using SDL_HasRectIntersectionFloat, for attacks, contact damage, pickups and projectiles.	Game::update

Level manager	When no enemies remain it waits, then spawns the next wave at random positions on the screen edge, at least 250 pixels from the player. Enemy count is base_enemies + enemies_per_room * room + difficulty bonus. After the last room it flags the boss.	LevelManager

Boss state machine	States: Chase, Telegraph, Charge, Recover. The phase comes from remaining health (above 66%, above 33%, below). Phases change chase speed, warning time, and ring size (8, 12, 16 projectiles). Phase 3 alternates a ring with a charge.	Boss::update

Projectiles	Move in a straight line and expire after 4 seconds. A roll lets the player pass through them.	Projectile, Game::update

Loot	When an enemy dies, Item::rollDrop rolls the drop chance, then the rarity from the configured shares, then potion or sword. A Pickup is created where the enemy died.	Item::rollDrop, Game::update

Inventory	Four slots in a fixed array. Empty slots hold an empty Item. Potions heal and are consumed (not used at full health). Swords are equipped, and the old sword returns to the slot.	Inventory, Player::useItem

Drop item	The DROP button or Q enables drop mode. A dropped item lands near the player and can't be collected for 1.5 seconds, so it isn't picked straight back up.	Game::dropFromSlot, Pickup

Data-driven values	Item power, drop chance, rarity shares, room count and enemy scaling are read from items.txt and levels.txt at startup.	GameData

Rendering	Sprites are BMP textures, with magenta treated as transparent. Missing sprites fall back to coloured squares. State is shown by tinting (roll, hit, boss telegraph).	Assets, each entity's render

| # | Test | Expected result | Checked |
|---|---|---|---|
| 1 | `items.txt` and `levels.txt` missing | Logs a message and uses defaults | [x] |
| 2 | Unknown rarity word (`sword epic 3`) | Line ignored, message logged | [x] |
| 3 | Text instead of a number (`weapon legendary abc`) | Line ignored, "not a number" logged | [x] |
| 4 | Negative number (`weapon common -4`) | Line ignored, "out of range" logged | [x] |
| 5 | Line with a missing value (`potion common`) | Line ignored, message logged | [x] |
| 6 | `drop_chance 2.5` (above 1) | Line ignored, default kept | [x] |
| 7 | Unknown setting name | Line ignored, message logged | [x] |
| 8 | `rare_share` + `legendary_share` above 1 | Both reset to defaults | [x] |
| 9 | `rooms 99` (above the maximum of 10) | Line ignored, previous value kept | [x] |
| 10 | Valid lines mixed in with bad ones | Valid lines are still applied | [x] |
| 11 | `save.txt` missing | Starts with zero progress | [x] |

These were run by hand in the game, and each gave the expected result.

| # | Test | Expected result | Checked |
|---|---|---|---|
| 14 | Sprite file missing from `assets` | Coloured squares are used, message logged | [x] |
| 15 | Exe launched by double-clicking from its own folder | Assets and data load correctly | [x] |
| 16 | Press 1 to 4 with empty slots | Nothing happens | [x] |
| 17 | Drop from an empty slot | Nothing happens | [x] |
| 18 | Use a potion at full health | Potion is not consumed | [x] |
| 19 | Pick up an item with a full inventory | Item stays on the floor | [x] |
| 20 | Walk back over a just-dropped item | Not collected until the 1.5 s delay ends | [x] |
| 21 | Hold every key and mouse button at once | No crash | [x] |
| 22 | Mash Space and the attack button | Cooldowns limit it | [x] |
| 23 | Die with items in the inventory | New run starts with an empty inventory | [x] |
| 24 | Press Esc during a run, then start again | Run restarts cleanly | [x] |
| 25 | Minimise, restore and close the window | No crash, clean shutdown | [x] |
| 26 | Build from a clean copy of the source folder | Builds with no errors | [x] |

Use of AI 

I used Claude as a coding assistant and assets generator.
I have the Claude pro plan witch I got and used only at the end of the project.

AI was used for:

Code generation and design advice: the class structure, the player, enemy, boss and loot systems, the menu, the data loader and the save file. 
Debugging: diagnosing the build and linker errors I hit.
Asset creation: the player, enemy, boss, potion and sword sprites and the dungeon background. These were generated as pixel art by a script, not hand-drawn.
Documentation: drafting the structure of this report.

AI Development log 

Claude: code, debugging, sprites and background, documentation draft.

Example prompts 

These are real prompts from my development, in my own words:

"can you make the enemys get knocked back on hit"
"i would lkie to make a start menu that can in creas the dificulty by addding more bots"
"now items"
"is there any other way" (after a build error with std::optional)
"can you create the sprites"
"now a back groud for the map"
"i want to add a button to drop items"

Evaluation 

what I achieved throughout this project. 
I completed a playable game where users can complete levels to eventually get to the boss fight to win the game. this game has a looting system to allow users to gear up before fighting the boss. And a menu to increase the difficulty if needed.

What I would do differently.
in my proposal I said I wanted the player to work through different rooms and as the game stands now, the player only stays in one arena. I believe adding the ability to move through rooms would give the game more depth by adding more time between levels making the game seem bigger and more complete.
there are also some features/bugs E.G. Clicking buttons in the HUD also triggers a weapon swing. for example clicking on a potion using the mouse instead of using number keys would trigger an attack. another known feature the equipped weapon can't be dropped directly, this doesn't directly effect the playability of thew game but is something to consider when making QOL updates.

What I would do if I had more time.
features I would add:
Armor system
sprite animations 
sound effects 
background music that changes with boss fight 
lighting effect that change with low health 

