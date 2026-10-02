#pragma once

typedef struct {
    int w, h;
    float *data;
} ZBuffer;

bool zbuffer_init(ZBuffer *zb, int w, int h);
void zbuffer_free(ZBuffer *zb);

// returns true if the fragment is closer and updates the buffer
static inline bool zbuffer_test(ZBuffer *zb, int x, int y, float z) {
    float *d = &zb->data[y * zb->w + x];
    if (z > *d) { *d = z; return true; }
    return false;
}
