/*
 * render_obj.h
 * Martin Duquesnoy (150193@supinfo.com)
 */

#ifndef RENDER_OBJ
#define RENDER_OBJ

#include "fshark.h"

enum render_obj_type
{
     ROPlane,
     ROBeam,
     ROEnemy,
     ROFighter,
     ROTrail,
     ROETrail,
     RORocket,
     ROTesla,
     ROExplosion,
     ROLast,
};

struct render_obj
{
     SDL_Surface *s, *sshadow;
     SDL_Rect geo, sgeo;
     enum render_obj_type type;
#define RENDER_OBJ_EPHEMERAL 0x01 /* Timer use as life time */
#define RENDER_OBJ_FRAGABLE  0x02 /* Die at beam collision */
#define RENDER_OBJ_FLASH     0x04 /* Object is flashing */
#define RENDER_OBJ_SHAKE     0x08 /* Shake randomly the object */
#define RENDER_OBJ_SHADOW    0x10 /* Use shadow surface ptr to draw a shadow */
     Flags flags;
     Timer timer;                 /* For limited life time objects or repeating task */
     bool blit;                   /* For flash state */
     int shake_intensity;
     int shake_phase;             /* Slow down shake moves */
     Timer shake_timer;
     int altitude;                /* for shadow shift */
     STAILQ_ENTRY(render_obj) next;
};

void render_obj_init(void);
struct render_obj *render_obj_new(SDL_Surface *s, SDL_Rect *geo, enum render_obj_type type);
void render_obj_render(void);
void render_obj_remove(struct render_obj *r, bool remlist);
void render_obj_flush_list(void);
void render_obj_flush_list_by_type(enum render_obj_type t);


#endif /* RENDER_OBJ */
