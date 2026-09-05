# NEON STRIKE — DEVELOPMENT PROGRESS

This file is persistent agent state.

The agent MUST update it after every completed milestone.

---

# CURRENT STATE

```text
Current Milestone: M03
Status: NOT STARTED
Overall Status: IN DEVELOPMENT
```

---

# MILESTONE STATUS

```text
M00  Baseline Audit              [x]
M01  Core Game Loop              [x]
M02  Player                      [x]
M03  Primary Weapon              [ ]
M04  Weapon System               [ ]
M05  Enemy Framework             [ ]
M06  Power-ups / Bomb / Combo    [ ]
M07  Stage Data System           [ ]
M08  Stage 1                     [ ]
M09  Boss Framework              [ ]
M10  Stages 2–3                  [ ]
M11  Stages 4–5                  [ ]
M12  Stage 6                     [ ]
M13  Final Boss                  [ ]
M14  HUD / Score / Game Flow     [ ]
M15  Audio                       [ ]
M16  Effects / Polish            [ ]
M17  Performance                 [ ]
M18  Full Campaign Validation    [ ]
M19  Release                     [ ]
```

Use:

```text
[ ] NOT STARTED
[-] IN PROGRESS
[x] PASS
[!] BLOCKED
```

---

# CURRENT MILESTONE

```text
ID: M03
Name: Primary Weapon
Status: NOT STARTED
```

## Current Objective

```text
Give the player something to shoot with: Vulcan L1-L3 (SPEC.md §7),
a player-projectile pool (TASKS.md M03), fired on the A button.
```

## Current Work

```text
Not started. M02 just completed.
```

## Next Action

```text
Read TASKS.md M03 tasks in full. Design a fixed-size Projectile pool
(no heap, per AGENT.md §8) sized per SPEC.md §24 Object Budgets, and a
Weapon module that owns fire-rate/spread per Vulcan level and spawns
into that pool from Player_update's A-button input.
```

---

# VALIDATION STATUS

```text
Build:       PASS   (make clean && make succeeds)
Tests:       PASS   (make test: 6/6 game_state + 4/4 player_logic assertions, host-native, no emulator)
Emulator:    PASS   (BlastEm, built from source, boots the ROM cleanly)
Visual:      PASS   (title, NX-01 placeholder sprite, FPS counter, debug hitbox/lives HUD,
             damage blink, HIT!/GAME OVER flow all confirmed by user on screen)
Performance: UNKNOWN (never stress-tested; trivial scene only so far)
Gameplay:    UNKNOWN (player exists; no weapons/enemies/stages implemented yet)
Audio:       PASS   (PSG melody plays in GAME state, keeps playing through PAUSE)
Campaign:    UNKNOWN
```

---

# COMPLETED MILESTONES

## M00

```text
Status: PASS
Date: 2026-09-05
Summary: Baseline environment audit. m68k-elf-gcc/binutils toolchain built
from source (Homebrew, with a texinfo build-dep fix), SGDK cloned at
~/dev/sgdk with GDK env var set, official libmd.a relinked with its LTO
bytecode stripped (built with a newer GCC than the archived library),
BlastEm built from source at ~/dev/blastem-src (the 0.6.2 release binary
crashes BlastEm's Z80 core on this ROM's driver; a from-source build does
not). Prototype gameplay existed: VDP text, one PSG-driven melody, one
animated PNG sprite (SGDK's donut sample) moved by the D-pad, split into
input.c/player.c/audio.c.

Build:       PASS - `make clean && make` from ~/mega-drive-hello-world
Tests:       N/A - no test harness existed yet, out of scope for M00
Emulator:    PASS - `blastem out/rom.bin` (wrapper at /usr/local/bin/blastem)
Visual:      PASS - HELLO WORLD text + spinning/moving donut sprite confirmed on screen
Performance: N/A - scene was trivial, not representative of gameplay load

Notes: NS-M00-005's literal directory tree (src/game/, src/enemies/, ...,
assets/, tests/, tools/, validation/, build/ as sibling roots) was NOT
created as specified. Per DEVELOPMENT_PLAN.md §1, `res/` already is the
assets root and `out/` already is the build root (AGENT.md §3: "do not
introduce another toolchain" / no second build system); duplicating them
as assets/ and build/ would violate that. Resolution: keep SGDK's src/
res/ inc/ out/ roots, organize by subsystem inside src/ incrementally as
each milestone actually touches it, add tests/ standalone (done, see M01).
This is a deliberate, documented deviation, not an oversight.
```

## M01

```text
Status: PASS
Date: 2026-09-05
Summary: Replaced the ad-hoc main-loop prototype with an explicit game
state machine. Split into a hardware-free transition table
(game_state_logic.h/.c: GameState enum with all 12 states from the
embedded SPEC.md §33 — TITLE, GAME, BOSS, STAGE_CLEAR, NEXT_STAGE, PAUSE,
PLAYER_HIT, PLAYER_DEAD, GAME_OVER, CONTINUE, ENDING, CREDITS — plus
GameState_computeNext(), a pure function with no SGDK/hardware calls) and
an SGDK-side wrapper (game_state.c) that drives enter/update per state and
owns the existing input.c/player.c/audio.c modules as GAME state's
content. main.c is now just SPR_init + Input_init + GameState_init + a
loop calling GameState_update()/SYS_doVBlankProcess(). Only TITLE, GAME
and PAUSE have real behavior (M01 scope per MILESTONES.md acceptance);
BOSS/STAGE_CLEAR/NEXT_STAGE/PLAYER_HIT/PLAYER_DEAD/GAME_OVER/CONTINUE/
ENDING/CREDITS exist in the enum and pass through START unchanged, ready
for the milestones that give them behavior.

Conflict identified and resolved (AGENT.md §2): MILESTONES.md's M01 state
list uses "STAGE" where the higher-priority embedded SPEC.md §33 (inside
PROJECT.md) and TASKS.md both use "GAME" for the same slot. Went with
SPEC.md/TASKS.md ("GAME") per the documented source-of-truth order
(SPEC.md > AGENT.md > MILESTONES.md). VALIDATION.md §6 lists both GAME
and STAGE as distinct states, but VALIDATION.md isn't in AGENT.md §2's
priority chain at all, so it didn't override this.

Design choices made where SPEC/TASKS left them open: START toggles
GAME<->PAUSE (edge-detected via new InputState.startPressed in input.c,
not "held", to avoid rapid re-toggling); PAUSE freezes Player_update and
SPR_update (donut visually stops) but keeps Audio_update ticking (music
continues) — the common arcade convention, and what TASKS.md NS-M01-004's
"audio behavior correct" is read to mean here.

Build:       PASS - `make clean && make`
Tests:       PASS - `make test` (6/6 assertions on GameState_computeNext,
             native host binary, no emulator): TITLE stays TITLE without
             START, TITLE->GAME, GAME->PAUSE, PAUSE->GAME (resume), GAME
             stays GAME without START, states with no M01 transition
             (e.g. BOSS) pass through unchanged. Covers TASKS.md
             NS-M01-005's "state transitions" and "pause"; "frame update"
             and "reset" are structural (VBlank-gated loop, GameState_init
             sets TITLE) and covered by emulator/visual validation instead,
             per DEVELOPMENT_PLAN.md §2's test-split rationale.
Emulator:    PASS - boots cleanly in BlastEm, no crash observed
Visual:      PASS - confirmed by user on screen: title screen appears,
             START starts the game (text+sprite+music), START again shows
             PAUSE and freezes the donut while music keeps playing, START
             again resumes cleanly
Performance: N/A - no additional per-frame cost of consequence over M00's
             trivial scene; 60 FPS baseline unaffected (still purely
             VBlank-gated, no busy-waiting or wall-clock-dependent timing
             anywhere)

Notes: No on-screen debug/FPS overlay (AGENT.md §15) was built — not
required by MILESTONES.md's M01 acceptance list, deferred until a
milestone actually needs it.
```

## M02

```text
Status: PASS
Date: 2026-09-05
Summary: Replaced the placeholder donut with a purpose-built NX-01
placeholder (res/sprite/nx01.png, 24x16, ImageMagick-generated indexed
PNG — a readable triangular ship silhouette, not final art) and gave
Player the full entity per SPEC.md §6 / TASKS.md NS-M02-001: position,
velocity, direction (bitmask of held D-pad buttons), speed (5 px/frame
max, was 2), weapon+weaponLevel, lives, bombs, invulnerability state,
and a 6x6 hitbox tracked independently of the 24x16 sprite (centered,
recomputed every frame). Screen boundary clamp updated for the smaller
sprite size.

Damage/death resolution (weapon-level-down vs. life-loss decision) lives
in player_logic.h/.c as a pure function (Player_applyHit), mirroring
game_state_logic.c's split — unit-tested natively, no emulator. Player.c
wraps it with real effects: invulnerability window (90 frames, ~1.5s)
with a blinking sprite (SPR_setVisibility toggle), and on a life-costing
hit, hides the sprite and marks the player dead for game_state.c to react
to. game_state.c now drives GAME -> PLAYER_HIT -> (respawn back to GAME
if lives remain, else GAME_OVER) -> TITLE (on START), extending M01's
skeleton states with real behavior for the first time. GAME_OVER's
sprite cleanup (SPR_releaseSprite) was added deliberately: without it,
repeated deaths across play sessions would leak hardware sprites, since
the next TITLE->GAME creates a fresh Player_init().

No real hazard exists yet (weapons are M03, enemies are M05), so there
was no way to trigger or visually validate damage/invulnerability/death
without one. Resolution: B is wired, for now, to Player_hit() as an
explicit debug-only self-damage trigger (see debug.h and the comment in
game_state.c) — this is scaffolding to validate a real, already-built
feature, not a faked completion; it gets removed once M03/M05 give the
player something else to actually get hit by.

Also added per user request, gated behind the same SHOW_DEBUG_HUD flag
(debug.h): an on-screen FPS counter (VDP_showFPS, top-right) and a
LIVES/weapon-level debug readout (bottom-left). AGENT.md §15: debug
rendering isn't required in the final release build — SHOW_DEBUG_HUD is
a single flag to flip before M19.

Conflict/gap identified and resolved (AGENT.md §2): MILESTONES.md/
TASKS.md don't specify how PLAYER_HIT/PLAYER_DEAD/GAME_OVER concretely
wire together (only that "death works"/"lives work"). Implemented the
conventional shmup reading consistent with VALIDATION.md §6's example
flow (GAME -> PLAYER_HIT -> PLAYER_DEAD -> CONTINUE/GAME_OVER): a fixed
~1s PLAYER_HIT beat, then respawn-in-place if lives remain or GAME_OVER
if not. The dedicated CONTINUE state/counter (SPEC.md §19, §26) was left
unwired — GAME_OVER goes straight to TITLE — since a continue-credit
system is not part of M02's scope and doesn't have a milestone number of
its own yet in MILESTONES.md.

Build:       PASS - `make clean && make`
Tests:       PASS - `make test`: game_state (8/8 now, incl. new
             GAME_OVER<->TITLE assertions) + player_logic (4/4: L3 hit
             -> L2 no death, L1 hit with lives left costs a life not
             death, last-life L1 hit is game over, hit-at-0-lives never
             underflows)
Emulator:    PASS - boots cleanly in BlastEm, no crash
Visual:      PASS - confirmed by user on screen: NX-01 placeholder ship
             renders and moves (5 px/frame, 8-direction, clamped to
             screen), FPS counter visible, LIVES/LV debug readout
             visible, debug hitbox outline visible, B triggers a visible
             invulnerability blink, repeated B hits at weapon L1 produce
             HIT! then (after 3 lives lost) GAME OVER, START from
             GAME OVER returns cleanly to TITLE
Performance: N/A - two sprites (ship + debug hitbox marker) plus text;
             not representative of real gameplay load, no stress test yet

Notes: "animation" (MILESTONES.md M02) is, for now, only the
invulnerability blink — the NX-01 placeholder has no idle/bank frames of
its own (single static image). Revisit once real art exists or a later
milestone specifically asks for one. Weapon/bombs fields exist on Player
but are inert (no firing) until M03/M06.
```

---

# KNOWN ISSUES

| ID | Severity | Description | Status |
| -- | -------- | ----------- | ------ |
|    |          |             |        |

Severity:

```text
CRITICAL
HIGH
MEDIUM
LOW
```

---

# DECISIONS

Record important technical decisions made during development.

| Date | Decision | Reason |
| ---- | -------- | ------ |
| 2026-09-05 | State machine uses "GAME" not "STAGE" for the core gameplay state | Resolves a MILESTONES.md vs. embedded SPEC.md/TASKS.md naming conflict per AGENT.md §2 priority order |
| 2026-09-05 | Pure transition logic lives in game_state_logic.c/h with zero SGDK includes | Only way to unit-test 68000 ROM logic on the host without an emulator (DEVELOPMENT_PLAN.md §2) |
| 2026-09-05 | NS-M00-005's full directory tree not created; reorganizing src/ incrementally per-milestone instead | Avoids a second assets/build root duplicating res//out/, and a big-bang restructure risking the working baseline (AGENT.md §3, §16; DEVELOPMENT_PLAN.md §1) |
| 2026-09-05 | B button wired to Player_hit() as a debug-only self-damage trigger | No real hazard exists before M03 (weapons)/M05 (enemies); needed a way to actually exercise and visually validate the already-implemented damage/invulnerability/death system now, per AGENT.md §18 (do not fake completion) |
| 2026-09-05 | GAME_OVER goes straight to TITLE; the CONTINUE state is left unwired | SPEC.md §19/§26's continue-credit system has no milestone of its own yet and is out of M02's scope; the enum/state already exists for whichever milestone claims it |
| 2026-09-05 | On-hit resolution (weapon-down vs. life-loss) is a pure function in player_logic.c, mirroring game_state_logic.c | Same host-native-testability rationale as M01 (DEVELOPMENT_PLAN.md §2) |

---

# PERFORMANCE NOTES

```text
Target FPS: 60
Observed FPS: on-screen counter now exists (VDP_showFPS, top-right, SHOW_DEBUG_HUD); no numeric reading recorded yet
Max enemies: 0 (none implemented yet)
Max projectiles: 0 (none implemented yet)
Max sprites: 2 (player + debug hitbox marker)
Worst-case frame time: not measured
```

---

# FINAL VALIDATION

Complete only at M19.

```text
BUILD: PASS/FAIL
TESTS: PASS/FAIL
EMULATOR: PASS/FAIL
VISUAL: PASS/FAIL
PERFORMANCE: PASS/FAIL
GAMEPLAY: PASS/FAIL
AUDIO: PASS/FAIL
CAMPAIGN: PASS/FAIL

DEFINITION OF DONE: PASS/FAIL
```
