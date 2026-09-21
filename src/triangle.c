#include "vec3.h"

typedef struct {
    Vec3 v0;
    Vec3 v1;
    Vec3 v2;
} Triangle;

static void swap_vec3(Vec3 *a, Vec3 *b)
{
    Vec3 temp = *a;
    *a = *b;
    *b = temp;
}

// v0 < v1 < v2
static void sort_vertices(Triangle *t) {
    if (t->v0.y > t->v1.y) swap_vec3(&t->v0, &t->v1);
    if (t->v1.y > t->v2.y) swap_vec3(&t->v1, &t->v2);
    if (t->v0.y > t->v1.y) swap_vec3(&t->v0, &t->v1);

    return;
}

// t must be sorted
static void get_boundaries(const Triangle *t, Vec3 *left, Vec3 *right) {
    // if equal, takes top vertex,
    if (t->v1.x < t->v2.x) {
        *left = t->v1;
        *right = t->v2;
    }
    else if (t->v1.x < t->v2.x) {
        *left = t->v2;
        *right = t->v1;
    }
    else {
        // v1.x == v2.x
        if (t->v1.x < t->v0.x) {
            *left = t->v1;
            *right = t->v2;
        }
        else {
            *left = t->v2;
            *right = t->v1;
        }
    }

    return;
}

// copies triangle to sort the copied version
void draw_triangle(Triangle t) {
    sort_vertices(&t);

    Vec3 left, right, center = t.v0;
    get_boundaries(&t, &left, &right);
}