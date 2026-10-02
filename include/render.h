#pragma once

#include "tgaimage.h"
#include "model.h"
#include "triangle.h"
#include "zbuffer.h"

typedef void (*TriangleRasterizer)(const TriangleRasterData *raster_data);

void draw_wireframe(const Model *model, TGAImage *framebuffer, const TGAColor line_color);
void draw_vertices(const Model *model, TGAImage *framebuffer, const TGAColor vertex_color);
void draw_filled(const Model *model, TGAImage *framebuffer, ZBuffer *zbuffer, TriangleRasterizer raster, TriangleInside inside, float inside_factor);
void draw_gradient_background(TGAImage *framebuffer, const TGAColor color1, const TGAColor color2);