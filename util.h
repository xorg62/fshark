/*
 * util.h
 * Martin Duquesnoy (150193@supinfo.com)
 */

#ifndef UTIL_H
#define UTIL_H

#include "fshark.h"

#define RAND(m, M)        ((rand() % ((M) - (m) + 1)) + (m))
#define TIMER_IS_DONE(t)  (!t)

static inline void
timer_process(Timer *t)
{
     if(*t > 0)
          --(*t);
}

static inline SDL_Color
rgb_to_color(int color)
{
     SDL_Color c;

     c.r = (color >> 16) & 0xFF;
     c.g = (color >> 8)  & 0xFF;
     c.b =  color        & 0xFF;

     return c;
}

void sdl_print_text(SDL_Surface *s, TTF_Font *font, SDL_Color *color, int x, int y, const char *format, ...);
SDL_Color rgb_to_color(int color);
int accuracy_process(void);
void check_row_award(void);

#endif /* UTIL_H */
