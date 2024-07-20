#include "rasterizer.h"

#include "utils.h"

typedef struct {
    vec4 pos;
    color_t color;
} rvertex_t; // Result vertex

typedef struct {
    ivec2 pos;
    color_t color;
} svertex_t; // Screen vertex

static bool outside_viewbox(vec4 pos) {
    // clang-format off
    return -pos[3] > pos[0] || pos[0] > pos[3] 
        || -pos[3] > pos[1] || pos[1] > pos[3] 
        || -pos[3] > pos[2] || pos[2] > pos[3];
    // clang-format on
}

// Perform perspective division and map x and y to screen coordinates
static void clip_to_screen_coord(framebuffer_t *fb, vec4 pos, ivec2 result) {
    result[0] = (int)roundf((pos[0] / pos[3] + 1) * fb->width / 2);
    result[1] = (int)roundf((pos[1] / pos[3] + 1) * fb->height / 2);
}

static bool is_front_facing(svertex_t v0, svertex_t v1, svertex_t v2,
                            winding_order_t wo) {
    int sign = (v0.pos[0] - v1.pos[0]) * (v0.pos[1] - v2.pos[1]) -
               (v0.pos[0] - v2.pos[0]) * (v0.pos[1] - v1.pos[1]);
    return (wo == WINDING_ORDER_CC) ? sign < 0 : sign > 0;
}

static void draw_line(framebuffer_t *fb, svertex_t v0, svertex_t v1) {
    int dx = v1.pos[0] - v0.pos[0];
    int dy = v1.pos[1] - v0.pos[1];
    float x = v0.pos[0];
    float y = v0.pos[1];

    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    float incx = dx / (float)steps;
    float incy = dy / (float)steps;

    for (int i = 0; i <= steps; i++) {
        // Temporary vertex color test
        color_t color = lerp_color(v0.color, v1.color, (float)i / steps);

        set_pixel(fb, x, y, color);
        x += incx;
        y += incy;
    }
}

void draw_mesh(framebuffer_t *fb, uniform_data_t *data, mesh_t mesh,
               winding_order_t wo) {
    if (mesh.vertex_count <= 0 || mesh.index_count <= 0) {
        return;
    }

    // Don't shade vertices shared between edges multiple times
    rvertex_t results[mesh.vertex_count];
    for (int i = 0; i < mesh.vertex_count; i++) {
        vertex_t *vertex = mesh.vertices + i;
        rvertex_t *result = results + i;

        // This is equivalent to the vertex shader
        glm_mat4_mulv(data->mvp, vertex->pos, result->pos);
        result->color = vertex->color;
    }

    for (int i = 0; i < mesh.index_count - 2; i += 3) {
        int i0, i1, i2;       // Indices
        rvertex_t v0, v1, v2; // Result vertices
        svertex_t s0, s1, s2; // Screen vertices

        i0 = mesh.indices[i];
        i1 = mesh.indices[i + 1];
        i2 = mesh.indices[i + 2];

        // Make sure the indices are within bounds
        if (i0 >= mesh.vertex_count || i1 >= mesh.vertex_count ||
            i2 >= mesh.vertex_count) {
            break;
        }

        v0 = results[i0];
        v1 = results[i1];
        v2 = results[i2];

        // Don't render triangles that are not visible
        if (outside_viewbox(v0.pos) && outside_viewbox(v1.pos) &&
            outside_viewbox(v2.pos)) {
            continue;
        }

        clip_to_screen_coord(fb, v0.pos, s0.pos);
        clip_to_screen_coord(fb, v1.pos, s1.pos);
        clip_to_screen_coord(fb, v2.pos, s2.pos);

        // Only render front-facing triangles
        if (!is_front_facing(s0, s1, s2, wo)) {
            continue;
        }

        // Copy the rest of the data
        s0.color = v0.color;
        s1.color = v1.color;
        s2.color = v2.color;

        // Draw wireframe for now
        draw_line(fb, s0, s1);
        draw_line(fb, s1, s2);
        draw_line(fb, s2, s0);
    }
}