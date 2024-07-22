#ifndef __SRC_WINDOW_H__
#define __SRC_WINDOW_H__

#include <stdbool.h>
#include <stdint.h>
#include "renderer/utils.h"

void init_window(int width, int height, const char *title, bool resizable,
                 void (*resize_callback)(int, int));
void destroy_window();

void *lock_surface(int *pitch);
void unlock_surface();

bool is_window_open();
int get_window_width();
int get_window_height();
float get_window_aspect_ratio();

void poll_events();
void sleep(float seconds);
void print_sdl_error();

#endif /* __SRC_WINDOW_H__ */
