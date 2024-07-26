#include "shader0.h"

varying_t shader0_vertex_main(vertex_t vertex, void *userdata, vec4 pos) {
    // Transform from model to clip space
    shader0_userdata_t data = *(shader0_userdata_t *)userdata;
    glm_mat4_mulv(data.mvp, vertex.pos, pos);

    varying_t result;
    glm_vec3_copy(vertex.color, result.color);
    return result;
}

varying_t shader0_interp_func(varying_t v0, float w0, varying_t v1, float w1,
                              varying_t v2, float w2) {
    varying_t result;
    result.color[0] = v0.color[0] * w0 + v1.color[0] * w1 + v2.color[0] * w2;
    result.color[1] = v0.color[1] * w0 + v1.color[1] * w1 + v2.color[1] * w2;
    result.color[2] = v0.color[2] * w0 + v1.color[2] * w1 + v2.color[2] * w2;
    return result;
}

void shader0_pixel_main(varying_t in, void *userdata, vec3 color) {
    // Just return vertex color
    glm_vec3_copy(in.color, color);
}