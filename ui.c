/*
 * ui.c
 * Martin Duquesnoy (150193@supinfo.com)
 */

#include <string.h>

#include "ui.h"
#include "util.h"

void
ui_init(void)
{
     SLIST_INIT(&fs.console_msgs);
     ui_msg_new(0xFF00FF, "Welcome to fshark!");
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
     int barw = (fs.health > 0 ? fs.health * 2 : 0);

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
     if(fs.health > 0 && fs.health <= 33)
          bcol = 0xDD0000;
     else if(fs.health > 33 && fs.health <= 66)
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
}

