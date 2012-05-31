/*
 * render_obj.h
 * Martin Duquesnoy (150193@supinfo.com)
 */

#ifndef RENDER_OBJ
#define RENDER_OBJ

#include "fshark.h"

enum render_obj_type
{
     ROBeam,
     ROEnemy,
     ROFighter,
     ROTrail,
     ROETrail,
     RORocket,
     ROLast,
};

struct render_obj
{
     SDL_Surface *s;
     SDL_Rect geo;
     enum render_obj_type type;
#define RENDER_OBJ_EPHEMERAL 0x01 /* use timer */
#define RENDER_OBJ_FRAGABLE  0x02 /* Die at beam collision */
     Flags flags;
     Timer timer; /* <<< For limited life time objects or repeating task */
     STAILQ_ENTRY(render_obj) next;
};

void render_obj_init(void);
struct render_obj *render_obj_new(SDL_Surface *s, SDL_Rect *geo, enum render_obj_type type);
void render_obj_render(void);
void render_obj_remove(struct render_obj *r, bool remlist);
void render_obj_flush_list(void);
void render_obj_flush_list_by_type(enum render_obj_type t);


#endif /* RENDER_OBJ */
