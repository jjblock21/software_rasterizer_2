#ifndef __SRC_SHADERS_SHADER0_H__
#define __SRC_SHADERS_SHADER0_H__

#include "../rendering/rasterizer.h"

typedef struct {
    mat4 mvp;
} shader0_userdata_t;

varying_t shader0_vertex_main(vertex_t vertex, void *userdata, vec4 pos);
varying_t shader0_interp_func(varying_t v0, float w0, varying_t v1, float w1,
                              varying_t v2, float w2);
void shader0_pixel_main(varying_t in, void *userdata, vec3 color);

#endif /* __SRC_SHADERS_SHADER0_H__ */
