#include "bomb.h"
#include "enemy.h"
#include "projectile.h"
#include "combo.h"
#include "score.h"
#include "audio.h"
#include "explosion.h"

// Not specified exactly in SPEC.md ("alto dano", "~0.5 segundos") — chosen
// here, see PROGRESS.md. 10 comfortably exceeds Shield's 6 max HP, the
// highest of any M05 enemy, so a bomb always one-shots everything on screen.
#define BOMB_DAMAGE 10
#define BOMB_INVULN_FRAMES 30 // ~0.5s at 60Hz

bool Bomb_use(Player* player)
{
    if (!Player_useBomb(player))
        return FALSE;

    Audio_playSfx(SFX_BOMB);

    Enemy* enemies = Enemy_getPool();

    for (u16 i = 0; i < ENEMY_POOL_SIZE; i++)
    {
        Enemy* e = &enemies[i];

        if (!e->active)
            continue;

        // A bomb bypasses Shield's temporary directional resistance — it's
        // an emergency clear-the-screen tool (SPEC.md §9), not a normal
        // attack a shield should be able to block.
        if (e->type == ENEMY_SHIELD)
            e->vulnerable = TRUE;

        s16 deathX = e->x;
        s16 deathY = e->y;
        u16 scoreGained = Enemy_hit(e, BOMB_DAMAGE);

        if (scoreGained > 0)
        {
            Score_add((u16) (scoreGained * Combo_getMultiplier()));
            Combo_onKill();
            Explosion_spawn(deathX, deathY);
        }
    }

    Projectile* projectiles = Projectile_getPool();

    for (u16 i = 0; i < PROJECTILE_POOL_SIZE; i++)
    {
        Projectile* p = &projectiles[i];

        if (p->active && p->owner == PROJECTILE_OWNER_ENEMY)
            Projectile_release(p);
    }

    player->state = PLAYER_STATE_INVULNERABLE;
    player->invulnFrames = BOMB_INVULN_FRAMES;

    return TRUE;
}
