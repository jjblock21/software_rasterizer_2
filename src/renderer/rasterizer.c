#include "rasterizer.h"
#include "utils.h"

typedef struct {
    vec3 color;
} finput_t; // Fragment shader input

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

static finput_t interp_vertices(vertex_t v0, float w0, vertex_t v1, float w1,
                                vertex_t v2, float w2) {
    finput_t result;
    result.color[0] = v0.color[0] * w0 + v1.color[0] * w1 + v2.color[0] * w2;
    result.color[1] = v0.color[1] * w0 + v1.color[1] * w1 + v2.color[1] * w2;
    result.color[2] = v0.color[2] * w0 + v1.color[2] * w1 + v2.color[2] * w2;
    return result;
}

static color_t shade_fragment(finput_t input, uniform_data_t *uniforms) {
    return (color_t){
        .r = (char)(clampf(input.color[0], 0, 1) * 255),
        .g = (char)(clampf(input.color[1], 0, 1) * 255),
        .b = (char)(clampf(input.color[2], 0, 1) * 255),
        .a = 255,
    };
}

static void draw_triangle(framebuffer_t *fb, vertex_t v0, ivec2 p0, vertex_t v1,
                          ivec2 p1, vertex_t v2, ivec2 p2,
                          uniform_data_t *uniforms) {
    // Only draw front-facing (clockwise winding order) triangles
    float area = edge_function(p0, p1, p2);
    if (area < 0) return;

    // Find triangle bounding box
    int minX = clampi(mini3(p0[0], p1[0], p2[0]), 0, fb->width);
    int minY = clampi(mini3(p0[1], p1[1], p2[1]), 0, fb->height);
    int maxX = clampi(maxi3(p0[0], p1[0], p2[0]), 0, fb->width);
    int maxY = clampi(maxi3(p0[1], p1[1], p2[1]), 0, fb->height);

    for (int y = minY; y < maxY; y++) {
        for (int x = minX; x < maxX; x++) {
            ivec2 pos = {x, y};

            // Test if point is in triangle
            float ef01 = edge_function(p0, p1, pos);
            if (ef01 < 0) continue;
            float ef12 = edge_function(p1, p2, pos);
            if (ef12 < 0) continue;
            float ef20 = edge_function(p2, p0, pos);
            if (ef20 < 0) continue;

            // Interpolate the data of the triangle vertices
            finput_t fi = interp_vertices(v0, ef12 / area, v1, ef20 / area, v2,
                                          ef01 / area);

            set_pixel(fb, x, y, shade_fragment(fi, uniforms));
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
            mesh->vertices[i2], positions[i2], //
            uniforms                           //
        );
    }
}