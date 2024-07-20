#include "rasterizer.h"

#include <math.h>

#define COLOR                                                                            \
    (color_t) { 255, 255, 255, 255 }

void clear(framebuffer_t fb, color_t color) {
    for (int i = 0; i < fb.width * fb.height; i++) {
        fb.pixels[i] = color;
    }
}

void set_pixel(framebuffer_t fb, int x, int y, color_t color) {
    if (x < 0 || y < 0 || x >= fb.width || y >= fb.height) return;

    fb.pixels[y * fb.width + x] = color;
}

static bool not_in_viewbox(vec4 pos) {
    // clang-format off
    return -pos[3] > pos[0] || pos[0] > pos[3] 
        || -pos[3] > pos[1] || pos[1] > pos[3] 
        || -pos[3] > pos[2] || pos[2] > pos[3];
    // clang-format on
}

static void clip_to_screen_coord(vec4 clip, framebuffer_t fb, int *x, int *y) {
    *x = (int)((clip[0] / clip[3] + 1) * fb.width / 2);
    *y = (int)((clip[1] / clip[3] + 1) * fb.height / 2);
}

static void draw_line(framebuffer_t fb, int x0, int y0, int x1, int y1, color_t color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    float incx = dx / (float)steps;
    float incy = dy / (float)steps;

    float x = x0;
    float y = y0;
    for (int i = 0; i <= steps; i++) {
        set_pixel(fb, x, y, color);
        x += incx;
        y += incy;
    }
}

static void swapi(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

static void draw_scanline(framebuffer_t fb, int x0, int x1, int y, color_t color) {
    if (x0 > x1) swapi(&x0, &x1);

    for (int x = x0; x <= x1; x++) {
        set_pixel(fb, x, y, color);
    }
}

static void fill_triangle_flat_bottom(
    framebuffer_t fb, int x0, int y0, int x1, int y1, int x2, int y2, color_t color
) {
    float invslope0 = (float)(x1 - x0) / (y1 - y0);
    float invslope1 = (float)(x2 - x0) / (y2 - y0);
    float curx0 = x0;
    float curx1 = x0;

    for (int y = y0; y <= y1; y++) {
        draw_scanline(fb, (int)curx0, (int)curx1, y, color);
        curx0 += invslope0;
        curx1 += invslope1;
    }
}

static void fill_triangle_flat_top(
    framebuffer_t fb, int x0, int y0, int x1, int y1, int x2, int y2, color_t color
) {
    float invslope0 = (float)(x2 - x0) / (y2 - y0);
    float invslope1 = (float)(x2 - x1) / (y2 - y1);
    float curx0 = x2;
    float curx1 = x2;

    for (int y = y2; y > y0; y--) {
        draw_scanline(fb, (int)curx0, (int)curx1, y, color);
        curx0 -= invslope0;
        curx1 -= invslope1;
    }
}

static void fill_triangle(
    framebuffer_t fb, int x0, int y0, int x1, int y1, int x2, int y2, color_t color
) {
    // Sort vertices by Y
    if (y0 > y1) {
        swapi(&x0, &x1);
        swapi(&y0, &y1);
    }
    if (y0 > y2) {
        swapi(&x0, &x2);
        swapi(&y0, &y2);
    }
    if (y1 > y2) {
        swapi(&x1, &x2);
        swapi(&y1, &y2);
    }

    // Draw simple cases
    if (y1 == y2) {
        fill_triangle_flat_bottom(fb, x0, y0, x1, y1, x2, y2, color);
    } else if (y0 == y1) {
        fill_triangle_flat_top(fb, x0, y0, x1, y1, x2, y2, color);
    } else {
        int x3 = (int)(x0 + ((float)(y1 - y0) / (float)(y2 - y0)) * (x2 - x0));
        fill_triangle_flat_bottom(fb, x0, y0, x1, y1, x3, y1, color);
        fill_triangle_flat_top(fb, x1, y1, x3, y1, x2, y2, color);
    }
}

void draw_mesh(framebuffer_t fb, mesh_t mesh, mat4 mvp) {
    vec4 positions[mesh.vertex_count];

    // Don't shade vertices multiple times
    for (int i = 0; i < mesh.vertex_count; i++) {
        glm_mat4_mulv(mvp, mesh.vertices[i].pos, positions[i]);
    }

    for (int i = 0; i < mesh.index_count - 2; i += 3) {
        float *pos0 = positions[mesh.indices[i]];
        float *pos1 = positions[mesh.indices[i + 1]];
        float *pos2 = positions[mesh.indices[i + 2]];

        if (not_in_viewbox(pos0) && not_in_viewbox(pos1) && not_in_viewbox(pos2)) {
            continue;
        }

        int x0, y0, x1, y1, x2, y2;
        clip_to_screen_coord(pos0, fb, &x0, &y0);
        clip_to_screen_coord(pos1, fb, &x1, &y1);
        clip_to_screen_coord(pos2, fb, &x2, &y2);

        // Draw wireframe for now
        // draw_line(fb, x0, y0, x1, y1, COLOR);
        // draw_line(fb, x1, y1, x2, y2, COLOR);
        // draw_line(fb, x2, y2, x0, y0, COLOR);
        fill_triangle(fb, x0, y0, x1, y1, x2, y2, COLOR);
    }
}

// Todo: Rewrite with structs so it will be easier to extend with more data later and
// improve triangle rasterization algorithm.
