#include <math.h>
#include <stdio.h>

#include "vec3.h"
#include "tgaimage.h"
#include "model.h"
#include "line.h"
#include "triangle.h"
#include "color.h"
#include "render.h"

// converts a single object-space coordinate in [-1, 1] to a pixel coordinate in [0, size-1]
static int remap_coord(float coord, int size) {
    return (int)roundf((coord + 1.0f) * size / 2.0f);
}

// remaps vec3 coords to framebuffer space and collapses z to 0
static Vec3 remap_vec(Vec3 v, const TGAImage *framebuffer) {
    v.x = remap_coord(v.x, framebuffer->w);
    v.y = remap_coord(v.y, framebuffer->h);
    v.z = 0;
    return v;
}

// fetches the i-th face's three vertices, already in framebuffer space
static Triangle get_face_screen_triangle(const Model *model, int face, const TGAImage *framebuffer) {
    int base = face * 3;
    Triangle t = {
        remap_vec(model->verts[model->faces_vrt[base]],     framebuffer),
        remap_vec(model->verts[model->faces_vrt[base + 1]], framebuffer),
        remap_vec(model->verts[model->faces_vrt[base + 2]], framebuffer),
    };
    return t;
}

static inline float distance(int x1, int y1, int x2, int y2) {
    return sqrt((y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1));
}

void draw_wireframe(const Model *model, TGAImage *framebuffer, const TGAColor line_color) {
    for (int f = 0; f < model->nfaces; f++) {
        Triangle t = get_face_screen_triangle(model, f, framebuffer);
        draw_line(t.v0, t.v1, framebuffer, line_color);
        draw_line(t.v1, t.v2, framebuffer, line_color);
        draw_line(t.v2, t.v0, framebuffer, line_color);
    }
}

void draw_vertices(const Model *model, TGAImage *framebuffer, const TGAColor vertex_color) {
    for (int i = 0; i < model->nverts; i++) {
        Vec3 v = remap_vec(model->verts[i], framebuffer);
        tga_set(framebuffer, (int)v.x, (int)v.y, vertex_color);
    }
}

void draw_filled(const Model *model, TGAImage *framebuffer, const TriangleRasterizer raster, const TriangleInside inside, const float inside_factor) {
    for (int f = 0; f < model->nfaces; f++) {
        Triangle t = get_face_screen_triangle(model, f, framebuffer);
        raster(framebuffer, random_color(), t, inside, inside_factor);
    }
}

void draw_gradient_background(TGAImage *framebuffer, TGAColor color1, TGAColor color2) {
    int cx = framebuffer->w / 2;
    int cy = framebuffer->h / 2;

    int mx = distance(0, 0, cx, cy);
    
    for (int x = 0; x <= framebuffer->w; x++) {
        for (int y = 0; y <= framebuffer->h; y++) {            
            float dist = distance(cx, cy, x, y);
            float part = sqrt(dist / mx);

            TGAColor final_color = tga_color(
                color1.bgra[0] * part + color2.bgra[0] * (1 - part),
                color1.bgra[1] * part + color2.bgra[1] * (1 - part),
                color1.bgra[2] * part + color2.bgra[2] * (1 - part),
                255,
                TGA_RGB
            );
            
            tga_set(framebuffer, x, y, final_color);
        }
    }
}
