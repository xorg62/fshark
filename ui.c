/*
 * ui.c
 * Martin Duquesnoy (150193@supinfo.com)
 */

#include <string.h>

#include "ui.h"
#include "util.h"
#include "event.h"

void
ui_init(void)
{
     SLIST_INIT(&fs.console_msgs);
     ui_msg_new(0xFF00FF, "Welcome to fshark!");

     fs.teslaicon = IMG_Load("img/bolt.png");
     fs.menu = IMG_Load("img/menu.png");
     fs.goframe = IMG_Load("img/goframe.png"); /* Game over frame */

     fs.flags |= FS_BACK_MENU;
}

void
ui_free(void)
{
     SDL_FreeSurface(fs.teslaicon);
     SDL_FreeSurface(fs.menu);
     SDL_FreeSurface(fs.goframe);
}

struct console_msg*
ui_msg_new(int color, const char *format, ...)
{
     char *str;
     va_list a;
     struct console_msg *m = calloc(1, sizeof(struct console_msg));

     va_start(a, format);
     vasprintf(&str, format, a);
     va_end(a);

     m->msg = strdup(str);
     m->color = rgb_to_color(color);

     SLIST_INSERT_HEAD(&fs.console_msgs, m, next);

     free(str);

     printf("%s\n", m->msg);

     return m;
}

void
ui_msg_remove(struct console_msg *m)
{
     SLIST_REMOVE(&fs.console_msgs, m, console_msg, next);
     free(m->msg);
     free(m);
}

static void
ui_console(void)
{
     SDL_Color col = { .r = 0xFF, .g = 0xFF, .b = 0xFF };
     int c = 0, y = 395;
     struct console_msg *m;

     sdl_print_text(fs.root, fs.mfont, &col, 512, 380, "_____console_____");

     SLIST_FOREACH(m, &fs.console_msgs, next)
     {
          /* Only 4 last messages, free the remaining */
          if(c++ > 3)
          {
               ui_msg_remove(m);
               continue;
          }

          sdl_print_text(fs.root, fs.mfont, &m->color, 510, y, m->msg);

          y += 14;
     }
}

static void
ui_health_bar(void)
{
     int bcol = 0x00AA00;
     int shift_frame = (fs.flags & FS_STATE_DAMAGE ? 4 : 0 );
     int barw = (fs.health > 0 ? fs.health : 0);

     /*
      * Frame geo: make it bigger at damage moment,
      * use a shift previously set.
      */
     SDL_Rect rf =
     {
          .x = 220 - shift_frame,
          .y = 447 - shift_frame,
          .w = 204 + (shift_frame * 2),
          .h = 25  + (shift_frame * 2)
     };

     /* Progress bar geo */
     SDL_Rect hf =
     {
          .x = 222,
          .y = 449,
          .w = barw,
          .h = 21
     };

     /* Bar color, depend of health state */
     if(fs.health > 0 && fs.health <= 66)
          bcol = 0xDD0000;
     else if(fs.health > 66 && fs.health <= 133)
          bcol = 0xDDDD00;

     /*
      * Frame of the health bar "flick" at damage moment,
      * so we make it red filled.
      */
     if(fs.flags & FS_STATE_DAMAGE)
     {

          SDL_FillRect(fs.root, &rf, 0xAA0000);
          fs.flags ^= FS_STATE_DAMAGE;
     }
     else
          SDL_FillRect(fs.root, &rf, 0xFFFFFF);

     SDL_FillRect(fs.root, &hf, bcol);
}

static void
ui_tesla(void)
{
     SDL_Color col =  { .r = 0xFF, .g = 0xFF, .b = 0xFF };
     SDL_Rect r =
     {
          .x = 428,
          .y = 438
     };

     sdl_print_text(fs.root, fs.font, &col, 465, 450, "x%d", fs.ntesla);
     SDL_BlitSurface(fs.teslaicon, NULL, fs.root, &r);
}

void
ui_menu(void)
{
     SDL_Color col[2] = { rgb_to_color(0xBB3333), rgb_to_color(0x333333) };
     SDL_Rect r = { .x = 260 };

     SDL_ShowCursor(SDL_ENABLE);
     fs.flags |= FS_STATE_MENU;
     fs.chmenu = false;

     while(fs.flags & FS_STATE_MENU)
     {
          event_menu_loop();

          /* Background */
          SDL_BlitSurface(fs.menu, NULL, fs.root, NULL);

          /* Selector plane */
          r.y = (!fs.chmenu) ? 200 : 240;
          SDL_BlitSurface(fs.plane.s, NULL, fs.root, &r);

          sdl_print_text(fs.root, fs.font, &col[fs.chmenu], 300, 210, "PLAY");
          sdl_print_text(fs.root, fs.font, &col[!fs.chmenu], 300, 250, "QUIT");

          /* Console */
          ui_console();

          SDL_Flip(fs.root);
          SDL_Delay(TIMING * 2);
     }
}

void
ui_menu_choice(void)
{
     fs.flags &= ~FS_STATE_MENU;

     if(fs.chmenu)
          fs.flags &= ~(FS_RUNNING | FS_BACK_MENU);
     else
          fs.flags |= FS_RUNNING;

     Mix_PlayChannel(-1, fs.snd.pause, 0);
}

void
ui_menu_set_choice(bool choice)
{
     if(fs.chmenu == choice)
          return;

     fs.chmenu = choice;
     Mix_PlayChannel(-1, fs.snd.alert, 0);
}

void
ui_end(void)
{
     SDL_Color grey = rgb_to_color(0xDDDDDD);
     SDL_Color red  = rgb_to_color(0xBB0000);
     SDL_Rect r;

     r.x = 180;
     r.y = 100;

     SDL_BlitSurface(fs.goframe, NULL, fs.root, &r);

     sdl_print_text(fs.root, fs.font, &red, 270, 130, "GAME OVER!");
     sdl_print_text(fs.root, fs.font, &grey, 240, 220, "Score: %.5d", fs.score);
     sdl_print_text(fs.root, fs.font, &grey, 240, 250, "Accuracy: %.2f", fs.accuracy);
     sdl_print_text(fs.root, fs.font, &grey, 240, 280, "Total fire: %d", fs.nfire);

     SDL_Flip(fs.root);
}

void
ui_render(void)
{
     SDL_Color col = { .r = 0xFF, .g = 0xFF, .b = 0xFF }; /* white */

     /* Bottom infos */
     sdl_print_text(fs.root, fs.font, &col, 1, 450, "Accuracy: %.1f / 100", fs.accuracy);
     sdl_print_text(fs.root, fs.font, &col, 510, 450, "Score: %.5d", fs.score);

     /* Console */
     ui_console();

     /* Health bar */
     ui_health_bar();

     /* Tesla weapon count */
     ui_tesla();
}

