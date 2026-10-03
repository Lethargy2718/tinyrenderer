#include <math.h>
#include <stdio.h>

#include "triangle.h"
#include "vec.h"
#include "tgaimage.h"

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#define MAX3(a, b, c) ((a) > (b) ? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))

// swap two vectors
static void swap_vec3(Vec3 *a, Vec3 *b)
{
    Vec3 temp = *a;
    *a = *b;
    *b = temp;
}

// v0 < v1 < v2 (y coordinate)
static void sort_vertices(Triangle *t) {
    if (t->v0.y > t->v1.y) swap_vec3(&t->v0, &t->v1);
    if (t->v1.y > t->v2.y) swap_vec3(&t->v1, &t->v2);
    if (t->v0.y > t->v1.y) swap_vec3(&t->v0, &t->v1);

    return;
}

// x of the edge a->b at row y. If the edge is horizontal, return a.x.
static float edge_x(Vec3 a, Vec3 b, int y) {
    if (b.y == a.y) return a.x;
    return a.x + (y - a.y) * (b.x - a.x) / (b.y - a.y);
}

// barycentric weights of (px, py)
static Bary barycentric(const Triangle *t, float inv, float px, float py) {
    const Vec3 *v0 = &t->v0;
    const Vec3 *v1 = &t->v1;
    const Vec3 *v2 = &t->v2;

    Bary w;
    w.a = ((v1->x - px) * (v2->y - py) - (v1->y - py) * (v2->x - px)) * inv;
    w.b = ((v2->x - px) * (v0->y - py) - (v2->y - py) * (v0->x - px)) * inv;
    w.c = 1.0f - w.a - w.b;
    return w;
}

int inside_default(const Bary *w, const float factor) {
    return w->a >= 0.0f && w->b >= 0.0f && w->c >= 0.0f;
}

int inside_hollow(const Bary *w, const float factor) {
    return w->a >= 0.0f && w->b >= 0.0f && w->c >= 0.0f && !(w->a >= factor && w->b >= factor && w->c >= factor);
}

void draw_triangle_scanline(const TriangleRasterData *rd) {
    TGAImage *img = rd->img;
    ZBuffer *zb = rd->zbuffer;
    TGAColor c = rd->c;
    Triangle t = rd->t;

    // cull before sorting since sorting can flip the face
    float d = (t.v1.x - t.v0.x) * (t.v2.y - t.v0.y) - (t.v1.y - t.v0.y) * (t.v2.x - t.v0.x);
    if (d < 0.0f) return; // degenerate or sub-pixel triangle

    sort_vertices(&t);

    // recompute for the sorted triangle so the barycentrics match
    d = (t.v1.x - t.v0.x) * (t.v2.y - t.v0.y) - (t.v1.y - t.v0.y) * (t.v2.x - t.v0.x);
    if (fabsf(d) < 1.0f) return;
    float inv = 1.0f / d;

    int yStart = (int)t.v0.y;
    int yEnd   = (int)t.v2.y;
    if (yStart < 0) yStart = 0;
    if (yEnd > img->h - 1) yEnd = img->h - 1;

    for (int y = yStart; y <= yEnd; y++) {
        float x1 = edge_x(t.v0, t.v2, y);
        float x2 = (y < (int)t.v1.y) ? edge_x(t.v0, t.v1, y) : edge_x(t.v1, t.v2, y);

        int xl = (int)roundf(fminf(x1, x2));
        int xr = (int)roundf(fmaxf(x1, x2));
        if (xl < 0) xl = 0;
        if (xr > img->w - 1) xr = img->w - 1;

        for (int x = xl; x <= xr; x++) {
            Bary w = barycentric(&t, inv, x + 0.5f, y + 0.5f);
            float z = w.a * t.v0.z + w.b * t.v1.z + w.c * t.v2.z;
            if (zbuffer_test(zb, x, y, z)) tga_set(img, x, y, c);
        }
    }
}

void draw_triangle_aabb(const TriangleRasterData *rd) {
    TGAImage *img = rd->img;
    ZBuffer *zb = rd->zbuffer;
    TriangleInside inside = rd->inside;
    float inside_factor = rd->inside_factor;
    Triangle t = rd->t;

    float d = (t.v1.x - t.v0.x) * (t.v2.y - t.v0.y) - (t.v1.y - t.v0.y) * (t.v2.x - t.v0.x);
    if (d < 0.0f) return; // degenerate or sub-pixel triangle
    float inv = 1.0f / d;

    // 1. bounding box, clamped to the framebuffer
    
    // NOTE: uses macros to potentially become branchless instructions
    int minX = (int)floorf(MIN3(t.v0.x, t.v1.x, t.v2.x));
    int maxX = (int)ceilf (MAX3(t.v0.x, t.v1.x, t.v2.x));
    int minY = (int)floorf(MIN3(t.v0.y, t.v1.y, t.v2.y));
    int maxY = (int)ceilf (MAX3(t.v0.y, t.v1.y, t.v2.y));

    // TODO: add general clamp function somewhere
    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX > img->w - 1) maxX = img->w - 1;
    if (maxY > img->h - 1) maxY = img->h - 1;

    // 2. for each pixel in bbox, depth-test and color if inside
    #pragma omp parallel for if((maxY - minY) > 256)
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            Bary w = barycentric(&t, inv, x + 0.5f, y + 0.5f);
            if (inside(&w, inside_factor) != 1) continue;

            float z = w.a * t.v0.z + w.b * t.v1.z + w.c * t.v2.z;
            if (!zbuffer_test(zb, x, y, z)) continue;

            TGAColor clr = tga_color(255 * w.a * w.a, 255 * w.b * w.b, 255 * w.c * w.c, 255, TGA_RGB);
            tga_set(img, x, y, clr);
        }
    }
}