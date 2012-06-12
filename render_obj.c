/*
 * render_obj.c
 * Martin Duquesnoy (150193@supinfo.com)
 */

#include "render_obj.h"
#include "util.h"

void
render_obj_init(void)
{
     STAILQ_INIT(&fs.render_objs);
}

static void
render_obj_fix_sgeo(struct render_obj *r)
{
     if(r->geo.x < 0)
          r->sgeo.x = -r->geo.x;

     if(r->geo.y < 0)
          r->sgeo.y = -r->geo.y;
}

struct render_obj*
render_obj_new(SDL_Surface *s, SDL_Rect *geo, enum render_obj_type type)
{
     struct render_obj *r = calloc(1, sizeof(struct render_obj));

     STAILQ_INSERT_TAIL(&fs.render_objs, r, next);

     r->s = s;
     r->sshadow = NULL;
     r->geo = *geo;
     r->type = type;
     r->flags = 0;
     r->timer = 0;
     r->blit = 1;
     r->shake_intensity = 0;
     r->shake_timer = 0;
     r->altitude = 20;

     r->sgeo.x = r->sgeo.y = 0;
     r->sgeo.w = s->w;
     r->sgeo.h = s->h;

     render_obj_fix_sgeo(r);

     return r;
}

static void
render_obj_process_shake(struct render_obj *r)
{
     if(!(r->flags & RENDER_OBJ_SHAKE))
          return;

     if(!TIMER_IS_DONE(r->shake_timer))
     {
          timer_process(&r->shake_timer);
          return;
     }

     r->geo.x += RAND(-r->shake_intensity, r->shake_intensity);
     r->geo.y += RAND(-r->shake_intensity, r->shake_intensity);

     r->shake_timer = r->shake_phase;
     timer_process(&r->shake_timer);

}

static void
render_obj_process_ephemeral(struct render_obj *r)
{
     if(r->flags & RENDER_OBJ_EPHEMERAL)
     {
          timer_process(&r->timer);

          if(TIMER_IS_DONE(r->timer))
          {
               if(r->type == ROTesla)
                    fs.flags &= ~FS_STATE_TESLA;

               render_obj_remove(r, true);
          }
     }
}

static void
render_obj_process_flash(struct render_obj *r)
{
     if(r->flags & RENDER_OBJ_FLASH)
          r->blit = !r->blit;
}

static void
render_obj_process_shadow(struct render_obj *r)
{
     if(r->flags & RENDER_OBJ_SHADOW)
     {
          SDL_Rect shadow_rect = r->geo;

          shadow_rect.x += r->altitude;
          shadow_rect.y += r->altitude;

          SDL_BlitSurface(r->sshadow, NULL, fs.root, &shadow_rect);
     }
}

void
render_obj_render(void)
{
     struct render_obj *r;

     STAILQ_FOREACH(r, &fs.render_objs, next)
     {
          render_obj_process_shake(r);
          render_obj_fix_sgeo(r);
          render_obj_process_ephemeral(r);
          render_obj_process_flash(r);
          render_obj_process_shadow(r);

          if(r->blit) /* always true if no RENDER_OBJ_FLASH flag */
               SDL_BlitSurface(r->s, &r->sgeo, fs.root, &r->geo);
     }
}

void
render_obj_remove(struct render_obj *r, bool remlist)
{
     if(remlist)
          STAILQ_REMOVE(&fs.render_objs, r, render_obj, next);

     free(r);
}

void
render_obj_flush_list(void)
{
     struct render_obj *r;

     while(!STAILQ_EMPTY(&fs.render_objs))
     {
          r = STAILQ_FIRST(&fs.render_objs);
          STAILQ_REMOVE_HEAD(&fs.render_objs, next);
          render_obj_remove(r, false);
     }

     STAILQ_INIT(&fs.render_objs);
}

void
render_obj_flush_list_by_type(enum render_obj_type t)
{
     struct render_obj *r;

     STAILQ_FOREACH(r, &fs.render_objs, next)
          if(r->type == t)
               render_obj_remove(r, true);
}
