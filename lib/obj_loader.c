#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "obj_loader.h"
#include "vec3.h"
#include "line.h"

#define LINE_SIZE 128
#define FAIL -1

// gets vertex count in an obj file
int get_vertex_count(char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "couldn't open file\n");
        return FAIL;
    }

    char line[128];
    int count = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        char first_token[16];
        sscanf(line, "%s", first_token);
        if (strcmp(first_token, "v") == 0) count++;
    }

    fclose(fp);

    return count;
}

// converts a single object-space continuous coordinate in [-1, 1] to a discrete pixel coordinate in [0, size-1]
int remap_coord(float coord, int size) {
    return (int)((coord + 1.0f) * size / 2.0f);
}

int draw_wireframe(char *path, TGAImage *framebuffer, TGAColor line_color, TGAColor vertex_color) {
    int vertex_count = 0;
    if ((vertex_count = get_vertex_count(path)) == FAIL) {
        return 1;
    }
    
    Vec3 *vertices = malloc(sizeof(Vec3) * vertex_count);
    if (vertices == NULL) {
        fprintf(stderr, "Failed to allocate vertex array\n");
        return 1;
    }

    int next_vertex_idx = 0;

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "couldn't open file\n");
        return 1;
    }

    char line[LINE_SIZE];

    while (fgets(line, sizeof(line), fp) != NULL) {
        char first_token[16];
        sscanf(line, "%s", first_token);

        if (strcmp(first_token, "v") == 0) {
            float x, y, z;
            sscanf(line, "v %f %f %f", &x, &y, &z);
            vertices[next_vertex_idx].x = remap_coord(x, framebuffer->w);
            vertices[next_vertex_idx].y = remap_coord(y, framebuffer->h);
            vertices[next_vertex_idx].z = 0; // collapse to 0
            next_vertex_idx++;
        }
        else if (strcmp(first_token, "f") == 0) {
            int v0, v1, v2;
            sscanf(line, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &v0, &v1, &v2);
            v0--; v1--; v2--;

            Vec3 vec0 = vertices[v0];
            Vec3 vec1 = vertices[v1];
            Vec3 vec2 = vertices[v2];

            // Wires
            draw_line(vec0, vec1, framebuffer, line_color);
            draw_line(vec1, vec2, framebuffer, line_color);
            draw_line(vec2, vec0, framebuffer, line_color);
        }
    }

    // Draw vertices
    for (int i = 0; i < vertex_count; i++) {
        tga_set(framebuffer, vertices[i].x, vertices[i].y, vertex_color);
    }

    fclose(fp);
    free(vertices);
    return 0;
}