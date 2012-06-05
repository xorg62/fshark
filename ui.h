/*
 * ui.h
 * Martin Duquesnoy (150193@supinfo.com)
 */

#ifndef UI_H
#define UI_H

#include "fshark.h"

struct console_msg
{
     char *msg;
     SDL_Color color;
     SLIST_ENTRY(console_msg) next;
};

void ui_init(void);
void ui_free(void);
void ui_render(void);
struct console_msg *ui_msg_new(int color, const char *format, ...);
void ui_msg_remove(struct console_msg *m);
void ui_menu(void);
void ui_menu_choice(void);
void ui_menu_set_choice(bool choice);

#endif /* UI_H */

