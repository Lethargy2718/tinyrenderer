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

static void sort_vertices(Triangle *t) {
    if (t->v0.y > t->v1.y) swap_vec3(&t->v0, &t->v1);
    if (t->v1.y > t->v2.y) swap_vec3(&t->v1, &t->v2);
    if (t->v0.y > t->v1.y) swap_vec3(&t->v0, &t->v1);
}