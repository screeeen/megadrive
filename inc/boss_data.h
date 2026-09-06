#ifndef _BOSS_DATA_H_
#define _BOSS_DATA_H_

#include "boss.h"

// One BossDef per stage boss (SPEC.md §16-18). Every field a new stage's
// boss would need to supply lives here, not in boss.c/.h — see
// PROGRESS.md's M09/M10 summaries.
extern const BossDef orbitalGuardianDef;   // Stage 1: Orbital City
extern const BossDef sandWormDef;          // Stage 2: Red Desert
extern const BossDef industrialCoreDef;    // Stage 3: Space Colony
extern const BossDef miningFortressDef;    // Stage 4: Asteroid Belt
extern const BossDef admiralXDef;          // Stage 5: Helios Fleet
extern const BossDef heliosDef;            // Stage 6: Helios Core ("final boss entrance" — see PROGRESS.md M12/M13 split)

// StageDef.bossId -> BossDef, so game_state.c can spawn the right boss for
// whichever stage is currently active without a stage-by-stage switch of
// its own.
const BossDef* BossDef_forId(u8 bossId);

#endif // _BOSS_DATA_H_
