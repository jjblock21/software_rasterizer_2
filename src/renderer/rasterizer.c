#include "rasterizer.h"
#include "utils.h"

// Perform perspective division and map x and y to screen space
static void clip_space_to_viewport(framebuffer_t *fb, vec4 pos, ivec2 result) {
    result[0] = roundf((pos[0] / pos[3] + 1) * fb->width / 2);
    result[1] = roundf((pos[1] / pos[3] + 1) * fb->height / 2);
}

// Positive if winding order is clockwise
static float edge_function(ivec2 v0, ivec2 v1, ivec2 v2) {
    return ((v2[0] - v0[0]) * (v1[1] - v0[1]) -
            (v2[1] - v0[1]) * (v1[0] - v0[0]));
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

void draw_triangle(framebuffer_t *fb, vertex_t v0, ivec2 p0, vertex_t v1,
                   ivec2 p1, vertex_t v2, ivec2 p2) {
    // Only draw front-facing triangles
    float area = edge_function(p0, p1, p2);
    if (area < 0) return;

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
            if (ef01 < 0 || ef12 < 0 || ef20 < 0) continue;

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

void draw_mesh(framebuffer_t *fb, uniform_data_t *uniforms, mesh_t *mesh) {
    // Transform vertices to screen space
    ivec2 positions[mesh->vertex_count];
    for (int i = 0; i < mesh->vertex_count; i++) {
        float *pos = mesh->vertices[i].pos;
        int *screen_pos = positions[i];

        // Transform from model to clip space
        vec4 clip_pos;
        glm_mat4_mulv(uniforms->mvp, pos, clip_pos);

        // Transform from clip to screen space
        clip_space_to_viewport(fb, clip_pos, screen_pos);
    }

    for (int i = 0; i < mesh->index_count - 2; i += 3) {
        int i0 = mesh->indices[i];
        int i1 = mesh->indices[i + 1];
        int i2 = mesh->indices[i + 2];

        // Make sure the indices are within bounds
        if (i0 >= mesh->vertex_count || i1 >= mesh->vertex_count ||
            i2 >= mesh->vertex_count) {
            break;
        }

        draw_triangle(                         //
            fb,                                //
            mesh->vertices[i0], positions[i0], //
            mesh->vertices[i1], positions[i1], //
            mesh->vertices[i2], positions[i2]  //
        );
    }
}