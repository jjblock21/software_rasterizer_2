#include "rasterizer.h"
#include "utils.h"

typedef struct {
    vec3 color;
} rvertex_t; // Result vertex

// Perform perspective division and map x and y to screen space
static void clip_to_screen_space(framebuffer_t *fb, vec4 pos, ivec2 result) {
    result[0] = roundf((pos[0] / pos[3] + 1) * fb->width / 2);
    result[1] = roundf((pos[1] / pos[3] + 1) * fb->height / 2);
}

// Positive if winding order is clockwise
static float edge_function(ivec2 v0, ivec2 v1, ivec2 v2) {
    return ((v0[0] - v1[0]) * (v0[1] - v2[1]) -
            (v0[0] - v2[0]) * (v0[1] - v1[1]));
}

static void draw_line(framebuffer_t *fb, int x0, int y0, int x1, int y1,
                      color_t color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    float x = x0;
    float y = y0;

    int steps = maxi(abs(dx), abs(dy));
    float incx = dx / (float)steps;
    float incy = dy / (float)steps;

    for (int i = 0; i <= steps; i++) {
        set_pixel(fb, x, y, color);

        x += incx;
        y += incy;
    }
}

static void draw_triangle(framebuffer_t *fb, rvertex_t v0, ivec2 p0,
                          rvertex_t v1, ivec2 p1, rvertex_t v2, ivec2 p2) {
    // Only draw front-facing triangles
    float area = edge_function(p0, p1, p2);
    if (area > 0) return;

    // Find triangle bounding box
    int left = mini3(p0[0], p1[0], p2[0]);
    int top = mini3(p0[1], p1[1], p2[1]);
    int right = maxi3(p0[0], p1[0], p2[0]);
    int bottom = maxi3(p0[1], p1[1], p2[1]);

    for (int y = top; y < bottom; y++) {
        for (int x = left; x < right; x++) {
            ivec2 pos = {x, y};

            // Test if point is in triangle
            float ef01 = edge_function(p0, p1, pos);
            float ef12 = edge_function(p1, p2, pos);
            float ef20 = edge_function(p2, p0, pos);

            if (ef01 > 0 || ef12 > 0 || ef20 > 0) continue;

            // Calculate barycentric coordinates for interpolation
            float b0 = ef12 / area;
            float b1 = ef20 / area;
            float b2 = ef01 / area;

            // Interpolate vertex colors
            float r = v0.color[0] * b0 + v1.color[0] * b1 + v2.color[0] * b2;
            float g = v0.color[1] * b0 + v1.color[1] * b1 + v2.color[1] * b2;
            float b = v0.color[2] * b0 + v1.color[2] * b1 + v2.color[2] * b2;
            char cr = (char)(clampf(r, 0, 1) * 255);
            char cg = (char)(clampf(g, 0, 1) * 255);
            char cb = (char)(clampf(b, 0, 1) * 255);

            set_pixel(fb, x, y, (color_t){cr, cg, cb, 255});
        }
    }
}

void draw_mesh(framebuffer_t *fb, const uniform_data_t *uniforms,
               const mesh_t *mesh) {
    if (mesh->vertex_count <= 0 || mesh->index_count <= 0) {
        return;
    }

    rvertex_t results[mesh->vertex_count];
    vec4 positions[mesh->vertex_count];

    // Don't shade vertices shared between edges multiple times
    for (int i = 0; i < mesh->vertex_count; i++) {
        vertex_t *in = mesh->vertices + i;
        rvertex_t *out = results + i;
        float *pos = positions[i];

        // This is equivalent to the vertex shader
        glm_mat4_mulv(uniforms->mvp, in->pos, pos);
        glm_vec3_copy(in->color, out->color);
    }

    for (int i = 0; i < mesh->index_count - 2; i += 3) {
        int i0 = mesh->indices[i], i1 = mesh->indices[i + 1],
            i2 = mesh->indices[i + 2];

        // Make sure the indices are within bounds
        if (i0 >= mesh->vertex_count || i1 >= mesh->vertex_count ||
            i2 >= mesh->vertex_count) {
            break;
        }

        // Vertex shader results
        rvertex_t v0 = results[i0], v1 = results[i1], v2 = results[i2];
        float *p0 = positions[i0], *p1 = positions[i1], *p2 = positions[i2];

        ivec2 s0, s1, s2; // Screen positions
        clip_to_screen_space(fb, p0, s0);
        clip_to_screen_space(fb, p1, s1);
        clip_to_screen_space(fb, p2, s2);

        draw_triangle(fb, v0, s0, v1, s1, v2, s2);
    }
}