#ifndef __SRC_RENDERER_RASTERIZER_H__
#define __SRC_RENDERER_RASTERIZER_H__

#include <cglm/cglm.h>
#include "framebuffer.h"

typedef struct {
    mat4 mvp;
} uniform_data_t;

typedef struct {
    vec4 pos;
    rgba32_t color;
} vertex_t;

typedef struct {
    vertex_t *vertices;
    unsigned short *indices;
    int vertex_count;
    int index_count;
} mesh_t;

// Shorten the syntax for creating vertices
#define make_vertex(x, y, z, r, g, b)                                          \
    (vertex_t) { .pos = {x, y, z, 1}, .color = (rgba32_t){r, g, b, 255}, }

// Draw a mesh to the given framebuffer
void draw_mesh(framebuffer_t *fb, const uniform_data_t *uniforms,
               const mesh_t *mesh);

#endif /* __SRC_RENDERER_RASTERIZER_H__ */
