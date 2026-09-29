#pragma once

#include "tgaimage.h"
#include "model.h"

int draw_wireframe(const Model *model, TGAImage *framebuffer, const TGAColor line_color, const TGAColor vertex_color);