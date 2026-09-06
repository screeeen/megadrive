# NEON STRIKE — DEVELOPMENT PROGRESS

This file is persistent agent state.

The agent MUST update it after every completed milestone.

---

# CURRENT STATE

```text
Current Milestone: M17
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
M08  Stage 1                     [x]
M09  Boss Framework              [x]
M10  Stages 2–3                  [x]
M11  Stages 4–5                  [x]
M12  Stage 6                     [x]
M13  Final Boss                  [x]
M14  HUD / Score / Game Flow     [x]
M15  Audio                       [x]
M16  Effects / Polish            [x]
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
ID: M17
Name: Performance
Status: NOT STARTED
```

## Current Objective

```text
Actually measure what every milestone since M05 has recorded as
"Performance: UNKNOWN" — worst-case simultaneous sprite/projectile/
asteroid/explosion counts, and whether 60 FPS holds. SGDK's VDP_showFPS
already exists (SHOW_DEBUG_HUD-gated) but nobody has read a number off it
under real load yet.
```

## Current Work

```text
Not started. M16 just completed.
```

## Next Action

```text
Read TASKS.md's M17 tasks in full. Identify the worst-case scene this
engine can produce (likely Stage 6 or a boss's VULNERABLE window with a
bomb + full enemy pool + explosions all active at once) and get an actual
FPS reading from BlastEm during it, rather than continuing to record
UNKNOWN. Fix anything that measurably drops below 60 FPS; if nothing
does, that itself is the real, honest result to record.
```

---

# VALIDATION STATUS

```text
Build:       PASS   (make clean && make succeeds, zero warnings)
Tests:       PASS   (make test: 8/8 game_state + 10/10 player_logic + 7/7 collision_logic + 3/3 enemy_logic + 5/5 boss_logic assertions, host-native, no emulator)
Emulator:    PASS   (BlastEm, built from source, boots the ROM cleanly)
Visual:      PASS   (title, NX-01 sprite, full debug HUD, damage blink,
             HIT!/GAME OVER/CONTINUE/STAGE CLEAR/NEXT STAGE flow, all 5
             weapons, all 7 enemy types (including data-driven formations),
             the mini-boss, all 8 power-ups, bomb, combo multiplier, real
             BG_B parallax scroll (starfield + building silhouette
             variant), checkpoint-aware respawn, and the full Orbital
             Guardian boss fight (intro/attack/telegraph/vulnerable/phase
             transitions/death, on-screen HP readout, "VULNERABLE!"
             feedback) all confirmed by user — including several real bugs
             the user caught and this milestone fixed (see M08 summary),
             plus a hard crash on defeating a second boss in one session
             found and fixed at M09 (see NS-12), re-verified crash-free
             across multiple consecutive boss kills
Performance: UNKNOWN (never formally stress-tested; up to 12 enemies + up to
             96 pooled projectiles + up to 8 power-ups + 1 boss now
             possible, heaviest scene so far but not pushed to its limit)
Gameplay:    PARTIAL (player, all 5 weapons, all 7 enemy types + mini-boss,
             combat, power-ups, bomb, combo, a data-driven stage/scroll/
             checkpoint system, and one full boss fight now exist and are
             confirmed working end-to-end for Stage 1; Stages 2-6 and the
             final boss are M10-M13's job)
Audio:       PASS   (PSG melody plays in GAME/BOSS states, keeps playing
             through PAUSE, and is properly silenced on GAME_OVER and
             STAGE_CLEAR — both were left stuck on a note before being
             fixed; correctly resumes after STAGE_CLEAR/NEXT_STAGE, which
             would otherwise have stayed muted forever)
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

## M08

```text
Status: PASS
Date: 2026-09-05
Summary: Replaced M07's generic testStage with real Orbital City content
(SPEC.md §16/§40.2) via a new stage1_data.c: a hand-authored 19,200-frame
(38,400px / 2px-frame, 320s) SpawnEvent table following the documented
0-25/25-50/50-75/75-100% density bands (Drone/Fighter intro -> +Turret ->
Swarm groups/combos -> boss-prep with a mini-boss), plus a new
ENEMY_MINIBOSS type (enemy.h/.c: HP15, score 5000, fires bursts) and a
background variant (scroll.c's backgroundId=1: starfield + a repeating
building-silhouette tile, vs. testStage's pure starfield).

NS-8 (M08 needs a working boss but Boss Framework is M09) was resolved by
building the real, general-purpose Boss Framework now — boss_logic.h/.c
(pure Boss_applyDamage: phase-2 at 2/3 HP, phase-3 at 1/3 HP, unit tested,
5/5 assertions) and boss.h/.c (INTRO/ATTACK/TELEGRAPH/VULNERABLE/
TRANSITION/DEATH state machine per MILESTONES.md M09's NS-M09-002, 3
vulnerable-point sub-rectangles sharing one HP pool per NS-M09-003,
phase-specific attacks: phase 1 frontal cannon, phase 2 spawns
ENEMY_DRONE, phase 3 double diagonal, per SPEC.md's Orbital Guardian
phases) — mirroring how M05 built the full Enemy framework once rather
than incrementally. combat.c gained resolvePlayerBulletsVsBoss (checks
all vulnerable points, respects pierce) and resolveBossContactVsPlayer.
game_state.c gained a real STATE_BOSS case and transitions STATE_GAME's
completion check to spawn the Orbital Guardian (250 HP, SPEC.md §22)
instead of going straight to STAGE_CLEAR.

Four real bugs were found and fixed during this milestone's own testing,
none from a TASKS.md checklist:

1. A self-caught authoring bug in the first draft of stage1_data.c: every
   SpawnEvent.frame value was roughly half of what its section-band
   comment claimed, so the back half of the real 19,200-frame stage would
   have had zero spawns before the boss trigger. Fixed by doubling every
   frame value before the ROM was ever built with this data.

2. User-reported: "los jefes no mueren nunca" (bosses never die). Root
   cause was a legibility gap, not a logic bug — Boss_applyDamage/
   Boss_hit correctly reduce HP and trigger DEATH at 0 (proven by the 5
   passing boss_logic unit tests); the real problem was that
   BOSS_STATE_ATTACK and BOSS_STATE_VULNERABLE rendered as the identical
   solid, fully-visible sprite, so the player had no way to tell "this
   hit will count" from "this hit does nothing" once TELEGRAPH's blink
   ended — violating SPEC.md §36's "mostrar claramente cuando es
   vulnerable". Fixed by drawing a "VULNERABLE!" text cue for exactly
   that window (cleared on leaving it), and by adding a live BOSS
   HP:xxx/xxx text readout (there was no boss HP feedback of any kind
   before this, making a real damage bug and a legibility problem
   indistinguishable from the outside).

3. A related omission also surfaced during that investigation:
   Enemy_poolUpdate() was never called from STATE_BOSS, so phase-2's
   ENEMY_DRONE spawns sat frozen (no movement, no firing) for the rest of
   the fight. Fixed by adding the call, matching STATE_GAME.

4. User-reported: the PSG beep stuck on STAGE_CLEAR, the same class of
   bug M06 already fixed for GAME_OVER (a PSG channel latches its last
   note forever once nothing calls Audio_update() on it). Fixed by adding
   Audio_stop() to StageClear_enter(). Investigating it surfaced a second,
   not-yet-reported half of the same issue: NextStage_resume() never
   called Audio_init(), so after StageClear_enter()'s Audio_stop() silenced
   the envelope, gameplay would have resumed permanently muted for the
   rest of the run. Fixed pre-emptively in the same pass.

Build:       PASS - `make clean && make`, zero warnings
Tests:       PASS - `make test`: game_state/player_logic/collision_logic/
             enemy_logic unchanged, new boss_logic (5/5: partial damage,
             phase-2 threshold crossing, phase-3 threshold crossing,
             overkill never negative, no spurious phase change on a
             same-phase hit)
Emulator:    PASS - boots cleanly in BlastEm; boss encounter validated via
             a temporarily shortened stage1 (lengthFrames dropped to 300
             so the boss trigger fires almost immediately instead of
             after 320s), then rebuilt and retested at the real 19200
             before this milestone was marked done
Visual:      PASS - confirmed by user across several iterations: building
             silhouette background variant renders, mini-boss spawns/
             fires/dies for its documented score, and the full Orbital
             Guardian fight works end-to-end (intro approach, all 3
             phases' distinct attacks, telegraph blink, "VULNERABLE!" +
             HP readout during the real damage window, phase transitions
             at the 2/3 and 1/3 HP thresholds, death, STAGE CLEAR with no
             stuck audio, and NEXT STAGE resuming with music intact) —
             user's final confirmation: "todo ok"
Performance: UNKNOWN - heaviest scene yet (up to 12 enemies + 96
             projectiles + 8 power-ups + 1 boss with 3 vulnerable points),
             not formally stress-tested (M17's job)

Notes: Per PROGRESS.md's prior "Next Action", this milestone deliberately
also completed most of M09's scope (the general Boss Framework) rather
than building a one-off Orbital-Guardian-only boss — M09 is expected to
be a lighter confirmation/generalization pass as a result, see M09's
"Next Action" above. Boss vulnerable-point hitboxes remain small (8x8)
sub-rectangles of the 32x32 sprite with no per-point visual marker (only
the WHEN is signaled, not the WHERE) — SPEC.md §36 only requires the
former explicitly ("mostrar claramente cuando es vulnerable"); revisit if
future playtesting shows the WHERE is still too hard to read.
```

## M09

```text
Status: PASS
Date: 2026-09-05
Summary: Per M08's own "Next Action" plan, M09 was a generalization pass
over the Boss Framework already built (and battle-tested) during M08,
rather than new construction. MILESTONES.md M09's acceptance explicitly
requires "patterns are data/config driven" — not yet true before this
milestone, since boss.c hardcoded the Orbital Guardian's phase attacks
(a switch on boss->phase calling BulletPattern_diagonal/Enemy_spawn
directly) and its 3 vulnerable-point rectangles as struct-literal
constants inside Boss_spawn().

Refactored into a data/BossDef split, mirroring stage.h's StageDef/
SpawnEvent precedent: boss.h now declares `BossAttackFn` (a plain
function-pointer typedef, `void (*)(const Boss*)` — the same
dispatch-table idiom enemy.c's spriteFor() already uses, not a new
scripting/bytecode layer nothing needs yet) and `BossDef` (sprite,
maxHp, score, vulnerablePoints/count, phaseAttacks/count). boss.c is now
fully generic — zero Orbital-Guardian-specific code remains in it. All of
that boss's actual data (sprite, 250 HP, 50,000 score, 3 vulnerable-point
rectangles, and its 3 phase-attack functions: frontal cannon / drone
spawn / double diagonal) now lives in a new src/boss_data.c +
inc/boss_data.h, exactly matching stage_data.c/stage1_data.c's existing
shape. Boss_spawn's signature changed from 4 scalar args to
`Boss_spawn(Boss*, const BossDef*, x, y)`; combat.c's vulnerable-point
checks now read through `boss->def` instead of fields that used to live
directly on Boss.

Also added the death-sequence "explosion" beat MILESTONES.md/TASKS.md
NS-M09-006 asks for (previously the boss sprite just vanished instantly
at the start of its 90-frame DEATH window): a fast strobe blink for most
of DEATH_FRAMES, settling into fully hidden for the final ~20 frames
before the STAGE_CLEAR transition — no dedicated explosion sprite/
particle system exists yet, so this is the honest placeholder available
now (documented, not presented as final VFX).

A second, differently-shaped boss (to empirically prove "reusable") was
deliberately NOT built this milestone — that would be speculative work
with no real stage to justify it yet (M10's Stage 2 boss is the actual,
non-throwaway proof, see this file's M10 "Next Action"); building one
just to demonstrate the point would be exactly the kind of unnecessary
work AGENT.md §8 asks to avoid.

A real, serious bug was found and fixed during this milestone's own
regression testing (TASKS.md NS-M09-007: "Stage 1 boss still works"),
not from a checklist — the user hit a hard crash ("M68K attempted to
execute code at unmapped or I/O address") specifically on defeating a
SECOND boss within one continuous play session (verified via a
temporarily shortened stage1.lengthFrames so the stage loop and a second
encounter could be reached quickly, then a temporarily lowered
orbitalGuardianDef.maxHp for fast iteration — both restored to their
real values, 19200 and 250, before this milestone was marked done).
Diagnosed by rebuilding BlastEm with `-l` (68K code-address logging) and
cross-referencing the last JIT-translated block against symbol.txt right
before the fault: Player_respawn — which pointed at stack corruption
manifesting somewhere unrelated to its actual cause, not a bug in
Player_respawn itself. Root cause: game_state.c's drawDebugHud() (SHOW_
DEBUG_HUD's per-frame HUD, called every frame in both STATE_GAME and
STATE_BOSS) sprintf'd LIVES/weapon/LV/BOMB/combo/SCORE into a 40-byte
stack buffer — already only barely large enough for a 4-digit score with
a 6-letter weapon name, and a genuine overflow once SCORE crosses into
5+ digits. Two boss kills (50,000 points each) is the first time any
play session in this entire project pushed the score past 5-6 digits,
overflowing the stack buffer for the first time and corrupting a saved
return address with raw score-digit bytes — explaining both why it only
ever appeared on a second boss kill and why the crash surfaced inside
unrelated code. Fixed by growing the buffer to 64 bytes (comfortably
covers the true worst case: a u32's 10 digits + the longest weapon name +
the literal text). Audited every other sprintf/char-buffer pair in
game_state.c while at it: GameOver_enter's own SCORE readout had the
same class of bug at a much higher, less realistic threshold (a 16-byte
buffer, 1 byte short of a 10-digit score) — fixed to 20 bytes for margin;
Continue_enter's tiny "%d LEFT" buffer and drawBossHp's "BOSS HP:%3d/
%3d" buffer were both checked and are genuinely safe as sized. Also
hardened Boss_spawn to use SPR_addSpriteSafe (SGDK's own documented
defrag-and-retry wrapper around SPR_addSprite, per its header comments)
instead of the plain call, and added a defensive release-before-spawn
guard — an initial, ultimately-wrong hypothesis for this same crash
before the real cause (the sprintf overflow) was found; kept because it's
real, cheap hardening in its own right (the boss is the single largest
sprite in the game and the only one repeatedly spawned/released across a
whole play session), not because it turned out to be load-bearing here.

Build:       PASS - `make clean && make`, zero warnings
Tests:       PASS - `make test`: unchanged (game_state/player_logic/
             collision_logic/enemy_logic/boss_logic all still passing;
             boss_logic's assertions didn't need updating since
             Boss_applyDamage's pure signature never changed)
Emulator:    PASS - boots cleanly in BlastEm; the crash above was caught
             and fixed during this milestone's own regression pass, then
             re-verified crash-free across multiple consecutive boss
             kills in one continuous session before sign-off
Visual:      PASS - confirmed by user: Stage 1's boss fight is unchanged
             from the player's perspective after the BossDef refactor
             (same intro/phases/attacks/vulnerable window/death), and the
             crash no longer reproduces after the sprintf fix ("parece
             que esta fixeado")
Performance: UNKNOWN - not stress-tested (M17's job); the fixed sprintf
             buffer is 24 bytes larger per call, negligible

Notes: This milestone is a good example of AGENT.md §2's "identify and
document conflicts rather than silently resolving them" applied to a
debugging process, not just a spec conflict: the first fix attempted
(SPR_addSpriteSafe) was plausible, cheap, and genuinely worth keeping,
but empirically did NOT resolve the crash (same exact fault address
reproduced after it) — recorded here rather than quietly dropped, so a
future reader understands why both fixes exist and which one actually
mattered.
```

## M10

```text
Status: PASS
Date: 2026-09-05
Summary: First real proof that M08/M09's stage and boss systems generalize
beyond Stage 1. Built Stage 2 (Red Desert, SPEC.md §17/§40.3) and Stage 3
(Space Colony, SPEC.md §18/§40.4) as real content, each with its own
StageDef/SpawnEvent table (stage2_data.c, stage3_data.c, following
stage1_data.c's exact shape and honesty caveats) and its own BossDef (Sand
Worm, Industrial Core, in boss_data.c alongside the Orbital Guardian).

Length/frame math: Stage 2 is 43,200px / 2px-frame = 21600 frames (360s);
Stage 3 is 48,000px / 2px-frame = 24000 frames (400s). GAME_SPEC.md's own
stated durations for both ("~4:45" and "~5:00") don't match their own
length+scroll numbers, same class of internal inconsistency already
flagged for Stage 1 — length/scroll is treated as the authoritative pair,
duration as an approximate/rounded figure, consistently across all three
stages now. Stage 3's spec'd "2-3 px/frame" scroll range was flattened to
a single 2, since StageDef only has one scrollSpeed field and no mid-stage
ramp mechanism exists (NS-M07-005's already-documented "maximum" gap).

Enemy rosters use only already-existing types (no new enemy type needed):
Stage 2 = Drone/Fighter/Turret/Swarm/Charger (5 types, matching §40.3's
target exactly; Charger is explicitly named as this stage's "new
mechanic"). Stage 3 = Fighter/Turret/Swarm/Charger/Shield/Bomber (6 types,
matching §40.4's target exactly). Both reuse ENEMY_MINIBOSS for their
mini-boss (its HP/score are a single shared constant across all stages —
a known simplification, not scaled per stage, see KNOWN ISSUES).

New BossDefs (boss_data.c), both using the same generic boss.c framework
from M09 with no engine changes needed:
- Sand Worm (350 HP, SPEC.md §40.3): phase1 = BulletPattern_burst
  ("embestida"/charge, approximated as a spray since the boss doesn't
  move), phase2 = BulletPattern_topBottom ("proyectiles verticales",
  direct match), phase3 = BulletPattern_cross ("ataques desde ambos
  lados", approximated as all 4 directions since the boss has no left/
  right positioning to attack "from both sides" of).
- Industrial Core (450 HP, SPEC.md §40.4): phase1 = BulletPattern_aimed,
  phase2 = spawns ENEMY_TURRET as a stand-in for the "brazos mecanicos"
  that are supposed to "modify the safe zone" (no dynamic safe-zone
  concept exists in the engine — a stationary hazard is the honest
  approximation available now, not a fake of the real mechanic), phase3 =
  BulletPattern_combined for peak intensity.
Both bosses' "unique movement" (Sand Worm burrowing in/out of the ground;
Industrial Core's arms reshaping the screen) is NOT implemented — the
framework's boss movement is still only a straight INTRO approach with no
per-boss custom movement system. Documented as a real, deliberate scope
limit rather than presented as done.

Two new background tile variants (scroll.c, matching M08's technique
exactly): backgroundId=2 (dune silhouette strip along the bottom, Red
Desert) and backgroundId=3 (vertical pipe columns spanning full height,
Space Colony). Sandstorm visibility reduction (§17) and Stage 3's
doors/moving walls/presses obstacle set (§40.4) are NOT implemented — no
visibility/fog or scenery-obstacle mechanic exists in the engine (NS-9).

Engine changes needed to actually reach and play this content:
game_state.c's single `currentStage` pointer became a `campaign[]` array
(stage1, stage2, stage3) plus a `stageIndex` walked by NextStage_resume()
(wrapping back to Stage 1 after Stage 3, since Stages 4-6 don't exist yet
— same honest "loop back" choice M07/M08 already made with one stage,
just with one more stop now); `currentStage` is now a macro reading
`campaign[stageIndex]` so every existing call site kept working unchanged.
The boss spawn call changed from a hardcoded `&orbitalGuardianDef` to
`BossDef_forId(currentStage->bossId)` (boss_data.c), a small dispatch so
game_state.c doesn't need a switch of its own for every stage's boss.

Build:       PASS - `make clean && make`, zero warnings
Tests:       PASS - `make test`: unchanged (all 5 suites) — this
             milestone's new files (stage2_data.c, stage3_data.c, and the
             boss_data.c additions) are all data/orchestration with no new
             hardware-free logic worth testing natively
Emulator:    PASS - boots cleanly in BlastEm; validated via temporarily
             shortened stage1/stage2/stage3 lengthFrames (300 each) and
             temporarily lowered boss maxHp (4 each) so the full Stage 1
             -> Stage 2 -> Stage 3 loop could be reached and confirmed in
             one sitting, then rebuilt and re-verified with every real
             value restored (19200/21600/24000 frames, 250/350/450 HP)
             before this milestone was marked done
Visual:      PASS - confirmed by user across two passes: the desert
             background + Sand Worm ("si ha cambiado"), then the full
             3-stage/3-boss loop with distinct backgrounds and boss
             silhouettes for all three ("todo ok")
Performance: UNKNOWN - not stress-tested (M17's job)

Notes: Boss/mini-boss score and HP-effective values came straight from
SPEC.md §23/§40.3/§40.4 (score is a flat 50,000 for every boss, 5,000 for
every mini-boss — not scaled per stage, so no inconsistency there). Sand
Worm and Industrial Core's vulnerable-point rectangles are hand-picked
sub-regions of their own 32x32 placeholder art, same approach and same
NS-11 caveat (no per-point visual marker) as the Orbital Guardian.
```

## M11

```text
Status: PASS
Date: 2026-09-05
Summary: Stage 4 (Asteroid Belt, SPEC.md §19/§40.5) and Stage 5 (Helios
Fleet, SPEC.md §20/§40.6), continuing M10's stageN_data.c + BossDef
pattern. Unlike M10, this milestone needed one genuine new engine
capability: MILESTONES.md M11 explicitly calls for an "asteroid system",
not just more content data.

Added asteroid.h/.c: a fixed 15-slot pool (SPEC.md §40.5's "15 obstaculos
simultaneos" max) of Asteroid entities in 3 sizes (small/medium: 1/2 HP,
destructible, drift at -3/-2 px-frame; large: indestructible — Asteroid_
hit is always a no-op for it — drifts at -1 px-frame as a slow navigation
obstacle). SPEC.md §19 doesn't tie destructible/indestructible to size
explicitly, but doing so needs no extra field and is the natural reading
— documented as a deliberate simplification. A new SPAWN_TYPE_ASTEROID
(stage.h) lets stageN_data.c tables place asteroids exactly like enemies/
power-ups; spawn_manager.c dispatches it to Asteroid_spawn(). combat.c
gained resolvePlayerBulletsVsAsteroids (destroys destructible ones,
awards score, respects pierce; large ones just absorb/stop a non-piercing
shot) and resolveAsteroidContactVsPlayer (always damages the player,
indestructible or not — it's a solid object either way). game_state.c
wires the pool's init/release/update into the same lifecycle as every
other pool (resetGameplayPools, GameOver_enter, STATE_GAME/STATE_BOSS's
per-frame update).

Two new BossDefs (boss_data.c), reusing the M09 framework unchanged:
- Mining Fortress (550 HP, SPEC.md §40.5): phase1 = frontal cannon
  (BulletPattern_diagonal), phase2 = "minas" approximated by spawning a
  small drifting Asteroid (a mine is conceptually a small drifting
  hazard too — a deliberate, reasonable reuse of this milestone's own new
  system rather than a dedicated mine entity), phase3 = spawns 2
  ENEMY_DRONE ("drones"). "Nucleo vulnerable" needed no new work — it's
  just this boss's own vulnerable-point rectangles.
- Admiral X (650 HP, SPEC.md §40.6): SPEC.md gives no per-phase attack
  breakdown for this boss (unlike every earlier one) beyond "nave de
  mando" — phase1 = aimed fire, phase2 = spawns an ENEMY_FIGHTER escort,
  phase3 = BulletPattern_circular for a peak-intensity barrage, chosen to
  escalate like every other boss here.

Stage 5's "8 tipos de enemigo" (SPEC.md §40.6) doesn't map cleanly onto
the engine's 7 combat EnemyType values ("Cruceros"/"Misiles" have no
dedicated entity) — resolved by counting ENEMY_MINIBOSS as the 8th type
and placing 2 separate mini-boss spawns (matching §40.6's own "Mini-
bosses: 2" target exactly), rather than inventing fake new enemy classes
for names SPEC.md never mechanically defines elsewhere. Stage 5's
"formaciones" (diagonal/V/line/circle/cross/wave) are approximated with
same-frame multi-enemy SpawnEvents at staggered y offsets — the same
technique M05/M07 already established for "group movement", not a new
formation-shape system.

Length/frame math, continuing the running pattern from every earlier
stage: Stage 4 = 52,800px / 3px-frame (SPEC.md's own faster "scroll
normal" for this stage) = 17600 frames (293s); Stage 5 = 57,600px /
3px-frame = 19200 frames (320s) — the closest any stage's stated duration
("~5:30"=330s) has come to matching its own numbers yet, still not exact.
Stage 4's "up to 4 px/frame" max scroll isn't modeled (NS-M07-005's
already-documented gap, same as Stage 3's speed range).

Two new background/boss sprite batches generated with the established
ImageMagick + `-remap` + `identify -verbose` verification technique (see
PROGRESS.md's M08 notes for the palette-order gotcha this always checks
for): Mining Fortress, Admiral X (32x32 boss sprites), and 3 asteroid
sprites (8x8/16x16/24x24). Hit a NEW ImageMagick gotcha this time: a
pure black+grey80 image (both channels equal, i.e. achromatic) gets
auto-detected and written as a 4-bit *grayscale* PNG (a smooth 16-level
ramp) instead of a 2-color *indexed* palette PNG unless `-type Palette`
(or `PNG8:` output) is forced — caught via `identify -verbose` showing
graya(0)/graya(17)/graya(34).../ramp instead of the expected 2-entry
Colormap, before it ever reached a build. Every 2-color (black+grey)
asset generated from here on uses `-type Palette PNG8:`.

Stage 4/5 backgrounds intentionally stayed plain starfield (backgroundId
4/5 fall through scroll.c's if/else chain unchanged) rather than adding
new tile art — asteroids are the real, gameplay-relevant visual content
for Stage 4, and Stage 5 ("gran batalla espacial") doesn't call for a
distinct backdrop the way desert/colony did.

A user-reported issue ("no veo las rocas" — asteroids not visible) was
investigated but NOT conclusively root-caused: the asteroid sprite art
was verified correct by direct visual inspection (Read tool on the PNG
files), and spawn/collision/pool-lifecycle code was reviewed line-by-line
with no bug found. Per the user's own follow-up request, added a
permanent-feeling SHOW_DEBUG_HUD readout ("STAGE X/Y: NAME AST:N") so
stage progression and live asteroid count are both directly observable
during play — this was necessary regardless of the asteroid report, since
there was previously no way to confirm which stage was even active. The
user confirmed things looked right after this addition ("bien
continuamos") without pinpointing whether the original report was a real
bug that self-resolved, a testing/attention issue, or something the new
HUD clarified — see KNOWN ISSUES NS-16 for this open thread.

Build:       PASS - `make clean && make`, zero warnings
Tests:       PASS - `make test`: unchanged (all 5 suites) — asteroid.c is
             pool/sprite orchestration like enemy.c/powerup.c, no new
             hardware-free logic worth testing natively
Emulator:    PASS - boots cleanly in BlastEm; validated via temporarily
             shortened stage1-5 lengthFrames and temporarily lowered boss
             maxHp (all 5 bosses to 4 HP) so the full 5-stage/5-boss loop
             could be reached and confirmed in one sitting, then rebuilt
             and re-verified with every real value restored (19200/21600/
             24000/17600/19200 frames, 250/350/450/550/650 HP) before this
             milestone was marked done
Visual:      PASS - confirmed by user ("bien continuamos") after the
             STAGE/AST HUD readout was added; see NS-16 for the still-open
             question about the original asteroid-visibility report
Performance: UNKNOWN - not stress-tested (M17's job); asteroids add up to
             15 more simultaneous sprites on top of the existing ceiling

Notes: Asteroid score values (small=50, medium=100) are chosen, not given
by SPEC.md — same rationale as every other placeholder-era number in this
project (SPEC.md gives behavior/counts, not point values, for anything
outside the explicit §23 scoring table).
```

## M12

```text
Status: PASS
Date: 2026-09-05
Summary: Stage 6 (Helios Core, SPEC.md §21/§40.7), the campaign's climax
stage. Continued M10/M11's stageN_data.c pattern; the one real decision
this milestone needed was how to handle MILESTONES.md M12's explicit
"final boss entrance" requirement, given M13 (Final Boss) is Helios's own
dedicated milestone with substantially different acceptance criteria
(death sequence, ending trigger, credits — none of which M12 needs).

Resolved by repeating the M08/M09 relationship: built a real, working
Helios encounter now (boss_data.c's heliosDef, bossId=6) using the
existing generic boss framework unchanged — phase1 = cannons (diagonal)
+ an aimed "missile" (approximated; no true homing exists, NS-4) + a
spawned drone, phase2 = circular pattern ("patrones circulares") +
BulletPattern_wave ("barridos"), phase3 = combined+cross attacks with
the framework's existing vulnerable-point system standing in for "nucleo
expuesto". SPEC.md §40.8 deliberately avoids giving Helios an explicit HP
number ("no utilizar unicamente una barra de HP") — continued the
established +100-per-stage progression (250/350/450/550/650) to 750 as
the honest, documented choice. Until M13, Helios's death behaves exactly
like every other boss (STAGE_CLEAR -> NEXT_STAGE, looping back to Stage
1) rather than the real ending — explicitly NOT faked, since the actual
death sequence/ending transition are M13's own acceptance items.

Stage 6's "9 tipos de enemigo" (SPEC.md §40.7) reached the same way
Stage 5 did (7 combat types + ENEMY_MINIBOSS = 8, see M11's summary),
plus M11's own Asteroid system reused here as SPEC.md's "obstaculos"
requirement, counted as a 9th distinct threat type — a stretch reading
flagged honestly rather than inventing a "Laser hazard" entity for one
spec line item. "Fast enemies" leans on Charger (already the roster's
fastest type) and tight Swarm/Fighter/Turret combinations rather than
inflating any HP value, matching SPEC.md's own explicit direction that
Stage 6's difficulty "debe provenir de patrones/posicionamiento/
velocidad/combinaciones, no de enemigos con cantidades absurdas de HP".
"Lasers" (both the stage element and Helios's phase 2 attack) have no
dedicated environmental-hazard entity — approximated by the existing
Laser weapon power-up being available, not a new stage hazard (same root
gap as NS-9/NS-15).

New asset: Helios's boss sprite (32x32) is the first in this project to
use a 4th color — SPEC.md §21's stated palette is "Negro, Rojo, Blanco,
Colores de energia", so white was added at a NEW index (3) while keeping
indices 0-2 (black/grey/red) byte-identical to every other PAL3-sharing
sprite, so anything spawned during the fight that relies on the
already-loaded palette (e.g. phase1's drone) still renders correctly.
Hit the same "-remap drops colors not in the reference" pitfall in
reverse this time: remapping the 4-color raw image against a 3-color
reference (bg_boss_guardian.png) silently dropped white entirely —
caught via `identify -verbose` before it shipped; fixed by skipping the
remap step for this one sprite and using its own correctly-ordered raw
output directly, since remapping was never necessary here (the goal was
extending the shared palette, not matching an existing one byte-for-byte).

Length/frame math, continuing the running pattern: Stage 6 = 48,000px /
3px-frame (mid-point of SPEC.md's own "2-4 px/frame" range for this
stage) = 16000 frames (267s); GAME_SPEC.md's stated "~5:00" (300s)
doesn't match either, same class of mismatch flagged for every earlier
stage.

Build:       PASS - `make clean && make`, zero warnings
Tests:       PASS - `make test`: unchanged (all 5 suites)
Emulator:    PASS - boots cleanly in BlastEm; validated via temporarily
             shortened stage1-6 lengthFrames and temporarily lowered boss
             maxHp (all 6 bosses to 4 HP) so the full 6-stage/6-boss loop
             could be reached and confirmed in one sitting, then rebuilt
             and re-verified with every real value restored (19200/21600/
             24000/17600/19200/16000 frames, 250/350/450/550/650/750 HP)
             before this milestone was marked done
Visual:      PASS - confirmed by user reaching Stage 6 and the Helios
             encounter as part of the full 6-stage loop test
Performance: UNKNOWN - not stress-tested (M17's job)

Notes: This is the last stage milestone before M13 (Final Boss) and M14+
(HUD/audio/effects/performance/full-campaign validation) — the campaign's
content is now structurally complete end-to-end (6 stages, 6 working
bosses, all sharing one generic, data-driven framework), even though
several per-spec numeric targets remain under-hit across every stage
(kill counts, exact power-up counts, projectile-per-stage limits) and are
tracked as open, honestly-reported gaps rather than claimed as met.
```

## M13

```text
Status: PASS
Date: 2026-09-05
Summary: Gave Helios (built as a working boss at M12) the actual FINAL
BOSS-specific behavior SPEC.md §22 and MILESTONES.md M13 ask for: a real
death sequence and ending, distinct from every other boss's generic
STAGE_CLEAR -> NEXT_STAGE loop.

game_state.c's Boss_isEncounterOver branch now checks `currentStage->
bossId == FINAL_BOSS_ID` (6) and, only for Helios, skips STAGE_CLEAR
entirely in favor of a new path: `Ending_enter()` (releases every
remaining gameplay sprite the same way GameOver_enter() already does,
stops audio, shows "HELIOS DESTROYED"/"ORBIT SECURED" for ENDING_FRAMES=
180 frames, ~3s) -> `Credits_enter()` (shows "NEON STRIKE"/"THE END"/
"FINAL SCORE:<score>"/"PRESS START") -> START returns to STATE_TITLE.
STATE_ENDING and STATE_CREDITS existed as GameState enum values since M01
but had zero real behavior before this (fell through the switch's default
case, doing nothing) — this milestone is what finally gives them one.
"Scroll detenido" (SPEC.md §22) falls out for free: neither new state
ever calls Scroll_update(). No explosion/particle system exists (M16's
job, same gap as every other entity's death in this engine) — the boss's
own existing DEATH-state strobe (boss.c, unchanged since M09) is the
visual beat under the "HELIOS DESTROYED" text.

"Ranking" (SPEC.md §22) is shown as the run's own final score, not a real
persistent leaderboard — no save mechanism (SRAM/EEPROM or otherwise)
exists anywhere in this project, so a genuine ranking/high-score table is
explicitly out of scope until one does; documented rather than faked as
equivalent.

No changes were needed to boss.c/boss_data.c's actual fight mechanics —
M12's heliosDef already had 3 phases with the framework's existing
telegraph (blink before VULNERABLE) and vulnerable windows, satisfying
that half of M13's acceptance list without new work.

Build:       PASS - `make clean && make`, zero warnings
Tests:       PASS - `make test`: unchanged (all 5 suites) — the new
             Ending_enter()/Credits_enter() logic is VDP/state
             orchestration with no hardware-free logic worth testing
             natively, and GameState_computeNext's pure transition table
             didn't need changes (STATE_ENDING/STATE_CREDITS's real
             transitions are imperative in game_state.c, same established
             pattern as STATE_STAGE_CLEAR/STATE_GAME_OVER etc.)
Emulator:    PASS - boots cleanly in BlastEm; validated via temporarily
             shortened stage1-6 lengthFrames and temporarily lowered boss
             maxHp (all 6 bosses to 4 HP) to reach and defeat Helios
             quickly, then rebuilt and re-verified with every real value
             restored before this milestone was marked done
Visual:      PASS - confirmed by user: "HELIOS DESTROYED" -> "NEON
             STRIKE"/"THE END"/"FINAL SCORE" -> START returns to TITLE,
             all as designed
Performance: UNKNOWN - not stress-tested (M17's job)

Notes: The campaign is now structurally complete end-to-end for the first
time: TITLE -> 6 real stages with 6 working bosses -> a real Final Boss
with its own death/ending sequence -> CREDITS -> back to TITLE. What
remains (M14-M19) is HUD/audio/effects polish, performance validation,
and a full-campaign pass against every numeric target this project has
been honestly under-hitting since M08 (kill counts, power-up counts,
projectile-per-stage limits) — none of that blocks calling the game
"playable start to finish" as of this milestone.
```

## M14

```text
Status: PASS
Date: 2026-09-05/06
Summary: Built the real, always-on HUD MILESTONES.md M14 asks for —
everything score/lives/weapon/bomb/combo-related had only ever existed
behind SHOW_DEBUG_HUD (a development flag meant to be off for a real
build, AGENT.md §15) or in transient screens (GAME_OVER's score).

Added score.c's Score_getHigh(): a session high score, updated inline in
Score_add() whenever the running score exceeds it, deliberately never
reset by Score_reset() (a new game shouldn't erase a high score set on an
earlier attempt this session) — only power-cycling the console resets it,
since no save mechanism exists (NS-17).

New game_state.c HUD, split across dedicated rows to stay clear of the
boss HP/VULNERABLE!/HIT!/PAUSE text that can appear mid-screen:
- Row 0: SCORE (left, zero-padded to 8 digits per SPEC.md's own "SCORE
  00124500 HI 9999999" example) and HI (right corner, user-requested
  layout — originally both sat together on the left).
- Row 1 (STATE_GAME only): a new "STAGE:XX%" progress readout
  (SpawnManager gained a getCurrentFrame() getter for this, user-
  requested) computed against the stage's own lengthFrames. Reuses the
  same row/column drawBossHp() already uses during STATE_BOSS — that
  function's wider VDP_clearText already covers this text's shorter
  width, so no stale digits linger when a boss fight starts.
  SHOW_DEBUG_HUD's FPS counter moved from row 0 to row 1 (right side,
  user-requested "FPS below HI score") to stay out of the new HI score's
  way.
- Row 26: LIVES, weapon name immediately followed by a '#'-per-level
  "POWER" bar (no bar-drawing tile exists, and a labeled "POWER:###" was
  found to overflow the 40-column plane in the worst case — fixed by
  dropping the separate label), BOMB count, combo multiplier.

Also closed NS-M14-007 ("stage clear: score bonus") — checked GAME_SPEC.md
for an exact bonus value (grepped for "bonus", nothing) and added a flat,
chosen STAGE_CLEAR_BONUS (1000 points), awarded and displayed
("BONUS:1000") in StageClear_enter(). Every other M14 checklist item
(lives/continues, checkpoint restoration, game over flow, ending/
victory/credits) was already real, working behavior from M02/M06/M07/
M13 — this milestone didn't need to touch any of it.

A real bug was caught and fixed during this milestone's own implementation
(not a checklist item): the first version of the bottom HUD line used
`VDP_clearText(1, 26, 46)` and a "LIVES:9 WEAPON:VULCAN POWER:### BOMB:9
x5"-shaped format string, both of which overflow the 40-column text
plane's 39 usable columns from x=1 — caught before any build was tested,
fixed by shortening the clear width to 39 and dropping the separate
"WEAPON:"/"POWER:" labels (the weapon name is immediately followed by its
own '#'s instead).

Build:       PASS - `make clean && make`, zero warnings
Tests:       PASS - `make test`: unchanged (all 5 suites) — drawHud()/
             drawStageProgress() are VDP text orchestration with no
             hardware-free logic worth testing natively
Emulator:    PASS - boots cleanly in BlastEm
Visual:      PASS - confirmed by user across two passes: the HUD layout
             itself ("esta ok" before the HI-score/FPS/stage-progress
             follow-up request, then confirmed again after), and the
             STAGE CLEAR bonus display ("si ok")
Performance: UNKNOWN - not stress-tested (M17's job); drawHud()/
             drawStageProgress() both redraw-guard on value change (same
             pattern as drawBossHp() from M08) to avoid redundant
             per-frame VDP text writes

Notes: The debug HUD (SHOW_DEBUG_HUD-gated) now only shows what the real
HUD doesn't: FPS and the STAGE X/Y: NAME AST:N readout from M11. Nothing
score/lives/weapon-related remains debug-only.
```

## M15

```text
Status: PASS
Date: 2026-09-06
Summary: Replaced the M00-era placeholder (one looping PSG arpeggio,
identical for every state/stage since the very first milestone) with a
real, data-driven multi-track music system and a set of real SFX — the
last major "obviously placeholder" system before M16-M19's polish/
validation passes.

Every "theme" is a genuine, distinct, hand-picked PSG note-frequency
sequence looped on channel 0 (audio.c's new MusicTrack table) — NOT
actual composed/orchestrated FM scores. SGDK's XGM driver plus real
tracker-authored music files would be needed for that, and none exist in
this project; every track here is exactly as "real" as the original M00
arpeggio was, just 9 more of them, each musically distinct (major/minor/
scale/tempo varied per stage's mood) rather than one tune reused
everywhere. StageDef.musicId (a field that has existed, unused, since
M07) now finally does something: stage1_data.c..stage6_data.c each carry
their own value (1-6), and game_state.c calls the new Audio_playMusic(id)
at every point music should change (Game_enter/Continue_resume/
NextStage_resume use currentStage->musicId; entering STATE_BOSS switches
to MUSIC_BOSS, or MUSIC_FINAL_BOSS for Helios specifically via the same
FINAL_BOSS_ID check M13 introduced; Credits_enter() plays MUSIC_CREDITS).

TASKS.md NS-M15-008's "six boss themes" was deliberately simplified to
one shared, more intense MUSIC_BOSS track reused by all 6 regular stage
bosses, plus a separate, grander MUSIC_FINAL_BOSS for Helios — 9 tracks
total (6 stage + boss + final boss + credits) rather than 13, a
documented scope choice given hand-authored note-array composition
doesn't scale well to a dozen genuinely distinct dramatic pieces for
diminishing returns.

Added a real SFX layer on PSG channel 1 (tones: SFX_SHOT/HIT/POWERUP/
BOMB/1UP/STAGE_CLEAR/BOSS) and channel 3 (noise: SFX_EXPLOSION), wired
into the actual gameplay events that should trigger them: Weapon_update
(every shot fired), combat.c's onEnemyKilled (every enemy/asteroid kill —
this one function already covered both since M11), applyPowerup (generic
pickup jingle, with POWERUP_1UP getting its own distinct fanfare),
Bomb_use, Player_hit (player takes damage — TASKS.md's separately-named
"hit" and "player damage" items are treated as the same event), and the
boss-defeat branch in game_state.c (a new SFX_BOSS, distinct from a
regular SFX_EXPLOSION). Channel 1/3 SFX are deliberately last-wins/non-
queued (triggering a new one cuts off whatever was still playing) — a
documented simplification for a single spare tone channel, not a bug;
in practice this means the SFX_BOSS triggered on a boss defeat almost
always gets cut short almost immediately by the SFX_STAGE_CLEAR (or
Audio_stop(), for the final boss) that follows right after it.

A real bug was caught and fixed during this milestone's own
implementation (not a checklist item): STATE_STAGE_CLEAR's case never
called Audio_update(), which was fine when Audio_stop() was the only
thing that mattered there (M06/M09's earlier fixes), but the new
SFX_STAGE_CLEAR fanfare needs Audio_update() ticking to actually advance
through its notes and self-silence — without it, the fanfare would freeze
on its first note for the whole STAGE_CLEAR+NEXT_STAGE beat (~2.5s), the
exact same "PSG channels latch" bug class as before, just newly
introduced on the SFX channel by this milestone's own change. Fixed by
adding Audio_update() to that case — safe because Audio_stop() already
silenced channel 0's envelope, so recomputing its frequency there is
inaudible. The same fix was needed and applied to STATE_CREDITS (whose
new MUSIC_CREDITS would otherwise never advance past its first note
either).

Build:       PASS - `make clean && make`, zero warnings
Tests:       PASS - `make test`: unchanged (all 5 suites) — audio.c is
             PSG-register orchestration with no hardware-free logic worth
             testing natively
Emulator:    PASS - boots cleanly in BlastEm; validated via a temporarily
             shortened stage1 and lowered Orbital Guardian HP to hear
             shot/explosion/powerup/bomb/hit SFX, the boss music switch,
             and the stage-clear fanfare + music resuming in one sitting,
             then rebuilt and re-verified with real values restored
Visual:      N/A (audio-only milestone)
Performance: UNKNOWN - not stress-tested (M17's job); PSG register writes
             are cheap and only happen on note-boundary frames (per-track
             frameDiv) or on discrete SFX trigger events, not every frame
             unconditionally

Notes: Title screen remains silent (SPEC.md's audio requirements list
covers stage/boss/final-boss/ending/credits explicitly, not title) —
unchanged pre-existing behavior, not a new gap introduced here.
```

## M16

```text
Status: PASS
Date: 2026-09-06
Summary: Added the visual feedback layer that's been a documented gap
since M05/M08 — no explosion/particle system existed anywhere; every
enemy, asteroid, and boss simply vanished on death. M15 already gave
these moments real sound (SFX_EXPLOSION/SFX_BOSS); this milestone is the
matching visual, plus a few smaller readability passes.

New explosion.h/.c: an 8-slot pool of transient Explosion entities (same
pool-lifecycle shape as every other entity module here), each a single
16x16 sprite (explosion.png, red burst + grey core, generated on the
same baseline black/grey/red palette every other PAL3-sharing asset
uses) shown for 16 frames with a fast visibility-toggle "flash" rather
than true multi-frame particle animation — no extra animation frames
were drawn, matching the same level of polish as every other
placeholder-era asset in this project, not a full particle system.
Wired into: combat.c's onEnemyKilled (every enemy/asteroid kill — one
function already covered both since M11), Bomb_use (one explosion per
enemy the bomb kills), PlayerHit_enter (a life-costing hit only, not
every hit), and a 3-burst spread across the boss's own footprint on
defeat (game_state.c). boss.c's DEATH state also now spawns a scattered
explosion every 15 frames throughout its strobe, upgrading what M09
could only describe as "no dedicated explosion sprite exists yet" now
that one does.

Added a per-enemy "hit flash": Enemy struct gained a hitFlashTimer field,
set by Enemy_hit() on a survived (non-lethal) hit, ticked down and
blinked in Enemy_poolUpdate() — previously an enemy that took damage but
didn't die had zero visual feedback that anything happened.

Added a phase-transition blink to BOSS_STATE_TRANSITION (boss.c) — it
previously rendered identically to a solid ATTACK/VULNERABLE boss, the
same "no visual distinction" gap M08 already found and fixed once for
ATTACK vs. VULNERABLE (the "VULNERABLE!" text), just never extended to
TRANSITION. A real bug was caught and fixed while adding this, before it
shipped: the new blink can leave the sprite HIDDEN on whatever frame
TRANSITION's timer happens to hit 0, and enterAttack() never explicitly
restored visibility (it never needed to before, since only TELEGRAPH
blinked and enterVulnerable() already handled restoring it) — fixed by
having enterAttack() itself force VISIBLE.

"Background polish" (NS-M16-006: parallax/traffic/machinery/atmosphere)
and most of "Obstacles" beyond fast-enemies/dense-patterns were NOT
touched — these are the same NS-9/NS-15 gaps already tracked (no
scenery-obstacle or multi-layer-parallax system exists), not new scope
for this milestone to invent. "Art consistency" (NS-M16-007) was audited
rather than built: every sprite/tile in this project is a flat-fill
indexed-PNG silhouette on a shared 3-4 color palette (no gradients, no
photographic/3D assets), and the HUD is plain monospace VDP text (no
modern UI chrome) — already true by construction from how every asset in
this project has been generated since M02, confirmed rather than
re-verified line by line.

Build:       PASS - `make clean && make`, zero warnings
Tests:       PASS - `make test`: unchanged (all 5 suites) — explosion.c
             and the hit-flash/transition-blink additions are all
             sprite/timer orchestration with no hardware-free logic worth
             testing natively
Emulator:    PASS - boots cleanly in BlastEm; validated via a temporarily
             shortened stage1 and lowered Orbital Guardian HP to trigger
             enemy/asteroid/bomb/player/boss explosions and the hit-flash
             in one sitting, then rebuilt and re-verified with real
             values restored
Visual:      PASS - confirmed by user ("Bien, continua")
Performance: UNKNOWN - not stress-tested (M17's job); the explosion pool
             adds up to 8 more simultaneous sprites on top of the
             existing ceiling

Notes: Explosion score/text feedback (e.g. a floating "+100") was not
added — not requested by MILESTONES.md's M16 list, and would need its
own small transient-text system; noted here in case a future polish pass
wants it, not tracked as an open issue since nothing currently claims it
should exist.
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
| NS-8 | — | M08 (Stage 1) needs a working mini-boss and boss (NS-M08-007/008), but Boss Framework is M09 — a forward-reference in TASKS.md/MILESTONES.md's own ordering, the same shape as M03's enemy-collision and M04's Homing-target-selection gaps | RESOLVED at M08 — built the real, general-purpose Boss Framework (boss.h/.c, boss_logic.h/.c) as part of M08 rather than a one-off boss, per the plan recorded in this file's prior M08 "Next Action". M09 is now a lighter confirmation/generalization pass |
| NS-9 | LOW | "spawn obstacles" (NS-M07-003) has no implementation — no obstacle entity/concept exists in the engine | OPEN — not needed by Stage 1 (Orbital City, SPEC.md §16) either; deferred to whichever stage first requires one (Space Colony/Asteroid Belt, §18-19) |
| NS-10 | LOW | Checkpoint respawn/continue reset BG_B's scroll position to 0 (Scroll_init() re-zeroes it) rather than resuming at the pixel offset matching the checkpoint's frame | OPEN — cosmetic only (the background is a seamlessly-repeating starfield, so a scroll-position jump isn't visible); revisit if a non-repeating background ever makes the seam visible |
| NS-11 | LOW | Boss vulnerable points (8x8 sub-rectangles of the 32x32 sprite) have no per-point visual marker — only WHEN the boss is vulnerable is signaled ("VULNERABLE!" text + solid sprite), not WHERE on its body to aim | OPEN — SPEC.md §36 only mandates the former explicitly; revisit if playtesting shows the WHERE is still too hard to read even with the WHEN fixed (see M08 summary, DECISIONS) |
| NS-12 | HIGH | drawDebugHud()'s sprintf buffer (game_state.c, SHOW_DEBUG_HUD's per-frame LIVES/weapon/SCORE readout) overflowed its 40-byte stack buffer once SCORE reached 5+ digits, corrupting the stack and crashing ("M68K attempted to execute code at unmapped address") — first reachable after two 50,000-point boss kills in one session | RESOLVED at M09 — buffer grown to 64 bytes; GameOver_enter's SCORE buffer had the same class of bug at a much higher threshold, grown from 16 to 20 bytes in the same pass |
| NS-13 | LOW | Sand Worm's "burrowing in/out of terrain" (SPEC.md §17) and Industrial Core's "arms that modify the screen's safe zone" (SPEC.md §18) are not implemented — boss.c's movement is still only a straight INTRO approach with no per-boss custom movement/safe-zone system | OPEN — approximated with attack-pattern variety instead (see M10 summary); revisit if a later milestone specifically needs per-boss movement |
| NS-14 | LOW | ENEMY_MINIBOSS's HP/score (enemy.c: 15 HP, 5000 score) are single shared constants used identically by every stage's mini-boss — difficulty doesn't scale by stage the way SPEC.md's per-stage "Boss HP efectivo" table scales real bosses | OPEN — same simplification class as boss timing constants not yet being per-boss (see M09 DECISIONS); revisit once enough stages exist to make the flatness clearly wrong |
| NS-15 | LOW | Sandstorm visibility reduction (SPEC.md §17) and Stage 3's door/moving-wall/press obstacle set (SPEC.md §40.4) are not implemented — no visibility/fog or scenery-obstacle mechanic exists in the engine (same root gap as NS-9's "spawn obstacles") | RESOLVED for the underlying "no obstacle/hazard-terrain system" gap at M11 (asteroid.c) — the sandstorm visibility/fog effect specifically is still not implemented (no such mechanic was built), but that's a distinct, still-open sub-item; re-opened as its own line if a later milestone needs it |
| NS-16 | LOW | User reported asteroids not visible in Stage 4 ("no veo las rocas"); sprite art and spawn/collision code were both reviewed and found correct, but the report was never conclusively reproduced or root-caused — user's later confirmation ("bien continuamos") came after an unrelated HUD addition (STAGE/AST readout), not a code fix | OPEN — no fix was applied because no bug was found; revisit if asteroid visibility is reported as an issue again, using the new STAGE X/Y: NAME AST:N debug readout to first confirm whether asteroids are spawning (AST count) or spawning-but-not-rendering |
| NS-17 | LOW | No persistent high-score/ranking storage exists (SPEC.md §22's "ranking") — no save mechanism (SRAM/EEPROM or otherwise) exists anywhere in this project; M13's credits screen shows only the current run's final score | OPEN — deferred until a milestone explicitly needs persistence; M14's "high score" (MILESTONES.md) will hit the exact same gap |

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
| 2026-09-05 | Built the general Boss Framework (boss.h/.c, boss_logic.h/.c) as part of M08 rather than a one-off Orbital-Guardian-only boss | M08 needs a working boss but Boss Framework is nominally M09 (NS-8) — resolved by building the reusable system now, mirroring how M05 built the full Enemy framework once instead of incrementally; M09 becomes a lighter confirmation pass |
| 2026-09-05 | Boss vulnerable points all share the boss's single HP pool rather than tracking per-point HP | SPEC.md §36 asks for 3-5 vulnerable points but doesn't specify independent HP per point; a shared pool is the simpler, still-spec-compliant reading, and keeps Boss_applyDamage's signature identical to Enemy_applyDamage's pattern |
| 2026-09-05 | stage1_data.c's first-draft SpawnEvent frame values were all roughly half their section-band comment's claim (self-caught before any build/test) | A hand-typed table this size is exactly the kind of place a systematic authoring slip survives a casual read — caught by cross-checking every event's frame against its stated 4800-frame band before trusting the data, per AGENT.md §18 |
| 2026-09-05 | Boss VULNERABLE state now draws "VULNERABLE!" text (cleared outside that window) and game_state.c now shows a live "BOSS HP:xxx/xxx" readout | User report ("los jefes no mueren nunca") root-caused to a legibility gap, not a damage bug: ATTACK and VULNERABLE rendered identically (SPEC.md §36 requires showing clearly WHEN a boss is vulnerable), and there was no boss HP feedback at all, making a real bug and a UX gap impossible to tell apart from outside the code — see NS-11 for the still-open WHERE-to-aim half of this |
| 2026-09-05 | Added the missing Enemy_poolUpdate() call to STATE_BOSS | Found while investigating the "bosses never die" report: phase 2's ENEMY_DRONE spawns were frozen (no movement/firing) because this call, present in STATE_GAME, was never added to STATE_BOSS |
| 2026-09-05 | Added Audio_stop() to StageClear_enter() and Audio_init() to NextStage_resume() | User-reported stuck PSG beep on STAGE_CLEAR, same root cause M06 already fixed for GAME_OVER (a PSG channel latches its last note once nothing calls Audio_update() on it); fixing it surfaced the mirror-image bug (nothing re-enabled the envelope after STAGE_CLEAR silenced it, so NEXT_STAGE gameplay would have resumed permanently muted) — fixed in the same pass rather than waiting for a second report |
| 2026-09-05 | Boss attack patterns and vulnerable points moved into a data/BossDef (boss_data.c), boss.c/.h left fully generic | MILESTONES.md M09 explicitly requires "patterns are data/config driven"; mirrors StageDef/SpawnEvent's already-proven data-driven shape rather than inventing a new mechanism |
| 2026-09-05 | Did not build a second, throwaway boss during M09 just to "prove" reusability | No real stage needs one yet; M10's Stage 2 boss is the real, non-speculative proof — building one now would be exactly the unnecessary work AGENT.md §8 warns against |
| 2026-09-05 | Boss DEATH state now strobes (fast blink) for most of its 90-frame window instead of vanishing instantly | MILESTONES.md/TASKS.md NS-M09-006 asks for an "explosions" death beat; no dedicated explosion sprite/particle system exists yet, so a strobe of the boss's own sprite is the honest placeholder available now |
| 2026-09-05 | grow drawDebugHud's sprintf buffer from 40 to 64 bytes (and GameOver_enter's SCORE buffer from 16 to 20) | Root cause of a real crash ("M68K attempted to execute code at unmapped address") found during M09's own regression testing: the 40-byte buffer overflowed once SCORE crossed into 5+ digits (first reachable after two 50,000-point boss kills in one session), corrupting the stack and crashing much later in unrelated code. Every other sprintf buffer in game_state.c was audited in the same pass and confirmed safe as sized |
| 2026-09-05 | Boss_spawn uses SPR_addSpriteSafe (SGDK's defrag-and-retry wrapper) instead of plain SPR_addSprite, plus a release-before-spawn guard | Real, cheap hardening (the boss is the single largest, most repeatedly-churned sprite in the game) kept even though it was an initial wrong hypothesis for the sprintf-overflow crash above — recorded per AGENT.md §2 rather than silently dropped once the real cause was found |
| 2026-09-05 | game_state.c's single `currentStage` pointer replaced with a `campaign[]` array + `stageIndex`, looping back to Stage 1 after Stage 3 | Stages 4-6 (M11/M12) don't exist yet — same honest "loop back" choice M07/M08 already made with a single stage, extended to three; `currentStage` stayed a macro over the array so every existing read site needed no changes |
| 2026-09-05 | Boss selection changed from a hardcoded `&orbitalGuardianDef` to `BossDef_forId(currentStage->bossId)` | Three stages now each need a different boss; a small id-to-BossDef dispatch in boss_data.c keeps game_state.c from needing its own per-stage switch |
| 2026-09-05 | Sand Worm's burrowing and Industrial Core's safe-zone-modifying arms (SPEC.md §17/§18) approximated with attack-pattern variety instead of new per-boss movement/safe-zone systems | boss.c's movement is still only a straight INTRO approach; building real per-boss movement for two bosses as a side effect of a content milestone would be scope creep — tracked as NS-13 instead of faked |
| 2026-09-05 | Stage 3's "2-3 px/frame" scroll range (SPEC.md §40.4) flattened to a single 2 | StageDef has one scrollSpeed field and no mid-stage ramp mechanism (NS-M07-005); same treatment as every other spec value that assumes engine capability not yet built |
| 2026-09-05 | Stage 2/3 both reuse ENEMY_MINIBOSS unchanged rather than stage-specific mini-boss HP/score | No per-stage scaling mechanism exists for mini-bosses (single shared constants in enemy.c); tracked as NS-14 rather than silently treated as correct |
| 2026-09-05 | Asteroid destructible/indestructible tied to size (small/medium destructible, large indestructible) rather than an independent flag | SPEC.md §19 doesn't tie the two explicitly, but this is the natural, common reading and needs no extra struct field; a large asteroid being a real navigation obstacle (§40.5) fits "always indestructible" cleanly |
| 2026-09-05 | Mining Fortress's "minas" attack reuses M11's new Asteroid_spawn (a small drifting asteroid) rather than a dedicated mine entity | A mine is conceptually a small drifting hazard too — reusing the just-built system is more honest than either faking a distinct mine mechanic or skipping the attack entirely |
| 2026-09-05 | Stage 5's "8 tipos de enemigo" reached by counting ENEMY_MINIBOSS as its own type (2 separate spawns, matching §40.6's "Mini-bosses: 2") rather than inventing "Cruceros"/"Misiles" as new entities | SPEC.md names them narratively but never mechanically defines them elsewhere; the engine's 8th actual EnemyType enum value already exists and reaching the target count with it is more honest than fabricating new types with no defined behavior |
| 2026-09-05 | Stage 4/5 kept the plain starfield background (no new tile art) rather than adding an asteroid-field or fleet-battle backdrop | Stage 4's asteroids are already the real, gameplay-relevant visual content; Stage 5 ("gran batalla espacial") doesn't call for a distinct backdrop the way Stage 2/3's desert/colony did |
| 2026-09-05 | Achromatic (black+grey, no color channel difference) sprite/tile PNGs must be generated with `-type Palette` (or `PNG8:` output) | Discovered mid-M11: ImageMagick auto-detects an all-grayscale image and writes a smooth 4-bit grayscale ramp instead of a 2-color indexed palette unless forced — caught via `identify -verbose` showing a graya(0/17/34/51...) ramp instead of a 2-entry Colormap, before any build relied on it |
| 2026-09-05 | Added a permanent SHOW_DEBUG_HUD readout ("STAGE X/Y: NAME AST:N") | User-requested (stage name/index); also directly addresses that there was previously no way to confirm which stage was active or whether asteroids were actually spawning, which is what made the "no veo las rocas" report (NS-16) hard to root-cause in the first place |
| 2026-09-05 | Built a real, working Helios encounter at M12 (bossId=6, using the unmodified generic boss framework) rather than only a stub "entrance" cutscene | Repeats the M08/M09 relationship: M12 needs *a* Stage 6 boss to satisfy its own acceptance list (which doesn't mention ending/death-sequence/credits at all), M13 then adds what's uniquely final-boss-shaped on top of already-working content |
| 2026-09-05 | Helios's HP set to 750, continuing the established +100-per-stage progression (250/350/450/550/650) | SPEC.md §40.8 deliberately gives no HP number for the final boss ("no utilizar unicamente una barra de HP") — the arithmetic progression is the same kind of chosen-not-specified value as every earlier boss's HP |
| 2026-09-05 | Helios's sprite adds white as a 4th palette color (index 3) while keeping indices 0-2 byte-identical to every other PAL3-sharing sprite | SPEC.md §21's palette is explicitly "Negro, Rojo, Blanco, Colores de energia" — extending the shared palette (not remapping onto an existing 3-color reference, which would have silently dropped white — caught via `identify -verbose`) keeps everything else spawned during the fight (e.g. phase1's drone) rendering correctly off the already-loaded palette |
| 2026-09-05 | Stage 6's "9 tipos de enemigo" reached via 7 combat types + ENEMY_MINIBOSS + M11's Asteroid system (as "obstaculos") | Same honest-stretch reasoning as Stage 5's Miniboss-counting (NS-11's precedent) — flagged directly rather than inventing a dedicated "Laser hazard" entity for one spec line item |
| 2026-09-05 | Helios's death branches on `currentStage->bossId == FINAL_BOSS_ID` rather than on stage index or a dedicated "is this the last stage" flag | Stays correct even if the campaign array's order or length ever changes (e.g. once Stage 6 stops being the last built stage, if a real Stage 7+ were ever added) — the boss identity is what actually determines "this is the ending", not stage position |
| 2026-09-05 | STATE_ENDING/STATE_CREDITS's real transitions are handled imperatively in game_state.c's switch rather than added to GameState_computeNext's pure table | Matches the established pattern already used for STATE_STAGE_CLEAR/STATE_GAME_OVER/STATE_CONTINUE etc. — these transitions need timer state and side effects the pure (state, startPressed) signature can't express, same reasoning recorded at M01/M02 |
| 2026-09-05 | "Ranking" (SPEC.md §22) shows the run's own final score instead of a persistent leaderboard | No save mechanism (SRAM/EEPROM or otherwise) exists anywhere in this project; tracked as NS-17 rather than silently treated as equivalent to a real ranking |
| 2026-09-06 | Score_getHigh() is never reset by Score_reset(), only by a power cycle | A new game (or continue) shouldn't erase a high score set on an earlier attempt within the same session; matches the common arcade "session high score" convention MILESTONES.md NS-M14-004 asks for |
| 2026-09-06 | Weapon "POWER" bar drawn as N '#' characters (N = weapon level) with no separate label, immediately after the weapon name | No bar-drawing tile/graphic exists; a labeled "WEAPON:X POWER:###" format was found to overflow the 40-column text plane's 39 usable columns in the worst case (LIVES:9 + VULCAN + ### + BOMB:9 + x5) — dropping the label was the fix, not a smaller/renamed field |
| 2026-09-06 | Stage-clear bonus (STAGE_CLEAR_BONUS = 1000, flat) chosen and added | MILESTONES.md NS-M14-007 asks for one; GAME_SPEC.md gives no exact value anywhere (checked, no "bonus" match) — same chosen-not-specified rationale as every other placeholder-era number in this project |
| 2026-09-06 | "Six boss themes" (TASKS.md NS-M15-008) simplified to one shared MUSIC_BOSS track + a distinct MUSIC_FINAL_BOSS for Helios, 9 tracks total instead of 13 | Hand-authored PSG note-array composition doesn't scale well to a dozen genuinely distinct dramatic pieces for diminishing returns — documented scope choice, not an oversight |
| 2026-09-06 | SFX channel 1/3 are last-wins, not queued — a new SFX cuts off whatever was still playing | Only one spare tone channel (0 is reserved for music) and one noise channel exist; a priority/mixing system is unwarranted complexity for a placeholder-era SFX layer (AGENT.md §8) |
| 2026-09-06 | Added Audio_update() to STATE_STAGE_CLEAR and STATE_CREDITS | Both states start a new sound (SFX_STAGE_CLEAR / MUSIC_CREDITS respectively) that needs per-frame ticking to advance past its first note — without it, either would freeze audibly for the whole state, the same "PSG channels latch" bug class M06/M09 already fixed, just newly introduced by this milestone's own additions and caught before shipping |
| 2026-09-06 | Explosion effect is an 8-slot pool of single-sprite, visibility-toggle "flash" entities, not a multi-frame particle animation | No extra animation frames were drawn for this placeholder-era asset — matches the same polish level as every other placeholder sprite in this project rather than investing in real particle art this milestone doesn't require |
| 2026-09-06 | enterAttack() (boss.c) now explicitly forces the boss sprite VISIBLE | Self-caught bug: TRANSITION's new phase-change blink (M16) can leave the sprite HIDDEN on whatever frame its timer hits 0, and nothing previously restored visibility on the TRANSITION->ATTACK edge (only TELEGRAPH used to blink, and enterVulnerable() already handled its own restore) |

---

# PERFORMANCE NOTES

```text
Target FPS: 60
Observed FPS: on-screen counter now exists (VDP_showFPS, top-right, SHOW_DEBUG_HUD); no numeric reading recorded yet
Max enemies: 12 (ENEMY_POOL_SIZE, SPEC.md §24)
Max projectiles: 96 (16 player + 80 enemy, shared pool, SPEC.md §24)
Max powerups: 8 (POWERUP_POOL_SIZE, not specified in SPEC.md)
Max asteroids: 15 (ASTEROID_POOL_SIZE, SPEC.md §40.5)
Max explosions: 8 (EXPLOSION_POOL_SIZE, not specified in SPEC.md)
Max bosses: 1 (module-level singleton, like Player)
Max sprites: 141 (1 player + up to 12 enemies + up to 96 projectiles + up
  to 8 powerups + up to 15 asteroids + up to 8 explosions + 1 boss) —
  theoretical pool ceiling, not a realistic simultaneous count
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
