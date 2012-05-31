/*
 * util.c
 * Martin Duquesnoy (150193@supinfo.com)
 */

#include "util.h"

void
sdl_print_text(SDL_Surface *s, TTF_Font *font, SDL_Color *color, int x, int y, const char *format, ...)
{
     SDL_Rect r = { x, y };
     SDL_Surface *surface;
     char *str;
     va_list a;

     va_start(a, format);
     vasprintf(&str, format, a);
     va_end(a);

     if(!(surface = TTF_RenderText_Blended(font, str, *color)))
     {
          SDL_FreeSurface(surface);
          free(str);
          fprintf(stderr, "TTF_RenderText_Solid: Can't be blitted\n");
          return;
     }

     SDL_BlitSurface(surface, NULL, s, &r);
     SDL_FreeSurface(surface);

     free(str);
}
