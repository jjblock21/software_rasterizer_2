#ifndef __SRC_RENDERER_RASTERIZER2_H__
#define __SRC_RENDERER_RASTERIZER2_H__

#include <cglm/cglm.h>
#include "framebuffer.h"

// Shorter syntax for creating vertices
#define new_vertex(x, y, z, r, g, b)                                           \
    (vertex_t) { .pos = {x, y, z, 1}, .color = {r, g, b}, }

// Vertex shader input
typedef struct {
    vec4 pos;
    // Renderer does not support blending so no alpha channel is needed
    vec3 color;
} vertex_t;

// Vertex shader output / pixel shader input
typedef struct {
    vec3 color;
} varying_t;

typedef struct {
    vertex_t *vertices;
    uint16_t *indices;
    int vertex_count;
    int index_count;
} mesh_t;

typedef struct {
    // Equivalent to the vertex shader
    varying_t (*vertex_main)(vertex_t in, void *userdata, vec4 pos);
    // Interpolate between three varyings
    varying_t (*interp_func)(varying_t v0, float w0, varying_t v1, float w1,
                             varying_t v2, float w2);
    // Equivalent to the fragment shader
    void (*pixel_main)(varying_t in, void *userdata, vec3 color);
} shader_t;

void draw_mesh(framebuffer_t fb, shader_t shader, mesh_t mesh, void *userdata);

#endif /* __SRC_RENDERER_RASTERIZER2_H__ */
