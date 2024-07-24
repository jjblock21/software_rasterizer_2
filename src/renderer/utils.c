#include "utils.h"

#include <math.h>

int mini(int a, int b) { return a < b ? a : b; }
int maxi(int a, int b) { return a > b ? a : b; }
int mini3(int a, int b, int c) { return mini(mini(a, b), c); }
int maxi3(int a, int b, int c) { return maxi(maxi(a, b), c); }

int roundintf(float val) { return (int)roundf(val); }

float lerpf(float a, float b, float t) { return b * t + a * (1 - t); }

float clampf(float val, float min, float max) {
    const float t = val < min ? min : val;
    return t > max ? max : t;
}

int clampi(int val, int min, int max) {
    const int t = val < min ? min : val;
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