#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "model.h"

#define LINE_SIZE 512

// gets vertex count in an obj file
static int get_model_data_count(Model *model, const char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "couldn't open file\n");
        return 1;
    }

    char line[128];

    model->nverts = 0;
    model->nfaces = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        char first_token[16];
        sscanf(line, "%s", first_token);
        if (strcmp(first_token, "v") == 0) model->nverts++;
        else if (strcmp(first_token, "f") == 0) model->nfaces++;
    }

    fclose(fp);
    return 0;
}

int model_load(Model *model, const char *path) {
    if (get_model_data_count(model, path) == 1) {
        return 1;
    }

    model->verts = malloc(sizeof(Vec3) * model->nverts);
    
    if (model->verts == NULL) {
        fprintf(stderr, "Failed to allocate vertex array\n");
        return 1;
    }

    model->faces_vrt = malloc(sizeof(int) * model->nfaces * 3);

    if (model->faces_vrt == NULL) {
        fprintf(stderr, "Failed to allocate face array\n");
        return 1;
    }

    int next_vertex_idx = 0;
    int next_face_idx = 0;

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Couldn't open .obj file\n");
        model_free(model);
        return 1;
    }

    char line[LINE_SIZE];

    while (fgets(line, sizeof(line), fp) != NULL) {
        char first_token[16];
        sscanf(line, "%s", first_token);

        if (strcmp(first_token, "v") == 0) {
            float x, y, z;
            sscanf(line, "v %f %f %f", &x, &y, &z);
            model->verts[next_vertex_idx++] = (Vec3){x, y ,z};
        }
        else if (strcmp(first_token, "f") == 0) {
            int v0, v1, v2;
            sscanf(line, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &v0, &v1, &v2);
            v0--; v1--; v2--;
            model->faces_vrt[next_face_idx++] = v0;
            model->faces_vrt[next_face_idx++] = v1;
            model->faces_vrt[next_face_idx++] = v2;
        }
    }

    return 0;
}

void model_free(Model *model) {
    free(model->verts);
    free(model->faces_vrt);
    model->verts = NULL;
    model->faces_vrt = NULL;
    model->nverts = 0;
    model->nfaces = 0;
}