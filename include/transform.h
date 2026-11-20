#pragma once

#include <math.h>

#include "mat.h"
#include "vec.h"

/* Basic transforms */

static inline Mat4 translate(Vec3 t) {
    return (Mat4){{
        {1, 0, 0, t.x},
        {0, 1, 0, t.y},
        {0, 0, 1, t.z},
        {0, 0, 0, 1}
    }};
}

static inline Mat4 scale(Vec3 s) {
    return (Mat4){{
        {s.x, 0,   0,   0},
        {0,   s.y, 0,   0},
        {0,   0,   s.z, 0},
        {0,   0,   0,   1}
    }};
}

static inline Mat4 rotate_x(double a) {
    double c = cos(a), s = sin(a);
    return (Mat4){{
        {1, 0,  0, 0},
        {0, c, -s, 0},
        {0, s,  c, 0},
        {0, 0,  0, 1}
    }};
}

static inline Mat4 rotate_y(double a) {
    double c = cos(a), s = sin(a);
    return (Mat4){{
        { c, 0, s, 0},
        { 0, 1, 0, 0},
        {-s, 0, c, 0},
        { 0, 0, 0, 1}
    }};
}

static inline Mat4 rotate_z(double a) {
    double c = cos(a), s = sin(a);
    return (Mat4){{
        {c, -s, 0, 0},
        {s,  c, 0, 0},
        {0,  0, 1, 0},
        {0,  0, 0, 1}
    }};
}

/* Transform */

typedef struct {
    Vec3 position;
    Vec3 rotation;   // euler angles in radians (x, y, z)
    Vec3 scale;
} Transform;

static inline Transform transform_identity(void) {
    return (Transform){ {0, 0, 0}, {0, 0, 0}, {1, 1, 1} };
}

// Rotation matrix from transform: Z first, then X, then Y
static inline Mat4 transform_rotation(const Transform *t) {
    return mat4_mul(rotate_y(t->rotation.y),
           mat4_mul(rotate_x(t->rotation.x),
                    rotate_z(t->rotation.z)));
}

// Model matrix (model -> world) = Translate * Rotate * Scale
static inline Mat4 transform_matrix(const Transform *t) {
    return mat4_mul(translate(t->position),
           mat4_mul(transform_rotation(t), scale(t->scale)));
}

// Local right
static inline Vec3 transform_right(const Transform *t) {
    Mat4 r = transform_rotation(t);
    return (Vec3){ r.m[0][0], r.m[1][0], r.m[2][0] };
}

// Local up
static inline Vec3 transform_up(const Transform *t) {
    Mat4 r = transform_rotation(t);
    return (Vec3){ r.m[0][1], r.m[1][1], r.m[2][1] };
}

// Local forward: Forward is -z (right-handed, same as the camera)
static inline Vec3 transform_forward(const Transform *t) {
    Mat4 r = transform_rotation(t);
    return (Vec3){ -r.m[0][2], -r.m[1][2], -r.m[2][2] };
}

/* Camera matrices */

// View matrix (world -> view): camera at `eye` looking at `center`.
// `up` must not be parallel to (eye - center)
// This applies basis change from standard coordinates to camera coordinates
static inline Mat4 look_at(Vec3 eye, Vec3 center, Vec3 up) {
    // Camera basis
    Vec3 n = vec3_normalize(vec3_sub(eye, center));   // backward
    Vec3 l = vec3_normalize(vec3_cross(up, n));       // right
    Vec3 m = vec3_normalize(vec3_cross(n, l));        // up

    // p_cam = R^T (p_world - eye)
    // p_cam = R^T * p_world - R^T * eye

    // Transpose of the basis change matrix
    Mat4 RT = {{
        {l.x, l.y, l.z, 0},
        {m.x, m.y, m.z, 0},
        {n.x, n.y, n.z, 0},
        {0,   0,   0,   1}
    }};

    // Shift camera position back to origin
    Mat4 shift = translate(vec3_neg(eye));

    // Shift is applied first: p = p_world - eye
    // Then R^T: p_final = R^T * p = R^T (p_world - eye)

    return mat4_mul(RT, shift);
}

// Perspective (view -> clip): w = -z, z_ndc = 1/-z (inverse depth, larger = closer)
// focal = 1/tan(fov/2), 1.0 = 90 degrees
static inline Mat4 perspective(double focal) {
    return (Mat4){{
        {focal, 0,     0, 0},
        {0,     focal, 0, 0},
        {0,     0,     0, 1},
        {0,     0,    -1, 0}
    }};
}

// Viewport (NDC -> pixels): rect with bottom-left corner (x, y), size w x h. z passes through
static inline Mat4 viewport(const int x, const int y, const int w, const int h) {
    return (Mat4){{
        {w/2., 0,    0, x + w/2.},
        {0,    h/2., 0, y + h/2.},
        {0,    0,    1, 0},
        {0,    0,    0, 1}
    }};
}

/* Camera */

typedef struct {
    Mat4 proj_view;   // Perspective * View (world -> clip)
    Mat4 viewport;    // NDC -> pixels
} Camera;

// focal: 1/tan(fov/2), 1.0 = 90 degree FOV, bigger = more zoomed in
static inline Camera camera_make(Vec3 eye, Vec3 center, Vec3 up, int w, int h, double focal) {
    Camera cam;
    cam.proj_view = mat4_mul(perspective(focal), look_at(eye, center, up));
    cam.viewport  = viewport(0, 0, w, h);
    return cam;
}