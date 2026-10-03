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

// assumes z goes from -1 to 1
void zbuffer_to_image(const ZBuffer *zb, TGAImage *out) {
    for (int y = 0; y < zb->h; y++) {
        for (int x = 0; x < zb->w; x++) {
            float z = zb->data[y * zb->w + x];

            // map from [-1,1] to [0,255]
            int v = (int)((z + 1.0f) / 2.0f * 255.0f);
            if (v < 0)   v = 0;
            if (v > 255) v = 255;

            tga_set(out, x, y, tga_color(v, 0, 0, 0, TGA_GRAYSCALE));
        }
    }
}