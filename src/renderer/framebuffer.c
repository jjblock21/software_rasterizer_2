#include "framebuffer.h"

#include <stdlib.h>
#include <string.h>

void clear(framebuffer_t *fb, rgba32_t color) {
    if (!fb->pixels) return;

    for (int i = 0; i < fb->width * fb->height; i++) {
        fb->pixels[i] = color;
    }
}

void set_pixel(framebuffer_t *fb, int x, int y, rgba32_t color) {
    if (fb->pixels && x >= 0 && y >= 0 && x < fb->width && y < fb->height) {
        fb->pixels[y * fb->width + x] = color;
    }
}