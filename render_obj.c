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

struct render_obj*
render_obj_new(SDL_Surface *s, SDL_Rect *geo, enum render_obj_type type)
{
     struct render_obj *r = calloc(1, sizeof(struct render_obj));

     STAILQ_INSERT_TAIL(&fs.render_objs, r, next);

     r->s = s;
     r->geo = *geo;
     r->type = type;
     r->flags = 0;
     r->timer = 0;

     return r;
}

void
render_obj_render(void)
{
     struct render_obj *r;

     STAILQ_FOREACH(r, &fs.render_objs, next)
     {
          SDL_BlitSurface(r->s, NULL, fs.root, &r->geo);

          if(r->flags & RENDER_OBJ_EPHEMERAL)
          {
               timer_process(&r->timer);

               if(TIMER_IS_DONE(r->timer))
                    render_obj_remove(r, true);
          }
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
