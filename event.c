/*
 * event.c
 * Martin Duquesnoy (150193@supinfo.com)
 */

#include "event.h"
#include "fshark.h"
#include "plane.h"
#include "game.h"

static void
event_quit(SDL_Event *ev)
{
     fs.flags ^= FS_RUNNING;
}

static void
event_mousemotion(SDL_Event *ev)
{
     /* Plane motion with mouse */
     plane_move(ev->motion.x, ev->motion.y);
}

static void
event_mousebuttondown(SDL_Event *ev)
{
     game_fire();
}

static void
event_keydown(SDL_Event *ev)
{
     if(ev->key.keysym.sym == SDLK_p)
          game_pause();
     else if(ev->key.keysym.sym == SDLK_q || ev->key.keysym.sym == SDLK_ESCAPE)
          fs.flags ^= FS_RUNNING;
     else if(ev->key.keysym.sym == SDLK_u)
          fs.flags ^= FS_HIDE_UI;
}

void
event_init(void)
{
     int i = SDL_NUMEVENTS;

     while(i--)
          fs.handlers[i] = NULL;

     fs.handlers[SDL_QUIT]            = event_quit;
     fs.handlers[SDL_MOUSEMOTION]     = event_mousemotion;
     fs.handlers[SDL_MOUSEBUTTONDOWN] = event_mousebuttondown;
     fs.handlers[SDL_KEYDOWN]         = event_keydown;
}

void
event_loop(void)
{
     SDL_Event ev;

     while(SDL_PollEvent(&ev))
     {
          if(fs.handlers[ev.type])
               fs.handlers[ev.type](&ev);
     }
}
