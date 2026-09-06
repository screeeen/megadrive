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

* [~] skyline — approximated by the repeating building-silhouette tile, not a distinct skyline layer (Genesis has only 2 tile planes; BG_A is committed to text/HUD)
* [x] buildings — bg_building.png, an 8x8 silhouette tile mixed into the BG_B scroll
* [ ] traffic — not implemented, no moving-foreground-detail concept exists
* [ ] defense towers — not implemented; no stage-embedded (non-enemy) obstacle concept exists yet (see PROGRESS.md NS-9)
* [x] space — the original testStage starfield, reused as the base layer

## NS-M08-002 — Stage length

`38,400 px` — [x] exact (stage1.lengthFrames = 19200 @ 2px/frame)

## NS-M08-003 — Intro

* [x] max 5 enemies — 0-25% band caps at ~5 simultaneous (Drone/Fighter only)
* [x] movement teaching — simple single/triple Drone formations
* [x] shooting teaching — Fighters return fire, teaching dodge-and-shoot
* [x] first power-up — POWERUP_P at frame 840

## NS-M08-004 — Turret section

* [~] bursts up to 8 — design target baked into stage1_data.c's 25-50% band spacing; not hardware-enforced or measured against, see PROGRESS.md's stage1_data.c header note (honest pacing placeholder, not verified exact compliance)
* [~] peaks up to 12 — same caveat

## NS-M08-005 — Combination section

* [~] up to 10 enemies — 50-75% band's Fighter+Turret+Swarm combos target this; same measured-vs-designed caveat as NS-M08-004
* [~] up to 18 bullets — same caveat

## NS-M08-006 — Boss preparation

* [~] max 12 enemies — 75-100% band (= ENEMY_POOL_SIZE, the engine's hard global cap); same measured-vs-designed caveat
* [~] max 20 bullets — same caveat

## NS-M08-007 — Mini-boss

* [x] implement — ENEMY_MINIBOSS (enemy.h/.c: 15 HP, 5000 score, burst fire), placed at frame 16200
* [x] validate — confirmed by user: spawns, fires, dies for its score

## NS-M08-008 — Orbital Guardian

* [x] ~250 HP — ORBITAL_GUARDIAN_MAX_HP = 250 (game_state.c)
* [x] 3 phases — boss.c phase 1/2/3, thresholds at 2/3 and 1/3 HP (boss_logic.c, unit tested)
* [~] ~75 sec — not measured/tuned against a clock; ATTACK/TELEGRAPH/VULNERABLE frame counts were chosen values (see PROGRESS.md DECISIONS), not derived from this target
* [x] cannons — phase 1: frontal diagonal cannon fire
* [x] drones — phase 2: spawns ENEMY_DRONE
* [x] diagonal attacks — phase 3: double diagonal fire

## NS-M08-009 — Stage validation

* [ ] 90–120 kills — not counted/verified (44 enemy spawn events exist, not all necessarily killed by every playthrough)
* [x] 4 enemy types — Drone/Fighter/Turret/Swarm all used in stage1Events (Bomber/Charger/Shield exist but aren't placed in Stage 1's table)
* [ ] 12 power-ups — 9 SPAWN_TYPE_POWERUP events in stage1Events, not 12; under target, not yet revisited
* [x] 1UP — one POWERUP_1UP event at frame 4800
* [x] 2 checkpoints — checkpointFrames = { 7680, 14400 } (~40%/~75%)
* [x] boss — Orbital Guardian, full state machine, confirmed working end-to-end
* [ ] 60 FPS — never measured (see PROGRESS.md Performance Notes: UNKNOWN, M17's job)

---

# M09 — Boss Framework

## NS-M09-001 — Boss base

Implement: [x] all fields present on the Boss struct (inc/boss.h)

```text
position           -> x, y
hp                 -> hp, maxHp
phase              -> phase
timer              -> timer
vulnerable_points   -> def->vulnerablePoints/vulnerablePointCount (BossDef, not Boss itself — see PROGRESS.md M09)
attack_pattern      -> def->phaseAttacks/phaseCount (BossDef)
state               -> state (BossState)
sprite_parts        -> spriteParts[1]
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

* [x] independent collision — each of BossDef's vulnerablePoints is checked separately in combat.c's resolvePlayerBulletsVsBoss
* [x] damage — shared HP pool (Boss_applyDamage), not per-point HP (see PROGRESS.md DECISIONS for why that's a deliberate simplification)

## NS-M09-004 — Pattern manager

* [x] deterministic — fixed frame counters (ATTACK_FIRE_EVERY, etc.), no RNG
* [x] cooldowns — ATTACK_FRAMES/ATTACK_FIRE_EVERY
* [x] telegraphs — BOSS_STATE_TELEGRAPH (blink) before every VULNERABLE window
* [x] transitions — BOSS_STATE_TRANSITION on every phase change

## NS-M09-005 — Boss HUD

* [x] optional health bar — text readout ("BOSS HP:xxx/xxx"), not a graphical bar; added at M08, gated by nothing (always shown during STATE_BOSS) — a real graphical bar is later-milestone polish, see PROGRESS.md
* [x] never obscure gameplay — drawn at the top-left corner (row 1), clear of the play area and away from other HUD text

## NS-M09-006 — Boss death

* [x] stop attacks — DEATH state fires nothing, no further ATTACK/TELEGRAPH/VULNERABLE cycling
* [~] explosions — no dedicated explosion sprite/particle system exists; a fast strobe blink of the boss's own sprite is the honest placeholder added this milestone (see PROGRESS.md)
* [x] score — boss->score awarded (with combo multiplier) once Boss_isEncounterOver()
* [ ] music — no distinct boss-death/victory music exists; only M15's placeholder arpeggio melody plays throughout, unchanged on death
* [x] transition — STATE_BOSS -> STATE_STAGE_CLEAR

## NS-M09-007 — Framework regression

* [x] Stage 1 boss still works — confirmed after the BossDef refactor; also where a real crash (NS-12) was found and fixed this milestone (see PROGRESS.md)

---

# M10 — Stages 2–3

## NS-M10-001 — Stage 2 background

* [x] red desert — dune silhouette tile, backgroundId=2 (scroll.c)
* [ ] canyons — not implemented
* [ ] storms — no visibility/fog mechanic exists in the engine (NS-15)
* [ ] bases — not implemented
* [ ] convoys — not implemented

## NS-M10-002 — Stage 2 length

`43,200 px` — [x] exact (stage2.lengthFrames = 21600 @ 2px/frame)

## NS-M10-003 — Stage 2 metrics

* [ ] 120–150 kills — 36 enemy spawn events in stage2_data.c, not counted against actual kills; under the design target (same honest gap as Stage 1's own table, see PROGRESS.md)
* [x] 5 types — Drone/Fighter/Turret/Swarm/Charger, matching the target count exactly
* [ ] <=30 bullets — design assumption baked into event spacing, not measured or hardware-enforced per stage
* [ ] 12 power-ups — 9 SPAWN_TYPE_POWERUP events in stage2_data.c, under target
* [x] 1UP — one POWERUP_1UP event
* [x] 2 checkpoints — checkpointFrames = { 8640, 16200 }

## NS-M10-004 — Storm sections

* [ ] visibility effect — no visibility/fog mechanic exists (NS-15)
* [ ] attacks remain readable — N/A, no storm effect exists to keep readable

## NS-M10-005 — Destructible obstacles

* [ ] collision — no obstacle entity/concept exists in the engine (same root gap as NS-9)
* [ ] destruction — N/A
* [ ] visual feedback — N/A

## NS-M10-006 — Sand Worm

* [x] ~350 HP — sandWormDef.maxHp = 350 (boss_data.c)
* [x] 3 phases — same generic boss.c state machine as Orbital Guardian, phase thresholds at 2/3 and 1/3 HP
* [~] 80–90 sec — not measured/tuned against a clock, same caveat as Orbital Guardian's own duration target
* [x] charge — phase1: BulletPattern_burst, approximated (see PROGRESS.md NS-13 — the boss doesn't move, so a real charge/dash isn't modeled)
* [x] vertical projectiles — phase2: BulletPattern_topBottom, direct match
* [~] both-side attacks — phase3: BulletPattern_cross (4 directions), an approximation since the boss has no left/right positioning to attack "from both sides" of (NS-13)

## NS-M10-007 — Stage 2 validation

* [x] gameplay — confirmed by user (background/boss change correctly on Stage 1 -> Stage 2)
* [x] boss — Sand Worm fight confirmed working (intro/phases/attacks/death) as part of the full 3-boss loop test
* [ ] performance — not stress-tested (M17's job)
* [x] visuals — dune background and Sand Worm sprite both confirmed by user

## NS-M10-008 — Stage 3 background

* [x] corridors — approximated by vertical pipe columns, backgroundId=3 (scroll.c)
* [ ] doors — not implemented
* [x] pipes — the pipe tile itself is the implemented element
* [ ] machinery — not implemented beyond the pipe tile
* [ ] moving obstacles — no obstacle entity/concept exists in the engine (NS-9/NS-15)

## NS-M10-009 — Stage 3 length

`48,000 px` — [x] exact (stage3.lengthFrames = 24000 @ 2px/frame)

## NS-M10-010 — Stage 3 metrics

* [ ] 130–170 kills — 38 enemy spawn events in stage3_data.c, not counted against actual kills; under the design target
* [x] 6 types — Fighter/Turret/Swarm/Charger/Shield/Bomber, matching the target count exactly
* [ ] <=40 bullets — design assumption, not measured or hardware-enforced per stage
* [ ] 14 power-ups — 9 SPAWN_TYPE_POWERUP events in stage3_data.c, under target
* [x] 0–1 1UP — zero POWERUP_1UP events placed (0 is within the spec'd 0-1 range)
* [x] 2 checkpoints — checkpointFrames = { 9600, 18000 }

## NS-M10-011 — Stage 3 scroll

`2–3 px/frame` — [~] flattened to a single 2 (StageDef has one scrollSpeed field, no mid-stage ramp mechanism — NS-M07-005)

## NS-M10-012 — Vertical space

Normally maintain >=48 px free space except telegraphed patterns. [ ] Not enforced — no obstacle/safe-zone system exists to constrain (N/A until NS-9/NS-15 is addressed)

## NS-M10-013 — Industrial Core

* [x] ~450 HP — industrialCoreDef.maxHp = 450 (boss_data.c)
* [~] ~90 sec — not measured/tuned against a clock, same caveat as every other boss's duration target
* [~] mechanical arms — approximated as a stationary ENEMY_TURRET spawn (phase2), not a real per-boss "arm" entity (NS-13)
* [ ] changing safe areas — no dynamic safe-zone concept exists in the engine (NS-13)

## NS-M10-014 — Stage 3 validation

* [x] gameplay — confirmed by user as part of the full 3-stage/3-boss loop test ("todo ok")
* [x] boss — Industrial Core fight confirmed working (intro/phases/attacks/death)
* [ ] performance — not stress-tested (M17's job)
* [x] visuals — pipe background and Industrial Core sprite both confirmed by user

---

# M11 — Stages 4–5

## NS-M11-001 — Asteroid background

* [x] stars — plain starfield reused (backgroundId=4, no dedicated asteroid-field art — see PROGRESS.md)
* [x] asteroids — the real, gameplay-relevant visual content: 3 sizes (asteroid.c), spawned as real entities, not background decoration
* [ ] mines — approximated by the Mining Fortress boss's phase2 attack (a spawned small Asteroid), not a stage-hazard mine
* [ ] mining ships — not implemented, no such entity exists

## NS-M11-002 — Stage 4 length

`52,800 px` — [x] exact (stage4.lengthFrames = 17600 @ 3px/frame)

## NS-M11-003 — Stage 4 scroll

* [x] 3 px/frame normal — stage4.scrollSpeed = 3
* [ ] 4 px/frame maximum — not modeled, StageDef has one flat scrollSpeed field, no mid-stage ramp (NS-M07-005)

## NS-M11-004 — Asteroid distribution

* [~] 70% small — stage4_data.c: 13/19 (68%), close to target
* [~] 25% medium — 3/19 (16%), under target
* [~] 5% large — 3/19 (16%), over target (see PROGRESS.md — an honest placeholder table, not tuned to hit the exact ratio)

## NS-M11-005 — Stage 4 metrics

* [ ] 35–50 asteroids — 19 total asteroid spawn events, under target
* [ ] 140–180 kills — 21 enemy spawn events, not counted against actual kills; under the design target (same honest gap as every earlier stage's own table)
* [x] 7 enemy types — Drone/Fighter/Bomber/Turret/Swarm/Charger/Shield, matching the target count exactly (ENEMY_MINIBOSS also appears for the mini-boss requirement, counted separately)
* [ ] <=50 bullets — design assumption, not measured or hardware-enforced per stage
* [x] <=15 obstacles — ASTEROID_POOL_SIZE = 15 is a hard engine cap, always respected

## NS-M11-006 — Destructible asteroids

* [x] collision — resolvePlayerBulletsVsAsteroids/resolveAsteroidContactVsPlayer (combat.c)
* [x] destruction — Asteroid_hit releases the sprite and awards score for small/medium; large ones are never destroyed (SPEC.md §19)
* [ ] effects — no destruction visual effect (explosion/particle) exists, same gap as every other entity's death in this engine (M16's job)

## NS-M11-007 — Mining Fortress

* [x] ~550 HP — miningFortressDef.maxHp = 550 (boss_data.c)
* [~] 90–100 sec — not measured/tuned against a clock, same caveat as every other boss's duration target
* [x] cannons — phase1: BulletPattern_diagonal
* [~] mines — phase2: spawns a small Asteroid as an approximation (see PROGRESS.md NS-13-style simplification note)
* [x] drones — phase3: spawns 2x ENEMY_DRONE
* [x] exposed core — the "nucleo vulnerable" vulnerable-point rectangle, centered on the sprite

## NS-M11-008 — Stage 4 validation

* [x] gameplay — confirmed by user as part of the full 5-stage/5-boss loop test
* [x] boss — Mining Fortress fight confirmed working (intro/phases/attacks/death)
* [ ] performance — not stress-tested (M17's job)
* [x] visuals — Mining Fortress sprite confirmed by user; asteroid visibility was reported as an issue and investigated but not conclusively resolved (see PROGRESS.md NS-16)

## NS-M11-009 — Helios Fleet background

* [x] large battle — high enemy density from the very start of the table (no low-density intro band, unlike every earlier stage)
* [ ] ships — no distinct "ship" background art, plain starfield reused (backgroundId=5)
* [ ] projectiles — N/A to background art specifically
* [ ] background depth — single BG_B layer only, same limit as every stage (Genesis has 2 tile planes, BG_A hosts text/HUD — see M07's PROGRESS.md note)

## NS-M11-010 — Formations

* [x] diagonal — staggered frame+y multi-Fighter SpawnEvents
* [x] V — converging-y multi-Fighter SpawnEvents
* [x] lines — same-frame multi-enemy SpawnEvents at a shared frame
* [ ] circles — approximated only as a "circle-ish cluster" (4 Swarm at once, no literal ring layout); BulletPattern_circular exists for *bullets*, not enemy *formations*
* [x] crossed — 4-lane same-frame Fighter spawn ("cross-attack formation")
* [x] waves — staggered-frame ascending-y Fighter spawns

## NS-M11-011 — Stage 5 length

`57,600 px` — [x] exact (stage5.lengthFrames = 19200 @ 3px/frame)

## NS-M11-012 — Stage 5 metrics

* [ ] 180–230 kills — 39 enemy spawn events (after the Drone fix), not counted against actual kills; under the design target
* [x] 8 enemy types — all 7 combat types (Drone/Fighter/Bomber/Turret/Swarm/Charger/Shield) + ENEMY_MINIBOSS, see PROGRESS.md for why this honestly reaches "8" without inventing "Cruceros"/"Misiles"
* [ ] <=65 bullets — design assumption, not measured or hardware-enforced per stage
* [ ] 16 power-ups — 9 SPAWN_TYPE_POWERUP events, under target
* [x] 1UP — one POWERUP_1UP event
* [x] 2 checkpoints — checkpointFrames = { 7680, 14400 }
* [x] 2 mini-bosses — two separate ENEMY_MINIBOSS spawn events, matching the target exactly

## NS-M11-013 — Escape routes

Every dense pattern must retain at least one viable route. [ ] Not verified/tuned — no automated or manual playtest specifically checked this; same caveat as every other "feel" requirement in this project

## NS-M11-014 — Admiral X

* [x] ~650 HP — admiralXDef.maxHp = 650 (boss_data.c)
* [~] 100–110 sec — not measured/tuned against a clock
* [x] projectile patterns — phase1 aimed, phase3 circular barrage
* [x] fighter formations — phase2 spawns an ENEMY_FIGHTER escort (a single spawn, not a multi-ship formation — SPEC.md gives no per-phase breakdown for this boss, see PROGRESS.md)
* [x] phase changes — same generic boss.c state machine as every other boss

## NS-M11-015 — Boss validation

* [x] patterns — confirmed working as part of the full 5-boss loop test
* [x] phases — phase transitions at 2/3 and 1/3 HP confirmed (shared, tested Boss_applyDamage logic)
* [x] collision — vulnerable-point hits confirmed landing and dealing damage
* [ ] performance — not stress-tested (M17's job)

## NS-M11-016 — Stage 5 validation

* [x] complete playthrough — confirmed by user as part of the full 5-stage/5-boss loop test
* [ ] metrics — not measured against the numeric targets (kills/bullets/power-ups all under target, see NS-M11-012)
* [x] visuals — Admiral X sprite and dense enemy formations confirmed by user
* [ ] performance — not stress-tested (M17's job)

---

# M12 — Stage 6

## NS-M12-001 — Helios Core background

* [ ] black — no distinct background art beyond the plain starfield's own black backdrop
* [ ] red — not in the background art (present in the Helios boss sprite itself)
* [ ] white — not in the background art (present in the Helios boss sprite itself)
* [ ] energy — not implemented, no such visual effect exists
* [ ] machinery — not implemented, plain starfield reused (backgroundId=6, see PROGRESS.md — Helios's own sprite carries the stage's stated palette instead)

## NS-M12-002 — Stage length

`48,000 px` — [x] exact (stage6.lengthFrames = 16000 @ 3px/frame)

## NS-M12-003 — Stage metrics

* [ ] 160–200 kills — 36 enemy spawn events, not counted against actual kills; under the design target
* [x] 9 types — 7 combat types + ENEMY_MINIBOSS + M11's Asteroid system (as "obstaculos"), see PROGRESS.md for the honest reasoning behind counting a non-enemy hazard toward this
* [ ] <=80 bullets — design assumption, not measured or hardware-enforced per stage
* [ ] 12 power-ups — 7 SPAWN_TYPE_POWERUP events, under target
* [x] 0 1UP — zero POWERUP_1UP events placed, matching the target exactly
* [x] 2 checkpoints — checkpointFrames = { 6400, 12000 }
* [x] 2 mini-bosses — two separate ENEMY_MINIBOSS spawn events, matching the target exactly

## NS-M12-004 — Scroll

`2 → 3 → 4 px/frame` selectively. [~] Flattened to a single 3 (the range's mid-point) — StageDef has one flat scrollSpeed field, no mid-stage ramp mechanism (NS-M07-005, same gap as every earlier variable-speed stage)

## NS-M12-005 — Obstacles

* [ ] lasers — no dedicated environmental-hazard entity exists; approximated by the existing Laser weapon power-up being available (NS-9/NS-15)
* [ ] walls — not implemented, no scenery-obstacle system exists (NS-9/NS-15)
* [ ] energy barriers — not implemented, same gap
* [x] fast enemies — leans on ENEMY_CHARGER (the roster's fastest existing type) placed throughout the table, rather than a new speed-override mechanism
* [x] dense patterns — no low-density intro band, unlike every earlier stage (SPEC.md: this stage is the campaign's climax)

## NS-M12-006 — Difficulty

Use:

* [x] patterns — existing bullet_pattern.c variety, no new patterns needed
* [x] positioning — asteroid obstacles (large, indestructible) force navigation choices
* [x] speed — Charger-heavy placement
* [x] combinations — Shield+Bomber+Turret+Fighter mixed spawns throughout

Avoid excessive HP. [x] No enemy HP values were changed for this stage — difficulty comes entirely from placement/density/type mix, per SPEC.md's own explicit direction

## NS-M12-007 — Stage validation

* [ ] <=80 bullets — not measured (design assumption only)
* [x] <=12 enemies — ENEMY_POOL_SIZE = 12 is a hard engine cap, always respected
* [ ] escape routes — not verified/tuned, no automated or manual playtest specifically checked this (same caveat as NS-M11-013)
* [ ] 60 FPS — never measured (M17's job)

---

# M13 — Final Boss

## NS-M13-001 — HELIOS sprite

* [ ] large silhouette — 32x32, same size as every other boss (the largest single Genesis hardware sprite), not a larger multi-part composition
* [ ] multiple hardware sprites — spriteParts[1] (single part), same as every other boss; SPEC.md §31's "reusable blocks" composition isn't used here either
* [x] readable core — the vulnerable-point rectangles are clear sub-regions; white (a new 4th palette color, see PROGRESS.md) marks the visual core

## NS-M13-002 — Boss structure

```text
PHASE_1
PHASE_2
PHASE_3
DEATH
```

[x] Same generic BossState machine every boss already uses (boss.c, unchanged since M09) — INTRO/ATTACK/TELEGRAPH/VULNERABLE/TRANSITION/DEATH, phase 1-3 via HP thresholds

## NS-M13-003 — Phase 1

* [~] 30–35 sec — not measured/tuned against a clock, same caveat as every other boss's duration target
* [x] cannons — BulletPattern_diagonal
* [x] drones — spawns ENEMY_DRONE
* [~] missiles — approximated via BulletPattern_aimed (no true homing exists — NS-4)
* [ ] <=35 bullets — design assumption, not measured or hardware-enforced per phase

## NS-M13-004 — Phase 2

* [~] 30–40 sec — not measured/tuned against a clock
* [x] circular attacks — BulletPattern_circular
* [~] positional attacks — approximated via BulletPattern_wave ("barridos"); no true player-position-relative sweep exists
* [ ] <=55 bullets — design assumption, not measured or hardware-enforced per phase

## NS-M13-005 — Phase 3

* [~] 35–45 sec — not measured/tuned against a clock
* [x] exposed core — the framework's existing vulnerable-point system (nothing extra needed, same reading as every earlier boss)
* [ ] smaller safe zone — no dynamic safe-zone system exists (NS-13's already-tracked gap, same as Industrial Core's arms)
* [ ] openings — N/A without a real safe-zone system to open/close
* [ ] <=70 bullets — design assumption, not measured or hardware-enforced per phase

## NS-M13-006 — Escape routes

* [ ] every pattern has escape — not verified/tuned, same caveat as NS-M11-013/NS-M12-007
* [x] telegraphs — TELEGRAPH state (blink) before every VULNERABLE window, same as every boss
* [ ] no impossible combinations — not formally verified (no automated pattern-safety check exists)

## NS-M13-007 — Final death

* [x] stop attacks — DEATH state fires nothing, same as every boss
* [x] stop scroll — STATE_ENDING/STATE_CREDITS never call Scroll_update()
* [~] explosion — no dedicated explosion sprite/particle system exists (M16's job); the boss's existing DEATH-state strobe (M09) is the visual beat under the "HELIOS DESTROYED" text
* [x] ending — STATE_ENDING -> STATE_CREDITS -> STATE_TITLE, real behavior added this milestone (both states existed unused since M01)
* [~] ranking — shows the run's final score, not a persistent leaderboard (NS-17: no save mechanism exists anywhere in this project)
* [x] credits — "NEON STRIKE" / "THE END" / "FINAL SCORE:N" / "PRESS START"

## NS-M13-008 — Final boss validation

* [ ] 100–120 sec — not measured/tuned against a clock
* [x] 3 phases — confirmed via the shared, unit-tested Boss_applyDamage phase-threshold logic (same code every boss uses)
* [ ] <=70 bullets — not measured (design assumption only)
* [ ] no impossible patterns — not formally verified
* [ ] 60 FPS — never measured (M17's job)

---

# M14 — HUD / Score / Game Flow

## NS-M14-001 — HUD

Implement: [x] real, always-on (game_state.c's drawHud()), not SHOW_DEBUG_HUD-gated

```text
SCORE 00124500 HI 9999999
```

[x] SCORE (left) and HI (right corner, user-requested layout) both zero-padded to 8 digits, row 0

## NS-M14-002 — Bottom HUD

```text
POWER ███
WEAPON LASER
BOMB ×2
```

* [x] POWER — a '#'-per-weapon-level bar (no bar-drawing tile exists), immediately after the weapon name rather than its own labeled field (a labeled version overflowed the 40-column plane — see PROGRESS.md)
* [x] WEAPON — weapon name text
* [x] BOMB — count, plus LIVES and combo multiplier folded into the same row (not itemized in MILESTONES.md's snippet but clearly needed)

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

[x] Unchanged since M05/M06 (enemy.c/boss_data.c constants) — all match SPEC.md §23 exactly, already verified when each was introduced

## NS-M14-004 — High score

* [x] session high score — score.c's Score_getHigh(), updated inline in Score_add()
* [x] display — HUD row 0, right corner
* [x] reset — deliberately NOT reset by Score_reset() (only a power cycle resets it); see PROGRESS.md for why "session" reads that way here

## NS-M14-005 — Lives/continues

* [x] 3 lives — PLAYER_INITIAL_LIVES (player.h, since M02)
* [x] 3 continues — INITIAL_CONTINUES (game_state.c, since M06)
* [x] checkpoint return — respawnAtCheckpoint()/SpawnManager_resumeFromCheckpoint (since M07)
* [x] weapon L1 — Player_respawn/Continue_resume both reset to WEAPON_VULCAN L1
* [x] one bomb retained — Continue_resume() sets player.bombs = 1 (since M06)

## NS-M14-006 — Checkpoint restoration

* [x] reset enemies — Enemy_releaseAll/poolInit (resetGameplayPools, since M07)
* [x] reset bullets — Projectile_releaseAll/poolInit (resetGameplayPools, since M07)
* [x] reset dynamic obstacles — Asteroid_releaseAll/poolInit (resetGameplayPools, since M11)
* [x] restore scroll — Scroll_init(currentStage->backgroundId) (resetGameplayPools, since M07)

## NS-M14-007 — Stage clear

* [x] score bonus — STAGE_CLEAR_BONUS = 1000 (flat, chosen — GAME_SPEC.md gives no exact value), awarded and shown ("BONUS:1000") in StageClear_enter()
* [x] transition — STATE_STAGE_CLEAR -> STATE_NEXT_STAGE (since M07)
* [x] next stage — NextStage_resume() advances stageIndex (since M10)

## NS-M14-008 — Game over

* [x] GAME OVER — GameOver_enter() (since M02/M06)
* [x] continue — STATE_CONTINUE (since M06)
* [x] title — STATE_TITLE on exhausted continues (since M02)

## NS-M14-009 — Ending

* [x] victory — "HELIOS DESTROYED"/"ORBIT SECURED" (Ending_enter(), M13)
* [x] message — "NEON STRIKE"/"THE END" (Credits_enter(), M13)
* [~] ranking — shows the run's final score, not a persistent leaderboard (NS-17: no save mechanism exists)
* [x] credits — Credits_enter() (M13)

---

# M15 — Audio

## NS-M15-001 — Audio architecture

Verify:

```text
68000 -> audio commands
Z80 -> audio
YM2612 -> FM
PSG -> support/SFX
```

[~] Only the PSG half of this architecture is used (channel 0 = music, channel 1 = tone SFX, channel 3 = noise SFX) — no YM2612/FM synthesis, no Z80-driven sample playback exists; this was already true of M00's original placeholder and remains true here, just with real distinct content on the PSG channels now instead of one reused arpeggio

## NS-M15-002 — Stage 1 music

* [x] functional theme — musicId=1, audio.c track1 (upbeat major)

## NS-M15-003 — Stage 2 music

* [x] functional theme — musicId=2, track2 (exotic/minor, slower)

## NS-M15-004 — Stage 3 music

* [x] functional theme — musicId=3, track3 (mechanical/staccato, fast)

## NS-M15-005 — Stage 4 music

* [x] functional theme — musicId=4, track4 (tense minor syncopation)

## NS-M15-006 — Stage 5 music

* [x] functional theme — musicId=5, track5 (heroic/triumphant, wide range)

## NS-M15-007 — Stage 6 music

* [x] functional theme — musicId=6, track6 (dark, driving, fast)

## NS-M15-008 — Boss themes

* [~] six boss themes — simplified to one shared MUSIC_BOSS track (track7) reused by all 6 regular stage bosses, see PROGRESS.md for why 6 fully distinct tracks wasn't worth hand-authoring

## NS-M15-009 — Final boss theme

* [x] HELIOS theme — MUSIC_FINAL_BOSS (track8), distinct from the shared boss theme, grander/wider range

## NS-M15-010 — Credits theme

* [x] credits music — MUSIC_CREDITS (track9), gentle/resolving

## NS-M15-011 — SFX

* [x] shot — SFX_SHOT, Weapon_update on every fire
* [x] explosion — SFX_EXPLOSION (noise channel), combat.c's onEnemyKilled (enemies + asteroids)
* [x] hit — SFX_HIT, Player_hit (same event as "player damage" below — treated as one, see PROGRESS.md)
* [x] power-up — SFX_POWERUP, applyPowerup (all types except 1UP)
* [x] bomb — SFX_BOMB, Bomb_use
* [x] boss — SFX_BOSS, game_state.c's boss-defeat branch (distinct from a regular SFX_EXPLOSION; in practice gets cut short by the SFX/music that follows it — see PROGRESS.md)
* [x] 1UP — SFX_1UP, applyPowerup's POWERUP_1UP case
* [x] player damage — same as "hit" above (SFX_HIT)
* [x] stage clear — SFX_STAGE_CLEAR, StageClear_enter()

## NS-M15-012 — Audio validation

* [x] music — confirmed by user: distinct themes per stage, boss music switch, credits theme, all audible in one playthrough
* [x] SFX — confirmed by user: shot/explosion/powerup/bomb/hit all heard
* [x] transitions — confirmed working (stage->boss->stage-clear->next-stage music switches correctly)
* [x] no crashes — `make test` unchanged, BlastEm stable through the full test session
* [ ] no Z80 lockups — N/A to verify meaningfully; no Z80-driven audio path is used at all (PSG only, driven directly from 68000 each frame)

---

# M16 — Effects / Polish

## NS-M16-001 — Player effects

* [ ] muzzle flash — not implemented (projectiles appear at the gun position already, no extra flash sprite)
* [x] damage flash — pre-existing invulnerability blink (player.c, since M02), unchanged
* [x] explosion — Explosion_spawn(player.x, player.y) in PlayerHit_enter() (only on a life-costing hit)
* [x] invulnerability feedback — same pre-existing blink, still real

## NS-M16-002 — Enemy effects

* [~] small explosion — one shared explosion.c effect for every size/type, not size-differentiated
* [~] large explosion — same shared effect (see above); the boss defeat gets 3 spread across its footprint instead, a distinct-enough "large" version
* [x] hit flash — new Enemy.hitFlashTimer, set in Enemy_hit() on a survived hit, blinked in Enemy_poolUpdate()
* [x] boss damage feedback — the existing "VULNERABLE!" text/HP readout (M08) already covers this; no separate new feedback added

## NS-M16-003 — Power-up effects

* [ ] spawn — no spawn-in effect (power-ups simply appear and start drifting, unchanged since M06)
* [x] pickup — SFX_POWERUP/SFX_1UP (M15); no additional visual pickup effect
* [x] audio — SFX_POWERUP/SFX_1UP (M15)

## NS-M16-004 — Bomb effect

* [~] expanding explosion — one Explosion_spawn() per enemy killed by the bomb, not a single expanding screen-wide effect
* [x] projectile clear — pre-existing (bomb.c, since M06)
* [x] screen feedback — pre-existing "BOMB!" text flash (game_state.c, since M06)
* [x] sprite-safe — Explosion pool is a proper fixed-size pool with its own release lifecycle, same pattern as every other pool here

## NS-M16-005 — Boss effects

* [x] phase transition — new fast blink on BOSS_STATE_TRANSITION (boss.c), previously identical-looking to a solid ATTACK/VULNERABLE boss
* [x] warning — same blink serves as the warning
* [x] explosion — DEATH state now spawns a real Explosion every 15 frames across the boss's footprint (previously just a sprite strobe with no explosion sprite, since none existed before this milestone)
* [x] death sequence — DEATH state's existing strobe (M09) + the new explosions + the 3-burst spread on final defeat (game_state.c)

## NS-M16-006 — Background polish

* [ ] parallax — unchanged since M07 (one BG_B layer); Genesis has only 2 tile planes and BG_A hosts text/HUD (see M07's PROGRESS.md note) — not revisited
* [x] stars — pre-existing starfield (M07)
* [ ] traffic — not implemented (NS-9/NS-15's tracked gap)
* [ ] machinery — not implemented beyond Stage 3's pipe tile (M10)
* [ ] atmosphere — no fog/weather/lighting effect exists (NS-15's tracked gap, Stage 2's sandstorm specifically)

## NS-M16-007 — Art consistency

* [x] 16-bit look — audited: every sprite/tile is a flat-fill indexed PNG, no photographic or high-color-depth assets anywhere
* [x] coherent palette — audited: every PAL3-sharing asset verified byte-identical at shared indices throughout M05-M13 (the established `identify -verbose` technique)
* [x] silhouettes — audited: every sprite is a simple geometric silhouette, no fine detail rendering
* [x] no modern HUD — audited: HUD is plain monospace VDP text, no modern UI chrome/icons
* [x] no unnecessary gradients — audited: every asset uses flat color fills only, zero gradients anywhere
* [x] no 3D — audited: no 3D assets or perspective effects exist anywhere in the project

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
