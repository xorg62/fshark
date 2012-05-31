/*
 * map.c
 * Martin Duquesnoy (150193@supinfo.com)
 */

#include "map.h"
#include "fshark.h"

void
map_init(void)
{
     fs.map.cropgeo.w = WIDTH;
     fs.map.cropgeo.h = HEIGHT;
     fs.map.geo.x = fs.map.geo.y = 0;
     fs.map.cropgeo.x = fs.map.cropgeo.y = 0;
     fs.map.s = IMG_Load("img/map.png");

     fs.map.cropgeo.y = fs.map.s->h - HEIGHT;
}

void
map_render(void)
{
     /* Scroll map surface, and come back to the bottom at the end */
     if((fs.map.cropgeo.y -= MAP_SPEED) <= 0)
          fs.map.cropgeo.y = fs.map.s->h - HEIGHT;

     SDL_BlitSurface(fs.map.s, &fs.map.cropgeo, fs.root, &fs.map.geo);
}

void
map_free(void)
{
     SDL_FreeSurface(fs.map.s);
}
