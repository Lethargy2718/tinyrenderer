#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#include "zbuffer.h"

bool zbuffer_init(ZBuffer *zb, int w, int h) {
    zb->w = w; zb->h = h;
    zb->data = malloc(sizeof(float) * w * h);
    if (!zb->data) return false;
    for (int i = 0; i < w * h; i++) zb->data[i] = -INFINITY;
    return true;
}

void zbuffer_free(ZBuffer *zb) {
    free(zb->data);
    zb->data = NULL; 
}

void zbuffer_to_image(const ZBuffer *zb, TGAImage *out) {
    float zmin = INFINITY, zmax = -INFINITY;
    for (int i = 0; i < zb->w * zb->h; i++) {
        float z = zb->data[i];
        if (z == -INFINITY) continue;
        if (z < zmin) zmin = z;
        if (z > zmax) zmax = z;
    }
    float range = (zmax > zmin) ? (zmax - zmin) : 1.0f;

    for (int y = 0; y < zb->h; y++) {
        for (int x = 0; x < zb->w; x++) {
            float z = zb->data[y * zb->w + x];
            int g = (z == -INFINITY) ? 0 : (int)(255.0f * (z - zmin) / range);
            tga_set(out, x, y, tga_color(g, 0, 0, 0, TGA_GRAYSCALE));
        }
    }
}