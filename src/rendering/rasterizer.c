#include "rasterizer.h"

static float edge_function(ivec2 v0, ivec2 v1, ivec2 v2) {
    return ((v2[0] - v0[0]) * (v1[1] - v0[1]) -
            (v2[1] - v0[1]) * (v1[0] - v0[0]));
}

static void draw_triangle(             //
    framebuffer_t fb, shader_t shader, //
    varying_t v0, ivec2 p0,            // Vertex 1
    varying_t v1, ivec2 p1,            // Vertex 2
    varying_t v2, ivec2 p2,            // Vertex 3
    void *userdata                     //
) {
    // TODO Return if triangle doesn't overlap viewport

    // Cull backwards facing triangles (counter clockwise winding order)
    float area = edge_function(p0, p1, p2);
    if (area < 0) return;

    // Find triangle bounding box
    int minx = clampi(mini3(p0[0], p1[0], p2[0]), 0, fb.width);
    int miny = clampi(mini3(p0[1], p1[1], p2[1]), 0, fb.height);
    int maxx = clampi(maxi3(p0[0], p1[0], p2[0]), 0, fb.width);
    int maxy = clampi(maxi3(p0[1], p1[1], p2[1]), 0, fb.height);

    for (int y = miny; y < maxy; y++) {
        for (int x = minx; x < maxx; x++) {
            ivec2 pos = {x, y};

            // Test if point is in triangle
            float ef01 = edge_function(p0, p1, pos);
            if (ef01 < 0) continue;
            float ef12 = edge_function(p1, p2, pos);
            if (ef12 < 0) continue;
            float ef20 = edge_function(p2, p0, pos);
            if (ef20 < 0) continue;

            // Interpolate between the vertices of the triangle
            float w0 = ef12 / area, w1 = ef20 / area, w2 = ef01 / area;
            varying_t varying = shader.interp_func(v0, w0, v1, w1, v2, w2);

            // Run pixel shader
            vec3 color;
            shader.pixel_main(varying, userdata, color);

            // Write color to framebuffer
            set_pixel(fb, x, y, vec3_to_color(color));
        }
    }
}

void draw_mesh(framebuffer_t fb, shader_t shader, mesh_t mesh, void *userdata) {
    varying_t outbuf[mesh.vertex_count];
    ivec2 posbuf[mesh.vertex_count];

    // Shade each vertex
    for (int i = 0; i < mesh.vertex_count; i++) {
        vertex_t vertex = mesh.vertices[i];
        vec4 clip_pos;

        // Run vertex shader
        outbuf[i] = shader.vertex_main(vertex, userdata, clip_pos);

        // Do perspective division and map to viewport
        int *pos = posbuf[i];
        pos[0] = (int)roundf((clip_pos[0] / clip_pos[3] + 1) * fb.width / 2);
        pos[1] = (int)roundf((clip_pos[1] / clip_pos[3] + 1) * fb.height / 2);
    }

    for (int i = 0; i < mesh.index_count - 2; i += 3) {
        int i0 = mesh.indices[i], i1 = mesh.indices[i + 1],
            i2 = mesh.indices[i + 2];

        // Make sure indices are within bounds
        if (i0 >= mesh.vertex_count || i1 >= mesh.vertex_count ||
            i2 >= mesh.vertex_count) {
            continue;
        }

        varying_t v0 = outbuf[i0], v1 = outbuf[i1], v2 = outbuf[i2];
        int *p0 = posbuf[i0], *p1 = posbuf[i1], *p2 = posbuf[i2];

        // Rasterize triangle
        draw_triangle(fb, shader, v0, p0, v1, p1, v2, p2, userdata);
    }
}