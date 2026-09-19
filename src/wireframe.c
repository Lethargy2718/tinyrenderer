#include <math.h>

#include "vec3.h"
#include "tgaimage.h"
#include "model.h"
#include "line.h"

// converts a single object-space continuous coordinate in [-1, 1] to a discrete pixel coordinate in [0, size-1]
static int remap_coord(float coord, int size) {
    return (int)roundf((coord + 1.0f) * size / 2.0f);
}

// remaps vec3 coords to framebuffer space and collapses the z-coordinate to 0
static Vec3 remap_vec(Vec3 v, TGAImage *framebuffer) {
    v.x = remap_coord(v.x, framebuffer->w);
    v.y = remap_coord(v.y, framebuffer->h);
    v.z = 0;
    return v;
}

void draw_wireframe(const Model *model, TGAImage *framebuffer, TGAColor line_color, TGAColor vertex_color) {
    for (int start = 0; start < model->nfaces * 3; start += 3) {
        Vec3 v0 = remap_vec(model->verts[model->faces_vrt[start]], framebuffer);
        Vec3 v1 = remap_vec(model->verts[model->faces_vrt[start + 1]], framebuffer);
        Vec3 v2 = remap_vec(model->verts[model->faces_vrt[start + 2]], framebuffer);
        
        draw_line(v0, v1, framebuffer, line_color);
        draw_line(v1, v2, framebuffer, line_color);
        draw_line(v2, v0, framebuffer, line_color);
    }
    
    
    // Draw vertices
    for (int i = 0; i < model->nverts; i++) {
        Vec3 v = remap_vec(model->verts[i], framebuffer);
        tga_set(framebuffer, (int)v.x, (int)v.y, vertex_color);
    }

    return;
}
