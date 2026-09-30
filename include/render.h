#pragma once

#include "tgaimage.h"
#include "model.h"
#include "triangle.h"

typedef void (*TriangleRasterizer)(TGAImage *framebuffer, TGAColor color, Triangle t);

void draw_wireframe(const Model *model, TGAImage *framebuffer, const TGAColor line_color);
void draw_vertices(const Model *model, TGAImage *framebuffer, const TGAColor vertex_color);
void draw_filled(const Model *model, TGAImage *framebuffer, const TriangleRasterizer raster);
