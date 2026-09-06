#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <genesis.h>

// StageDef.musicId values (SPEC.md §16-21 gives each stage/boss a name,
// not actual composed music — every track here is a chosen, hand-authored
// PSG note sequence, not a real orchestral/FM score; see PROGRESS.md's
// M15 summary for exactly what "music" means in this engine). 0 is the
// generic placeholder M07's testStage still uses; 1-6 are the real stage
// themes (stageN_data.c). 7-9 aren't stage musicIds — they're switched to
// directly by game_state.c for the boss/final-boss/credits beats, which
// aren't tied to a single stage's own musicId.
#define MUSIC_BOSS       7 // shared by every regular stage boss (Orbital Guardian..Admiral X)
#define MUSIC_FINAL_BOSS 8 // Helios only
#define MUSIC_CREDITS    9

void Audio_init(void); // one-time PSG reset; starts track 0

// Switches the looping melody on channel 0 to a different track,
// resetting its position — used whenever gameplay moves between stage
// music and boss/final-boss/credits music. Un-silences channel 0 if
// Audio_stop() had muted it.
void Audio_playMusic(u8 musicId);

void Audio_update(void);

// Silences every PSG channel (music + any in-progress SFX). Without this,
// the last tone/noise set keeps sounding indefinitely once nothing is
// updating it anymore (e.g. on GAME_OVER) — PSG channels latch until told
// otherwise, they don't stop on their own.
void Audio_stop(void);

// One-shot sound effects on PSG channel 1 (tones) or channel 3 (noise,
// SFX_EXPLOSION only) — channel 0 stays dedicated to the music so SFX
// never interrupt it. Triggering a new SFX while one is still playing
// cuts the previous one off (last-wins; a deliberate simplification for
// a single spare tone channel, not a bug — see PROGRESS.md).
typedef enum
{
    SFX_SHOT,
    SFX_HIT,        // player takes damage (TASKS.md's "player damage"/"hit" are treated as the same event)
    SFX_POWERUP,
    SFX_BOMB,
    SFX_1UP,
    SFX_STAGE_CLEAR,
    SFX_BOSS,       // a boss is defeated — distinct from a regular enemy/asteroid's SFX_EXPLOSION
    SFX_EXPLOSION   // enemy or asteroid destroyed (uses the noise channel, not a tone)
} SfxId;

void Audio_playSfx(SfxId sfx);

#endif // _AUDIO_H_
