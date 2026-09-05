#ifndef _DEBUG_H_
#define _DEBUG_H_

// Development-time overlay (FPS, hitbox outline, lives/weapon readout) and
// the B-button self-damage trigger used to test Player_hit() before real
// hazards (enemies/projectiles, M03+) exist. AGENT.md §15: debug rendering
// must not be required in the final release build — flip this to 0 (or gate
// it behind a build flag) when M19 approaches.
#define SHOW_DEBUG_HUD 1

#endif // _DEBUG_H_
