#include <math.h>

#include "triangle.h"
#include "vec3.h"
#include "tgaimage.h"

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#define MAX3(a, b, c) ((a) > (b) ? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))

typedef struct { float a, b, c; } Bary;

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

static int inside(const Bary *w) {
    return w->a >= 0.0f && w->b >= 0.0f && w->c >= 0.0f;
}

void draw_triangle_scanline(TGAImage *img, TGAColor c, Triangle t) {
    sort_vertices(&t);

    for (int y = (int)t.v0.y; y <= (int)t.v2.y; y++) {
        // long edge (v0 -> v2)
        float x1 = edge_x(t.v0, t.v2, y);

        // short edges (v0 -> v1 then v1 -> v2)
        float x2 = (y < (int)t.v1.y) ? edge_x(t.v0, t.v1, y) : edge_x(t.v1, t.v2, y);

        float xl = fminf(x1, x2);
        float xr = fmaxf(x1, x2);

        for (int x = (int)roundf(xl); x <= (int)roundf(xr); x++) {
            tga_set(img, x, y, c);
        }
    }
}

void draw_triangle_aabb(TGAImage *img, TGAColor c, Triangle t) {
    float d = (t.v1.x - t.v0.x) * (t.v2.y - t.v0.y) - (t.v1.y - t.v0.y) * (t.v2.x - t.v0.x);
    if (d == 0.0f) return; // degenerate triangle
    float inv = 1.0f / d;
    
    // 1. get bounding box

    // NOTE: uses macros to potentially become branchless instructions
    float minX_f = MIN3(t.v0.x, t.v1.x, t.v2.x);
    float maxX_f = MAX3(t.v0.x, t.v1.x, t.v2.x);
    float minY_f = MIN3(t.v0.y, t.v1.y, t.v2.y);
    float maxY_f = MAX3(t.v0.y, t.v1.y, t.v2.y);

    int minX = (int)floorf(minX_f);
    int maxX = (int)ceilf(maxX_f);
    int minY = (int)floorf(minY_f);
    int maxY = (int)ceilf(maxY_f);
        
    // 2. for each pixel in bbox, color if in triangle

    #pragma omp parallel for if((maxY - minY) > 256)
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            Bary w = barycentric(&t, inv, x + 0.5f, y + 0.5f);
            if (inside(&w)) tga_set(img, x, y, c);
        }
    }
}