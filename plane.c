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

/* init plane surfaces */
void
plane_init(void)
{
     fs.plane.s = IMG_Load("img/plane.png");
     fs.plane.sshadow = IMG_Load("img/planeshadow.png");
     fs.plane.sdamage = IMG_Load("img/plane_damage.png");
     fs.plane.beam = IMG_Load("img/beam.png");
     fs.plane.trail = IMG_Load("img/chemtrail.png");
     fs.plane.shtrail = IMG_Load("img/shtrail.png");
     fs.plane.tesla = IMG_Load("img/tesla.png");
}

void
plane_free(void)
{
     SDL_FreeSurface(fs.plane.s);
     SDL_FreeSurface(fs.plane.sshadow);
     SDL_FreeSurface(fs.plane.beam);
     SDL_FreeSurface(fs.plane.trail);
     SDL_FreeSurface(fs.plane.shtrail);
     SDL_FreeSurface(fs.plane.sdamage);
     SDL_FreeSurface(fs.plane.tesla);
}

void
plane_move(int x, int y)
{
     fs.plane.plane->geo.x = x;
     fs.plane.plane->geo.y = y;
}

static void
plane_trail(void)
{
     struct render_obj *ro;
     SDL_Rect r =
     {
          .x = fs.plane.plane->geo.x + 9,
          .y = fs.plane.plane->geo.y + 20,
          TRAIL_W,
          TRAIL_H
     };

     /* Left trail */
     ro = render_obj_new(fs.plane.trail, &r, ROTrail);
     ro->flags |= RENDER_OBJ_EPHEMERAL | RENDER_OBJ_SHAKE | RENDER_OBJ_SHADOW;
     ro->timer = TRAIL_LENGTH;
     ro->shake_intensity = 1;
     ro->sshadow = fs.plane.shtrail;

     /* Right trail */
     r.x += 13;
     ro = render_obj_new(fs.plane.trail, &r, ROTrail);
     ro->flags |= RENDER_OBJ_EPHEMERAL | RENDER_OBJ_SHAKE | RENDER_OBJ_SHADOW;
     ro->timer = TRAIL_LENGTH;
     ro->shake_intensity = 1;
     ro->sshadow = fs.plane.shtrail;

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

     /* Remove beam objects */
     if(TIMER_IS_DONE(fs.plane.beam_timer))
          render_obj_flush_list_by_type(ROBeam);
}

void
plane_render_beam(struct render_obj *enemy)
{
     int beamy = -BEAM_H;
     SDL_Rect r =
     {
          .x = fs.plane.plane->geo.x + (PLANE_W >> 1) - 3,
          .y = fs.plane.plane->geo.y - (PLANE_H >> 2) - 1,
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

