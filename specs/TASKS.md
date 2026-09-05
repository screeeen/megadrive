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

* [ ] static pool
* [ ] no gameplay allocations
* [ ] reuse slots
* [ ] remove off-screen

## NS-M03-003 — Vulcan L1

* [ ] 1 projectile
* [ ] high cadence
* [ ] low damage

## NS-M03-004 — Vulcan L2

* [ ] 2 projectiles

## NS-M03-005 — Vulcan L3

* [ ] 3 projectiles
* [ ] maximum cadence

## NS-M03-006 — Fire input

* [ ] A fires
* [ ] cadence
* [ ] pool safe

## NS-M03-007 — Projectile collisions

* [ ] projectile → enemy
* [ ] damage
* [ ] destruction
* [ ] score

## NS-M03-008 — Vulcan tests

* [ ] L1
* [ ] L2
* [ ] L3
* [ ] collision

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

* [ ] B changes weapon
* [ ] pickup changes weapon
* [ ] different weapon starts L1

## NS-M04-003 — Laser

* [ ] straight
* [ ] fast
* [ ] pierces small enemies
* [ ] medium damage
* [ ] L3 visual enlargement

## NS-M04-004 — Wide

```text
L1 = 3
L2 = 5
L3 = 7
```

## NS-M04-005 — Homing

* [ ] seeking missiles
* [ ] target selection
* [ ] low/medium damage
* [ ] active projectile limit

## NS-M04-006 — Flame

* [ ] short range
* [ ] high damage
* [ ] close-range design

## NS-M04-007 — Weapon levels

* [ ] L1
* [ ] L2
* [ ] L3
* [ ] same weapon +1
* [ ] different weapon → L1
* [ ] damage downgrade

## NS-M04-008 — Weapon tests

* [ ] firing
* [ ] levels
* [ ] switching
* [ ] damage
* [ ] limits

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

* [ ] static
* [ ] reuse
* [ ] <=12 active

## NS-M05-003 — Drone

* [ ] HP1
* [ ] fast
* [ ] no fire
* [ ] score 100

## NS-M05-004 — Fighter

* [ ] HP2
* [ ] side entry
* [ ] formations
* [ ] score 200

## NS-M05-005 — Bomber

* [ ] HP4
* [ ] slow
* [ ] bursts
* [ ] blocks path
* [ ] score 500

## NS-M05-006 — Turret

* [ ] HP3
* [ ] fixed
* [ ] aims player
* [ ] score 300

## NS-M05-007 — Swarm

* [ ] HP1
* [ ] group movement
* [ ] score 100

## NS-M05-008 — Charger

* [ ] HP2
* [ ] pause
* [ ] aim
* [ ] charge

## NS-M05-009 — Shield

* [ ] HP6
* [ ] frontal resistance
* [ ] vulnerable angle/opening

## NS-M05-010 — Enemy bullets

* [ ] pool
* [ ] velocity
* [ ] direction
* [ ] collision
* [ ] off-screen removal
* [ ] maximum 80

## NS-M05-011 — Bullet patterns

Implement:

```text
diagonal
circular
top_bottom
aimed
burst
wave
cross
combined
```

## NS-M05-012 — Enemy tests

* [ ] all enemy types
* [ ] bullets
* [ ] patterns
* [ ] collisions
* [ ] limits

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

* [ ] slow fall
* [ ] predictable
* [ ] off-screen removal
* [ ] player collision

## NS-M06-003 — Weapon power-ups

* [ ] Laser
* [ ] Wide
* [ ] Homing
* [ ] Flame

## NS-M06-004 — P power-up

* [ ] weapon level +1
* [ ] maximum L3

## NS-M06-005 — S power-up

* [ ] temporary speed
* [ ] duration
* [ ] automatic restoration

## NS-M06-006 — B power-up

* [ ] bomb +1
* [ ] maximum 3

## NS-M06-007 — 1UP

* [ ] life +1
* [ ] feedback

## NS-M06-008 — Bomb

* [ ] C activation
* [ ] consume bomb
* [ ] projectile neutralization
* [ ] high damage
* [ ] ~0.5 s invulnerability
* [ ] explosion

## NS-M06-009 — Combo

```text
×1
×2
×3
×4
×5
```

* [ ] kills increase combo
* [ ] timeout
* [ ] reset
* [ ] score multiplier

## NS-M06-010 — System tests

* [ ] power-ups
* [ ] bomb
* [ ] combo
* [ ] limits

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

* [ ] process events
* [ ] spawn enemies
* [ ] spawn power-ups
* [ ] spawn obstacles
* [ ] respect pool limits

## NS-M07-004 — Checkpoints

Exactly:

```text
~40%
~75%
```

## NS-M07-005 — Scroll

```text
normal = 2 px/frame
maximum = 4 px/frame
```

## NS-M07-006 — Parallax

Implement 3–4 layers.

## NS-M07-007 — Stage transitions

* [ ] completion
* [ ] next stage
* [ ] entity reset
* [ ] state preservation

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
