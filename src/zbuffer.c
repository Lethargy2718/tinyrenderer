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