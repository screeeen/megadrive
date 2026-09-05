#include <genesis.h>
#include "audio.h"

// C major arpeggio up and back down (Hz), looped on PSG channel 0
static const u16 melody[] = {
    262, 294, 330, 349, 392, 440, 494, 523,
    494, 440, 392, 349, 330, 294
};
#define MELODY_LEN  (sizeof(melody) / sizeof(melody[0]))
#define NOTE_FRAMES 12

static u16 noteIndex;
static u16 frameCount;

void Audio_init(void)
{
    noteIndex = 0;
    frameCount = 0;

    PSG_reset();
    PSG_setEnvelope(0, 4);
    PSG_setFrequency(0, melody[0]);
}

void Audio_update(void)
{
    if (++frameCount >= NOTE_FRAMES)
    {
        frameCount = 0;
        noteIndex = (noteIndex + 1) % MELODY_LEN;
        PSG_setFrequency(0, melody[noteIndex]);
    }
}

void Audio_stop(void)
{
    PSG_setEnvelope(0, PSG_ENVELOPE_MIN);
}
