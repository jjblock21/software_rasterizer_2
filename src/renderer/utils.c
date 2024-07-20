#include "utils.h"

int mini(int a, int b) { return a < b ? a : b; }
int maxi(int a, int b) { return a > b ? a : b; }

float lerpf(float a, float b, float t) { return b * t + a * (1 - t); }
int lerpi(int a, int b, float t) { return (int)lerpf(a, b, t); }

float clampf(float val, float min, float max) {
    const float t = val < min ? min : val;
    return t > max ? max : t;
}

void swapf(float *a, float *b) {
    float t = *a;
    *a = *b;
    *b = t;
}

void swapi(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

color_t lerp_color(color_t a, color_t b, float t) {
    return (color_t){
        lerpi(a.r, b.r, t),
        lerpi(a.g, b.g, t),
        lerpi(a.b, b.b, t),
        lerpi(a.a, b.a, t),
    };
}