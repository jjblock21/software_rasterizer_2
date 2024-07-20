#ifndef __SRC_RENDERER_UTILS_H__
#define __SRC_RENDERER_UTILS_H__

#include <cglm/cglm.h>
#include "../color.h"

int mini(int a, int b);
int maxi(int a, int b);

float lerpf(float a, float b, float t);
int lerpi(int a, int b, float t);

float clampf(float val, float min, float max);

void swapf(float *a, float *b);
void swapi(int *a, int *b);

color_t lerp_color(color_t a, color_t b, float t);

#endif /* __SRC_RENDERER_UTILS_H__ */
