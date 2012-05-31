/*
 * game.c
 * Martin Duquesnoy (150193@supinfo.com)
 */

#include "game.h"
#include "util.h"
#include "render_obj.h"
#include "ui.h"
#include "plane.h"
#include "enemy.h"
#include "map.h"

void
game_init(void)
{
     fs.score = fs.nfire = fs.accuracy = fs.row = 0;
     fs.health = 100;
}

void
game_row_award(void)
{
     /* Multiple frag in a row award */
     if(!(fs.row % 5))
          ui_msg_new(0xAAAA00, "Row of %d!!", fs.row);

     /* Each 10 frag in a row, IMPRESSIVE award */
     if(!(fs.row % 10))
     {
          Mix_PlayChannel(-1, fs.snd.impressive, 0);
          ui_msg_new(0x000000, "IMPRESSIVE!");
     }
}

int
game_accuracy_process(void)
{
     fs.accuracy = (float)(fs.score - (fs.nfire - fs.score)) / (float)fs.nfire;
     fs.accuracy *= 100;

     if(fs.accuracy < 0)
          fs.accuracy = 0;

     return fs.accuracy;
}

static void
game_frag(struct render_obj *r)
{
     ++fs.score;
     ++fs.row;

     /* Replace enemy object by explosion */
     render_obj_new(fs.enemy.sboom, &r->geo, ROBeam);
     render_obj_remove(r, true);

     /* Show row indication, when its not a "Row of x" message */
     if(fs.row % 5 && fs.row % 10)
          ui_msg_new(0x00BBAA, "Enemy fragged! (%d)", fs.row);
}


void
game_fire(void)
{
     bool hit = false;
     int x_aim = fs.plane.geo.x + (PLANE_W / 2);
     struct render_obj *ro;

     /* Check if reload is done */
     if(!TIMER_IS_DONE(fs.plane.beam_timer))
          return;

     ++fs.nfire;

     /* Dang! */
     Mix_PlayChannel(-1, fs.snd.beam, 0);

     /* Check colision between beam and enemies */
     STAILQ_FOREACH(ro, &fs.render_objs, next)
          if(ro->flags & RENDER_OBJ_FRAGABLE)
          {
               if((x_aim >= ro->geo.x && x_aim < ro->geo.x + (ENEMY_SIZE - 3))
                  && (ro->geo.y <= fs.plane.geo.y))
               {
                    game_frag(ro);
                    hit = true;
                    break; /* << one enemy fragged at a time */
               }
          }

     /* Are you accurate? */
     if(fs.nfire)
          game_accuracy_process();

     fs.plane.beam_timer = BEAM_RELOAD_TIMER;

     /* Row of frag set back to 0 if not hitted */
     if(!hit)
          fs.row = 0;
     else
          game_row_award();

     /* Render plane beam */
     plane_render_beam(hit ? ro : NULL);
}

void
game_damage(int damage, SDL_Rect *e_geo)
{
     /* Collision check */
     if((fs.plane.geo.x <= e_geo->x + e_geo->w && fs.plane.geo.x + PLANE_W >= e_geo->x)
        && (fs.plane.geo.y <= e_geo->y + e_geo->h && fs.plane.geo.y + PLANE_H >= e_geo->y))
     {
          struct render_obj *d;

          fs.health -= damage;
          fs.plane.collision_timer = 8;
          d = render_obj_new(fs.plane.sdamage, &fs.plane.geo, ROTrail);
          d->flags |= RENDER_OBJ_EPHEMERAL;

          fs.flags |= FS_STATE_DAMAGE;

          ui_msg_new(0xFF0000, "Collision Damage (-%d)", damage);

          /* Tic! */
          Mix_PlayChannel(-1, fs.snd.alert, 0);
     }
}

void
game_enemy_missed(struct render_obj *r)
{
     ++fs.nfire; /* To lose accuracy when an enemy disappear */
     game_accuracy_process();
     render_obj_remove(r, true);
     ui_msg_new(0xBB0000, "Enemy missed :-(");
}

void
game_pause(void)
{
     if((fs.flags ^= FS_PAUSE))
     {
          SDL_Color col = rgb_to_color(0x00FF00);

          sdl_print_text(fs.root, fs.font, &col, 295, 410, "PAUSE");
     }

     /* Doot! */
     Mix_PlayChannel(-1, fs.snd.pause, 0);
}

void
game_process(void)
{
     if(fs.flags & FS_PAUSE)
          return;

     map_render();

     /* Remove beam objects */
     if(TIMER_IS_DONE(fs.plane.beam_timer))
          render_obj_flush_list_by_type(ROBeam);

     /* Move enemy render_obj to follow map or flying */
     enemy_process();

     /* Render render_obj list */
     render_obj_render();

     /* Render plane features (plane is the last rendered) */
     plane_process_render();

     /* User interface */
     if(!(fs.flags & FS_HIDE_UI))
          ui_render();

     /* Main timers process */
     timer_process(&fs.plane.beam_timer);
     timer_process(&fs.plane.collision_timer);
     timer_process(&fs.enemy.release_timer);
     timer_process(&fs.enemy.fighter_timer);
}
