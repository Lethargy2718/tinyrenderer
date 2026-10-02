#pragma once

#include "tgaimage.h"
#include "vec3.h"
#include "zbuffer.h"

typedef struct {
    Vec3 v0;
    Vec3 v1;
    Vec3 v2;
} Triangle;

typedef struct {
     float a;
     float b;
     float c;
} Bary;

typedef int (*TriangleInside)(const Bary *w, const float factor);

typedef struct {
    TGAImage *img;
    ZBuffer *zbuffer;
    TGAColor c;
    Triangle t;
    TriangleInside inside;
    float inside_factor;
} TriangleRasterData;

void draw_triangle_scanline(const TriangleRasterData *raster_data);
void draw_triangle_aabb(const TriangleRasterData *raster_data);

int inside_default(const Bary *w, const float factor);
int inside_hollow(const Bary *w, const float factor);