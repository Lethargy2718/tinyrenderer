#pragma once

#include "tgaimage.h"
#include "vec3.h"

typedef struct {
    Vec3 v0;
    Vec3 v1;
    Vec3 v2;
} Triangle;

typedef struct { float a, b, c; } Bary;

typedef int (*TriangleInside)(const Bary *w, const float factor);

void draw_triangle_scanline(TGAImage *img, const TGAColor c, const Triangle t, const TriangleInside inside, const float inside_factor);
void draw_triangle_aabb(TGAImage *img, const TGAColor c, const Triangle t, const TriangleInside inside, const float inside_factor);

int inside_default(const Bary *w, const float factor);
int inside_hollow(const Bary *w, const float factor);
