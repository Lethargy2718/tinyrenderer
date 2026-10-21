#pragma once

#include <math.h>

#include "mat.h"
#include "vec.h"

// Scales the scene to the size of the screen. Renders at a viewport with
// a top-left corner of (x, y) and dimensions w x h
static inline Mat4 viewport(const int x, const int y, const int w, const int h) {
    return (Mat4){{{w/2., 0, 0, x+w/2.}, {0, h/2., 0, y+h/2.}, {0,0,1,0}, {0,0,0,1}}};
}

// Applies perspective projection at the z-axis (meaning the camera eye
// lies on the z-axis) with focal distance f
static inline Mat4 perspective(const double f) {
    return (Mat4){{{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0, -1/f,1}}};
}

// Constructs a view matrix for a camera at `eye`, looking toward `center`,
// using `up` as the approximate world-up direction.
static inline Mat4 look_at(Vec3 eye, Vec3 center, Vec3 up) {
    Vec3 n = vec3_normalize(vec3_sub(eye, center));
    Vec3 l = vec3_normalize(vec3_cross(up, n));
    Vec3 m = vec3_normalize(vec3_cross(n, l));

    Mat4 rotation = {{
        {l.x, l.y, l.z, 0},
        {m.x, m.y, m.z, 0},
        {n.x, n.y, n.z, 0},
        {0,   0,   0,   1}
    }};

    Mat4 translation = {{
        {1, 0, 0, -center.x},
        {0, 1, 0, -center.y},
        {0, 0, 1, -center.z},
        {0, 0, 0, 1}
    }};

    return mat4_mul(rotation, translation);
}

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

// NOTE: angle in rad
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

// Model matrix = Translate * Rotate * Scale
// Applied right to left: scale, rotate, then translate
static inline Mat4 transform_matrix(const Transform *t) {
    Mat4 r = mat4_mul(rotate_y(t->rotation.y),
             mat4_mul(rotate_x(t->rotation.x),
                      rotate_z(t->rotation.z)));
    return mat4_mul(translate(t->position),
           mat4_mul(r, scale(t->scale)));
}

/* ---------- Camera ---------- */

typedef struct {
    Mat4 proj_view;  // Perspective * View (world space -> clip space)
    Mat4 viewport;   // NDC -> pixels
} Camera;

static inline Camera camera_make(Vec3 eye, Vec3 center, Vec3 up, int w, int h) {
    Vec3 d = vec3_sub(eye, center);
    double f = sqrt(d.x*d.x + d.y*d.y + d.z*d.z);   // focal distance = |eye-center|

    Camera cam;
    cam.proj_view = mat4_mul(perspective(f), look_at(eye, center, up));
    cam.viewport  = viewport(w/16, h/16, w*7/8, h*7/8);
    return cam;
}