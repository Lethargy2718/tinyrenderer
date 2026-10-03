#pragma once

#include "vec.h"

typedef struct {
    Vec3 *verts;
    int nverts;
    int *faces_vrt;
    int nfaces;
} Model;

int model_load(Model *model, const char *path);
void model_free(Model *model);

static inline void model_rotate_axis(Model *model, Vec3 axis, float angle) {
    for (int i = 0; i < model->nverts; i++) {
        model->verts[i] = vec3_rotate_axis(model->verts[i], axis, angle);
    }
}
