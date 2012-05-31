/*
 * enemy.h
 * Martin Duquesnoy (150193@supinfo.com)
 */

#ifndef ENEMY_H
#define ENEMY_H

#include "fshark.h"

#define ENEMY_SIZE (32)

#define ROCKET_H (6)
#define ROCKET_W (2)

#define SPAWN_TIMER (RAND(20, 60))
#define SPAWN_FIGHTER_TIMER (RAND(50, 200))
#define ROCKET_SPAWN_TIMER (10)

#define ROCKET_SPEED (10)
#define FIGHTER_SPEED (14)
#define FIGHTER_TRAIL_LENGTH (14)

void enemy_init(void);
void enemy_process(void);
void enemy_free(void);

#endif /* ENEMY_OBJ */
