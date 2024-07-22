#ifndef __SRC_RENDERER_UTILS_H__
#define __SRC_RENDERER_UTILS_H__

#include <stdint.h>
#include <cglm/cglm.h>

int mini(int a, int b);
int maxi(int a, int b);
int mini3(int a, int b, int c);
int maxi3(int a, int b, int c);

int roundintf(float val);

float lerpf(float a, float b, float t);

float clampf(float val, float min, float max);

void swapf(float *a, float *b);
void swapi(int *a, int *b);

#endif /* __SRC_RENDERER_UTILS_H__ */
