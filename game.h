/*
 * game.h
 * Martin Duquesnoy (150193@supinfo.com)
 */

#ifndef GAME_H
#define GAME_H

#include "fshark.h"

#define FULL_HEALTH (200)
#define BEAM_RELOAD_TIMER (5)

void game_init(void);
int game_accuracy_process(void);
void game_row_award(void);
void game_fire(void);
void game_tesla_weapon(void);
void game_damage(int damage, SDL_Rect *e_geo);
void game_enemy_missed(struct render_obj *r);
void game_pause(void);
void game_process(void);
void game_frag(struct render_obj *r);
void game_end(void);

#endif /* GAME_H */
