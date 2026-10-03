#pragma once
#include <math.h>
#include <stdbool.h>
#include "vec.h"

typedef struct {
    float m[3][3];
} Mat3;

typedef struct {
    float m[4][4];
} Mat4;

// Mat3

static inline Mat3 mat3_identity(void) {
    return (Mat3){{
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 }
    }};
}

static inline Mat3 mat3_mul(Mat3 a, Mat3 b) {
    Mat3 r = {{{ 0 }}};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                r.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return r;
}

static inline Vec3 mat3_mul_vec3(Mat3 a, Vec3 v) {
    return (Vec3){
        a.m[0][0] * v.x + a.m[0][1] * v.y + a.m[0][2] * v.z,
        a.m[1][0] * v.x + a.m[1][1] * v.y + a.m[1][2] * v.z,
        a.m[2][0] * v.x + a.m[2][1] * v.y + a.m[2][2] * v.z
    };
}

static inline Mat3 mat3_transpose(Mat3 a) {
    Mat3 r;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            r.m[i][j] = a.m[j][i];
        }
    }
    return r;
}

static inline float mat3_det(Mat3 a) {
    return a.m[0][0] * (a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1])
         - a.m[0][1] * (a.m[1][0] * a.m[2][2] - a.m[1][2] * a.m[2][0])
         + a.m[0][2] * (a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0]);
}

// Returns false if matrix is singular.
static inline bool mat3_inverse(Mat3 a, Mat3 *out) {
    float det = mat3_det(a);
    if (fabsf(det) < 1e-8f) return false;
    float inv = 1.0f / det;

    // adjugate / det
    Mat3 r;
    r.m[0][0] =  (a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1]) * inv;
    r.m[0][1] = -(a.m[0][1] * a.m[2][2] - a.m[0][2] * a.m[2][1]) * inv;
    r.m[0][2] =  (a.m[0][1] * a.m[1][2] - a.m[0][2] * a.m[1][1]) * inv;
    r.m[1][0] = -(a.m[1][0] * a.m[2][2] - a.m[1][2] * a.m[2][0]) * inv;
    r.m[1][1] =  (a.m[0][0] * a.m[2][2] - a.m[0][2] * a.m[2][0]) * inv;
    r.m[1][2] = -(a.m[0][0] * a.m[1][2] - a.m[0][2] * a.m[1][0]) * inv;
    r.m[2][0] =  (a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0]) * inv;
    r.m[2][1] = -(a.m[0][0] * a.m[2][1] - a.m[0][1] * a.m[2][0]) * inv;
    r.m[2][2] =  (a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0]) * inv;
    *out = r;
    return true;
}

// Mat4

static inline Mat4 mat4_identity(void) {
    return (Mat4){{
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 }
    }};
}

static inline Mat4 mat4_mul(Mat4 a, Mat4 b) {
    Mat4 r = {{{ 0 }}};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                r.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return r;
}

static inline Vec4 mat4_mul_vec4(Mat4 a, Vec4 v) {
    return (Vec4){
        a.m[0][0] * v.x + a.m[0][1] * v.y + a.m[0][2] * v.z + a.m[0][3] * v.w,
        a.m[1][0] * v.x + a.m[1][1] * v.y + a.m[1][2] * v.z + a.m[1][3] * v.w,
        a.m[2][0] * v.x + a.m[2][1] * v.y + a.m[2][2] * v.z + a.m[2][3] * v.w,
        a.m[3][0] * v.x + a.m[3][1] * v.y + a.m[3][2] * v.z + a.m[3][3] * v.w
    };
}

static inline Mat4 mat4_transpose(Mat4 a) {
    Mat4 r;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            r.m[i][j] = a.m[j][i];
        }
    }
    return r;
}

// 3x3 matrix left after removing `row` and `col` from a 4x4
static inline Mat3 mat4_submatrix(Mat4 a, int row, int col) {
    Mat3 r;
    int ri = 0;
    for (int i = 0; i < 4; i++) {
        if (i == row) continue;
        int rj = 0;
        for (int j = 0; j < 4; j++) {
            if (j == col) continue;
            r.m[ri][rj] = a.m[i][j];
            rj++;
        }
        ri++;
    }
    return r;
}

static inline float mat4_cofactor(Mat4 a, int row, int col) {
    float minor = mat3_det(mat4_submatrix(a, row, col));
    return ((row + col) % 2 == 0) ? minor : -minor;
}

static inline float mat4_det(Mat4 a) {
    float det = 0;
    for (int j = 0; j < 4; j++) {
        det += a.m[0][j] * mat4_cofactor(a, 0, j);
    }
    return det;
}

// Returns false if matrix is singular.
static inline bool mat4_inverse(Mat4 a, Mat4 *out) {
    float det = mat4_det(a);
    if (fabsf(det) < 1e-8f) return false;
    float inv = 1.0f / det;

    Mat4 r;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            // adjugate is the transposed cofactor matrix
            r.m[j][i] = mat4_cofactor(a, i, j) * inv;
        }
    }
    *out = r;
    return true;
}