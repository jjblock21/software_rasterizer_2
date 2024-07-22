#include <stdio.h>
#include <time.h>
#define SDL_MAIN_HANDLED
#include "window.h"
#include "renderer/rasterizer.h"

struct app_state {
    framebuffer_t fb;
    mesh_t mesh;
    uniform_data_t uniforms;
    mat4 view, proj, vp;
    vec3 rotation;
} state = {0};

vertex_t mesh_vertices[8] = {
    make_vertex(-.5, -.5, -.5, 255, 0, 0),   //
    make_vertex(.5, -.5, -.5, 255, 255, 0),  //
    make_vertex(.5, .5, -.5, 0, 255, 0),     //
    make_vertex(-.5, .5, -.5, 0, 255, 255),  //
    make_vertex(-.5, -.5, .5, 0, 0, 255),    //
    make_vertex(.5, -.5, .5, 255, 0, 255),   //
    make_vertex(.5, .5, .5, 128, 255, 255),  //
    make_vertex(-.5, .5, .5, 255, 128, 128), //
};

unsigned short mesh_indices[36] = {
    0, 1, 3, 3, 1, 2, //
    1, 5, 2, 2, 5, 6, //
    5, 4, 6, 6, 4, 7, //
    4, 0, 7, 7, 0, 3, //
    3, 2, 7, 7, 2, 6, //
    4, 5, 0, 0, 5, 1, //
};

static float get_elapsed_seconds(clock_t start, clock_t end) {
    return (end - start) / (float)CLOCKS_PER_SEC;
}

static void init() {
    state.fb = (framebuffer_t){
        .width = get_window_width(),
        .height = get_window_height(),
        .pixels = NULL, // Use pixel buffer provided by window
    };

    state.mesh = (mesh_t){
        .vertices = mesh_vertices,
        .indices = mesh_indices,
        .vertex_count = 8,
        .index_count = 36,
    };

    vec4 camera_pos = {0, 0, -1.5};
    vec4 model_pos = {0, 0, 0};

    // Camera matrices
    glm_lookat(camera_pos, model_pos, GLM_YUP, state.view);
    glm_perspective_default(get_window_aspect_ratio(), state.proj);
    glm_mat4_mul(state.proj, state.view, state.vp);
}

static void update(float dt) {
    state.fb.pixels = lock_surface();
    clear(&state.fb, (color_t){0, 0, 0, 1});

    // Rotate mesh
    state.rotation[0] += .5 * dt;
    state.rotation[1] += 1 * dt;

    // Calculate mvp matrix
    mat4 model;
    glm_euler(state.rotation, model);
    glm_mat4_mul(state.vp, model, state.uniforms.mvp);

    // Only wireframe supported right now
    draw_mesh(&state.fb, &state.uniforms, &state.mesh);

    unlock_surface();
}

static void on_resize(int width, int height) {
    state.fb.width = width;
    state.fb.height = height;

    // Update projection matrix
    glm_perspective_default(get_window_aspect_ratio(), state.proj);
    glm_mat4_mul(state.proj, state.view, state.vp);
}

int main() {
    init_window(800, 600, "Software Renderer", true, on_resize);
    init();

    int fps_limit = 60;
    float interval = 1.0 / fps_limit;

    int frames = 0;
    float time = 0;
    clock_t start = clock();

    while (is_window_open()) {
        // Calculate delta time
        clock_t now = clock();
        float dt = get_elapsed_seconds(start, now);
        start = now;

        // Update FPS counter
        time += dt;
        if (time >= 1) {
            int fps = (int)roundf(frames / time);
            float ft = time / frames * 1000;

            printf("FPS: %d (Limit: %d), avg. Frame Time: %.2fms\n", fps,
                   fps_limit, ft);

            frames = 0;
            time = 0;
        }

        poll_events();
        update(dt);

        // Framerate limit
        float elapsed = get_elapsed_seconds(start, clock());
        if (elapsed < interval) {
            sleep(interval - elapsed);
        }

        frames++;
    }
    destroy_window();
    return 0;
}