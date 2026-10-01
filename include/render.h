#pragma once

#include "tgaimage.h"
#include "model.h"
#include "triangle.h"

typedef void (*TriangleRasterizer)(TGAImage *img, const TGAColor c, const Triangle t, const TriangleInside inside, const float inside_factor);

void draw_wireframe(const Model *model, TGAImage *framebuffer, const TGAColor line_color);
void draw_vertices(const Model *model, TGAImage *framebuffer, const TGAColor vertex_color);
void draw_filled(const Model *model, TGAImage *framebuffer, const TriangleRasterizer raster, const TriangleInside inside, const float inside_factor);
void draw_gradient_background(TGAImage *framebuffer, const TGAColor color1, const TGAColor color2);