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


