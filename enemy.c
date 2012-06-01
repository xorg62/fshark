/*
 * enemy.c
 * Martin Duquesnoy (150193@supinfo.com)
 */

#include "enemy.h"
#include "plane.h"
#include "render_obj.h"
#include "map.h"
#include "ui.h"
#include "util.h"
#include "game.h"

void
enemy_init(void)
{
     fs.enemy.release_timer = SPAWN_TIMER;
     fs.enemy.fighter_timer = SPAWN_FIGHTER_TIMER;

     fs.enemy.s = IMG_Load("img/turret.png");
     fs.enemy.sboom = IMG_Load("img/sun.png");
     fs.enemy.srocket = IMG_Load("img/rocket.png");
     fs.enemy.sfighter = IMG_Load("img/fighter.png");
     fs.enemy.sufo = IMG_Load("img/ufo.png");
}

static void
enemy_spawn(void)
{
     struct render_obj *ro;
     SDL_Rect r;

     if(!TIMER_IS_DONE(fs.enemy.release_timer))
          return;

     r.x = RAND(0, WIDTH - ENEMY_SIZE);
     r.y = -5;
     r.w = r.h = ENEMY_SIZE;

     ro = render_obj_new(fs.enemy.s, &r, ROEnemy);
     ro->flags |= RENDER_OBJ_FRAGABLE | RENDER_OBJ_SHAKE;
     ro->shake_intensity = 5;
     ro->timer = ROCKET_SPAWN_TIMER;

     fs.enemy.release_timer = SPAWN_TIMER;
}

static void
enemy_spawn_fighter(void)
{
     struct render_obj *ro;
     SDL_Rect r;

     if(!TIMER_IS_DONE(fs.enemy.fighter_timer))
          return;

     r.x = fs.plane.geo.x;
     r.y = -5;
     r.w = r.h = ENEMY_SIZE;

     ro = render_obj_new(fs.enemy.sfighter, &r, ROFighter);
     ro->flags |= RENDER_OBJ_FRAGABLE;

     fs.enemy.fighter_timer = SPAWN_FIGHTER_TIMER;
}

static void
enemy_spawn_rocket(struct render_obj *r)
{
     /* Check if enemy is above the plane, to spawn a rocket */
     if(r->geo.x <= fs.plane.geo.x + PLANE_W - 1
        && r->geo.x + ENEMY_SIZE >= fs.plane.geo.x)
     {
          SDL_Rect rgeo = r->geo;

          rgeo.x += ENEMY_SIZE / 2;
          rgeo.y += ENEMY_SIZE / 2;
          rgeo.w = ROCKET_W;
          rgeo.h = ROCKET_H;

          render_obj_new(fs.enemy.srocket, &rgeo, RORocket);
          r->timer = ROCKET_SPAWN_TIMER;
     }
}

static void
enemy_process_rocket(void)
{
     int t;
     struct render_obj *r;

     STAILQ_FOREACH(r, &fs.render_objs, next)
          if(r->type == RORocket)
          {
               if((t = r->geo.y + ROCKET_SPEED) > HEIGHT)
                    render_obj_remove(r, true);
               else
                    r->geo.y = t;

               game_damage(1, &r->geo);
          }
}

static void
enemy_fighter_trail(struct render_obj *r)
{
     struct render_obj *ro;
     SDL_Rect geo = r->geo;

     /* Left trail */
     geo.x += 9;
     geo.y -= TRAIL_H;
     geo.w = TRAIL_W;
     geo.h = TRAIL_H;

     ro = render_obj_new(fs.plane.trail, &geo, ROETrail);
     ro->flags |= RENDER_OBJ_EPHEMERAL;
     ro->timer = FIGHTER_TRAIL_LENGTH;

     /* Right trail */
     geo.x += 12;

     ro = render_obj_new(fs.plane.trail, &geo, ROETrail);
     ro->flags |= RENDER_OBJ_EPHEMERAL;
     ro->timer = FIGHTER_TRAIL_LENGTH;

     /* Check plane collision with previous trails */
     STAILQ_FOREACH(ro, &fs.render_objs, next)
          if(ro->type == ROETrail)
               game_damage(1, &ro->geo);
}

static void
enemy_process_fighter(void)
{
     int t;
     struct render_obj *r;

     STAILQ_FOREACH(r, &fs.render_objs, next)
          if(r->type == ROFighter)
          {
               if((t = r->geo.y + FIGHTER_SPEED) > HEIGHT)
                    render_obj_remove(r, true);
               else
                    r->geo.y = t;

               enemy_fighter_trail(r);

               game_damage(10, &r->geo);
          }
}

static void
enemy_process_enemy(void)
{
     int t;
     struct render_obj *r;

     STAILQ_FOREACH(r, &fs.render_objs, next)
          if(r->type == ROEnemy)
          {
               /* Enemy gone out of screen? Remove it */
               if((t = r->geo.y + MAP_SPEED) > HEIGHT) /* <<< enemy on the ground, following the map scrolling */
                    game_enemy_missed(r);
               /* Move it */
               else
                    r->geo.y = t;

               /* Check if we enemy have to launcher rocket (plane before him) */
               if(TIMER_IS_DONE(r->timer))
                    enemy_spawn_rocket(r);

               /* Check enemy - plane collision */
               if(TIMER_IS_DONE(fs.plane.collision_timer))
                    game_damage(5, &r->geo);

               /* To moderate rocket spawn timing */
               timer_process(&r->timer);
          }
}

void
enemy_process(void)
{
     /* Spawn enemies if timer is done */
     enemy_spawn();
     enemy_spawn_fighter();

     /* Process enemies objects movemen */
     enemy_process_enemy();
     enemy_process_fighter();
     enemy_process_rocket();
}

void
enemy_free(void)
{
     SDL_FreeSurface(fs.enemy.s);
     SDL_FreeSurface(fs.enemy.sboom);
     SDL_FreeSurface(fs.enemy.srocket);
     SDL_FreeSurface(fs.enemy.sfighter);
     SDL_FreeSurface(fs.enemy.sufo);
}

