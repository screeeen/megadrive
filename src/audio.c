#include <genesis.h>
#include "audio.h"

// Every "theme" below is a hand-picked note-frequency sequence looped on
// PSG channel 0 — real, audible, distinct chip-tune music, but NOT actual
// composed/orchestrated scores (that would need SGDK's XGM driver plus
// real tracker-authored music files, which don't exist in this project —
// see PROGRESS.md's M15 summary). "6 boss themes" (TASKS.md NS-M15-008)
// is deliberately simplified to one shared, more intense track (index
// MUSIC_BOSS) reused by all 6 regular stage bosses, plus a distinct,
// grander MUSIC_FINAL_BOSS for Helios and its own MUSIC_CREDITS — 9
// tracks total rather than 12, a documented scope choice, not an
// oversight.
typedef struct
{
    const u16* notes;
    u8 count;
    u8 frameDiv; // frames each note holds
} MusicTrack;

// --- Track 0: M07's original placeholder (kept for testStage/testStage
// regression use, see stage_data.c) ---
static const u16 track0Notes[] = {
    262, 294, 330, 349, 392, 440, 494, 523,
    494, 440, 392, 349, 330, 294
};

// --- Track 1: Stage 1, Orbital City — upbeat major ---
static const u16 track1Notes[] = { 262, 330, 392, 523, 392, 330, 294, 349, 440, 587, 440, 349 };

// --- Track 2: Stage 2, Red Desert — exotic/minor, slower ---
static const u16 track2Notes[] = { 165, 175, 196, 233, 262, 233, 196, 175 };

// --- Track 3: Stage 3, Space Colony — mechanical/staccato, fast ---
static const u16 track3Notes[] = { 262, 262, 311, 262, 196, 196, 233, 196 };

// --- Track 4: Stage 4, Asteroid Belt — tense minor syncopation ---
static const u16 track4Notes[] = { 220, 262, 220, 330, 220, 262, 294, 262 };

// --- Track 5: Stage 5, Helios Fleet — heroic/triumphant, wide range ---
static const u16 track5Notes[] = { 262, 330, 392, 523, 587, 523, 392, 330, 349, 440, 523, 440 };

// --- Track 6: Stage 6, Helios Core — dark, driving, fast ---
static const u16 track6Notes[] = { 165, 165, 196, 165, 175, 175, 208, 175 };

// --- Track 7: shared boss theme — fast, tense alternation ---
static const u16 track7Notes[] = { 262, 311, 370, 440, 370, 311 };

// --- Track 8: final boss (Helios) — grander, alternating low/high ---
static const u16 track8Notes[] = { 165, 247, 330, 247, 175, 262, 349, 262, 196, 294, 392, 294 };

// --- Track 9: credits — gentle, resolving ---
static const u16 track9Notes[] = { 392, 349, 330, 294, 262, 294, 330, 262 };

#define TRACK(notes, div) { notes, sizeof(notes) / sizeof(notes[0]), div }

static const MusicTrack tracks[] = {
    TRACK(track0Notes, 12),
    TRACK(track1Notes, 10),
    TRACK(track2Notes, 16),
    TRACK(track3Notes,  7),
    TRACK(track4Notes,  9),
    TRACK(track5Notes,  8),
    TRACK(track6Notes,  6),
    TRACK(track7Notes,  6),
    TRACK(track8Notes,  6),
    TRACK(track9Notes, 14),
};
#define TRACK_COUNT (sizeof(tracks) / sizeof(tracks[0]))

static u8 currentTrack;
static u16 noteIndex;
static u16 frameCount;

// --- SFX (PSG channel 1, tone-based; SFX_EXPLOSION uses channel 3/noise
// instead — see Audio_playSfx) ---
typedef struct
{
    const u16* notes;
    u8 count;
    u8 frameDiv;
} SfxTrack;

static const u16 sfxShotNotes[]       = { 880, 660 };
static const u16 sfxHitNotes[]        = { 440, 220 };
static const u16 sfxPowerupNotes[]    = { 523, 659, 880 };
static const u16 sfxBombNotes[]       = { 880, 660, 440, 220 };
static const u16 sfx1upNotes[]        = { 523, 659, 784, 1047 };
static const u16 sfxStageClearNotes[] = { 523, 587, 659, 784, 1047 };
static const u16 sfxBossNotes[]       = { 784, 659, 523, 440, 349, 262 };

// Indexed by SfxId — must stay in the same order as the enum (audio.h),
// up to but not including SFX_EXPLOSION (handled separately, see
// Audio_playSfx: it uses the noise channel, not this tone table).
static const SfxTrack sfxTracks[] = {
    TRACK(sfxShotNotes,       3),
    TRACK(sfxHitNotes,        5),
    TRACK(sfxPowerupNotes,    4),
    TRACK(sfxBombNotes,       5),
    TRACK(sfx1upNotes,        5),
    TRACK(sfxStageClearNotes, 6),
    TRACK(sfxBossNotes,       7),
};

#define EXPLOSION_FRAMES 15

static u8 sfxIndex;      // 0..sfxTrack.count-1 = playing a note; == count = about-to-silence tick; > count = idle
static u8 sfxFrameCount;
static u8 sfxTrackId;
static u8 explosionFramesLeft;

void Audio_init(void)
{
    PSG_reset();

    currentTrack = 0;
    noteIndex = 0;
    frameCount = 0;
    PSG_setEnvelope(0, 4);
    PSG_setFrequency(0, tracks[0].notes[0]);

    sfxIndex = 0xFF; // idle
    explosionFramesLeft = 0;
}

void Audio_playMusic(u8 musicId)
{
    if (musicId >= TRACK_COUNT)
        musicId = 0;

    currentTrack = musicId;
    noteIndex = 0;
    frameCount = 0;
    PSG_setEnvelope(0, 4); // un-silence in case Audio_stop() muted it
    PSG_setFrequency(0, tracks[currentTrack].notes[0]);
}

void Audio_update(void)
{
    const MusicTrack* track = &tracks[currentTrack];

    if (++frameCount >= track->frameDiv)
    {
        frameCount = 0;
        noteIndex = (noteIndex + 1) % track->count;
        PSG_setFrequency(0, track->notes[noteIndex]);
    }

    if (sfxIndex <= sfxTracks[sfxTrackId].count)
    {
        if (++sfxFrameCount >= sfxTracks[sfxTrackId].frameDiv)
        {
            sfxFrameCount = 0;

            if (sfxIndex < sfxTracks[sfxTrackId].count)
                PSG_setFrequency(1, sfxTracks[sfxTrackId].notes[sfxIndex]);
            else
                PSG_setEnvelope(1, PSG_ENVELOPE_MIN); // one tick past the last note: silence

            sfxIndex++;
        }
    }

    if (explosionFramesLeft > 0)
    {
        if (--explosionFramesLeft == 0)
            PSG_setEnvelope(3, PSG_ENVELOPE_MIN);
    }
}

void Audio_stop(void)
{
    PSG_setEnvelope(0, PSG_ENVELOPE_MIN);
    PSG_setEnvelope(1, PSG_ENVELOPE_MIN);
    PSG_setEnvelope(3, PSG_ENVELOPE_MIN);
    sfxIndex = 0xFF;
    explosionFramesLeft = 0;
}

void Audio_playSfx(SfxId sfx)
{
    if (sfx == SFX_EXPLOSION)
    {
        PSG_setEnvelope(3, 4);
        PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_CLOCK2);
        explosionFramesLeft = EXPLOSION_FRAMES;
        return;
    }

    sfxTrackId = (u8) sfx;
    sfxFrameCount = 0;
    PSG_setEnvelope(1, 4);
    PSG_setFrequency(1, sfxTracks[sfxTrackId].notes[0]);
    sfxIndex = 1; // note 0 already sounding; next tick advances to note 1
}
