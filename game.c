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
     SDL_Rect geo;

     SDL_ShowCursor(SDL_DISABLE);

     fs.score = fs.nfire = fs.accuracy = fs.row = 0;
     fs.ntesla = 5;
     fs.health = FULL_HEALTH;

     /* Clean list of everything */
     render_obj_flush_list();

     geo.x = WIDTH / 2;
     geo.y = HEIGHT / 2;
     geo.w = PLANE_W;
     geo.h = PLANE_H;

     fs.plane.plane = render_obj_new(fs.plane.s, &geo, ROPlane);
     fs.plane.plane->flags |= RENDER_OBJ_SHADOW;
     fs.plane.plane->sshadow = fs.plane.sshadow;
     fs.plane.collision_timer = 0;
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
     /* NO division by 0 */
     if(!fs.nfire)
          return 0;

     fs.accuracy = (float)(fs.score - (fs.nfire - fs.score)) / (float)fs.nfire;
     fs.accuracy *= 100;

     if(fs.accuracy < 0)
          fs.accuracy = 0;
     else if(fs.accuracy > 100)
          fs.accuracy = 100;

     return fs.accuracy;
}

void
game_frag(struct render_obj *r)
{
     struct render_obj *ro;

     ++fs.score;
     ++fs.row;

     /* Replace enemy object by explosion */
     ro = render_obj_new(fs.enemy.sboom, &r->geo, ROExplosion);
     ro->flags |= RENDER_OBJ_EPHEMERAL | RENDER_OBJ_FLASH;
     ro->shake_intensity = 2;
     ro->timer = 10;

     render_obj_remove(r, true);

     /* Show row indication, when its not a "Row of x" message */
     if(fs.row % 5 && fs.row % 10)
          ui_msg_new(0x00BBAA, "Enemy fragged! (%d)", fs.row);

     game_accuracy_process();
     game_row_award();
}

void
game_fire(void)
{
     bool hit = false;
     int x_aim = fs.plane.plane->geo.x + (PLANE_W / 2);
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
                  && (ro->geo.y <= fs.plane.plane->geo.y))
               {
                    game_frag(ro);
                    hit = true;
                    //break; /* << one enemy fragged at a time */
               }
          }

     fs.plane.beam_timer = BEAM_RELOAD_TIMER;

     /* Row of frag set back to 0 if not hitted */
     if(!hit)
          fs.row = 0;

     /* Render plane beam */
     plane_render_beam(hit ? ro : NULL);
}

void
game_tesla_weapon(void)
{
     struct render_obj *ro;
     SDL_Rect r =
     {
          .x = fs.plane.plane->geo.x - 116,
          .y = fs.plane.plane->geo.y - 110,
     };

     /* No more tesla weapon */
     if(!fs.ntesla)
     {
          ui_msg_new(0x0000FF, "No more Tesla");
          return;
     }

     fs.flags |= FS_STATE_TESLA;
     ++fs.nfire;
     --fs.ntesla;

     /* Kaboom! */
     Mix_PlayChannel(-1, fs.snd.tesla, 0);

     ui_msg_new(0x0000FF, "Tesla Weapon!");

     ro = render_obj_new(fs.plane.tesla, &r, ROTesla);
     ro->flags |= RENDER_OBJ_EPHEMERAL | RENDER_OBJ_FLASH | RENDER_OBJ_SHAKE;
     ro->shake_intensity = 2;
     ro->timer = 30;

     /* frag every fragable object in list */
     STAILQ_FOREACH(ro, &fs.render_objs, next)
          if(ro->flags & RENDER_OBJ_FRAGABLE)
               game_frag(ro);
}

void
game_damage(int damage, SDL_Rect *e_geo)
{
     SDL_Rect *g = &fs.plane.plane->geo;

     /* Collision check */
     if((g->x <= e_geo->x + e_geo->w && g->x + PLANE_W >= e_geo->x)
        && (g->y <= e_geo->y + e_geo->h && g->y + PLANE_H >= e_geo->y))
     {
          struct render_obj *d;

          fs.health -= damage;
          fs.plane.collision_timer = 8;

          d = render_obj_new(fs.plane.sdamage, g, ROTrail);
          d->flags |= RENDER_OBJ_EPHEMERAL;

          fs.flags |= FS_STATE_DAMAGE;

          ui_msg_new(0xFF0000, "Collision Damage (-%d)", damage);

          /* Tic! */
          Mix_PlayChannel(-1, fs.snd.alert, 0);

          if(fs.health <= 0)
               game_end();
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
game_end(void)
{
     fs.flags &= ~FS_RUNNING;
     fs.flags |= FS_END_LOOP;
}

void
game_process(void)
{
     if(fs.flags & FS_PAUSE)
          return;

     map_render();

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
