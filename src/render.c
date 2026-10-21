#include <math.h>
#include <stdio.h>

#include "vec.h"
#include "tgaimage.h"
#include "model.h"
#include "line.h"
#include "triangle.h"
#include "color.h"
#include "render.h"
#include "zbuffer.h"
#include "transform.h"

// 4x4 matrix times (x, y, z, w) -> out[4]
// TODO: move to mat.h maybe
static void mat4_apply(const Mat4 *M, const double in[4], double out[4]) {
    for (int r = 0; r < 4; r++) {
        out[r] = 0;
        for (int c = 0; c < 4; c++)
            out[r] += M->m[r][c] * in[c];
    }
}

// object space -> screen space (x,y in pixels, z = NDC depth)
static Vec3 project(const Mat4 *mvp, const Camera *cam, Vec3 v) {
    double in[4] = { v.x, v.y, v.z, 1.0 };
    double clip[4], ndc[4], scr[4];

    mat4_apply(mvp, in, clip);
    for (int i = 0; i < 3; i++) ndc[i] = clip[i] / clip[3];
    ndc[3] = 1.0;
    mat4_apply(&cam->viewport, ndc, scr);

    return (Vec3){ (float)scr[0], (float)scr[1], (float)scr[2] };
}

// fetches the i-th face's three vertices, already in framebuffer space
static Triangle get_face_screen_triangle(const Model *model, int face, const Mat4 *mvp, const Camera *cam) {
    int base = face * 3;
    Triangle t = {
        project(mvp, cam, model->verts[model->faces_vrt[base]]),
        project(mvp, cam, model->verts[model->faces_vrt[base + 1]]),
        project(mvp, cam, model->verts[model->faces_vrt[base + 2]]),
    };
    return t;
}

// TODO: move to some math lib or check if one already exists
static inline float distance(int x1, int y1, int x2, int y2) {
    return sqrt((y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1));
}

void draw_wireframe(const Model *model, TGAImage *fb, const Camera *cam, Mat4 model_matrix, TGAColor c) {
    Mat4 mat = mat4_mul(cam->proj_view, model_matrix);

    for (int f = 0; f < model->nfaces; f++) {
        Triangle t = get_face_screen_triangle(model, f, &mat, cam);
        draw_line(t.v0, t.v1, fb, c);
        draw_line(t.v1, t.v2, fb, c);
        draw_line(t.v2, t.v0, fb, c);
    }
}

void draw_vertices(const Model *model, TGAImage *framebuffer, const TGAColor vertex_color, const Camera *cam, Mat4 model_matrix) {
    Mat4 mat = mat4_mul(cam->proj_view, model_matrix);

    for (int i = 0; i < model->nverts; i++) {
        Vec3 v = project(&mat, cam, model->verts[i]);
        tga_set(framebuffer, (int)v.x, (int)v.y, vertex_color);
    }
}

void draw_filled(const Model *model, TGAImage *fb, ZBuffer *zb, const Camera *cam, Mat4 model_matrix, TriangleInside inside, float inside_factor) {
    Mat4 mat = mat4_mul(cam->proj_view, model_matrix);

    for (int f = 0; f < model->nfaces; f++) {
        const TriangleRasterData data = {
            .img           = fb,
            .zbuffer       = zb,
            .c             = random_color(),
            .t = get_face_screen_triangle(model, f, &mat, cam),
            .inside        = inside,
            .inside_factor = inside_factor,
        };
        draw_triangle_aabb(&data);
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
