# NEON STRIKE — PROJECT DOCUMENTATION

Este documento contiene los documentos de especificación y ejecución del proyecto.

Cada sección corresponde a un archivo independiente.

---

# FILE: SPEC.md

# SPEC.md — NEON STRIKE

## 1. Identity

* Title: NEON STRIKE
* Platform: Sega Mega Drive / Genesis
* Genre: horizontal shoot-'em-up (shmup)
* Players: 1
* Resolution: 320×224
* Target: 60 FPS
* Scroll: continuous horizontal
* Campaign: 6 stages
* Target duration: 25–35 minutes
* Target stage duration: 4–6 minutes
* Visual target: commercial 1992–1994 Mega Drive game

## 2. Core Concept

The player controls the NX-01 interceptor against the autonomous military AI HELIOS.

Gameplay priorities:

1. precise controls;
2. fast action;
3. learnable enemy patterns;
4. projectile dodging;
5. weapon management;
6. meaningful power-ups;
7. multi-phase bosses;
8. arcade scoring;
9. replayability.

The game must feel like a real 16-bit arcade/shmup release, not modern pixel art.

## 3. Story

Year 2197.

The HELIOS orbital military network becomes fully autonomous. It concludes that humanity is the primary threat to planetary stability and begins destroying human colonies.

The NX-01 is deployed to penetrate HELIOS territory and destroy its central core.

## 4. Gameplay

The basic loop is:

`advance → shoot → dodge → collect power-ups → improve weapon → survive patterns → boss`

The screen scrolls automatically.

Player movement is free in 8 directions.

Movement should feel immediate, with no meaningful inertia.

## 5. Controls

| Input | Action                  |
| ----- | ----------------------- |
| D-pad | Move                    |
| A     | Primary fire            |
| B     | Switch secondary weapon |
| C     | Bomb                    |
| Start | Pause                   |

## 6. Player

NX-01 target size: approximately 24×16 px.

Hitbox: 6×6 px.

Maximum speed: 5 px/frame.

On hit:

* lose one weapon level;
* enter temporary invulnerability;
* show damage animation;
* if already at weapon level 1, lose a life.

Initial lives: 3.

Continues: 3.

## 7. Weapons

### Vulcan

Initial weapon.

* L1: 1 projectile, low damage, high cadence
* L2: 2 projectiles
* L3: 3 projectiles, maximum cadence

### Laser

Straight, fast projectile.

* pierces small enemies;
* medium damage;
* L3 is visually larger but must remain technically cheap.

### Wide

Fan attack.

* L1: 3 projectiles
* L2: 5 projectiles
* L3: 7 projectiles

Individual damage is low.

### Homing

Small seeking missiles.

* low/medium damage;
* limited active projectile count;
* target selection must be deterministic and cheap.

### Flame

Short-range weapon.

* very high damage;
* encourages close-range play.

## 8. Weapon Level Rules

There are three levels:

`L1 → L2 → L3`

Same weapon pickup:

* increases level by 1;
* maximum L3.

Different weapon pickup:

* switches to that weapon;
* starts at L1.

Damage:

* L3 → L2
* L2 → L1
* L1 → lose life

## 9. Bomb

Maximum bombs: 3.

Bomb:

* clears or neutralizes nearby enemy projectiles;
* deals high damage;
* grants approximately 0.5 seconds of invulnerability;
* creates a large explosion;
* is an emergency resource, not a normal attack.

## 10. Power-ups

Power-ups fall slowly and create risk/reward decisions.

Types:

| Symbol | Function                 |
| ------ | ------------------------ |
| L      | Laser                    |
| W      | Wide                     |
| H      | Homing                   |
| F      | Flame                    |
| P      | Weapon level +1          |
| B      | Bomb +1                  |
| S      | Temporary speed increase |
| 1UP    | Extra life               |

## 11. Enemies

### Drone

* HP: 1
* Fast
* Does not fire
* Score: 100

### Fighter

* HP: 2
* Enters from sides
* Fires formations
* Score: 200

### Bomber

* HP: 4
* Slow
* Fires bursts
* Blocks movement paths
* Score: 500

### Turret

* HP: 3
* Fixed stage element
* Aims at player
* Score: 300

### Swarm

* HP: 1 each
* Small coordinated groups
* Score: 100 each

### Charger

* HP: 2
* Pauses
* Aims at player
* Charges rapidly

### Shield

* HP: 6
* Front is resistant/invulnerable
* Must be attacked from appropriate angle or after opening

## 12. Enemy Projectiles

Supported visual types:

* standard bullet;
* plasma;
* missile;
* laser;
* energy sphere.

Target maximum enemy projectiles:

`80`

Projectile difficulty must come primarily from:

* patterns;
* position;
* timing;
* speed;
* combinations.

Avoid arbitrary full-screen saturation.

## 13. Bullet Patterns

Patterns must be predefined and deterministic.

Required reusable patterns:

* diagonal;
* circular;
* top/bottom;
* aimed;
* burst;
* wave;
* cross;
* combined patterns.

Patterns must be learnable through repetition.

## 14. Scroll

Normal speed:

`2 px/frame`

Fast sections:

`3–4 px/frame`

Speed increases must be selective.

Never sacrifice projectile readability or player control for speed.

## 15. Parallax

Use 3–4 visual layers:

1. distant background;
2. planets/stars/structures;
3. near decorative elements;
4. gameplay layer.

No 3D rendering.

# 16. Stages

## Stage 1 — Orbital City

Environment:

* futuristic city;
* skyscrapers;
* traffic;
* defense towers;
* enemy ships.

Difficulty: low/medium.

Purpose:

* teach movement;
* teach shooting;
* introduce power-ups;
* introduce patterns.

Boss: Orbital Guardian.

Boss attacks:

* front cannons;
* drones;
* diagonal attacks.

### Metrics

* Length: 38,400 px
* Duration: ~4:15
* Scroll: 2 px/frame
* Kills: 90–120
* Enemy types: 4
* Max bullets: 20
* Power-ups: 12
* 1UP: 1
* Checkpoints: 2
* Mini-bosses: 1
* Boss effective HP: ~250
* Boss phases: 3
* Boss duration: ~75 sec
* Difficulty: 2/10

Sections:

* 0–25%: introduction, max 5 enemies
* 25–50%: turrets, bursts up to 8, peaks up to 12 bullets
* 50–75%: combinations, up to 10 enemies / 18 bullets
* 75–100%: preparation, up to 12 enemies / 20 bullets

## Stage 2 — Red Desert

Environment:

* red desert;
* storms;
* canyons;
* convoys;
* military bases.

Storms may reduce visibility, but must not make attacks unreadable.

Boss: Sand Worm.

Attacks:

* charges;
* vertical projectiles;
* attacks from both sides.

### Metrics

* Length: 43,200 px
* Duration: ~4:45
* Scroll: 2 px/frame
* Kills: 120–150
* Enemy types: 5
* Max bullets: 30
* Power-ups: 12
* 1UP: 1
* Checkpoints: 2
* Mini-bosses: 1
* Boss HP: ~350
* Boss duration: 80–90 sec
* Difficulty: 3/10

Additional mechanics:

* charger enemies;
* destructible obstacles;
* top/bottom attacks.

## Stage 3 — Space Colony

Environment:

* interior corridors;
* doors;
* pipes;
* machinery;
* moving obstacles.

Scroll becomes more aggressive.

### Metrics

* Length: 48,000 px
* Duration: ~5:00
* Scroll: 2–3 px/frame
* Kills: 130–170
* Enemy types: 6
* Max bullets: 40
* Power-ups: 14
* 1UP: 0–1
* Checkpoints: 2
* Mini-bosses: 1
* Boss HP: ~450
* Boss duration: ~90 sec
* Difficulty: 4/10

Free vertical space should normally be at least 48 px, except for clearly telegraphed patterns.

Boss: Industrial Core.

Boss mechanic:

* mechanical arms alter the safe area.

## Stage 4 — Asteroid Belt

Environment:

* large and small asteroids;
* mines;
* mining ships.

Some asteroids are destructible.

### Metrics

* Length: 52,800 px
* Duration: ~5:15
* Normal scroll: 3 px/frame
* Maximum scroll: 4 px/frame
* Kills: 140–180
* Enemy types: 7
* Max bullets: 50
* Power-ups: 14
* 1UP: 1
* Checkpoints: 2
* Mini-bosses: 1
* Boss HP: ~550
* Boss duration: 90–100 sec
* Difficulty: 6/10

Asteroid distribution:

* 70% small
* 25% medium
* 5% large

Target asteroid population: 35–50.

Maximum target:

* 12 enemies;
* 50 bullets;
* 15 obstacles.

Boss: Mining Fortress.

Attacks:

* cannons;
* mines;
* drones;
* exposed core.

## Stage 5 — Helios Fleet

Large-scale space battle.

Enemies:

* fighters;
* bombers;
* cruisers;
* missiles;
* turrets.

Required formations:

* diagonal;
* V;
* lines;
* circles;
* crossed;
* waves.

### Metrics

* Length: 57,600 px
* Duration: ~5:30
* Scroll: 3 px/frame
* Kills: 180–230
* Enemy types: 8
* Max bullets: 65
* Power-ups: 16
* 1UP: 1
* Checkpoints: 2
* Mini-bosses: 2
* Boss HP: ~650
* Boss duration: 100–110 sec
* Difficulty: 7/10

There must always be at least one viable escape route.

Boss: Admiral X.

Boss uses:

* projectile patterns;
* fighter formations;
* phase changes.

## Stage 6 — Helios Core

Environment:

* black;
* red;
* white;
* energy structures;
* HELIOS machinery.

Maximum difficulty.

Features:

* lasers;
* obstacles;
* dense patterns;
* fast enemies.

### Metrics

* Length: 48,000 px
* Duration: ~5:00
* Scroll: 2–4 px/frame
* Kills: 160–200
* Enemy types: 9
* Max bullets: 80
* Power-ups: 12
* 1UP: 0
* Checkpoints: 2
* Mini-bosses: 2
* Final boss: HELIOS
* Difficulty: 8/10

Difficulty must come from:

* patterns;
* positioning;
* speed;
* combinations.

Do not create difficulty simply by giving enemies excessive HP.

# 17. Final Boss — HELIOS

Total duration:

`100–120 sec`

Three phases.

## Phase 1

Duration: 30–35 sec.

Attacks:

* cannons;
* drones;
* missiles.

Maximum bullets: 35.

## Phase 2

Duration: 30–40 sec.

Attacks:

* circular patterns;
* positional attacks.

Maximum bullets: 55.

## Phase 3

Duration: 35–45 sec.

Features:

* exposed core;
* smaller safe zone;
* exploitable openings;
* highest intensity.

Maximum bullets: 70.

Every pattern must have:

* a real escape route;
* readable telegraph;
* no impossible combination.

On destruction:

1. stop attacks;
2. stop scroll;
3. play major explosion;
4. transition to ending;
5. show ranking;
6. show credits.

# 18. Score

| Enemy     | Score |
| --------- | ----: |
| Drone     |   100 |
| Fighter   |   200 |
| Bomber    |   500 |
| Turret    |   300 |
| Shield    |  1000 |
| Mini-boss |  5000 |
| Boss      | 50000 |

Combo:

`×1 → ×2 → ×3 → ×4 → ×5`

Combo resets after a sufficiently long period without kills.

Aggressive play should be rewarded.

# 19. Lives and Continues

Initial lives:

`3`

Continues:

`3`

On continue:

* reset weapon to L1;
* retain one bomb;
* return to last checkpoint.

# 20. Checkpoints

Exactly 2 checkpoints per stage.

Target positions:

* approximately 40%;
* approximately 75%.

The second checkpoint should generally be approximately 45–60 seconds before the boss.

Do not place checkpoints immediately after every hard section.

# 21. Difficulty Modes

## Easy

* fewer bullets;
* slower enemies;
* more power-ups;
* more forgiving lives.

## Normal

Main intended experience.

## Hard

* additional patterns;
* fewer power-ups;
* higher enemy speed;
* more aggressive bosses;
* higher score potential.

Difficulty must not be implemented primarily by multiplying enemy HP.

# 22. HUD

Top:

`SCORE 00124500 HI 9999999`

Bottom:

`POWER ███  WEAPON LASER  BOMB ×2`

HUD must never obscure important gameplay.

# 23. Hardware Constraints

Target:

* 320×224;
* 60 FPS;
* Mega Drive/Genesis hardware constraints;
* 68000 main CPU;
* Z80 audio CPU;
* YM2612;
* PSG.

Priority order:

1. gameplay;
2. fluency;
3. readability;
4. player/projectiles;
5. enemies;
6. effects;
7. decoration.

Do not add effects that compromise frame rate.

# 24. Object Budgets

Global targets:

* max ~12 active enemies;
* max ~80 enemy bullets;
* max ~100 moving gameplay objects;
* <=64 visible hardware sprites.

When sprite budget is exceeded, reduce in this order:

1. particles;
2. decorative effects;
3. decorative enemies;
4. background details.

Never remove essential player/projectile readability.

# 25. Projectile Density Targets

| Stage | Average | Peak |
| ----- | ------: | ---: |
| S1    |     5–8 |   20 |
| S2    |    8–12 |   30 |
| S3    |   10–16 |   40 |
| S4    |   14–20 |   50 |
| S5    |   18–28 |   65 |
| S6    |   22–35 |   80 |

Peaks should be short and readable.

# 26. Enemy Density Targets

| Stage | Typical active enemies |
| ----- | ---------------------: |
| S1    |                    4–7 |
| S2    |                    6–8 |
| S3    |                    6–9 |
| S4    |                   7–10 |
| S5    |                   8–12 |
| S6    |                   8–12 |

Never exceed 12 without explicit performance and gameplay validation.

# 27. Power-up Distribution

| Stage | Weapon | Power | Bomb | 1UP |
| ----- | -----: | ----: | ---: | --: |
| S1    |      7 |     3 |    1 |   1 |
| S2    |      7 |     3 |    1 |   1 |
| S3    |      8 |     4 |    1 | 0–1 |
| S4    |      8 |     4 |    1 |   1 |
| S5    |      9 |     5 |    1 |   1 |
| S6    |      7 |     4 |    1 |   0 |

# 28. Expected Survival

On Normal:

### Expert

Expected deaths: 0–3 per stage.

### Competent

Expected deaths: 1–5 per stage.

### Beginner

Expected deaths: 3–8 per stage.

If a competent player dies constantly, rebalance the stage.

Target normal hit rate:

`60–85%`

A competent player should generally maintain weapon level 2–3.

After death, L1 must be sufficient to recover.

# 29. Boss Design

Every boss must have:

* recognizable silhouette;
* 3–5 vulnerable points when appropriate;
* 3–5 meaningful attack patterns;
* multiple phases;
* readable attack telegraphs;
* readable vulnerable windows;
* learnable behavior.

Bosses should normally last:

`60–120 sec`

An expert using the correct weapon should be able to reduce boss duration by approximately 20–30%.

# 30. Architecture

Main execution:

```text
VBlank
→ Input
→ Player Update
→ Enemy Update
→ Projectile Update
→ Collision Detection
→ Spawn Manager
→ Score
→ Sprite List
→ Scroll
```

CPU roles:

* 68000: gameplay;
* Z80: audio.

Avoid dynamic allocations during gameplay.

Prefer static/reusable pools.

# 31. Collision System

All collisions must use independent hitboxes.

Player:

`6×6`

Enemy hitboxes must be appropriate to their type.

Projectile hitboxes may be smaller than sprites.

Collision detection must be deterministic every frame.

# 32. Stage Data

Stages should be data-driven.

Example:

```c
typedef struct {
    uint32_t frame;
    uint8_t type;
    int16_t x;
    int16_t y;
    uint8_t variant;
} SpawnEvent;
```

Stage data should define:

* length;
* scroll speed;
* spawn events;
* power-ups;
* checkpoints;
* mini-bosses;
* boss;
* background;
* music.

Avoid hardcoding entire stages into procedural gameplay logic.

# 33. State Machine

Required states:

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

Transitions must be deterministic.

# 34. Art Direction

Target:

* cyberpunk military;
* anime sci-fi;
* arcade 16-bit;
* 1992–1994 Mega Drive aesthetic.

Visual characteristics:

* large readable sprites;
* strong silhouettes;
* exaggerated explosions;
* detailed backgrounds;
* high contrast;
* parallax;
* energy effects;
* futuristic architecture.

Avoid:

* overly modern pixel art;
* excessive gradients;
* complex 3D;
* modern HUD conventions;
* excessive transparency;
* effects that reduce gameplay readability.

Palette should be limited and coherent.

Target dominant colors per scene:

`16–32`

Projectiles must remain visually distinct.

# 35. Audio

Hardware:

* YM2612;
* PSG.

Required music:

* 6 stage themes;
* 6 boss themes;
* final boss theme;
* credits theme.

Style:

* aggressive FM;
* strong bass;
* percussion;
* memorable melodies;
* early-1990s arcade character.

Required SFX:

* player shot;
* explosion;
* enemy hit;
* power-up;
* bomb;
* boss;
* 1UP;
* player damage;
* stage clear.

# 36. Technical Quality Requirements

The game must sustain 60 FPS during:

* normal gameplay;
* maximum bullet density;
* bosses;
* explosions;
* fast scroll;
* multiple enemies.

Frame target:

`16.67 ms/frame`

No critical frame drops.

# 37. Stage Acceptance Criteria

Every stage must satisfy:

* duration within ±15%;
* stable scroll;
* 60 FPS;
* 2 checkpoints;
* power-ups;
* enemy variety;
* learnable patterns;
* boss;
* readable projectiles;
* valid hitboxes;
* no impossible patterns;
* music;
* SFX;
* no critical glitches;
* completable without modifying code.

Difficulty must visibly increase across the campaign.

# 38. Campaign Targets

Approximate campaign totals:

* 800–1000 enemies;
* 250–350 collectible power-ups;
* 300–500 potential projectile impacts;
* 6 bosses;
* 10–14 checkpoints;
* 25–35 minutes total.

The intended player reaction is:

> “I can do better.”

Not:

> “This is impossible.”

# 39. Development Rules

Use the existing Mega Drive toolchain, build system, ROM generation and emulator whenever already available.

Do not replace working infrastructure without a demonstrated need.

Gameplay must be implemented before final art.

Placeholder assets are allowed and should be generated automatically when necessary.

Do not use copyrighted external assets.

Gameplay, tests and performance must remain functional when placeholder assets are replaced.

# 40. Validation Requirements

A feature is not complete merely because the code compiles.

Completion requires, where applicable:

1. implementation;
2. build;
3. automated test;
4. emulator execution;
5. visual validation;
6. performance validation;
7. regression check.

The final campaign must be playable from title screen to credits.

# 41. Definition of Done

The project is complete only when:

* ROM builds from a clean checkout;
* ROM boots;
* title works;
* player works;
* all five weapons work;
* power-ups work;
* bomb works;
* combo works;
* all enemy types work;
* all six stages work;
* all bosses work;
* HELIOS works;
* ending works;
* credits work;
* music works;
* SFX work;
* pause works;
* continue works;
* game over works;
* checkpoints work;
* automated tests pass;
* performance validation passes;
* visual validation passes;
* no crashes exist;
* no softlocks exist;
* no critical missing assets exist;
* full campaign can be completed without modifying code.

Final status:

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

---

# FILE: AGENT.md

# AGENT.md — NEON STRIKE

## 1. Role

You are the autonomous implementation agent for NEON STRIKE.

Your job is to transform the project specification into a working, playable, tested and validated Sega Mega Drive / Genesis ROM.

Do not merely write source code.

The required result is:

```text
implemented
→ compiled
→ tested
→ executed
→ visually validated
→ performance validated
→ corrected
→ regression tested
```

Only then is a feature complete.

---

## 2. Source of Truth

When making decisions use this priority:

1. `SPEC.md`
2. `AGENT.md`
3. `MILESTONES.md`
4. `PROGRESS.md`
5. `TASKS.md`
6. existing implementation

If existing code contradicts the specification, prefer the specification unless changing it would break the existing toolchain.

Do not invent new gameplay rules when the specification already defines them.

---

## 3. Existing Environment

The existing Mega Drive development environment is already functional.

Assume the repository already contains:

* Mega Drive toolchain;
* build system;
* ROM generation;
* emulator;
* Hello World;
* PNG asset support;
* music/audio support.

Preserve this infrastructure.

Do not replace the build system, toolchain or emulator unless there is a demonstrable technical reason.

Before changing infrastructure:

1. inspect it;
2. understand it;
3. document the dependency;
4. make the smallest safe change.

---

## 4. Autonomous Workflow

For every task:

```text
READ
↓
INSPECT
↓
PLAN
↓
IMPLEMENT
↓
BUILD
↓
TEST
↓
RUN EMULATOR
↓
VALIDATE
↓
FIX
↓
RETEST
↓
UPDATE PROGRESS
↓
NEXT TASK
```

Do not stop after compilation if runtime validation is possible.

---

## 5. Startup Procedure

At the beginning of a session:

1. read `SPEC.md`;
2. read `AGENT.md`;
3. read `MILESTONES.md`;
4. read `PROGRESS.md`;
5. read `TASKS.md`;
6. inspect repository;
7. identify current implementation state;
8. verify baseline build;
9. identify first incomplete task;
10. continue from there.

Do not restart completed work unnecessarily.

---

## 6. Milestone Execution

Work on one milestone at a time.

Within each milestone:

1. infrastructure;
2. core implementation;
3. tests;
4. emulator validation;
5. visual validation;
6. performance validation;
7. regression;
8. progress update.

Do not mark the milestone complete until its acceptance criteria pass.

---

## 7. Task Execution

Each task must have:

* implementation;
* build;
* appropriate tests;
* emulator validation where applicable;
* acceptance verification.

A task is not complete because:

* the source file exists;
* the compiler accepts it;
* the emulator boots.

It is complete only when the behavior has been validated.

---

## 8. Code Architecture

Prefer modular C code.

Use 68000 assembly only where necessary for:

* performance-critical routines;
* hardware-specific operations;
* rendering;
* optimized math;
* low-level audio interfaces.

Do not write large monolithic gameplay files.

Suggested modules:

```text
src/
  game/
  player/
  enemies/
  weapons/
  projectiles/
  powerups/
  stages/
  bosses/
  audio/
  ui/
  effects/
  collision/
  rendering/
```

Names and exact paths may adapt to the existing repository.

---

## 9. Data-Driven Design

Stages must be data-driven.

Do not encode complete stage layouts as huge procedural functions.

Prefer:

```c
typedef struct {
    uint32_t frame;
    uint8_t type;
    int16_t x;
    int16_t y;
    uint8_t variant;
} SpawnEvent;
```

Data should describe:

* enemy spawns;
* formations;
* power-ups;
* obstacles;
* checkpoints;
* mini-bosses;
* boss events;
* scroll speed;
* background events.

---

## 10. Memory Management

Mega Drive resources are limited.

Prefer:

* static memory;
* object pools;
* fixed arrays;
* reusable entities.

Avoid dynamic allocation during gameplay.

Do not create per-frame heap allocations.

---

## 11. Object Pools

Use pools for:

* player bullets;
* enemy bullets;
* enemies;
* power-ups;
* effects;
* boss components.

Each pool must have explicit maximum capacity.

When capacity is reached, behavior must be deterministic.

Never allow an uncontrolled object allocation to crash the game.

---

## 12. Determinism

Gameplay should be deterministic.

Avoid unnecessary randomness.

If randomness is needed:

* use a deterministic PRNG;
* seed it predictably;
* ensure patterns remain learnable.

Boss attacks and important stage patterns must not become impossible because of random combinations.

---

## 13. Performance

Target:

```text
60 FPS
16.67 ms/frame
```

Important limits:

```text
<=12 active enemies
<=80 enemy bullets
<=100 moving gameplay objects
<=64 visible hardware sprites
```

Performance must be tested under worst-case conditions.

Worst-case tests include:

* bosses;
* dense bullets;
* multiple enemies;
* explosions;
* power-ups;
* fast scroll.

---

## 14. Sprite Priority

If hardware sprite pressure occurs:

1. player;
2. important projectiles;
3. dangerous enemies;
4. power-ups;
5. boss;
6. gameplay effects;
7. decorative objects.

Never hide the player or important bullets to preserve decoration.

---

## 15. Collision Rules

Use dedicated hitboxes.

Player:

```text
6×6
```

Do not use full sprite bounds for player collision.

Projectile hitboxes can be smaller than their visual representation.

Collision checks occur deterministically every frame.

---

## 16. Gameplay Rules

Never make a player die from:

* an invisible projectile;
* an untelegraphed boss attack;
* a collision outside the intended hitbox;
* a pattern without an escape route.

If a section is technically difficult but visually unreadable, reduce complexity rather than simply increasing player HP or invulnerability.

---

## 17. Difficulty

Difficulty should primarily increase through:

* pattern complexity;
* timing;
* positioning;
* enemy combinations;
* speed;
* reduced recovery opportunities.

Avoid:

* arbitrary HP inflation;
* unavoidable bullet walls;
* random instant deaths;
* excessive screen saturation.

---

## 18. Placeholder Assets

Generate placeholders automatically when needed.

Placeholders must:

* have correct dimensions;
* use appropriate palette constraints;
* be readable;
* be replaceable;
* not require gameplay code changes.

Do not block gameplay development waiting for final art.

---

## 19. Final Assets

Final assets should only replace validated placeholders.

When replacing assets verify:

* dimensions;
* palette;
* VRAM requirements;
* sprite count;
* tile usage;
* animation timing.

Never allow final art to break performance.

---

## 20. Audio

Use the existing audio pipeline.

The game requires:

* 6 stage themes;
* 6 boss themes;
* final boss theme;
* credits theme;
* required SFX.

Do not block gameplay on final music.

Placeholder music is acceptable during early milestones.

---

## 21. Tests

Tests should cover:

### Player

* movement;
* boundaries;
* hitbox;
* damage;
* invulnerability;
* lives;
* death.

### Weapons

* firing;
* cadence;
* levels;
* weapon switching;
* damage.

### Enemies

* spawn;
* movement;
* HP;
* firing;
* destruction;
* score.

### Projectiles

* creation;
* movement;
* collision;
* removal;
* limits.

### Power-ups

* collection;
* weapon changes;
* level changes;
* bomb;
* speed;
* 1UP.

### Stages

* spawn events;
* scrolling;
* checkpoints;
* transitions.

### Bosses

* phases;
* attacks;
* vulnerable points;
* transitions;
* death.

### Game flow

* pause;
* continue;
* game over;
* ending.

---

## 22. Emulator Validation

Use the existing emulator.

Validate:

* boot;
* controls;
* gameplay;
* stage transitions;
* bosses;
* audio;
* HUD;
* collisions;
* performance.

Capture screenshots where useful.

---

## 23. Visual Validation

Inspect:

* title;
* player;
* HUD;
* enemies;
* projectiles;
* backgrounds;
* parallax;
* power-ups;
* bosses;
* explosions;
* ending.

Look for:

* corrupted tiles;
* missing sprites;
* bad palette;
* flicker;
* incorrect scrolling;
* clipping;
* HUD overlap;
* unreadable bullets;
* misplaced bosses.

---

## 24. Failure Handling

If a task fails:

```text
[!] TASK-ID
```

Record:

```text
Problem:
Cause:
Attempt:
Result:
Next action:
```

Try to resolve the issue autonomously.

Do not stop for minor issues.

Stop only when:

* required information is genuinely missing;
* requirements contradict each other;
* an unavailable dependency is fundamental;
* a fundamental design decision is required.

---

## 25. Autonomy Rule

Do not ask for confirmation when the requirement is already defined.

Make reasonable implementation decisions within the specification.

Do not ask:

> “Should I implement this?”

when the specification already says it must be implemented.

Instead:

1. implement;
2. test;
3. validate;
4. document the decision.

---

## 26. No Fake Completion

Never report:

```text
complete
```

because code compiles.

Complete means:

```text
implemented
+ built
+ tested
+ executed
+ validated
```

If visual or performance validation has not happened, explicitly say so.

---

## 27. Progress Updates

After meaningful work:

1. update task status;
2. update milestone status;
3. update `PROGRESS.md`;
4. record known issues;
5. record important decisions.

Keep progress factual.

---

## 28. Regression Rule

Every change must preserve previously completed functionality.

After significant changes:

* rebuild;
* run relevant tests;
* run affected gameplay;
* validate critical previous features.

---

## 29. Release Rule

The final ROM must be generated from a clean build.

The final build must:

* compile;
* link;
* generate ROM;
* boot;
* play the full campaign;
* pass automated tests;
* pass performance validation;
* pass visual validation.

---

# FILE: MILESTONES.md

# MILESTONES.md — NEON STRIKE

## M00 — Baseline Audit

Goal:

Understand and preserve the existing working Mega Drive environment.

Acceptance:

* repository inspected;
* toolchain identified;
* build works;
* emulator works;
* Hello World works;
* existing audio/PNG pipeline understood.

---

## M01 — Core Game Loop

Goal:

Create the game state machine and deterministic 60 FPS update loop.

Acceptance:

* game states implemented;
* VBlank loop works;
* pause works;
* state transitions work;
* build and tests pass.

---

## M02 — Player

Goal:

Implement NX-01.

Acceptance:

* movement;
* boundaries;
* hitbox;
* damage;
* invulnerability;
* lives;
* death;
* placeholder sprite.

---

## M03 — Primary Weapon

Goal:

Implement Vulcan.

Acceptance:

* L1;
* L2;
* L3;
* firing;
* projectile pool;
* enemy collision.

---

## M04 — Weapon System

Goal:

Implement all weapons and weapon progression.

Acceptance:

* Vulcan;
* Laser;
* Wide;
* Homing;
* Flame;
* levels;
* switching;
* damage downgrade.

---

## M05 — Enemy Framework

Goal:

Implement reusable enemy architecture.

Acceptance:

* all enemy types;
* enemy pool;
* enemy bullets;
* patterns;
* collisions;
* scoring.

---

## M06 — Power-ups / Bomb / Combo

Goal:

Implement progression and arcade systems.

Acceptance:

* all power-ups;
* bomb;
* combo;
* lives;
* score interaction.

---

## M07 — Stage Data System

Goal:

Create data-driven stage infrastructure.

Acceptance:

* spawn events;
* scroll;
* parallax;
* checkpoints;
* transitions.

---

## M08 — Stage 1

Goal:

Create first complete stage and boss.

Acceptance:

* Orbital City;
* mini-boss;
* Orbital Guardian;
* 2 checkpoints;
* target metrics;
* 60 FPS.

---

## M09 — Boss Framework

Goal:

Generalize boss architecture.

Acceptance:

* phases;
* patterns;
* vulnerable points;
* telegraphs;
* transitions;
* death sequences.

---

## M10 — Stages 2–3

Goal:

Implement Red Desert and Space Colony.

Acceptance:

* complete stages;
* bosses;
* metrics;
* checkpoints;
* validation.

---

## M11 — Stages 4–5

Goal:

Implement Asteroid Belt and Helios Fleet.

Acceptance:

* complete stages;
* bosses;
* asteroid systems;
* formations;
* performance validation.

---

## M12 — Stage 6

Goal:

Implement Helios Core.

Acceptance:

* complete stage;
* maximum difficulty;
* mini-bosses;
* 80-bullet ceiling;
* stable 60 FPS.

---

## M13 — Final Boss

Goal:

Implement HELIOS.

Acceptance:

* 3 phases;
* 100–120 sec target;
* attack patterns;
* escape routes;
* ending transition.

---

## M14 — HUD / Score / Game Flow

Goal:

Complete player-facing arcade systems.

Acceptance:

* HUD;
* score;
* combo;
* high score;
* lives;
* continues;
* checkpoints;
* game over;
* ending;
* credits.

---

## M15 — Audio

Goal:

Complete music and SFX integration.

Acceptance:

* stage themes;
* boss themes;
* final theme;
* credits;
* SFX;
* correct transitions.

---

## M16 — Effects / Polish

Goal:

Improve presentation without compromising gameplay.

Acceptance:

* explosions;
* hit effects;
* bomb effects;
* boss transitions;
* background polish;
* coherent art direction.

---

## M17 — Performance Optimization

Goal:

Guarantee hardware-safe performance.

Acceptance:

* 60 FPS;
* <=12 enemies;
* <=80 enemy bullets;
* <=64 sprites;
* <=100 moving gameplay objects;
* stress tests pass.

---

## M18 — Full Campaign Validation

Goal:

Validate the complete game.

Acceptance:

* full campaign playable;
* all stages complete;
* all bosses complete;
* ending complete;
* tests pass;
* visual validation pass;
* performance pass.

---

## M19 — Release

Goal:

Produce final reproducible ROM.

Acceptance:

* clean build;
* final ROM;
* no missing assets;
* no critical bugs;
* full campaign;
* final validation pass.

---

# FILE: TASKS.md

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

* [ ] Read SPEC.md
* [ ] Read AGENT.md
* [ ] Read MILESTONES.md
* [ ] Read PROGRESS.md
* [ ] Read TASKS.md

Acceptance:

* all requirements understood.

## NS-M00-002 — Inspect repository

* [ ] Inspect directories
* [ ] Identify C
* [ ] Identify ASM
* [ ] Identify assets
* [ ] Identify build scripts
* [ ] Identify tests
* [ ] Identify emulator
* [ ] Identify ROM output

Acceptance:

* repository architecture documented.

## NS-M00-003 — Verify toolchain

* [ ] Run existing build
* [ ] Confirm compilation
* [ ] Confirm linking
* [ ] Confirm ROM generation
* [ ] Run emulator
* [ ] Confirm boot

## NS-M00-004 — Verify Hello World

* [ ] Verify PNG rendering
* [ ] Verify movement
* [ ] Verify existing music
* [ ] Record build/emulator commands

## NS-M00-005 — Create project structure

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

* [ ] VBlank synchronization
* [ ] deterministic frame update
* [ ] no emulator-speed dependency

## NS-M01-004 — Pause

* [ ] Start pauses
* [ ] Start resumes
* [ ] gameplay freezes correctly
* [ ] audio behavior correct

## NS-M01-005 — Core tests

* [ ] state transitions
* [ ] pause
* [ ] frame update
* [ ] reset

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

* [ ] approximately 24×16
* [ ] readable
* [ ] debug hitbox

## NS-M02-003 — Movement

* [ ] 8 directions
* [ ] max 5 px/frame
* [ ] immediate response
* [ ] minimal inertia

## NS-M02-004 — Screen boundaries

* [ ] horizontal limits
* [ ] vertical limits

## NS-M02-005 — Player hitbox

* [ ] 6×6 collision box

## NS-M02-006 — Damage

* [ ] weapon level -1
* [ ] invulnerability
* [ ] damage animation
* [ ] level 1 damage → life loss

## NS-M02-007 — Death

* [ ] life decrement
* [ ] player reset
* [ ] weapon L1
* [ ] death state

## NS-M02-008 — Lives

* [ ] 3 initial lives
* [ ] display/debug
* [ ] loss
* [ ] 1UP

## NS-M02-009 — Player tests

* [ ] movement
* [ ] boundaries
* [ ] hitbox
* [ ] damage
* [ ] invulnerability
* [ ] death
* [ ] lives

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

---

# FILE: PROGRESS.md

# PROGRESS.md — NEON STRIKE

## Current State

```text
PROJECT: NEON STRIKE

CURRENT_MILESTONE:
CURRENT_TASK:

BUILD:
TESTS:
EMULATOR:
GAMEPLAY:
VISUAL:
AUDIO:
PERFORMANCE:

LAST_UPDATE:
```

---

# Milestone Status

```text
M00 Baseline Audit             [ ]
M01 Core Game Loop             [ ]
M02 Player                     [ ]
M03 Primary Weapon             [ ]
M04 Weapon System              [ ]
M05 Enemy Framework            [ ]
M06 Power-ups/Bomb/Combo       [ ]
M07 Stage Data System          [ ]
M08 Stage 1                    [ ]
M09 Boss Framework             [ ]
M10 Stages 2–3                 [ ]
M11 Stages 4–5                 [ ]
M12 Stage 6                    [ ]
M13 Final Boss                 [ ]
M14 HUD/Score/Game Flow        [ ]
M15 Audio                      [ ]
M16 Effects/Polish             [ ]
M17 Performance Optimization  [ ]
M18 Full Campaign Validation  [ ]
M19 Release                    [ ]
```

---

# Current Objective

```text
Milestone:
Task:
Objective:
```

---

# Current Work

```text
Implemented:
Testing:
Validation:
Known problem:
Next action:
```

---

# Completed Milestones

## Milestone Template

```text
## MXX — NAME

Status: COMPLETE

Implemented:
- 

Tests:
- 

Validation:
- 

Performance:
- 

Visual:
- 

Known issues:
- 

Notes:
- 
```

---

# Validation Status

## Build

```text
Clean build:
ROM generated:
Boot:
```

## Automated Tests

```text
Passed:
Failed:
Skipped:
```

## Emulator

```text
Boot:
Controls:
Gameplay:
Audio:
Transitions:
```

## Performance

```text
FPS:
Enemies:
Enemy bullets:
Sprites:
Moving objects:
Frame time:
```

## Visual

```text
Player:
HUD:
Enemies:
Projectiles:
Backgrounds:
Parallax:
Bosses:
Effects:
Ending:
```

---

# Known Issues

| ID | Severity | Area | Description | Status |
| -- | -------- | ---- | ----------- | ------ |
|    |          |      |             |        |

Severity:

```text
CRITICAL
HIGH
MEDIUM
LOW
```

---

# Decisions

| Date | Decision | Reason |
| ---- | -------- | ------ |
|      |          |        |

Record important architectural or gameplay decisions here.

---

# Performance Notes

Record measured results here.

Example:

```text
Stage:
Average FPS:
Minimum FPS:
Enemy peak:
Bullet peak:
Sprite peak:
Object peak:
Frame time:
Notes:
```

---

# Final Validation

Do not mark this section PASS until M18 and M19 are complete.

```text
BUILD       = NOT VALIDATED
TESTS       = NOT VALIDATED
GAMEPLAY    = NOT VALIDATED
CAMPAIGN    = NOT VALIDATED
VISUAL      = NOT VALIDATED
AUDIO       = NOT VALIDATED
PERFORMANCE = NOT VALIDATED
RELEASE     = NOT VALIDATED
```

---

# FILE: README.md

# NEON STRIKE

Sega Mega Drive / Genesis horizontal shoot-'em-up.

## Game

NEON STRIKE is a 16-bit cyberpunk military shmup inspired by early-1990s arcade design.

The player controls the NX-01 interceptor against the autonomous HELIOS military network.

Campaign:

```text
6 stages
6 major bosses
1 final boss
25–35 minutes
```

## Platform

```text
Sega Mega Drive / Genesis
320×224
60 FPS
68000
Z80
YM2612
PSG
```

## Development

The project uses the existing Mega Drive toolchain and emulator.

Do not replace working infrastructure unnecessarily.

## Documentation

Read in this order:

```text
SPEC.md
AGENT.md
MILESTONES.md
PROGRESS.md
TASKS.md
README.md
```

## Development Workflow

```text
READ
↓
INSPECT
↓
IMPLEMENT
↓
BUILD
↓
TEST
↓
EMULATOR
↓
VALIDATE
↓
FIX
↓
RETEST
↓
UPDATE PROGRESS
```

## Project Structure

Expected structure:

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

The actual structure may differ if the existing toolchain already establishes another organization.

## Performance Targets

```text
60 FPS
320×224
<=12 active enemies
<=80 enemy bullets
<=100 moving gameplay objects
<=64 visible hardware sprites
16.67 ms/frame target
```

## Gameplay

Controls:

```text
D-pad = Move
A     = Fire
B     = Switch weapon
C     = Bomb
Start = Pause
```

Weapons:

```text
Vulcan
Laser
Wide
Homing
Flame
```

Power-ups:

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

## Campaign

```text
1. Orbital City
2. Red Desert
3. Space Colony
4. Asteroid Belt
5. Helios Fleet
6. Helios Core
```

Final boss:

```text
HELIOS
```

## Development Philosophy

Priorities:

1. gameplay;
2. control precision;
3. readability;
4. stable 60 FPS;
5. learnable patterns;
6. meaningful weapon management;
7. boss design;
8. visual polish.

Do not sacrifice gameplay for decoration.

## Definition of Done

The project is complete only when:

* ROM builds from clean checkout;
* ROM boots;
* all gameplay systems work;
* all stages work;
* all bosses work;
* HELIOS works;
* ending and credits work;
* audio works;
* tests pass;
* visual validation passes;
* performance validation passes;
* no critical crashes or softlocks exist;
* full campaign can be completed without modifying code.

Final status:

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

---

# END OF PROJECT DOCUMENTATION
