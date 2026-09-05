#ifndef _AUDIO_H_
#define _AUDIO_H_

void Audio_init(void);
void Audio_update(void);

// Silences the PSG channel. Without this, the last tone set by Audio_update
// keeps sounding indefinitely once nothing is calling it anymore (e.g. on
// GAME_OVER) — PSG channels latch their last frequency/envelope until told
// otherwise, they don't stop on their own.
void Audio_stop(void);

#endif // _AUDIO_H_
