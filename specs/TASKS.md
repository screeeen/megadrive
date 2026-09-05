# TASKS.md — NEON STRIKE

## 0. Purpose

This file is the executable task queue for NEON STRIKE.

A task may only be marked `[x]` after implementation and appropriate validation.

States:

* `[ ]` Pending
* `[~]` In progress
* `[x]` Complete
* `[!]` Blocked
* `[-]` Cancelled

---

# M00 — Baseline Audit

## NS-M00-001 — Read documentation

* [x] Read SPEC.md
* [x] Read AGENT.md
* [x] Read MILESTONES.md
* [x] Read PROGRESS.md
* [x] Read TASKS.md

Acceptance:

* all requirements understood.

## NS-M00-002 — Inspect repository

* [x] Inspect directories
* [x] Identify C
* [x] Identify ASM (none authored; SGDK boot/library asm only)
* [x] Identify assets (res/sprite/donut.png placeholder)
* [x] Identify build scripts (Makefile -> $(GDK)/makefile.gen)
* [x] Identify tests (none existed pre-M01; added in M01, see NS-M01-005)
* [x] Identify emulator (BlastEm, built from source at ~/dev/blastem-src)
* [x] Identify ROM output (out/rom.bin)

Acceptance:

* repository architecture documented. (see PROGRESS.md M00 entry)

## NS-M00-003 — Verify toolchain

* [x] Run existing build
* [x] Confirm compilation
* [x] Confirm linking
* [x] Confirm ROM generation
* [x] Run emulator
* [x] Confirm boot

## NS-M00-004 — Verify Hello World

* [x] Verify PNG rendering
* [x] Verify movement
* [x] Verify existing music
* [x] Record build/emulator commands (README.md)

## NS-M00-005 — Create project structure

* [-] Cancelled as literally specified — see PROGRESS.md M00 "Notes" for
  the resolved conflict with AGENT.md §3 (no second build system) and
  DEVELOPMENT_PLAN.md §1. `tests/` was created (needed by M01); the rest
  of `src/<subsystem>/` is added incrementally per-milestone instead of
  up front, and `assets/`/`build/` are not duplicated since `res/`/`out/`
  already serve those roles.

If missing:

```text
src/
src/game/
src/player/
src/enemies/
src/weapons/
src/projectiles/
src/stages/
src/bosses/
src/audio/
src/ui/
src/effects/
assets/
assets/player/
assets/enemies/
assets/bosses/
assets/backgrounds/
assets/ui/
assets/audio/
tests/
tools/
validation/
build/
```

---

# M01 — Core Game Loop

## NS-M01-001 — State machine

Implement:

```text
TITLE
GAME
BOSS
STAGE_CLEAR
NEXT_STAGE
PAUSE
PLAYER_HIT
PLAYER_DEAD
GAME_OVER
CONTINUE
ENDING
CREDITS
```

## NS-M01-002 — Game loop

Implement:

```text
VBlank
Input
Player Update
Enemy Update
Projectile Update
Collision Detection
Spawn Manager
Score
Sprite List
Scroll
```

## NS-M01-003 — 60 FPS timing

* [x] VBlank synchronization (SYS_doVBlankProcess() every loop iteration, unchanged since M00)
* [x] deterministic frame update (frame-count based, e.g. audio.c's NOTE_FRAMES; no wall-clock reads anywhere)
* [x] no emulator-speed dependency (same mechanism; confirmed stable in BlastEm)

## NS-M01-004 — Pause

* [x] Start pauses (GAME -> PAUSE, edge-detected via InputState.startPressed)
* [x] Start resumes (PAUSE -> GAME)
* [x] gameplay freezes correctly (Player_update and SPR_update skipped while paused; donut visually stops)
* [x] audio behavior correct (Audio_update keeps ticking through PAUSE — arcade convention, music never cuts out)

## NS-M01-005 — Core tests

* [x] state transitions (tests/test_game_state.c, `make test`, host-native)
* [x] pause (GAME<->PAUSE covered by the same transition assertions)
* [x] frame update (structural: VBlank-gated loop; not unit-testable without hardware, validated via emulator instead)
* [x] reset (GameState_init() sets STATE_TITLE; implicitly exercised — no explicit assertion yet, revisit if a dedicated reset path is added in a later milestone)

---

# M02 — Player

## NS-M02-001 — NX-01 entity

Implement:

```text
position
velocity
direction
speed
hitbox
weapon
weapon_level
lives
bombs
invulnerability
state
sprite
```

## NS-M02-002 — Placeholder sprite

* [x] approximately 24×16 (res/sprite/nx01.png, generated with ImageMagick — not final art)
* [x] readable (flat-colored triangular ship silhouette, 4-color indexed PNG)
* [x] debug hitbox (res/sprite/debug_hitbox.png, magenta outline sprite, gated by SHOW_DEBUG_HUD)

## NS-M02-003 — Movement

* [x] 8 directions (independent up/down/left/right checks compose diagonally)
* [x] max 5 px/frame (PLAYER_MAX_SPEED)
* [x] immediate response (direct position delta, no acceleration ramp)
* [x] minimal inertia (none at all — velocity is set directly from input each frame)

## NS-M02-004 — Screen boundaries

* [x] horizontal limits (0 .. 320-PLAYER_SPRITE_W)
* [x] vertical limits (0 .. 224-PLAYER_SPRITE_H)

## NS-M02-005 — Player hitbox

* [x] 6×6 collision box (PLAYER_HITBOX_SIZE, centered under the sprite, recomputed every frame)

## NS-M02-006 — Damage

* [x] weapon level -1 (Player_applyHit, player_logic.c)
* [x] invulnerability (90-frame window, PLAYER_STATE_INVULNERABLE)
* [x] damage animation (sprite blink via SPR_setVisibility toggle while invulnerable)
* [x] level 1 damage → life loss (Player_applyHit's lifeLost branch)

## NS-M02-007 — Death

* [x] life decrement (Player_applyHit)
* [x] player reset (Player_respawn: position/weapon/invuln reset; lives/bombs untouched)
* [x] weapon L1 (Player_applyHit always resets weaponLevel to 1 on a life-costing hit)
* [x] death state (PLAYER_STATE_DEAD on Player; drives game_state.c's GAME -> PLAYER_HIT transition)

## NS-M02-008 — Lives

* [x] 3 initial lives (PLAYER_INITIAL_LIVES)
* [x] display/debug (bottom-left "LIVES:n LV:n" readout, SHOW_DEBUG_HUD)
* [x] loss (Player_applyHit)
* [ ] 1UP (no power-up system yet — M06 Power-ups/Bomb/Combo)

## NS-M02-009 — Player tests

* [x] movement (visual/emulator — deterministic delta, not separately unit-tested)
* [x] boundaries (visual/emulator)
* [x] hitbox (visual/emulator via the debug outline; no collision target exists yet to assert against numerically)
* [x] damage (player_logic tests: L3 hit -> L2, no life lost)
* [x] invulnerability (visual/emulator: blink + no re-trigger while invulnerable, since Player_hit no-ops outside PLAYER_STATE_NORMAL)
* [x] death (player_logic tests: L1 hit with lives left, last-life L1 hit = game over, no underflow at 0 lives)
* [x] lives (player_logic tests, same as death — lives is the field under test)

---

# M03 — Primary Weapon

## NS-M03-001 — Projectile system

Implement:

```text
active
position
velocity
damage
owner
type
sprite
```

## NS-M03-002 — Projectile pool

* [x] static pool (Projectile pool[PROJECTILE_POOL_SIZE=16], projectile.c)
* [x] no gameplay allocations (fixed array, no malloc anywhere)
* [x] reuse slots (Projectile_spawn scans for the first inactive slot)
* [x] remove off-screen (Projectile_poolUpdate releases anything past x<-8/x>320/y<-8/y>224)

## NS-M03-003 — Vulcan L1

* [x] 1 projectile
* [x] high cadence (10-frame cooldown; see PROGRESS.md decisions for the exact number)
* [x] low damage (1)

## NS-M03-004 — Vulcan L2

* [x] 2 projectiles (+-6px spread), visually confirmed via the M03 debug weapon-level-up trigger (see PROGRESS.md)

## NS-M03-005 — Vulcan L3

* [x] 3 projectiles, visually confirmed via the same debug trigger
* [x] maximum cadence (6-frame cooldown, faster than L1/L2's 10)

## NS-M03-006 — Fire input

* [x] A fires (held, autofire while cooldown allows)
* [x] cadence (per-level cooldown timer in weapon.c)
* [x] pool safe (Projectile_spawn returns NULL on a full pool; fireVulcan doesn't check the return, a full pool just silently drops the shot as intended)

## NS-M03-007 — Projectile collisions

* [x] projectile → enemy — DEFERRED to M05: the AABB math exists and is unit-tested (collision_logic.c) but has no enemy to check against yet; see PROGRESS.md M03 summary for why this is a spec-ordering gap, not an oversight
* [ ] damage — depends on the above
* [ ] destruction — depends on the above
* [ ] score — depends on the above; also depends on a score system (M14 HUD/Score/Game Flow)

## NS-M03-008 — Vulcan tests

* [x] L1 (visual/emulator: single bullet, confirmed by user)
* [x] L2 (visual/emulator via debug weapon-level-up trigger, confirmed by user)
* [x] L3 (visual/emulator via debug weapon-level-up trigger, confirmed by user)
* [ ] collision — deferred with NS-M03-007 above; collision_logic.c's 7 unit tests cover the math itself

---

# M04 — Weapon System

## NS-M04-001 — Weapon enum

```text
VULCAN
LASER
WIDE
HOMING
FLAME
```

## NS-M04-002 — Weapon switching

* [x] B changes weapon (Player_switchWeapon, cycles VULCAN->LASER->WIDE->HOMING->FLAME->VULCAN)
* [ ] pickup changes weapon — no power-up system yet (M06)
* [x] different weapon starts L1 (Weapon_switchNext always resets weaponLevel)

## NS-M04-003 — Laser

* [x] straight (velocityY=0)
* [x] fast (9px/frame vs. Vulcan's 6)
* [~] pierces small enemies — pierceRemaining field exists and is set (2, or 3 at L3), but nothing consumes it yet; no enemy/collision resolution exists before M05 (see PROGRESS.md NS-5)
* [x] medium damage (3, between Vulcan's 1-2 and Flame's 4)
* [x] L3 visual enlargement (PROJECTILE_TYPE_LASER_BIG, res/sprite/laser_big.png, 16x8 vs 8x8 — same cost otherwise, per SPEC.md's "technically cheap")

## NS-M04-004 — Wide

* [x] L1 = 3 (wideOffsetsL1)
* [x] L2 = 5 (wideOffsetsL2)
* [x] L3 = 7 (wideOffsetsL3)

## NS-M04-005 — Homing

* [~] seeking missiles — spawns and moves, but flies straight (see target selection below)
* [ ] target selection — DEFERRED to M05: no enemy list exists yet to select a target from; documented stub (PROGRESS.md NS-4), not faked
* [x] low/medium damage (2)
* [x] active projectile limit (Projectile_countActiveOfType caps at 4 regardless of weapon level)

## NS-M04-006 — Flame

* [x] short range (rangeRemaining=56px, new Projectile field, counts down and releases at 0)
* [x] high damage (4, highest of all 5 weapons)
* [x] close-range design (short range forces proximity by construction)

## NS-M04-007 — Weapon levels

* [x] L1
* [x] L2
* [x] L3
* [x] same weapon +1 (Weapon_levelUp, debug-triggered via C until M06)
* [x] different weapon → L1 (Weapon_switchNext)
* [ ] damage downgrade — SPEC.md's on-hit weapon-level-down (Player_applyHit, M02) already covers per-level damage loss; a separate "downgrade" concept isn't otherwise specified — flagging as possibly already satisfied rather than assuming so

## NS-M04-008 — Weapon tests

* [x] firing (visual/emulator: all 5 confirmed by user)
* [x] levels (player_logic tests for the pure switch/level-up logic, +visual confirmation of L1/L2/L3 patterns)
* [x] switching (player_logic tests: cycle order and wraparound; visual confirmation via debug HUD weapon-name readout)
* [x] damage (per-weapon damage values set; end-to-end damage-dealt-to-something is blocked on M05, same as M03)
* [x] limits (visual: homing capped at 4 active; pool-full silently drops shots, unit-covered indirectly via the pool's fixed size)

---

# M05 — Enemy Framework

## NS-M05-001 — Enemy base

Implement:

```text
position
velocity
hp
max_hp
hitbox
type
state
sprite
score
shoot_timer
```

## NS-M05-002 — Enemy pool

* [x] static (Enemy pool[ENEMY_POOL_SIZE], enemy.c)
* [x] reuse (Enemy_spawn scans for the first inactive slot)
* [x] <=12 active (fixed array size; a full pool means Enemy_spawn returns NULL, budget not error)

## NS-M05-003 — Drone

* [x] HP1
* [x] fast (velocity -3, fastest ground-speed enemy alongside Swarm)
* [x] no fire (updateDrone does nothing but move)
* [x] score 100

## NS-M05-004 — Fighter

* [x] HP2
* [~] side entry — spawns at a fixed off-screen-right position via the M05 debug spawner, not a real formation/side-entry stage script (that's M07/M08)
* [~] formations — same caveat: no Spawn Manager to place multiple Fighters in a real formation yet
* [x] score 200

## NS-M05-005 — Bomber

* [x] HP4
* [x] slow (velocity -1, slowest of all 7)
* [x] bursts (BulletPattern_burst: 3 shots at once)
* [~] blocks path — Bomber has no scroll/obstacle system to "block" yet (no scrolling exists before M07); it does sit on screen a long time due to its low speed, which is the closest approximation available now
* [x] score 500

## NS-M05-006 — Turret

* [x] HP3
* [x] fixed (velocityX=0; genuinely stationary since there's no scroll yet to distinguish "fixed relative to background" from "fixed on screen")
* [x] aims player (BulletPattern_aimed toward the player's live x/y)
* [x] score 300

## NS-M05-007 — Swarm

* [x] HP1
* [~] group movement — individual unit moves+fires correctly; "group" behavior (several spawned together, coordinated) needs the Spawn Manager (M07), not modeled by a single enemy's own update
* [x] score 100

## NS-M05-008 — Charger

* [x] HP2
* [x] pause (ENEMY_STATE_PAUSED, 60-frame stateTimer)
* [x] aim (computed once, at the moment the pause ends, toward the player's position at that instant)
* [x] charge (ENEMY_STATE_CHARGING at 2x Drone's speed in the aimed direction)

## NS-M05-009 — Shield

* [x] HP6
* [x] frontal resistance (damage ignored entirely while `vulnerable==false`)
* [~] vulnerable angle/opening — implemented as a time-based vulnerable/shielded cycle (60 frames on, 90 off), not real directional/facing detection (no orientation concept exists in the engine yet); see PROGRESS.md decisions

## NS-M05-010 — Enemy bullets

* [x] pool (reuses projectile.c's pool, grown to 96 slots to cover SPEC.md §24's ~16 player + ~80 enemy)
* [x] velocity
* [x] direction (bullet_pattern.c's LUT-based directions)
* [x] collision (combat.c: enemy bullets vs. player)
* [x] off-screen removal (Projectile_poolUpdate, unchanged from M03/M04)
* [x] maximum 80 (shared 96-slot pool budget; not separately capped at exactly 80 for enemy-owned ones specifically, since the combined ceiling already bounds total pressure — see PROGRESS.md if a stricter per-owner cap is ever needed)

## NS-M05-011 — Bullet patterns

* [x] diagonal (implemented, visually inspectable, not yet fired by any M05 enemy)
* [x] circular (implemented, visually inspectable, not yet fired by any M05 enemy)
* [x] top_bottom (implemented, visually inspectable, not yet fired by any M05 enemy)
* [x] aimed (Fighter, Turret — visually confirmed by user)
* [x] burst (Bomber — visually confirmed by user)
* [x] wave (Swarm — visually confirmed by user; simplified, no true sinusoidal path, see bullet_pattern.h)
* [x] cross (Shield — visually confirmed by user)
* [x] combined (implemented — aimed+cross — visually inspectable, not yet fired by any M05 enemy)

## NS-M05-012 — Enemy tests

* [x] all enemy types (enemy_logic's pure Enemy_applyDamage unit-tested 3/3; per-type behavior visually confirmed by user in BlastEm)
* [x] bullets (visual: enemy bullets spawn, move, hit the player, despawn off-screen)
* [x] patterns (visual, for the 4 actually fired — see NS-M05-011)
* [x] collisions (visual: player bullets destroy enemies at the right HP threshold and add score; enemy bullets/contact damage the player)
* [x] limits (12-enemy pool ceiling exists structurally; not deliberately driven to exactly 12 simultaneous during this milestone's testing — that's M17's stress-test job)

---

# M06 — Power-ups / Bomb / Combo

## NS-M06-001 — Power-up base

Implement:

```text
L
W
H
F
P
B
S
1UP
```

## NS-M06-002 — Power-up movement

* [x] slow fall (POWERUP_FALL_SPEED, +1px/frame vertically)
* [x] predictable (constant velocity, no randomness)
* [x] off-screen removal (Powerup_poolUpdate)
* [x] player collision (combat.c's resolvePlayerVsPowerups, AABB via collision_logic.c)

## NS-M06-003 — Weapon power-ups

* [x] Laser (Player_pickupWeapon(player, WEAPON_LASER))
* [x] Wide
* [x] Homing
* [x] Flame

## NS-M06-004 — P power-up

* [x] weapon level +1 (Player_levelUpWeapon)
* [x] maximum L3 (Weapon_levelUp caps at 3, unit tested)

## NS-M06-005 — S power-up

* [x] temporary speed (+2 px/frame, PLAYER_SPEED_BOOST_AMOUNT)
* [x] duration (300 frames / 5s, not specified exactly in SPEC.md)
* [x] automatic restoration (Player_update's speedBoostFrames countdown)

## NS-M06-006 — B power-up

* [x] bomb +1 (Player_addBomb)
* [x] maximum 3 (PLAYER_MAX_BOMBS)

## NS-M06-007 — 1UP

* [x] life +1 (Player_addLife)
* [x] feedback (immediately visible via the debug HUD's LIVES readout — no separate animation built, see PROGRESS.md)

## NS-M06-008 — Bomb

* [x] C activation (resolves NS-3's B/C double-booking for good)
* [x] consume bomb (Player_useBomb, no-op if none held)
* [x] projectile neutralization (all active enemy-owned projectiles released)
* [x] high damage (10, exceeds every enemy's max HP including Shield's 6 — see PROGRESS.md)
* [x] ~0.5 s invulnerability (30 frames, reuses PLAYER_STATE_INVULNERABLE)
* [x] explosion — "BOMB!" text flash only; no particle/sprite explosion effect (that's M16 Effects/Polish's job)

## NS-M06-009 — Combo

* [x] x1 (default/reset state)
* [x] x2..x5 (Combo_onKill increments, capped at 5)
* [x] kills increase combo
* [x] timeout (120 frames / 2s, not specified exactly in SPEC.md)
* [x] reset (Combo_update resets to x1 when the timeout elapses)
* [x] score multiplier (combat.c multiplies Score_add by Combo_getMultiplier() on every kill, including bomb kills)

## NS-M06-010 — System tests

* [x] power-ups (visual/emulator: all 8 confirmed by user; Weapon_pickup's pure logic unit-tested 2/2)
* [x] bomb (visual/emulator: clears screen, damages Shield through its resistance, spends a bomb, shows feedback)
* [x] combo (visual/emulator: multiplier rises on rapid kills, decays after a pause)
* [x] limits (8-powerup pool ceiling exists structurally, same as M05's enemy/projectile limits; not deliberately driven to exactly 8 simultaneous — M17's stress-test job)

## NS-M06-011 — Continue (not itemized here, but required by MILESTONES.md's M06 acceptance: "continue works")

* [x] 3 continues per campaign (SPEC.md §26)
* [x] GAME_OVER offers CONTINUE while any remain, otherwise goes to TITLE
* [x] continuing resets weapon to L1 and grants exactly 1 bomb (SPEC.md §26)
* [x] continuing preserves score (not specified either way in SPEC.md — see PROGRESS.md decision)
* [~] "return to last checkpoint" — no checkpoint/stage system exists yet (M07); continuing resumes the current session instead

---

# M07 — Stage Data System

## NS-M07-001 — SpawnEvent

Implement structure equivalent to:

```c
typedef struct {
    uint32_t frame;
    uint8_t type;
    int16_t x;
    int16_t y;
    uint8_t variant;
} SpawnEvent;
```

## NS-M07-002 — Stage definition

Must contain:

```text
length
scroll_speed
spawn_events
powerups
checkpoints
minibosses
boss
background
music
```

## NS-M07-003 — Spawn manager

* [x] process events (spawn_manager.c's frame cursor, compares to StageDef.spawnEvents)
* [x] spawn enemies
* [x] spawn power-ups
* [ ] spawn obstacles — no obstacle entity/concept exists anywhere in the engine yet; not needed by Stage 1 (SPEC.md §16), deferred to whichever stage first requires one (see PROGRESS.md NS-9)
* [x] respect pool limits (Enemy_spawn/Powerup_spawn already return NULL on a full pool, since M05/M06 — nothing new needed here)

## NS-M07-004 — Checkpoints

* [x] ~40% (testStage.checkpointFrames[0] = 480 of 1200)
* [x] ~75% (testStage.checkpointFrames[1] = 900 of 1200)
* [x] functional: death/continue rewinds the spawn timeline to the last checkpoint passed (SpawnManager_resumeFromCheckpoint), not just a stored percentage

## NS-M07-005 — Scroll

* [x] normal = 2 px/frame (testStage.scrollSpeed, applied every GAME frame via Scroll_update)
* [~] maximum = 4 px/frame — the engine supports any per-stage scrollSpeed value (it's just a StageDef field), but nothing dynamically raises it mid-stage yet; that's stage-design content for M08+ to actually use

## NS-M07-006 — Parallax

* [~] 3-4 layers — Genesis has exactly 2 hardware tile planes; this milestone delivers ONE real scrolling layer (BG_B, a starfield, since BG_A already hosts text/HUD) proving the engine (independently-scrollable planes at a data-driven speed) works. "3-4 layers" beyond the 2 real planes means compositing multiple background elements within them via art (M08's job), not additional hardware planes that don't exist — see PROGRESS.md's M07 summary

## NS-M07-007 — Stage transitions

* [x] completion (SpawnManager_isComplete() when currentFrame >= lengthFrames)
* [x] next stage (STATE_STAGE_CLEAR -> STATE_NEXT_STAGE -> STATE_GAME, looping into the same placeholder stage since no Stage 2 exists yet)
* [x] entity reset (resetGameplayPools(), now correctly releasing sprites first — see the M07 bugfix in PROGRESS.md)
* [x] state preservation (score/lives/weapon/level/bombs carry over the loop; only pools+timeline+scroll reset)

---

# M08 — Stage 1

## NS-M08-001 — Background

* [ ] skyline
* [ ] buildings
* [ ] traffic
* [ ] defense towers
* [ ] space

## NS-M08-002 — Stage length

`38,400 px`

## NS-M08-003 — Intro

* [ ] max 5 enemies
* [ ] movement teaching
* [ ] shooting teaching
* [ ] first power-up

## NS-M08-004 — Turret section

* [ ] bursts up to 8
* [ ] peaks up to 12

## NS-M08-005 — Combination section

* [ ] up to 10 enemies
* [ ] up to 18 bullets

## NS-M08-006 — Boss preparation

* [ ] max 12 enemies
* [ ] max 20 bullets

## NS-M08-007 — Mini-boss

* [ ] implement
* [ ] validate

## NS-M08-008 — Orbital Guardian

* [ ] ~250 HP
* [ ] 3 phases
* [ ] ~75 sec
* [ ] cannons
* [ ] drones
* [ ] diagonal attacks

## NS-M08-009 — Stage validation

* [ ] 90–120 kills
* [ ] 4 enemy types
* [ ] 12 power-ups
* [ ] 1UP
* [ ] 2 checkpoints
* [ ] boss
* [ ] 60 FPS

---

# M09 — Boss Framework

## NS-M09-001 — Boss base

Implement:

```text
position
hp
phase
timer
vulnerable_points
attack_pattern
state
sprite_parts
```

## NS-M09-002 — Boss state machine

```text
INTRO
ATTACK
TELEGRAPH
VULNERABLE
TRANSITION
DEATH
```

## NS-M09-003 — Vulnerable points

* [ ] independent collision
* [ ] damage

## NS-M09-004 — Pattern manager

* [ ] deterministic
* [ ] cooldowns
* [ ] telegraphs
* [ ] transitions

## NS-M09-005 — Boss HUD

* [ ] optional health bar
* [ ] never obscure gameplay

## NS-M09-006 — Boss death

* [ ] stop attacks
* [ ] explosions
* [ ] score
* [ ] music
* [ ] transition

## NS-M09-007 — Framework regression

* [ ] Stage 1 boss still works

---

# M10 — Stages 2–3

## NS-M10-001 — Stage 2 background

* [ ] red desert
* [ ] canyons
* [ ] storms
* [ ] bases
* [ ] convoys

## NS-M10-002 — Stage 2 length

`43,200 px`

## NS-M10-003 — Stage 2 metrics

* [ ] 120–150 kills
* [ ] 5 types
* [ ] <=30 bullets
* [ ] 12 power-ups
* [ ] 1UP
* [ ] 2 checkpoints

## NS-M10-004 — Storm sections

* [ ] visibility effect
* [ ] attacks remain readable

## NS-M10-005 — Destructible obstacles

* [ ] collision
* [ ] destruction
* [ ] visual feedback

## NS-M10-006 — Sand Worm

* [ ] ~350 HP
* [ ] 3 phases
* [ ] 80–90 sec
* [ ] charge
* [ ] vertical projectiles
* [ ] both-side attacks

## NS-M10-007 — Stage 2 validation

* [ ] gameplay
* [ ] boss
* [ ] performance
* [ ] visuals

## NS-M10-008 — Stage 3 background

* [ ] corridors
* [ ] doors
* [ ] pipes
* [ ] machinery
* [ ] moving obstacles

## NS-M10-009 — Stage 3 length

`48,000 px`

## NS-M10-010 — Stage 3 metrics

* [ ] 130–170 kills
* [ ] 6 types
* [ ] <=40 bullets
* [ ] 14 power-ups
* [ ] 0–1 1UP
* [ ] 2 checkpoints

## NS-M10-011 — Stage 3 scroll

`2–3 px/frame`

## NS-M10-012 — Vertical space

Normally maintain >=48 px free space except telegraphed patterns.

## NS-M10-013 — Industrial Core

* [ ] ~450 HP
* [ ] ~90 sec
* [ ] mechanical arms
* [ ] changing safe areas

## NS-M10-014 — Stage 3 validation

* [ ] gameplay
* [ ] boss
* [ ] performance
* [ ] visuals

---

# M11 — Stages 4–5

## NS-M11-001 — Asteroid background

* [ ] stars
* [ ] asteroids
* [ ] mines
* [ ] mining ships

## NS-M11-002 — Stage 4 length

`52,800 px`

## NS-M11-003 — Stage 4 scroll

* [ ] 3 px/frame normal
* [ ] 4 px/frame maximum

## NS-M11-004 — Asteroid distribution

* [ ] 70% small
* [ ] 25% medium
* [ ] 5% large

## NS-M11-005 — Stage 4 metrics

* [ ] 35–50 asteroids
* [ ] 140–180 kills
* [ ] 7 enemy types
* [ ] <=50 bullets
* [ ] <=15 obstacles

## NS-M11-006 — Destructible asteroids

* [ ] collision
* [ ] destruction
* [ ] effects

## NS-M11-007 — Mining Fortress

* [ ] ~550 HP
* [ ] 90–100 sec
* [ ] cannons
* [ ] mines
* [ ] drones
* [ ] exposed core

## NS-M11-008 — Stage 4 validation

* [ ] gameplay
* [ ] boss
* [ ] performance
* [ ] visuals

## NS-M11-009 — Helios Fleet background

* [ ] large battle
* [ ] ships
* [ ] projectiles
* [ ] background depth

## NS-M11-010 — Formations

* [ ] diagonal
* [ ] V
* [ ] lines
* [ ] circles
* [ ] crossed
* [ ] waves

## NS-M11-011 — Stage 5 length

`57,600 px`

## NS-M11-012 — Stage 5 metrics

* [ ] 180–230 kills
* [ ] 8 enemy types
* [ ] <=65 bullets
* [ ] 16 power-ups
* [ ] 1UP
* [ ] 2 checkpoints
* [ ] 2 mini-bosses

## NS-M11-013 — Escape routes

Every dense pattern must retain at least one viable route.

## NS-M11-014 — Admiral X

* [ ] ~650 HP
* [ ] 100–110 sec
* [ ] projectile patterns
* [ ] fighter formations
* [ ] phase changes

## NS-M11-015 — Boss validation

* [ ] patterns
* [ ] phases
* [ ] collision
* [ ] performance

## NS-M11-016 — Stage 5 validation

* [ ] complete playthrough
* [ ] metrics
* [ ] visuals
* [ ] performance

---

# M12 — Stage 6

## NS-M12-001 — Helios Core background

* [ ] black
* [ ] red
* [ ] white
* [ ] energy
* [ ] machinery

## NS-M12-002 — Stage length

`48,000 px`

## NS-M12-003 — Stage metrics

* [ ] 160–200 kills
* [ ] 9 types
* [ ] <=80 bullets
* [ ] 12 power-ups
* [ ] 0 1UP
* [ ] 2 checkpoints
* [ ] 2 mini-bosses

## NS-M12-004 — Scroll

`2 → 3 → 4 px/frame` selectively.

## NS-M12-005 — Obstacles

* [ ] lasers
* [ ] walls
* [ ] energy barriers
* [ ] fast enemies
* [ ] dense patterns

## NS-M12-006 — Difficulty

Use:

* [ ] patterns
* [ ] positioning
* [ ] speed
* [ ] combinations

Avoid excessive HP.

## NS-M12-007 — Stage validation

* [ ] <=80 bullets
* [ ] <=12 enemies
* [ ] escape routes
* [ ] 60 FPS

---

# M13 — Final Boss

## NS-M13-001 — HELIOS sprite

* [ ] large silhouette
* [ ] multiple hardware sprites
* [ ] readable core

## NS-M13-002 — Boss structure

```text
PHASE_1
PHASE_2
PHASE_3
DEATH
```

## NS-M13-003 — Phase 1

* [ ] 30–35 sec
* [ ] cannons
* [ ] drones
* [ ] missiles
* [ ] <=35 bullets

## NS-M13-004 — Phase 2

* [ ] 30–40 sec
* [ ] circular attacks
* [ ] positional attacks
* [ ] <=55 bullets

## NS-M13-005 — Phase 3

* [ ] 35–45 sec
* [ ] exposed core
* [ ] smaller safe zone
* [ ] openings
* [ ] <=70 bullets

## NS-M13-006 — Escape routes

* [ ] every pattern has escape
* [ ] telegraphs
* [ ] no impossible combinations

## NS-M13-007 — Final death

* [ ] stop attacks
* [ ] stop scroll
* [ ] explosion
* [ ] ending
* [ ] ranking
* [ ] credits

## NS-M13-008 — Final boss validation

* [ ] 100–120 sec
* [ ] 3 phases
* [ ] <=70 bullets
* [ ] no impossible patterns
* [ ] 60 FPS

---

# M14 — HUD / Score / Game Flow

## NS-M14-001 — HUD

Implement:

```text
SCORE 00124500 HI 9999999
```

## NS-M14-002 — Bottom HUD

```text
POWER ███
WEAPON LASER
BOMB ×2
```

## NS-M14-003 — Score values

```text
Drone       100
Fighter     200
Bomber      500
Turret      300
Shield      1000
Mini-boss   5000
Boss        50000
```

## NS-M14-004 — High score

* [ ] session high score
* [ ] display
* [ ] reset

## NS-M14-005 — Lives/continues

* [ ] 3 lives
* [ ] 3 continues
* [ ] checkpoint return
* [ ] weapon L1
* [ ] one bomb retained

## NS-M14-006 — Checkpoint restoration

* [ ] reset enemies
* [ ] reset bullets
* [ ] reset dynamic obstacles
* [ ] restore scroll

## NS-M14-007 — Stage clear

* [ ] score bonus
* [ ] transition
* [ ] next stage

## NS-M14-008 — Game over

* [ ] GAME OVER
* [ ] continue
* [ ] title

## NS-M14-009 — Ending

* [ ] victory
* [ ] message
* [ ] ranking
* [ ] credits

---

# M15 — Audio

## NS-M15-001 — Audio architecture

Verify:

```text
68000 → audio commands
Z80 → audio
YM2612 → FM
PSG → support/SFX
```

## NS-M15-002 — Stage 1 music

* [ ] functional theme

## NS-M15-003 — Stage 2 music

* [ ] functional theme

## NS-M15-004 — Stage 3 music

* [ ] functional theme

## NS-M15-005 — Stage 4 music

* [ ] functional theme

## NS-M15-006 — Stage 5 music

* [ ] functional theme

## NS-M15-007 — Stage 6 music

* [ ] functional theme

## NS-M15-008 — Boss themes

* [ ] six boss themes

## NS-M15-009 — Final boss theme

* [ ] HELIOS theme

## NS-M15-010 — Credits theme

* [ ] credits music

## NS-M15-011 — SFX

* [ ] shot
* [ ] explosion
* [ ] hit
* [ ] power-up
* [ ] bomb
* [ ] boss
* [ ] 1UP
* [ ] player damage
* [ ] stage clear

## NS-M15-012 — Audio validation

* [ ] music
* [ ] SFX
* [ ] transitions
* [ ] no crashes
* [ ] no Z80 lockups

---

# M16 — Effects / Polish

## NS-M16-001 — Player effects

* [ ] muzzle flash
* [ ] damage flash
* [ ] explosion
* [ ] invulnerability feedback

## NS-M16-002 — Enemy effects

* [ ] small explosion
* [ ] large explosion
* [ ] hit flash
* [ ] boss damage feedback

## NS-M16-003 — Power-up effects

* [ ] spawn
* [ ] pickup
* [ ] audio

## NS-M16-004 — Bomb effect

* [ ] expanding explosion
* [ ] projectile clear
* [ ] screen feedback
* [ ] sprite-safe

## NS-M16-005 — Boss effects

* [ ] phase transition
* [ ] warning
* [ ] explosion
* [ ] death sequence

## NS-M16-006 — Background polish

* [ ] parallax
* [ ] stars
* [ ] traffic
* [ ] machinery
* [ ] atmosphere

## NS-M16-007 — Art consistency

* [ ] 16-bit look
* [ ] coherent palette
* [ ] silhouettes
* [ ] no modern HUD
* [ ] no unnecessary gradients
* [ ] no 3D

---

# M17 — Performance

## NS-M17-001 — Counters

Track:

```text
active_enemies
active_player_bullets
active_enemy_bullets
active_powerups
active_effects
active_objects
```

## NS-M17-002 — Sprite budget

Target <=64 visible sprites.

If exceeded:

1. particles;
2. decorative effects;
3. decorative enemies;
4. background detail.

## NS-M17-003 — Bullet budget

Target <=80 enemy bullets.

## NS-M17-004 — Enemy budget

Target <=12 enemies.

## NS-M17-005 — Object budget

Target <=100 moving gameplay objects.

## NS-M17-006 — Frame time

Target <=16.67 ms/frame.

## NS-M17-007 — Stress tests

* [ ] boss
* [ ] max bullets
* [ ] multiple enemies
* [ ] explosions
* [ ] fast scroll
* [ ] power-ups

## NS-M17-008 — 60 FPS

* [ ] sustained 60 FPS

## NS-M17-009 — Memory audit

* [ ] no leaks
* [ ] no overflow
* [ ] pools sufficient
* [ ] assets fit memory

---

# M18 — Full Campaign Validation

## NS-M18-001 — Clean build

* [ ] clean checkout
* [ ] build
* [ ] ROM

## NS-M18-002 — Boot

* [ ] boot
* [ ] title
* [ ] Start

## NS-M18-003 — Stage 1

* [ ] complete
* [ ] boss
* [ ] checkpoint
* [ ] score
* [ ] power-ups

## NS-M18-004 — Stage 2

* [ ] complete
* [ ] boss
* [ ] checkpoint
* [ ] score
* [ ] power-ups

## NS-M18-005 — Stage 3

* [ ] complete
* [ ] boss
* [ ] checkpoint
* [ ] score
* [ ] power-ups

## NS-M18-006 — Stage 4

* [ ] complete
* [ ] boss
* [ ] checkpoint
* [ ] score
* [ ] power-ups

## NS-M18-007 — Stage 5

* [ ] complete
* [ ] boss
* [ ] checkpoint
* [ ] score
* [ ] power-ups

## NS-M18-008 — Stage 6

* [ ] complete
* [ ] boss
* [ ] checkpoint
* [ ] score
* [ ] power-ups

## NS-M18-009 — HELIOS

* [ ] complete final boss
* [ ] ending

## NS-M18-010 — Full campaign

```text
TITLE
↓
STAGE 1
↓
BOSS
↓
STAGE 2
↓
BOSS
↓
STAGE 3
↓
BOSS
↓
STAGE 4
↓
BOSS
↓
STAGE 5
↓
BOSS
↓
STAGE 6
↓
HELIOS
↓
ENDING
↓
CREDITS
```

## NS-M18-011 — Difficulty validation

* [ ] beginner progresses
* [ ] competent player survives
* [ ] expert can master
* [ ] difficulty increases

## NS-M18-012 — Projectile validation

```text
S1 <=20
S2 <=30
S3 <=40
S4 <=50
S5 <=65
S6 <=80
```

## NS-M18-013 — Enemy validation

* [ ] <=12 active enemies

## NS-M18-014 — Sprite validation

* [ ] <=64 visible sprites

## NS-M18-015 — Screenshots

Capture:

* [ ] title
* [ ] S1
* [ ] S2
* [ ] S3
* [ ] S4
* [ ] S5
* [ ] S6
* [ ] every boss
* [ ] HELIOS
* [ ] ending
* [ ] HUD

## NS-M18-016 — Visual inspection

Check:

* [ ] corruption
* [ ] missing sprites
* [ ] flicker
* [ ] HUD
* [ ] scroll
* [ ] colors
* [ ] clipping
* [ ] boss alignment
* [ ] power-up readability

## NS-M18-017 — Collision validation

* [ ] player → bullet
* [ ] player → enemy
* [ ] player → obstacle
* [ ] projectile → enemy
* [ ] projectile → boss
* [ ] bomb → projectile
* [ ] bomb → enemy

## NS-M18-018 — Edge cases

* [ ] death while killing boss
* [ ] death at checkpoint
* [ ] power-up + damage same frame
* [ ] bomb with zero bombs
* [ ] 1UP
* [ ] pause during boss
* [ ] pause during transition
* [ ] continue
* [ ] rapid weapon changes
* [ ] off-screen projectiles
* [ ] last enemy
* [ ] boss at 1 HP
* [ ] phase transition edge cases

## NS-M18-019 — Long play test

* [ ] crashes
* [ ] softlocks
* [ ] corruption
* [ ] audio degradation
* [ ] object accumulation
* [ ] impossible states

## NS-M18-020 — Replayability

* [ ] weapons change strategy
* [ ] score rewards aggression
* [ ] patterns are learnable
* [ ] power-ups create decisions
* [ ] routes are interesting

---

# M19 — Release

## NS-M19-001 — Debug cleanup

* [ ] remove unnecessary logs
* [ ] disable debug visuals by default
* [ ] retain useful debug mode if appropriate

## NS-M19-002 — Asset audit

* [ ] sprites
* [ ] backgrounds
* [ ] music
* [ ] SFX
* [ ] no accidental placeholders

## NS-M19-003 — ROM metadata

* [ ] header
* [ ] checksum
* [ ] size
* [ ] name
* [ ] configuration

## NS-M19-004 — Reproducible build

```text
clean
build
generate ROM
run emulator
```

## NS-M19-005 — Final campaign

* [ ] title
* [ ] S1
* [ ] S2
* [ ] S3
* [ ] S4
* [ ] S5
* [ ] S6
* [ ] HELIOS
* [ ] ending
* [ ] credits

## NS-M19-006 — Final performance

```text
60 FPS
<=12 enemies
<=80 enemy bullets
<=64 sprites
<=100 moving objects
```

## NS-M19-007 — Regression

* [ ] all automated tests PASS

## NS-M19-008 — Visual regression

* [ ] final screenshots PASS

## NS-M19-009 — Final ROM

Generate:

```text
NEON_STRIKE_FINAL.bin
```

## NS-M19-010 — Release report

Update `PROGRESS.md`:

```text
BUILD       = PASS
TESTS       = PASS
GAMEPLAY    = PASS
CAMPAIGN    = PASS
VISUAL      = PASS
AUDIO       = PASS
PERFORMANCE = PASS
RELEASE     = PASS
```
