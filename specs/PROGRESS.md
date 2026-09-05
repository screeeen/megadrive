# NEON STRIKE — DEVELOPMENT PROGRESS

This file is persistent agent state.

The agent MUST update it after every completed milestone.

---

# CURRENT STATE

```text
Current Milestone: M09
Status: NOT STARTED
Overall Status: IN DEVELOPMENT
```

---

# MILESTONE STATUS

```text
M00  Baseline Audit              [x]
M01  Core Game Loop              [x]
M02  Player                      [x]
M03  Primary Weapon              [x]
M04  Weapon System               [x]
M05  Enemy Framework             [x]
M06  Power-ups / Bomb / Combo    [x]
M07  Stage Data System           [x]
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
ID: M08
Name: Stage 1
Status: NOT STARTED
```

## Current Objective

```text
Replace M07's placeholder testStage with the real Orbital City (SPEC.md
§16, §40.2): 38,400px length, real background/parallax art, the actual
enemy pacing/density curve across the stage's 4 sections, a mini-boss,
and the Orbital Guardian boss.
```

## Current Work

```text
Not started. M07 just completed.
```

## Next Action

```text
Read TASKS.md M08 tasks in full. M08 needs a working mini-boss and boss
(NS-M08-007/008) but Boss Framework is M09 — the same kind of
forward-reference TASKS.md/MILESTONES.md already had for M03's enemy
collision and M04's Homing target selection. DEVELOPMENT_PLAN.md §3
already flagged this (Phase B groups M08+M09 together for exactly this
reason) — likely resolution: build enough of a generic boss (multi-phase
HP gate, defeat -> STAGE_CLEAR) alongside Stage 1's content rather than
strictly finishing M08 before starting M09, or build a minimal boss now
and let M09 generalize it, mirroring how M05 was built once and M09 will
reuse its patterns for every other boss.
```

---

# VALIDATION STATUS

```text
Build:       PASS   (make clean && make succeeds, zero warnings)
Tests:       PASS   (make test: 8/8 game_state + 10/10 player_logic + 7/7 collision_logic + 3/3 enemy_logic assertions, host-native, no emulator)
Emulator:    PASS   (BlastEm, built from source, boots the ROM cleanly)
Visual:      PASS   (title, NX-01 sprite, full debug HUD, damage blink,
             HIT!/GAME OVER/CONTINUE/STAGE CLEAR/NEXT STAGE flow, all 5
             weapons, all 7 enemy types (including data-driven formations),
             all 8 power-ups, bomb, combo multiplier, real BG_B parallax
             scroll, and checkpoint-aware respawn all confirmed by user —
             including a real bug the user caught and this milestone fixed
             (orphaned enemy sprites after a hit, see M07 summary)
Performance: UNKNOWN (never formally stress-tested; up to 12 enemies + up to
             96 pooled projectiles + up to 8 power-ups now possible, heaviest
             scene so far but not pushed to its limit)
Gameplay:    UNKNOWN (player, all 5 weapons, all 7 enemy types, combat,
             power-ups, bomb, combo, and a data-driven stage/scroll/
             checkpoint system exist; still no real Stage 1 content or bosses)
Audio:       PASS   (PSG melody plays in GAME state, keeps playing through
             PAUSE, and is properly silenced on GAME_OVER — was left stuck
             on a note before M06's polish pass)
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

## M03

```text
Status: PASS
Date: 2026-09-05
Summary: Vulcan L1/L2/L3 (SPEC.md §7) firing on a held A, into a fixed
16-slot Projectile pool (projectile.c/.h — no gameplay-time allocation,
per AGENT.md §8; a full pool silently drops the shot, since SPEC.md's
projectile limits are a design budget, not an error condition). weapon.c
owns per-level cadence and spread: L1 = 1 shot/10-frame cooldown, L2 = 2
shots (offset +-6px)/same cooldown, L3 = 3 shots/6-frame cooldown
(cadence numbers aren't specified exactly in SPEC.md; chosen here, see
DECISIONS). Damage per shot: L1=1, L2=2, L3=2 — L3's differentiator is
cadence, not per-shot damage, per SPEC.md's own wording. Bullets are a
small placeholder sprite (res/sprite/bullet_vulcan.png, generated), on
PAL2 (temporarily "spent" ahead of DEVELOPMENT_PLAN.md's PAL2=enemies
reservation — see KNOWN ISSUES).

Added collision_logic.h/.c: a pure AABB-overlap function, unit-tested
with 7 cases (partial/identical/edge-touching/disjoint/contained boxes).
This is real, tested infrastructure per SPEC.md §31 (deterministic
per-frame hitbox checks) — it is NOT wired to anything yet, because
NS-M03-007 asks for "projectile -> enemy" collision but no enemy exists
before M05. This is a genuine ordering gap in TASKS.md itself (M03 is
scheduled before M05 yet references enemies), not an oversight here;
resolved by building everything checkable now (the math) and explicitly
deferring the end-to-end demonstration to M05, rather than faking it
against a placeholder target.

To actually validate L2/L3 (SPEC.md gives the player no way to gain
weapon levels before M06's power-ups), added a second debug-only trigger:
C raises weaponLevel by one (capped at 3), alongside M02's existing
B-triggers-Player_hit(). Both are explicitly temporary scaffolding (see
debug.h and the comments at their call sites in game_state.c), each
removed once the milestone that gives it a permanent replacement lands
(M04/B for weapon switching conflicts with M02's damage trigger — not
yet resolved, see KNOWN ISSUES; M06 for the weapon-level-up trigger).

Build:       PASS - `make clean && make`
Tests:       PASS - `make test`: game_state (unchanged, 8/8) + player_logic
             (unchanged, 4/4) + new collision_logic (7/7: partial overlap,
             identical boxes, overlap from either side, edge-touching X/Y
             is NOT overlap, disjoint boxes, small-box-inside-large-box)
Emulator:    PASS - boots cleanly in BlastEm, no crash
Visual:      PASS - confirmed by user on screen: holding A fires
             Vulcan, bullets travel right and despawn cleanly off-screen
             with no accumulation; using the debug C trigger to reach
             L2 then L3 visibly changes the pattern from 1 to 2 to 3
             simultaneous bullets
Performance: N/A - up to 16 additional small sprites possible now
             (projectile pool ceiling); not stress-tested against other
             load yet since there's nothing else to stress it with

Notes: NS-M03-007 "projectile collisions" (enemy hit, damage, destruction,
score) is DEFERRED to M05 — see summary above. Not marked as a milestone
failure; it's an inherent cross-milestone dependency in the spec's own
ordering, documented rather than silently skipped or faked.
```

## M04

```text
Status: PASS
Date: 2026-09-05
Summary: Generalized M03's Vulcan-only weapon.c into the full 5-weapon
system (SPEC.md §7). WeaponType (VULCAN/LASER/WIDE/HOMING/FLAME) moved
from player.h into player_logic.h (it's a plain enum with no hardware
dependency, so it belongs with the other pure types) alongside two new
pure functions: Weapon_switchNext (always advances + resets to L1, for B)
and Weapon_levelUp (+1 capped at 3, for same-weapon pickups) — unit
tested, same pattern as Player_applyHit. Player_switchWeapon/
Player_levelUpWeapon wrap them onto the real Player struct.

Per-weapon behavior (exact numbers not in SPEC.md, chosen here — see
DECISIONS): Laser is a single fast (9px/frame) pierce-2 shot, L3 swaps to
a visually larger sprite (PROJECTILE_TYPE_LASER_BIG, res/sprite/
laser_big.png, 16x8 vs. the normal 8x8) and pierce-3, matching SPEC.md's
"visually larger but technically cheap" — same speed/cooldown/damage as
L1/L2, so the "cheap" half is real, not just a claim. Wide fires a fan of
3/5/7 projectiles (L1/L2/L3) via a small fixed offset table (no trig —
embedded-appropriate). Homing is a single shot capped at 4 simultaneous
missiles (Projectile_countActiveOfType, new in projectile.c) regardless
of level; it flies straight since there is no enemy list yet to seek —
documented as a stub, not faked. Flame is short-range via a new
Projectile.rangeRemaining field (counts down px traveled, releases at 0)
and highest per-shot damage.

B now does its real job (SPEC.md §5: switch weapon), replacing M02's
debug self-damage trigger — that trigger is gone for good; damage/death
remain validated from M02's record since M04 didn't touch that code
path. Since M06's power-ups don't exist yet, C still does M03's debug
job (force a weapon level-up) so L2/L3 patterns across all 5 weapons
could actually be seen firing, not just assumed correct from code
review (AGENT.md §18) — noted in DECISIONS that C will be needed for
its real job (SPEC.md §5: bomb) once M06 lands, same kind of
double-booking B just went through.

Hardware palette reality check: only PAL2 was free (PAL0=text,
PAL1=player, PAL3=debug hitbox), and 5 different projectile sprites
can't each get their own hardware palette line. Since only one weapon
is ever held at a time, projectile.c reloads PAL2 for the held weapon's
type at spawn time (a no-op most frames, since the type rarely changes)
instead of giving every type its own line — the accepted tradeoff is a
harmless few-frame miscoloring of any previous weapon's bullets still
in flight at the moment of a switch, documented rather than silently
shipped as "it just happens to look right."

Build:       PASS - `make clean && make`, zero warnings (fixed a
             -Wswitch on the new WEAPON_COUNT sentinel)
Tests:       PASS - `make test`: game_state (unchanged, 8/8) + player_logic
             (8/8, +4 new: VULCAN->LASER switch resets to L1 even from L3,
             the switch cycle wraps FLAME->VULCAN, level-up L1->L2, L3 is
             capped) + collision_logic (unchanged, 7/7)
Emulator:    PASS - boots cleanly in BlastEm, no crash
Visual:      PASS - confirmed by user on screen: B cycles all 5 weapons
             in order (visible via the debug HUD's weapon-name readout),
             each fires a visually distinct projectile, C-driven level-ups
             visibly change Vulcan/Wide's shot count and Laser's sprite
             size at L3

Notes: NS-M04-002's "pickup changes weapon" is still open — there is no
pickup system before M06, same deferral shape as M03's collision item.
```

## M05

```text
Status: PASS
Date: 2026-09-05
Summary: Closed out M03/M04's deferred "projectile -> enemy" item for
real. All 7 enemy types (SPEC.md §11) implemented in enemy.c/enemy_logic.c
(the latter a pure Enemy_applyDamage mirroring player_logic.c's pattern —
unit tested), each in a fixed 12-slot pool (SPEC.md §24). Drone (HP1,
fast, no fire), Fighter (HP2, aimed fire), Bomber (HP4, slow, 3-shot
burst), Turret (HP3, stationary, aimed fire), Swarm (HP1, fast, a
simplified oscillating shot), Charger (HP2, pauses then dashes straight
at the player's position from the moment it commits), Shield (HP6, cycles
vulnerable/shielded — damage is silently ignored while shielded, the
simplification adopted for SPEC.md's "attack from an angle/opening"
without real directional-facing mechanics).

Added bullet_pattern.c implementing all 8 TASKS.md NS-M05-011 patterns as
small LUT-based functions (no trig — SGDK has no FPU) over a new shared
PROJECTILE_TYPE_ENEMY_BULLET. Only aimed/burst/wave/cross are actually
fired by an enemy type right now; diagonal/circular/top_bottom/combined
exist and are implemented but unexercised by any enemy yet — ready for
M07/M08 stage design, not faked as "done" (AGENT.md §18).

Added combat.c: player-bullets-vs-enemies (damage/destroy/score, using
collision_logic.c's already-tested AABB check), enemy-bullets-vs-player,
and enemy-contact-vs-player, all in one small module since they share the
same box-overlap primitive. This is Player_hit()'s first real (non-debug)
caller. Added score.c (Score_add/Score_get) — minimal but real; the full
HUD format (SPEC.md §22) stays M14's job.

Projectile pool grown from 16 to 96 slots (SPEC.md §24: ~16 player + ~80
enemy). Enemy bullets and all 7 enemy sprites deliberately share ONE
hardware palette line (PAL3): unlike weapons, several different enemy
types can be on screen and firing at once, so the "reload per active
type" trick from M04 doesn't work here. Retired the debug hitbox marker
sprite (M02) to free PAL3 — its data (Player.hitboxX/Y) is unaffected,
only the visual box is gone. All 8 placeholder PNGs (7 enemies + enemy
bullet) were generated with the exact same 2 colors, remapped onto one
reference palette image with `-remap`, verified byte-identical via
`identify -verbose`'s Colormap output before committing to the approach
— not assumed to work.

No Spawn Manager exists yet (M07), so a small debug-only cycling spawner
(game_state.c's debugSpawnEnemies) drops one enemy of the next type every
90 frames at a fixed off-screen-right position. This is explicitly
throwaway scaffolding for M05's own validation, not stage content — M07
replaces it outright with real spawn-event data.

Fixed a real bug before it shipped: several enemy shoot-timers used
`if (--timer == 0 || timer > COOLDOWN)`, relying on intentional unsigned
underflow (timer starts at 0) to fire on the first eligible frame. It
worked, but was a fragile, confusing pattern for something meant to be
deterministic and reviewable (AGENT.md §8) — rewritten as a plain
`if (timer > 0) timer--; else { fire; timer = COOLDOWN; }` before this
milestone was built, not after a bug report.

Build:       PASS - `make clean && make`, zero warnings
Tests:       PASS - `make test`: game_state/player_logic/collision_logic
             unchanged + new enemy_logic (3/3: partial damage, lethal
             damage, overkill damage never goes negative)
Emulator:    PASS - boots cleanly in BlastEm, stable over an extended run
             (20+s, long enough for the debug spawner to cycle every type
             at least once)
Visual:      PASS - confirmed by user on screen: all 7 types spawn, move,
             and (where applicable) fire distinctly; each dies in the HP
             count matching its spec; Shield visibly ignores damage while
             shielded; score increments on kill (debug HUD); enemy
             contact damages the player the same way enemy bullets do
Performance: UNKNOWN - heaviest scene so far (up to 12 enemies + up to 96
             projectiles theoretically possible) but not deliberately
             stress-tested against that ceiling yet (M17's job)

Notes: NS-M04-005's Homing "target selection" and NS-M04-003's Laser
"pierce" are STILL open even though enemies now exist — Weapon.c's
Homing/Laser firing code wasn't touched this milestone (out of scope;
M05 was about enemies existing and being damageable, not revisiting M04's
weapons). Tracked as a follow-up, not silently dropped.
```

## M06

```text
Status: PASS
Date: 2026-09-05
Summary: All 8 power-up types (SPEC.md §10) implemented in a new
powerup.c pool (8 slots, falls+drifts slowly, off-screen removal), plus
player_logic.c's new Weapon_pickup (unit tested: same weapon held levels
up, a different one switches to it at L1 — mirrors Weapon_switchNext/
Weapon_levelUp's pattern) and player.c wrappers for the non-weapon types
(Player_addBomb capped at 3, Player_applySpeedBoost with automatic
5-second restoration via a new Player.speedBoostFrames countdown,
Player_addLife). Power-ups drop from enemy kills — every 3rd kill,
cycling through all 8 types (no per-enemy drop table exists; that's
stage-design territory, M07/M08) — rather than a separate debug spawner,
since "enemies drop power-ups" already is the real mechanic.

Added bomb.c: C (finally resolving NS-3's double-booking with M03's debug
level-up trigger, which is retired now that real weapon/P pickups exist
in-game) consumes one bomb and deals lethal damage to every active enemy
— explicitly bypassing Shield's directional resistance, since SPEC.md §9
frames a bomb as an emergency tool, not a normal attack a shield should
block — clears every active enemy projectile, and grants ~0.5s of
player invulnerability via the existing PLAYER_STATE_INVULNERABLE path.

Added combo.c: a x1-x5 multiplier that increases per kill and resets
after a 2-second no-kill timeout (not specified exactly in SPEC.md).
combat.c now applies it to every kill's score, whether from a player
bullet or a bomb.

Finally gave the previously-unused STATE_CONTINUE (present since M01)
real behavior, resolving MILESTONES.md M06's explicit "continue works"
acceptance item that TASKS.md's own M06 task list didn't itemize (a gap
between the two documents, not a TASKS.md checkbox I skipped): GAME_OVER
now branches on a new continuesRemaining counter (3 per campaign,
SPEC.md §26) — START offers CONTINUE while any remain (a brief
"CONTINUE? N LEFT" beat, then gameplay resumes with weapon reset to L1,
exactly one bomb, a fresh Player_init, and score deliberately preserved
across the continue — not specified either way in SPEC.md, chosen to
reward the run rather than punish it further), or goes straight to
TITLE once continues are exhausted.

Also fixed three things flagged directly during this session's testing
(not part of any TASKS.md checklist, but real, user-reported bugs):
removed the leftover M00-era "HELLO WORLD!" text from the GAME screen;
added Audio_stop() (mutes PSG channel 0) called from GameOver_enter(),
since a PSG channel latches its last note forever once nothing calls
Audio_update() on it anymore — GAME_OVER previously left a stuck tone
playing; added a SCORE readout to the GAME OVER screen, which previously
showed none at all.

Build:       PASS - `make clean && make`, zero warnings
Tests:       PASS - `make test`: game_state/collision_logic/enemy_logic
             unchanged, player_logic grew from 8 to 10 (+2 for
             Weapon_pickup: same-weapon pickup levels up and keeps type,
             different-weapon pickup switches type and resets to L1 even
             from L3)
Emulator:    PASS - boots cleanly in BlastEm, stable over an extended run
Visual:      PASS - confirmed by user on screen: power-ups drop, fall,
             and are collectible; each of the 8 types has its documented
             effect (weapon switch/level-up for L/W/H/F, P levels the
             held weapon, B raises the bomb count, S visibly speeds up
             movement for its duration then reverts, 1UP raises the
             lives count); C clears the screen, damages/kills enemies
             (Shield included), shows "BOMB!", and spends a bomb; the
             combo multiplier rises on rapid kills and decays back to x1
             after a pause; GAME OVER offers CONTINUE while any remain
             (resuming with L1/1 bomb/preserved score) and falls through
             to TITLE once they're used up

Notes: NS-M04-005 (Homing target selection) and NS-M04-003 (Laser
pierce consumption details) remain open — still not this milestone's
scope (weapon.c wasn't touched). NS-6 (no free palette line) is now
fully realized rather than theoretical: power-ups share PAL3 with
enemies/enemy bullets using the same byte-identical-palette technique
as M05, verified the same way before relying on it.
```

## M07

```text
Status: PASS
Date: 2026-09-05
Summary: Replaced M05/M06's debugSpawnEnemies() cycling placeholder with
a real data-driven system. stage.h defines SpawnEvent (frame/type/x/y/
variant, per TASKS.md NS-M07-001 exactly) and StageDef (NS-M07-002:
length/scrollSpeed/spawnEvents/checkpoints/boss-background-music IDs).
`type` is a category byte (SPAWN_TYPE_ENEMY/SPAWN_TYPE_POWERUP) rather
than the spawnable thing's own type, since EnemyType and PowerupType are
separate enums with overlapping numeric ranges that don't fit in one
byte together — `variant` holds the actual EnemyType/PowerupType value,
disambiguated by `type`.

spawn_manager.c walks a StageDef's events by comparing a frame cursor to
each event's frame (NS-M07-003), calling Enemy_spawn/Powerup_spawn
directly — pool-limit enforcement was already built into those (M05/M06),
nothing new needed there. Checkpoints (NS-M07-004, ~40%/~75%) are tracked
as "how many passed" and drive a real rewind-on-death: SPEC.md §20's
convention (die -> resume from the last checkpoint, replaying content
between it and the death point) is implemented via
SpawnManager_resumeFromCheckpoint(), used by both mid-life respawn and
continue.

Added scroll.c: a real horizontal-scrolling BG_B plane (VDP_setPlaneSize
64x32 + a repeating starfield TILESET + VDP_setHorizontalScroll each
frame at the stage's data-driven scrollSpeed, SPEC.md §14's 2px normal).
BG_A keeps hosting text/HUD undisturbed (VDP_drawText's default plane) —
the two never conflict since they're independent hardware planes. The
starfield tile shares PAL3 with enemies/power-ups (same identical-palette
technique), since trying to match it to the player's own PAL1 palette
proved unreliable via ImageMagick's `-unique-colors` (its color-order
enumeration doesn't reliably match a PNG's real on-disk PLTE order —
caught and abandoned before shipping, not after a visual bug).

stage_data.c holds the one placeholder `testStage` this milestone
validates the system with — NOT "Stage 1" (SPEC.md §16 Orbital City's
real content/pacing/art is M08's job) — including two deliberate
formations (2 Fighters, 3 Swarm, same frame each) to exercise
NS-M07-003's "spawn enemies"/data-driven formations, finally closing the
"formations"/"group movement" items M05 had flagged partial.

Added STATE_STAGE_CLEAR and STATE_NEXT_STAGE real behavior (both existed
unused since M01): reaching a stage's lengthFrames shows STAGE CLEAR then
NEXT STAGE, then loops back into the same placeholder stage (no Stage 2+
content exists yet, M10+) with score/lives/weapon/bombs preserved and
only the gameplay pools + timeline + scroll reset — demonstrating
NS-M07-007's completion/next-stage/entity-reset/state-preservation
requirements even without new stage content to transition into.

A real bug was caught during this session's testing (not from a TASKS.md
checklist, a genuine defect): Enemy_poolInit()/Projectile_poolInit()/
Powerup_poolInit() only ever cleared bookkeeping (active flags) — they
never called SPR_releaseSprite(), correctly so for their ORIGINAL use
(the very first call in Game_enter, when the pool is provably already
empty). But this milestone's new resetGameplayPools() (used on respawn
and next-stage) calls them when OTHER enemies/projectiles/power-ups can
still be active — the player died, nothing else was cleared. Skipping the
release step orphaned those sprites in SGDK's sprite engine, which kept
rendering them forever: "enemies stay painted on screen after a hit," as
the user described it. Fixed by having resetGameplayPools() call each
pool's *_releaseAll() immediately before its *_poolInit(), so any still-
active entities are properly released first regardless of caller.

Build:       PASS - `make clean && make`, zero warnings
Tests:       PASS - `make test`: unchanged (game_state/player_logic/
             collision_logic/enemy_logic) — this milestone's new modules
             (stage_data.c, spawn_manager.c, scroll.c) are all VDP/pool-
             driving orchestration with no hardware-free logic worth
             splitting out and testing natively, per the established
             test-split rationale (DEVELOPMENT_PLAN.md §2)
Emulator:    PASS - boots cleanly in BlastEm, stable over an extended run
             (30+s, past the ~20s point the test stage completes and loops)
Visual:      PASS - confirmed by user on screen: BG_B starfield visibly
             scrolls left continuously; enemies spawn on a real schedule
             including the 2-Fighter and 3-Swarm formations; STAGE
             CLEAR -> NEXT STAGE -> stage restarts with score/lives/
             weapon/bombs intact; dying past a checkpoint resumes content
             from that checkpoint rather than the stage start; the
             orphaned-sprite bug above was caught by the user, then
             re-confirmed fixed after the patch

Notes: "obstacles" (NS-M07-003's "spawn obstacles") has no implementation
at all — no obstacle entity/concept exists anywhere in the engine yet.
Not flagged as a defect: SPEC.md's obstacles (walls, doors, asteroids)
are stage-specific art/geometry (§18 Space Colony, §19 Asteroid Belt)
that don't appear in Stage 1 (§16 Orbital City) at all, so there's
nothing for M08 to need yet either — deferred to whichever later stage
milestone first requires one.
```

---

# KNOWN ISSUES

| ID | Severity | Description | Status |
| -- | -------- | ----------- | ------ |
| NS-1 | — | B double-booking (M02 debug damage vs. M04 weapon switch) | RESOLVED at M04 — B now always switches weapons; the debug damage trigger was removed |
| NS-2 | — | Palette budget for projectiles (PAL2) | RESOLVED at M05's shape — PAL2=player projectiles (reload-per-weapon), PAL3=all 7 enemies+enemy bullets (identical shared palette, since several can be active at once). All 4 hardware lines are now fully spoken for: PAL0=text, PAL1=player, PAL2=player bullets, PAL3=enemies/enemy bullets. Any future sprite category (power-ups, effects, background) has NO free line left — flagging as NS-6. |
| NS-3 | — | C double-booking (M03 debug level-up vs. M06 bomb) | RESOLVED at M06 — C now always uses the bomb; the debug level-up trigger was removed (real P/weapon power-ups reach L2/L3 in-game now) |
| NS-4 | LOW | Homing (WEAPON_HOMING) flies straight — weapon.c still wasn't revisited at M06 either (out of scope both times), so "target selection" (TASKS.md NS-M04-005) is still a stub | OPEN — resolve when weapon.c is next touched |
| NS-5 | LOW | Laser's "pierces small enemies" (pierceRemaining field) is decremented by combat.c on a hit but nothing yet distinguishes "small" from other enemies — every enemy is currently pierceable | OPEN — revisit once enemy "size" is a meaningful concept (not yet modeled) |
| NS-6 | — | No free hardware palette line for power-ups/effects/background | RESOLVED for power-ups AND background at M06/M07 — power-ups (M06) and the M07 starfield tile both share PAL3 with enemies/enemy bullets via the same identical-palette technique as M05. Effects (M16) and Stage 1's real background art (M08) still have no free line of their own; will need the same treatment (or a different one) when they arrive |
| NS-7 | LOW | Power-up drop scheme (every 3rd kill, cycling all 8 types) is still a placeholder even after M07 — SpawnEvent can place power-ups directly now (stage_data.c doesn't, testStage only drops via kills), but no per-enemy/per-stage drop table exists (SPEC.md §27's per-stage counts are stage-design content) | OPEN — resolve at M08 once real Stage 1 data can specify drops directly as SpawnEvents |
| NS-8 | MEDIUM | M08 (Stage 1) needs a working mini-boss and boss (NS-M08-007/008), but Boss Framework is M09 — a forward-reference in TASKS.md/MILESTONES.md's own ordering, the same shape as M03's enemy-collision and M04's Homing-target-selection gaps | OPEN — must decide an approach when M08 starts (see PROGRESS.md's M08 "Next Action") |
| NS-9 | LOW | "spawn obstacles" (NS-M07-003) has no implementation — no obstacle entity/concept exists in the engine | OPEN — not needed by Stage 1 (Orbital City, SPEC.md §16) either; deferred to whichever stage first requires one (Space Colony/Asteroid Belt, §18-19) |
| NS-10 | LOW | Checkpoint respawn/continue reset BG_B's scroll position to 0 (Scroll_init() re-zeroes it) rather than resuming at the pixel offset matching the checkpoint's frame | OPEN — cosmetic only (the background is a seamlessly-repeating starfield, so a scroll-position jump isn't visible); revisit if a non-repeating background ever makes the seam visible |

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
| 2026-09-05 | Vulcan cadence/damage numbers (L1=10f/1dmg, L2=10f/2dmg, L3=6f/2dmg) chosen, not specified in SPEC.md | SPEC.md §7 gives relative behavior ("high cadence", "more damage", "max cadence") but no frame counts or damage values |
| 2026-09-05 | Projectile-vs-enemy collision (NS-M03-007) implemented as tested math only (collision_logic.c), not wired end-to-end | No enemy exists before M05; TASKS.md itself schedules this collision requirement in M03 before M05 introduces its target — a spec-ordering gap, not an oversight |
| 2026-09-05 | Added a second debug-only trigger (C raises weaponLevel) alongside M02's B-triggers-damage | SPEC.md gives no way to reach Vulcan L2/L3 before M06's power-ups; needed to actually see the multi-shot pattern fire rather than assume the code is correct from review alone (AGENT.md §18) |
| 2026-09-05 | WeaponType moved from player.h into player_logic.h | It's a plain enum with no hardware dependency — belongs with the other pure types (game_state_logic.h's GameState, player_logic.h's PlayerHitState) so Weapon_switchNext/Weapon_levelUp can be unit-tested |
| 2026-09-05 | B's M02 debug-damage trigger removed outright (not deferred) once M04 gave it a real job | Damage/death are already validated and recorded from M02; M04 doesn't touch that code path, so nothing is lost by retiring the trigger now instead of waiting |
| 2026-09-05 | Per-weapon numbers (Laser 9px/frame+pierce2, Wide's offset tables, Homing's 4-missile cap, Flame's 56px range) chosen, not specified in SPEC.md | Same rationale as M03's Vulcan numbers — SPEC.md gives relative behavior only |
| 2026-09-05 | PAL2 reloaded per active weapon type at spawn time, rather than giving each of the 5 projectile types its own hardware line | Only 1 of 4 palette lines was free (PAL0/1/3 already spoken for) and only one weapon is ever held at once, so a rare few-frame miscoloring of an in-flight volley from the weapon just switched away from is an acceptable, documented tradeoff (see NS-2) |
| 2026-09-05 | Homing flies straight (no seeking) and Laser's pierce count is stored but unused | No enemy list exists before M05 to seek or to pierce through; implemented as an honest stub rather than faked (AGENT.md §18) — see NS-4, NS-5 |
| 2026-09-05 | All 7 enemy sprites + the shared enemy bullet sprite forced onto one byte-identical palette via ImageMagick `-remap` against a common reference, verified with `identify -verbose` before relying on it | Several different enemy types (and their bullets) can be on screen and animating simultaneously, unlike weapons where only one type is ever active — the M04 "reload palette per active type" trick does not work here |
| 2026-09-05 | Retired the M02 debug hitbox marker sprite (visual box only; Player.hitboxX/Y data is untouched) | Freed PAL3, the only way to give all 7 enemy types a real color on a 4-line hardware palette budget |
| 2026-09-05 | Shield's "frontal resistance / vulnerable angle" (SPEC.md §11) implemented as a time-based vulnerable/shielded cycle, not real directional-facing detection | No orientation/facing concept exists in the engine; a timer is the honest, simple approximation available now, documented rather than presented as the real mechanic |
| 2026-09-05 | Enemy HP/score/cooldown numbers not given in SPEC.md (Fighter/Bomber/Turret/Swarm cooldowns, Charger's score, all bullet speeds) chosen here | Same rationale as M03/M04's weapon numbers — SPEC.md gives relative behavior and (mostly) HP/score, not timing |
| 2026-09-05 | Enemy shoot-timer countdown pattern rewritten from `if (--timer==0 \|\| timer>COOLDOWN)` (relies on intentional u16 underflow) to a plain `if (timer>0) timer--; else {fire; timer=COOLDOWN;}` | The underflow version worked but was a fragile, confusing pattern for logic meant to be deterministic and easy to review (AGENT.md §8); fixed before shipping, not after a bug |
| 2026-09-05 | Only 4 of TASKS.md NS-M05-011's 8 bullet patterns (aimed/burst/wave/cross) are fired by a real enemy; diagonal/circular/top_bottom/combined are implemented but unexercised | Each of the 7 enemy types' behavior is specified in SPEC.md §11 and doesn't call for all 8 patterns; the remaining 4 exist as tested building blocks for M07/M08 stage design rather than being forced into a type that doesn't call for them |
| 2026-09-05 | Post-M05 user-requested fixes: removed the leftover "HELLO WORLD!" text from Game_enter(), added Audio_stop() (mutes PSG channel 0) called from GameOver_enter(), added a SCORE readout to the GAME OVER screen | "HELLO WORLD!" was M00-era placeholder text, dead weight once real gameplay existed; the PSG channel latches its last note indefinitely once nothing calls Audio_update() anymore, so GAME_OVER left a stuck tone playing until this was added; GAME OVER previously showed no score at all |
| 2026-09-05 | Power-ups drop from enemy kills (every 3rd, cycling all 8 types) instead of a separate debug spawner | "Enemies drop power-ups" already is the real SPEC.md mechanic; no need for throwaway test scaffolding when the real trigger (a kill) already exists from M05 |
| 2026-09-05 | Bomb damage (10) explicitly bypasses Shield's vulnerable/shielded cycle | SPEC.md §9 frames a bomb as an emergency, high-damage clear-the-screen tool, not a normal attack — a temporary directional shield blocking it would contradict that framing |
| 2026-09-05 | Combo timeout (2s) and power-up S's boost duration (5s) chosen, not specified in SPEC.md | Same rationale as prior milestones' timing choices — SPEC.md gives relative/behavioral requirements, not frame counts |
| 2026-09-05 | Continuing preserves score (not reset to 0) and resets to exactly 1 bomb (not 0, not full) | SPEC.md §26 only specifies weapon->L1 and "keeps one bomb" explicitly; score reset isn't mentioned either way — chose to preserve it since punishing an already-costly continue further isn't asked for anywhere in SPEC.md |
| 2026-09-05 | GameState_computeNext's GAME_OVER->TITLE case (added at M01) removed; GAME_OVER's transition is now fully imperative in game_state.c | It must branch on continuesRemaining (CONTINUE vs. TITLE), state the pure function's simple (state, startPressed) signature can't express without overcomplicating it for one caller — same reasoning already applied to GAME->PLAYER_HIT since M02 |
| 2026-09-05 | SpawnEvent.type is a category byte (SPAWN_TYPE_ENEMY/POWERUP), not the spawnable thing's own enum value | EnemyType and PowerupType are separate enums with overlapping numeric ranges; `variant` holds the actual value, disambiguated by `type` — matches TASKS.md NS-M07-001's literal field names without inventing new ones |
| 2026-09-05 | Checkpoint rewind (die -> resume from last checkpoint) replays content between the checkpoint and the death point, rather than resuming exactly where the player died | The conventional shmup reading of SPEC.md §20; also the only reading that keeps the SpawnEvent cursor meaningful (a checkpoint IS a frame position in the timeline) |
| 2026-09-05 | Background starfield tile shares PAL3 (enemies/power-ups) rather than the player's PAL1 | Attempting to match nx01.png's real palette order via ImageMagick's `-unique-colors` produced a DIFFERENT order than the file's actual on-disk PLTE (confirmed via `identify -verbose` on the same file, two different orderings) — caught before shipping a broken-looking background, not after; PAL3's technique was already proven working three times over (M05 enemies, M05 enemy bullets, M06 power-ups) |
| 2026-09-05 | Stage completion loops back into the same placeholder testStage (no STAGE 2 content) rather than ending the session | Demonstrates NS-M07-007's transition/reset/state-preservation requirements now, without needing Stage 2+ content that belongs to M10+ |
| 2026-09-05 | resetGameplayPools() now calls *_releaseAll() before *_poolInit() for projectiles/enemies/power-ups | Bug fix: the *_poolInit() functions only ever cleared bookkeeping, never released sprites — safe for their original call site (Game_enter, pool provably empty) but not for respawn/next-stage, where other entities can still be active. Caught by the user during M07 testing ("enemies stay painted on screen after a hit") |

---

# PERFORMANCE NOTES

```text
Target FPS: 60
Observed FPS: on-screen counter now exists (VDP_showFPS, top-right, SHOW_DEBUG_HUD); no numeric reading recorded yet
Max enemies: 12 (ENEMY_POOL_SIZE, SPEC.md §24)
Max projectiles: 96 (16 player + 80 enemy, shared pool, SPEC.md §24)
Max powerups: 8 (POWERUP_POOL_SIZE, not specified in SPEC.md)
Max sprites: 117 (1 player + up to 12 enemies + up to 96 projectiles + up
  to 8 powerups) — theoretical pool ceiling, not a realistic simultaneous
  count; debug spawner only ever has ~1 enemy actually firing at a time
  so far
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
