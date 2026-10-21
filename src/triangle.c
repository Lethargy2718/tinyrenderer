#include <math.h>
#include <stdio.h>

#include "triangle.h"
#include "vec.h"
#include "tgaimage.h"
#include "mat.h"

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#define MAX3(a, b, c) ((a) > (b) ? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))

int inside_default(const Bary *w, const float factor) {
    return w->a >= 0.0f && w->b >= 0.0f && w->c >= 0.0f;
}

int inside_hollow(const Bary *w, const float factor) {
    return w->a >= 0.0f && w->b >= 0.0f && w->c >= 0.0f && !(w->a >= factor && w->b >= factor && w->c >= factor);
}

void draw_triangle_aabb(const TriangleRasterData *rd) {
    TGAImage *img = rd->img;
    ZBuffer *zb = rd->zbuffer;
    TriangleInside inside = rd->inside;
    float inside_factor = rd->inside_factor;
    Triangle t = rd->t;

    Mat3 ABC = {{
        { t.v0.x, t.v0.y, 1.0f },
        { t.v1.x, t.v1.y, 1.0f },
        { t.v2.x, t.v2.y, 1.0f },
    }};

    if (mat3_det(ABC) < 1.0f) return;   // backface + sub-pixel

    Mat3 inv;
    if (!mat3_inverse(ABC, &inv)) return;
    Mat3 invT = mat3_transpose(inv);    // once per triangle

    // NOTE: uses macros to potentially become branchless instructions
    int minX = (int)floorf(MIN3(t.v0.x, t.v1.x, t.v2.x));
    int maxX = (int)ceilf (MAX3(t.v0.x, t.v1.x, t.v2.x));
    int minY = (int)floorf(MIN3(t.v0.y, t.v1.y, t.v2.y));
    int maxY = (int)ceilf (MAX3(t.v0.y, t.v1.y, t.v2.y));

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX > img->w - 1) maxX = img->w - 1;
    if (maxY > img->h - 1) maxY = img->h - 1;

    #pragma omp parallel for if((maxY - minY) > 256)
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            Vec3 bc = mat3_mul_vec3(invT, (Vec3){ x + 0.5f, y + 0.5f, 1.0f });
            Bary w = { bc.x, bc.y, bc.z };

            if (inside(&w, inside_factor) != 1) continue;

            float z = w.a * t.v0.z + w.b * t.v1.z + w.c * t.v2.z;
            if (!zbuffer_test(zb, x, y, z)) continue;

            TGAColor clr = tga_color(255 * w.a * w.a, 255 * w.b * w.b, 255 * w.c * w.c, 255, TGA_RGB);
            // TGAColor clr = tga_color(0, 0, 255, 255, TGA_RGB);
            tga_set(img, x, y, clr);
        }
    }
}