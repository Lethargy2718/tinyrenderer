#pragma once

#include "tgaimage.h"
#include "model.h"

int draw_wireframe(Model *model, TGAImage *framebuffer, TGAColor line_color, TGAColor vertex_color);