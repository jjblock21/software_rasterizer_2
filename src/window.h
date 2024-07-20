#ifndef __SRC_WINDOW_H__
#define __SRC_WINDOW_H__

#include <stdbool.h>
#include "color.h"

void init_window(int width, int height, const char *title, bool resizable,
                 void (*resize_callback)(int, int));

color_t *lock_surface();
void unlock_surface();

void destroy_window();

bool is_window_open();
int get_window_width();
int get_window_height();

void poll_events();
void print_sdl_error();

#endif /* __SRC_WINDOW_H__ */
