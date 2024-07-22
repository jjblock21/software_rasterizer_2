#ifndef __SRC_RENDERER_FRAMEBUFFER_H__
#define __SRC_RENDERER_FRAMEBUFFER_H__

#include "utils.h"

typedef struct {
    int width, height;
    vec4 *pixels;
} framebuffer_t;

// Allocate or resize a framebuffer
void fb_alloc(framebuffer_t *fb, int width, int height);
void fb_free(framebuffer_t *fb);

// Converts the pixel data of the framebuffer to rgba32 and copies it into the
// specified destination buffer
// \param pitch The length of one row in the destination buffer in bytes
void fb_copy_rgba32(framebuffer_t *fb, void *dest, int width, int height,
                    int pitch);

void fb_clear(framebuffer_t *fb, vec4 color);
void fb_set_pixel(framebuffer_t *fb, int x, int y, vec4 color);

#endif /* __SRC_RENDERER_FRAMEBUFFER_H__ */
