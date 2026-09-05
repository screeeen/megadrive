# NEON STRIKE — IMPLEMENTATION MILESTONES

## PURPOSE

Implement NEON STRIKE incrementally.

Each milestone must compile, run and satisfy its acceptance criteria before the next milestone begins.

---

# M00 — BASELINE AUDIT

## Goal

Understand and preserve the existing Hello World environment.

## Tasks

* inspect repository
* inspect build system
* inspect emulator configuration
* inspect PNG pipeline
* inspect music pipeline
* document reusable components

## Acceptance

* [ ] existing project builds
* [ ] existing ROM runs
* [ ] existing graphics work
* [ ] existing music works
* [ ] no baseline regression

---

# M01 — CORE GAME LOOP

## Goal

Create the basic game architecture.

## Implement

* main loop
* 60 Hz update
* game state machine
* input system
* basic rendering
* stage manager

## States

```text
TITLE
STAGE
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

## Acceptance

* [ ] game boots
* [ ] title exists
* [ ] game starts
* [ ] pause works
* [ ] states transition correctly
* [ ] 60 FPS baseline

---

# M02 — PLAYER

## Implement

* NX-01
* 8-direction movement
* screen boundaries
* speed
* hitbox
* animation
* death
* invulnerability
* lives

## Acceptance

* [ ] movement works
* [ ] movement is responsive
* [ ] hitbox is 6×6
* [ ] player cannot leave screen
* [ ] death works
* [ ] invulnerability works
* [ ] lives work

---

# M03 — PRIMARY WEAPON

## Implement

* Vulcan
* firing
* cadence
* projectile creation
* collision
* levels 1/2/3

## Acceptance

* [ ] A fires
* [ ] projectile moves
* [ ] projectile collision works
* [ ] weapon levels work
* [ ] projectile limits work

---

# M04 — WEAPON SYSTEM

## Implement

* Laser
* Wide
* Homing
* Flame
* weapon switching
* weapon levels

## Acceptance

* [ ] all five weapons work
* [ ] switching works
* [ ] levels work
* [ ] distinct behavior exists
* [ ] projectile limits respected

---

# M05 — ENEMY FRAMEWORK

## Implement

* Drone
* Fighter
* Bomber
* Turret
* Swarm
* Charger
* Shield
* enemy health
* enemy destruction
* enemy projectiles
* scoring

## Acceptance

* [ ] all enemy types work
* [ ] movement works
* [ ] attacks work
* [ ] damage works
* [ ] destruction works
* [ ] scoring works

---

# M06 — POWER-UPS / BOMB / COMBO

## Implement

* weapon power-up
* speed
* bomb
* 1UP
* weapon level loss
* combo
* continue

## Acceptance

* [ ] power-ups work
* [ ] bomb works
* [ ] combo works
* [ ] 1UP works
* [ ] weapon degradation works
* [ ] continue works

---

# M07 — STAGE DATA SYSTEM

## Implement

Data-driven:

* enemy spawns
* formations
* power-ups
* checkpoints
* scroll speed
* stage events
* boss triggers

## Acceptance

* [ ] stage can be described through data
* [ ] spawn timing works
* [ ] formations work
* [ ] checkpoints work
* [ ] stage transition works

---

# M08 — STAGE 1

## Orbital City

Implement:

* background
* parallax
* enemies
* formations
* power-ups
* checkpoints
* mini-boss
* boss

## Acceptance

* [ ] stage reaches expected duration
* [ ] all planned enemy types appear
* [ ] power-ups distributed correctly
* [ ] checkpoints work
* [ ] boss works
* [ ] boss has multiple phases
* [ ] no impossible patterns
* [ ] 60 FPS target maintained

---

# M09 — BOSS FRAMEWORK

## Implement reusable boss system

Support:

* HP
* phases
* attacks
* patterns
* vulnerable points
* telegraphs
* transitions
* death sequence

## Acceptance

* [ ] reusable boss architecture
* [ ] multiple phases work
* [ ] patterns are data/config driven
* [ ] boss death works

---

# M10 — STAGES 2–3

## Stage 2

Red Desert.

## Stage 3

Space Colony.

## Acceptance

* [ ] both stages playable
* [ ] unique environments
* [ ] unique hazards
* [ ] checkpoints
* [ ] mini-bosses
* [ ] bosses
* [ ] difficulty increases correctly
* [ ] performance remains acceptable

---

# M11 — STAGES 4–5

## Stage 4

Asteroid Belt.

## Stage 5

Helios Fleet.

## Acceptance

* [ ] both stages playable
* [ ] asteroid system works
* [ ] formations work
* [ ] increased projectile density works
* [ ] bosses work
* [ ] performance remains acceptable

---

# M12 — STAGE 6

## Helios Core

Implement:

* maximum difficulty
* obstacles
* lasers
* dense patterns
* fast enemies
* mini-bosses
* final boss entrance

## Acceptance

* [ ] stage playable
* [ ] difficulty appropriate
* [ ] projectile budget respected
* [ ] escape routes exist
* [ ] performance acceptable

---

# M13 — FINAL BOSS

## HELIOS

Implement three phases.

### Phase 1

Cannons, drones, missiles.

### Phase 2

Circular attacks and combinations.

### Phase 3

Exposed core, smaller safe zone, final attack patterns.

## Acceptance

* [ ] three phases
* [ ] clear telegraphs
* [ ] vulnerable windows
* [ ] no impossible pattern
* [ ] death sequence
* [ ] final transition
* [ ] ending triggered

---

# M14 — HUD / SCORE / GAME FLOW

## Implement

* score
* high score
* combo
* weapon display
* bomb display
* lives
* pause
* game over
* continue
* stage clear
* ranking

## Acceptance

* [ ] HUD readable
* [ ] no gameplay obstruction
* [ ] score correct
* [ ] combo correct
* [ ] all transitions work

---

# M15 — AUDIO

## Implement/integrate

* stage music
* boss music
* final boss music
* ending
* credits
* shooting SFX
* explosion SFX
* power-up SFX
* bomb SFX
* boss SFX
* 1UP SFX

## Acceptance

* [ ] music plays
* [ ] transitions work
* [ ] SFX work
* [ ] no critical audio corruption
* [ ] existing audio infrastructure preserved

---

# M16 — EFFECTS / POLISH

## Implement

* explosions
* hit effects
* weapon effects
* boss effects
* bomb effect
* transitions
* screen feedback

## Acceptance

* [ ] effects readable
* [ ] effects do not obscure player
* [ ] no excessive sprite usage
* [ ] performance remains stable

---

# M17 — PERFORMANCE OPTIMIZATION

## Stress tests

* maximum enemies
* maximum bullets
* boss
* explosions
* fast scroll
* multiple effects

## Acceptance

* [ ] 60 FPS target
* [ ] sprite budget respected
* [ ] projectile budget respected
* [ ] no critical slowdown
* [ ] no graphical corruption

---

# M18 — FULL CAMPAIGN VALIDATION

Play the complete game:

```text
TITLE
→ STAGE 1
→ BOSS
→ STAGE 2
→ BOSS
→ STAGE 3
→ BOSS
→ STAGE 4
→ BOSS
→ STAGE 5
→ BOSS
→ STAGE 6
→ FINAL BOSS
→ ENDING
→ CREDITS
```

## Acceptance

* [ ] complete campaign playable
* [ ] no softlocks
* [ ] no crashes
* [ ] no missing assets
* [ ] no critical glitches
* [ ] no impossible progression
* [ ] audio throughout
* [ ] score throughout
* [ ] checkpoints throughout

---

# M19 — RELEASE

## Final tasks

* remove debug HUD
* remove temporary debug code
* verify release build
* verify ROM
* verify clean build
* update README
* update PROGRESS
* generate final validation report

## Acceptance

* [ ] clean build PASS
* [ ] tests PASS
* [ ] emulator PASS
* [ ] visual PASS
* [ ] performance PASS
* [ ] campaign PASS
* [ ] Definition of Done PASS
