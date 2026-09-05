#ifndef _PLAYER_LOGIC_H_
#define _PLAYER_LOGIC_H_

#include <stdbool.h>

// Hardware-free, like game_state_logic.h: no genesis.h, no SGDK calls.
// Compiles under both the m68k cross-compiler and a native host compiler.
// (Deliberately avoids <stdint.h>: m68k-elf-gcc was built --without-headers,
// so its bundled stdint.h #include_next's a target libc one that doesn't
// exist. Plain `unsigned char` is 8-bit and include-free on both sides.)

// SPEC.md §7 / TASKS.md NS-M04-001
typedef enum
{
    WEAPON_VULCAN,
    WEAPON_LASER,
    WEAPON_WIDE,
    WEAPON_HOMING,
    WEAPON_FLAME,
    WEAPON_COUNT
} WeaponType;

typedef struct
{
    WeaponType weapon;
    unsigned char weaponLevel; // 1..3
} WeaponState;

// B button (SPEC.md §5/§8): switching always resets to level 1.
WeaponState Weapon_switchNext(WeaponState state);

// Same-weapon pickup (SPEC.md §8): raises level by one, capped at 3.
WeaponState Weapon_levelUp(WeaponState state);

typedef struct
{
    unsigned char weaponLevel; // 1..3
    unsigned char lives;
} PlayerHitState;

typedef struct
{
    PlayerHitState state;
    bool lifeLost; // true if this hit cost a life (was already at weapon L1)
    bool gameOver; // true if lifeLost AND no lives remain afterwards
} PlayerHitResult;

// SPEC.md Player §6: a hit always costs one weapon level; a hit taken at
// weapon level 1 costs a life instead (and resets weapon to L1). Pure
// decision, no side effects — the caller applies the result to real state
// and drives sprite/animation/state-machine effects from it.
PlayerHitResult Player_applyHit(PlayerHitState state);

#endif // _PLAYER_LOGIC_H_
