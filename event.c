/*
 * event.c
 * Martin Duquesnoy (150193@supinfo.com)
 */

#include "event.h"
#include "fshark.h"
#include "plane.h"
#include "game.h"
#include "ui.h"

static void
event_quit(SDL_Event *ev)
{
     fs.flags &= ~(FS_RUNNING | FS_STATE_MENU | FS_BACK_MENU);
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
     else if(ev->key.keysym.sym == SDLK_ESCAPE)
     {
          fs.flags &= ~FS_RUNNING;
          fs.flags |= FS_STATE_MENU;
     }
     else if(ev->key.keysym.sym == SDLK_u)
          fs.flags ^= FS_HIDE_UI;
     else if(ev->key.keysym.sym == SDLK_w)
          game_tesla_weapon();
}

static void
event_menu_mousebuttondown(SDL_Event *ev)
{
     ui_menu_choice();
}

static void
event_menu_mousemotion(SDL_Event *ev)
{
     ui_menu_set_choice((ev->motion.y > 230));
}

static void
event_menu_keydown(SDL_Event *ev)
{
     if(ev->key.keysym.sym == SDLK_q || ev->key.keysym.sym == SDLK_ESCAPE)
          fs.flags &= ~(FS_RUNNING | FS_STATE_MENU | FS_BACK_MENU);

     if(ev->key.keysym.sym == SDLK_UP || ev->key.keysym.sym == SDLK_DOWN)
          ui_menu_set_choice(!fs.chmenu);

     if(ev->key.keysym.sym == SDLK_RETURN)
          ui_menu_choice();
}

void
event_init(void)
{
     int i = SDL_NUMEVENTS;

     /* Clean handlers arrays */
     while(i--)
     {
          fs.handlers[i] = NULL;
          fs.menu_handlers[i] = NULL;
     }

     /* Game handlers */
     fs.handlers[SDL_QUIT]            = event_quit;
     fs.handlers[SDL_MOUSEMOTION]     = event_mousemotion;
     fs.handlers[SDL_MOUSEBUTTONDOWN] = event_mousebuttondown;
     fs.handlers[SDL_KEYDOWN]         = event_keydown;

     /* Menu handlers */
     fs.menu_handlers[SDL_QUIT]            = event_quit;
     fs.menu_handlers[SDL_MOUSEMOTION]     = event_menu_mousemotion;
     fs.menu_handlers[SDL_MOUSEBUTTONDOWN] = event_menu_mousebuttondown;
     fs.menu_handlers[SDL_KEYDOWN]         = event_menu_keydown;
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

void
event_menu_loop(void)
{
     SDL_Event ev;

     while(SDL_PollEvent(&ev))
     {
          if(fs.menu_handlers[ev.type])
               fs.menu_handlers[ev.type](&ev);
     }
}
