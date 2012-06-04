/*
 * fshark.c
 * Martin Duquesnoy (150193@supinfo.com)
 */

#include "fshark.h"
#include "event.h"
#include "map.h"
#include "plane.h"
#include "render_obj.h"
#include "enemy.h"
#include "ui.h"
#include "util.h"
#include "game.h"

static void
sdl_init(void)
{
     /* Video/Audio init */
     if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
     {
          fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
          exit(EXIT_FAILURE);
     }

     if(Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096))
          fprintf(stderr, "Mix_OpenAudio: %s\n", SDL_GetError());

     if(TTF_Init() < 0)
     {
          fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
          exit(EXIT_FAILURE);
     }

     if(!(fs.root = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE | SDL_DOUBLEBUF)))
     {
          fprintf(stderr, "SDL_SetVideoMode: %s\n", SDL_GetError());
          exit(EXIT_FAILURE);
     }

     SDL_ShowCursor(SDL_DISABLE);

     /* Load media */
     fs.font = TTF_OpenFont("font/font.ttf", 20);
     fs.mfont = TTF_OpenFont("font/font.ttf", 12);

     fs.snd.impressive = Mix_LoadWAV("sound/impressive.wav");
     fs.snd.alert = Mix_LoadWAV("sound/alert.wav");
     fs.snd.pause = Mix_LoadWAV("sound/pause.wav");
     fs.snd.beam  = Mix_LoadWAV("sound/beam.wav");
     fs.snd.tesla = Mix_LoadWAV("sound/tesla.wav");
}

static void
sdl_quit(void)
{
     TTF_CloseFont(fs.font);
     TTF_CloseFont(fs.mfont);
     TTF_Quit();

     Mix_FreeChunk(fs.snd.impressive);
     Mix_FreeChunk(fs.snd.alert);
     Mix_FreeChunk(fs.snd.pause);
     Mix_FreeChunk(fs.snd.beam);
     Mix_FreeChunk(fs.snd.tesla);
     Mix_CloseAudio();

     SDL_Quit();
}

static void
run_loop(void)
{
     while(fs.flags & FS_RUNNING)
     {
          event_loop();
          game_process();

          SDL_Flip(fs.root);
          SDL_Delay(TIMING);
     }
}

int
main(int argc, char **argv)
{
     fs.flags |= FS_RUNNING;

     sdl_init();
     render_obj_init();
     game_init();
     map_init();
     plane_init();
     enemy_init();
     event_init();
     ui_init();

     run_loop();

     plane_free();
     map_free();
     enemy_free();
     ui_free();
     render_obj_flush_list();

     sdl_quit();

     return 0;
}
