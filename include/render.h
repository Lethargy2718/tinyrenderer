#pragma once

#include "tgaimage.h"
#include "model.h"
#include "triangle.h"
#include "zbuffer.h"
#include "transform.h"

void draw_wireframe(const Model *model, TGAImage *fb, const Camera *cam, TGAColor c);
void draw_vertices(const Model *model, TGAImage *framebuffer, const TGAColor vertex_color, const Camera *cam);
void draw_filled(const Model *model, TGAImage *fb, ZBuffer *zb, const Camera *cam, TriangleInside inside, float inside_factor);
void draw_gradient_background(TGAImage *framebuffer, const TGAColor color1, const TGAColor color2);