#include <math.h>

#include "triangle.h"
#include "vec3.h"
#include "tgaimage.h"

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
    // get bounding box
    // for every pixel in box (in parallel):
        // if inside triangle, color
}