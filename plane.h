/*
 * plane.h
 * Martin Duquesnoy (150193@supinfo.com)
 */

#ifndef _PLANE_H
#define _PLANE_H

#include "fshark.h"

/* Loaded surface dimensions */
#define PLANE_H (32)
#define PLANE_W (32)

#define BEAM_W (6)
#define BEAM_H (12)

#define TRAIL_W (2)
#define TRAIL_H (6)

#define TRAIL_LENGTH (8)

void plane_init(void);
void plane_free(void);
void plane_move(int x, int y);
void plane_process_render(void);
void plane_render_beam(struct render_obj *enemy);

#endif /* PLANE_H */
