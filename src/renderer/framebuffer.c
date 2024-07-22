#include "framebuffer.h"

#include <stdlib.h>
#include <string.h>

void fb_alloc(framebuffer_t *fb, int width, int height) {
    if (fb->pixels) fb_free(fb);

    fb->width = width;
    fb->height = height;
    fb->pixels = (vec4 *)calloc(width * height, sizeof(vec4));
}

void fb_free(framebuffer_t *fb) {
    if (fb->pixels) {
        free(fb->pixels);
        fb->pixels = NULL;
    }
}

void fb_copy_rgba32(framebuffer_t *fb, void *dest, int width, int height,
                    int pitch) {
    int w = mini(width, fb->width);
    int h = mini(height, fb->height);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float *sp = fb->pixels[y * fb->width + x];
            char *dp = dest + (y * pitch + x * 4);

            dp[0] = (int)(clampf(sp[0], 0, 1) * 255);
            dp[1] = (int)(clampf(sp[1], 0, 1) * 255);
            dp[2] = (int)(clampf(sp[2], 0, 1) * 255);
        }
    }
}

void fb_clear(framebuffer_t *fb, vec4 color) {
    if (!fb->pixels) return;

    for (int i = 0; i < fb->width * fb->height; i++) {
        glm_vec4_copy(color, fb->pixels[i]);
    }
}

void fb_set_pixel(framebuffer_t *fb, int x, int y, vec4 color) {
    if (x >= 0 && y >= 0 && x < fb->width && y < fb->height) {
        // This is a little slow (~.5ms)
        glm_vec4_copy(color, fb->pixels[y * fb->width + x]);
    }
}