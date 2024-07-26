#ifndef __SRC_RENDERER_FRAMEBUFFER_H__
#define __SRC_RENDERER_FRAMEBUFFER_H__

#include "utils.h"

typedef struct {
    int width, height;
    color_t *pixels;
} framebuffer_t;

// Overwrite pixel buffer with color
void clear(framebuffer_t fb, color_t color);
void set_pixel(framebuffer_t fb, int x, int y, color_t color);

#endif /* __SRC_RENDERER_FRAMEBUFFER_H__ */
