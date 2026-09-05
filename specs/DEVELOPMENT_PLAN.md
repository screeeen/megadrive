# NEON STRIKE — Development Plan

This document bridges the abstract specs (`SPEC.md` / `GAME_SPEC.md`, `AGENT.md`,
`MILESTONES.md`, `TASKS.md`, `VALIDATION.md`) with the concrete SGDK/BlastEm
environment that already exists in this repository. It does not repeat their
content; it makes the open engineering decisions they deliberately leave to
the implementer, and sequences the work.

Source-of-truth order remains as `AGENT.md` defines it:
`SPEC.md > AGENT.md > MILESTONES.md > PROGRESS.md > TASKS.md`.
This document is subordinate to all of them — where it disagrees, they win.

---

## 0. Where we actually stand (M00 = PASS)

See `PROGRESS.md` for the full audit. Summary: toolchain, SGDK and BlastEm are
all built from source and verified working; a prototype exists with VDP text,
one PSG melody, one animated sprite, and D-pad movement, split into
`input.c` / `player.c` / `audio.c`. None of this is NEON STRIKE gameplay yet —
it's scaffolding proving the pipeline works end to end.

## 1. File layout: reconciling `VALIDATION.md` with SGDK's actual conventions

`VALIDATION.md` §1 proposes `src/`, `assets/`, `tests/`, `tools/`, `build/` as
sibling top-level roots. SGDK's `makefile.gen` already globs up to 5
directory levels under `src/`, `res/`, `inc/`, and only those three (plus
`out/`) are meaningful to the build. Per `AGENT.md` §3 ("do not replace the
build system") and its own §8 ("names and exact paths may adapt to the
existing repository"), the right reading is: keep SGDK's roots, organize
*within* them by subsystem:

```
src/
  main.c          orchestration only (state machine drive loop)
  game/           state machine, game loop ordering
  player/         NX-01 entity: movement, damage, weapon holder
  weapons/        Vulcan/Laser/Wide/Homing/Flame + player projectile pool
  enemies/        enemy base + Drone/Fighter/Bomber/Turret/Swarm/Charger/Shield
  bosses/         boss framework + per-boss attack data
  stages/         stage data tables, spawn manager, scroll, parallax
  collision/      shared hitbox/AABB routines
  hud/            score, lives, weapon, bomb display
  effects/        explosions, hit flashes, muzzle flashes
  audio/          music/SFX glue (replaces today's single audio.c)
  input/          joypad wrapper (exists today)
inc/              mirrors src/ subfolders 1:1, SGDK convention
res/
  sprite/ image/ music/ sfx/   one .res per subsystem, not one giant file
tests/            host-native unit tests (see §3)
```

`assets/`, `tools/`, `build/` from `VALIDATION.md` aren't separate concepts
here: `res/` already *is* the assets root and `out/` already *is* the build
output. Duplicating them would violate "do not introduce a second build
system." Reorganize incrementally, one milestone at a time, as each
subsystem is actually touched — not as a big-bang restructure that risks the
working baseline for no gameplay gain (`AGENT.md` §3, §16).

## 2. Engineering decisions the specs deliberately leave open

**Object pools.** Fixed-size static arrays with an `active` flag per slot,
sized to the budgets in `SPEC.md` §24/§40.18:
`Enemy enemies[12]`, `Projectile enemyBullets[80]`, `Projectile playerBullets[16]`,
`Powerup powerups[8]`, `Effect effects[16]`. No heap allocation anywhere in
gameplay code (`AGENT.md` §10-11).

**Sprite budget, reality-checked.** Genesis hardware supports 80 sprites/frame
(20/scanline); the spec's ≤64 is a safety margin against `SPR_update()`
overhead and scanline limits under fast horizontal motion, not the hardware
ceiling itself. Track actual count via SGDK's sprite engine each frame in the
debug HUD (`AGENT.md` §15).

**Palette budget, reality-checked.** Genesis has 4 palette rows × 15 colors +
background = a hard ceiling around 64 on-screen colors, *not* a soft one.
"16-32 dominant colors per scene" (`GAME_SPEC.md` §32) must be planned against
that ceiling explicitly: PAL0 = HUD/text, PAL1 = player + player bullets,
PAL2 = enemies, PAL3 = background tiles/effects, following the same pattern
already used for the donut prototype (`PAL_setPalette(PAL1, ...)`).

**Stage data format.** Concrete to `SpawnEvent`/`PowerupEvent` as sketched in
`VALIDATION.md` §7, stored as `const` ROM arrays (zero RAM cost) — one file
per stage, e.g. `src/stages/stage1_data.c`. `StageManager` walks them by
comparing a stored cursor against the current frame counter. Changing a
spawn is a one-line data edit, never an engine change (`AGENT.md` §9).

**Automated tests — the specs don't say how, because 68000 ROM code can't run
a test runner.** The realistic split: pull hardware-free logic (collision
math, weapon-level transitions, combo/score math, spawn-event cursor
advance, RNG-if-any) into files with no `SPR_*`/`PSG_*`/`VDP_*` calls. Those
compile natively with host `gcc`/`clang` and run as real, fast unit tests in
`tests/` via `make test` — no emulator involved. Everything that *does* touch
hardware (rendering, sound, actual collision-against-sprite-position) is only
verifiable through BlastEm + the visual/functional checklists `AGENT.md` §12
and `VALIDATION.md` §5 already define. This is the only workable reading of
`AGENT.md` §11's "automated tests" requirement on real Mega Drive hardware,
and it's what "Tests: UNKNOWN" in `PROGRESS.md` is waiting on.

**Boss framework sequencing.** Built once, generalized from Stage 1's boss
(M08 before M09, exactly as `MILESTONES.md` already orders it) — this is the
highest-leverage reusable system after the core loop, since 6 stage bosses +
1 final boss all sit on top of it.

## 3. Phase grouping (same 20 milestones, grouped for checkpoints)

- **Phase A — Engine backbone (M01–M07).** State machine, player, all 5
  weapons, enemy framework, power-ups/bomb/combo, data-driven stage system.
  Highest risk: every later milestone is content built on these APIs. No
  stage content yet — this phase is invisible progress by design.
- **Phase B — First vertical slice (M08–M09).** Stage 1 fully playable
  end-to-end + boss framework generalized from it. First point where "is
  this actually fun" can be judged with real content.
- **Phase C — Content scale-out (M10–M13).** Stages 2–6 + HELIOS, built
  *purely as data* (new `SpawnEvent` tables, new boss data, new tiles) if
  Phase A/B were built correctly — should require zero new engine code.
- **Phase D — Game feel & flow (M14–M16).** HUD/score/lives/continue flow,
  real stage/boss music and SFX (replacing today's placeholder arpeggio),
  effects polish.
- **Phase E — Hardening (M17–M19).** Performance stress tests, full-campaign
  playthrough validation, release cleanup.

## 4. Immediate next action

Start **M01 — Core Game Loop**: introduce a `GameState` enum and dispatch,
rebuild `main.c` as a thin state-machine driver on top of the canonical
update order (`VBlank → Input → Player → Enemy → Projectile → Collision →
Spawn Manager → Score → Sprite List → Scroll`). Reuse `input.c`/`player.c`/
`audio.c` as-is for now; they migrate into `src/player/`, `src/audio/` etc.
as their own milestones touch them, not all at once.
