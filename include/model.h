#pragma once

#include "vec3.h"

typedef struct {
    Vec3 *verts;
    int nverts;
    int *faces_vrt;
    int nfaces;
    
} Model;

int model_load(Model *model, const char *path);
void model_free(Model *model);