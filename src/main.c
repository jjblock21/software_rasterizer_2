#include <stdio.h>
#include <time.h>

#define SDL_MAIN_HANDLED
#include <sdl/SDL.h>

#include "renderer/rasterizer.h"
#include "window.h"

// clang-format off
vertex_t plane_vertices[8] = {
    VERTEX(-.5, -.5, -.5, 255,   0,   0),
    VERTEX( .5, -.5, -.5, 255, 255,   0),
    VERTEX( .5,  .5, -.5,   0, 255,   0),
    VERTEX(-.5,  .5, -.5,   0, 255, 255),
    VERTEX(-.5, -.5,  .5,   0,   0, 255),
    VERTEX( .5, -.5,  .5, 255,   0, 255),
    VERTEX( .5,  .5,  .5, 128, 255, 255),
    VERTEX(-.5,  .5,  .5, 255, 128, 128),
};
// clang-format on

unsigned short plane_indices[36] = {
    0, 1, 3, 3, 1, 2, //
    1, 5, 2, 2, 5, 6, //
    5, 4, 6, 6, 4, 7, //
    4, 0, 7, 7, 0, 3, //
    3, 2, 7, 7, 2, 6, //
    4, 5, 0, 0, 5, 1, //
};

struct state {
    framebuffer_t fb;
    mat4 vp_mat;
} state;

static float get_ms_since(clock_t start) {
    clock_t current = clock();
    return (float)(current - start) / CLOCKS_PER_SEC * 1000;
}

static void create_viewproj_mat(float aspect, mat4 result) {
    mat4 view, proj;
    glm_lookat((vec3){0, 0, -3}, GLM_VEC3_ZERO, GLM_YUP, view);
    glm_perspective(glm_rad(40), aspect, .1, 100, proj);

    glm_mul(proj, view, result);
}

static void window_resized(int width, int height) {
    state.fb.width = width;
    state.fb.height = height;
    create_viewproj_mat((float)width / height, state.vp_mat);
}

int main() {
    init_window(800, 600, "Software Renderer", true, window_resized);
    int width = get_window_width();
    int height = get_window_height();

    state.fb = (framebuffer_t){
        .width = width,
        .height = height,
        .pixels = NULL, // Uses buffer provided by window
    };

    uniform_data_t uniforms;
    mesh_t mesh = {
        .vertices = plane_vertices,
        .indices = plane_indices,
        .vertex_count = 8,
        .index_count = 36,
    };

    float angle = 0;
    create_viewproj_mat((float)width / height, state.vp_mat);

    int frames = 0;
    clock_t start = clock();

    while (is_window_open()) {
        poll_events();

        // Calculate mvp matrix
        mat4 model;
        glm_euler((vec3){glm_rad(angle / 3), glm_rad(angle), 0}, model);
        glm_mul(state.vp_mat, model, uniforms.mvp);

        state.fb.pixels = lock_surface();
        clear(&state.fb, (color_t){0, 0, 0, 255});

        draw_mesh(&state.fb, &uniforms, mesh, 0);
        unlock_surface();

        angle += .05;

        // Print avg. frame time and fps over 200 frames
        if (++frames >= 200) {
            float avg = get_ms_since(start) / 200;
            int fps = roundf(1000 / avg);
            printf("Frame Time: %gms, FPS: %d\n", avg, fps);

            frames = 0;
            start = clock();
        }
    }

    destroy_window();
    return 0;
}