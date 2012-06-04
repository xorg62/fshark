/*
 * fshark.h
 * Martin Duquesnoy (150193@supinfo.com)
 */

#ifndef FSHARK_H
#define FSHARK_H

/* Std lib */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/queue.h>

/* SDL lib */
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_keysym.h>
#include <SDL/SDL_mixer.h>

/* Consts */
#define WIDTH  (640)
#define HEIGHT (480)

#define DEPTH  (0)
#define TIMING (25)

typedef unsigned int Flags;
typedef int Timer;

struct fshark
{
#define FS_RUNNING      0x01
#define FS_STATE_DAMAGE 0x02
#define FS_PAUSE        0x04
#define FS_HIDE_UI      0x08
#define FS_STATE_TESLA  0x10
     Flags flags;

     /* Root surface, where everything is blitted */
     SDL_Surface *root, *teslaicon;
     TTF_Font *font, *mfont;

     /* Game values */
     int score, nfire, row, health, ntesla;
     float accuracy;

     /* Array of event handle functions to associate with event type */
     void (*handlers[SDL_NUMEVENTS])(SDL_Event *ev);

     /* Loaded sound */
     struct
     {
          Mix_Chunk *impressive;
          Mix_Chunk *alert;
          Mix_Chunk *pause;
          Mix_Chunk *beam;
          Mix_Chunk *tesla;
     } snd;

     struct
     {
          SDL_Rect geo;
          SDL_Surface *s, *sdamage, *beam, *trail, *tesla;
          Timer beam_timer, collision_timer;
     } plane;

     struct
     {
          SDL_Rect geo, cropgeo;
          SDL_Surface *s;
          int scroll;
     } map;

     struct
     {
          SDL_Surface *s, *sboom, *srocket, *sfighter;
          SDL_Surface *sufo;
          Timer release_timer, fighter_timer;
     } enemy;

     STAILQ_HEAD(_render_obj, render_obj) render_objs;
     SLIST_HEAD(_console_msg, console_msg) console_msgs;
} fs;

#endif /* FSHARK_H */
