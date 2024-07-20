#ifndef __SRC_RASTERIZER_H__
#define __SRC_RASTERIZER_H__

#include <cglm/cglm.h>
#include "color.h"

typedef struct {
    int width, height;
    color_t *pixels;
} framebuffer_t;

typedef struct {
    vec4 pos;
} vertex_t;

typedef struct {
    vertex_t *vertices;
    unsigned short *indices;
    int vertex_count;
    int index_count;
} mesh_t;

void clear(framebuffer_t fb, color_t color);
void set_pixel(framebuffer_t fb, int x, int y, color_t color);

void draw_mesh(framebuffer_t fb, mesh_t mesh, mat4 mvp);

#endif /* __SRC_RASTERIZER_H__ */
