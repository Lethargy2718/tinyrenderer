#pragma once

#include "tgaimage.h"
#include "vec3.h"

typedef struct {
    Vec3 v0;
    Vec3 v1;
    Vec3 v2;
} Triangle;


void draw_triangle_scanline(TGAImage *img, TGAColor c, Triangle t);

void draw_triangle_aabb(TGAImage *img, TGAColor c, Triangle t);