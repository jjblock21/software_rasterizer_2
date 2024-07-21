#ifndef __SRC_RENDERER_RASTERIZER2_H__
#define __SRC_RENDERER_RASTERIZER2_H__

#include <cglm/cglm.h>
#include "framebuffer.h"

typedef enum {
    NO_BACKFACE_CULLING = 1 << 0,
    DRAW_WIREFRAME = 1 << 1,
} flags_t;

typedef struct {
    mat4 mvp;
} uniform_data_t;

typedef struct {
    vec4 pos;
    color_t color;
} vertex_t;

typedef struct {
    vertex_t *vertices;
    unsigned short *indices;
    int vertex_count;
    int index_count;
} mesh_t;

// Shorten the syntax for creating vertices
#define make_vertex(x, y, z, r, g, b)                                          \
    (vertex_t) { .pos = {x, y, z, 1}, .color = (color_t){r, g, b, 255}, }

// Draw a mesh to the given framebuffer
void draw_mesh(framebuffer_t *fb, const uniform_data_t *uniforms,
               const mesh_t *mesh, const flags_t flags);

#endif /* __SRC_RENDERER_RASTERIZER2_H__ */
