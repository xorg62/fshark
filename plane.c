/*
 * plane.c
 * Martin Duquesnoy (150193@supinfo.com)
 */

#include "fshark.h"
#include "plane.h"
#include "render_obj.h"
#include "enemy.h"
#include "ui.h"
#include "util.h"

void
plane_init(void)
{
     fs.plane.geo.x = 0;
     fs.plane.geo.y = 0;
     fs.plane.geo.w = PLANE_W;
     fs.plane.geo.h = PLANE_H;
     fs.plane.collision_timer = 0;

     fs.plane.s = IMG_Load("img/plane.png");
     fs.plane.sdamage = IMG_Load("img/plane_damage.png");
     fs.plane.beam = IMG_Load("img/beam.png");
     fs.plane.trail = IMG_Load("img/chemtrail.png");
}

void
plane_free(void)
{
     SDL_FreeSurface(fs.plane.s);
     SDL_FreeSurface(fs.plane.beam);
     SDL_FreeSurface(fs.plane.trail);
     SDL_FreeSurface(fs.plane.sdamage);
}

void
plane_move(int x, int y)
{
     fs.plane.geo.x = x;
     fs.plane.geo.y = y;
}

static void
plane_trail(void)
{
     struct render_obj *ro;
     SDL_Rect r =
     {
          .x = fs.plane.geo.x + 7,
          .y = fs.plane.geo.y + 20,
          TRAIL_W,
          TRAIL_H
     };

     /* Left trail */
     ro = render_obj_new(fs.plane.trail, &r, ROTrail);
     ro->flags |= RENDER_OBJ_EPHEMERAL;
     ro->timer = TRAIL_LENGTH;

     /* Right trail */
     r.x += 16;
     ro = render_obj_new(fs.plane.trail, &r, ROTrail);
     ro->flags |= RENDER_OBJ_EPHEMERAL;
     ro->timer = TRAIL_LENGTH;

     /* Move previous trails */
     STAILQ_FOREACH(ro, &fs.render_objs, next)
          if(ro->type == ROTrail)
               ++ro->geo.y;
}

void
plane_process_render(void)
{
     /* Plane trail */
     plane_trail();

     /* Plane itself */
     SDL_BlitSurface(fs.plane.s, NULL, fs.root, &fs.plane.geo);
}

void
plane_render_beam(struct render_obj *enemy)
{
     int beamy = -BEAM_H;
     SDL_Rect r =
     {
          .x = fs.plane.geo.x + (PLANE_W >> 1) - 3,
          .y = fs.plane.geo.y - (PLANE_H >> 2) - 1,
          .w = BEAM_W,
          .h = BEAM_H
     };

     if(enemy)
          beamy = enemy->geo.y;

     /* Make beam render objects until the top of the screen */
     while(r.y > beamy)
     {
          render_obj_new(fs.plane.beam, &r, ROBeam);
          r.y -= BEAM_H;
     }

}

