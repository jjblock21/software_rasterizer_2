#include "window.h"

#include <stdio.h>
#include <sdl/SDL.h>

struct window {
    SDL_Window *sdl_window;
    SDL_Surface *native_surface;
    SDL_Surface *surface;
    int width, height;
    bool is_open, surface_invalid;
    color_t *pixels;
    void (*resize_callback)(int, int);
} window;

void init_window(int width, int height, const char *title, bool resizable,
                 void (*resize_callback)(int, int)) {
    SDL_Init(SDL_INIT_EVERYTHING);

    window = (struct window){
        .width = width,
        .height = height,
        .is_open = true,
        .surface_invalid = true,
        .resize_callback = resize_callback,
    };

    int flags = SDL_WINDOW_SHOWN | (resizable ? SDL_WINDOW_RESIZABLE : 0);
    window.sdl_window = SDL_CreateWindow(                      //
        title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, //
        width, height, flags                                   //
    );
}

static void update_surface() {
    // Window surface
    window.native_surface = SDL_GetWindowSurface(window.sdl_window);

    // Working surface
    if (window.surface) {
        SDL_FreeSurface(window.surface);
    }
    window.surface = SDL_CreateRGBSurfaceWithFormat(               //
        0, window.width, window.height, 32, SDL_PIXELFORMAT_RGBA32 //
    );
}

color_t *lock_surface() {
    if (!window.pixels) {
        if (window.surface_invalid) {
            update_surface();
            window.surface_invalid = false;
        }

        SDL_LockSurface(window.surface);
        window.pixels = window.surface->pixels;
    }
    return window.pixels;
}

void unlock_surface() {
    if (!window.pixels) return;
    window.pixels = NULL;
    SDL_UnlockSurface(window.surface);

    // Copy pixels into window surface
    SDL_Rect rect = {0, 0, window.width, window.height};
    SDL_BlitSurface(window.surface, &rect, window.native_surface, &rect);

    // Swap buffers
    SDL_UpdateWindowSurface(window.sdl_window);
}

void destroy_window() {
    if (window.sdl_window) {
        if (window.surface) {
            SDL_FreeSurface(window.surface);
        }
        SDL_DestroyWindow(window.sdl_window);
        SDL_Quit();
        window.sdl_window = NULL;
    }
}

bool is_window_open() { return window.is_open; }
int get_window_width() { return window.width; }
int get_window_height() { return window.height; }

float get_window_aspect_ratio() {
    return (float)get_window_width() / get_window_height();
}

static void handle_window_event(SDL_WindowEvent event) {
    switch (event.event) {
    case SDL_WINDOWEVENT_CLOSE:
        window.is_open = false;
        break;

    case SDL_WINDOWEVENT_RESIZED:
    case SDL_WINDOWEVENT_SIZE_CHANGED:
        // Update window
        SDL_GetWindowSize(window.sdl_window, &window.width, &window.height);
        window.surface_invalid = true;

        // Callback for user code
        if (window.resize_callback) {
            window.resize_callback(window.width, window.height);
        }
        break;
    }
}

void poll_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            window.is_open = false;
            break;

        case SDL_WINDOWEVENT:
            handle_window_event(event.window);
            break;
        }
    }
}

void sleep(float seconds) { SDL_Delay(seconds * 1000); }

void print_sdl_error() {
    const char *message = SDL_GetError();
    if (message == NULL || message[0] == '\0') {
        fprintf(stderr, "No Error\n");
        return;
    }
    fprintf(stderr, "%s\n", message);
}