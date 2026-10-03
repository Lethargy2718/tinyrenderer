#pragma once
#include <math.h>

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} Vec4;

// Vec2

static inline Vec2 vec2_add(Vec2 a, Vec2 b) {
    return (Vec2){ a.x + b.x, a.y + b.y };
}

static inline Vec2 vec2_sub(Vec2 a, Vec2 b) {
    return (Vec2){ a.x - b.x, a.y - b.y };
}

static inline Vec2 vec2_scale(Vec2 a, float s) {
    return (Vec2){ a.x * s, a.y * s };
}

static inline Vec2 vec2_neg(Vec2 a) {
    return (Vec2){ -a.x, -a.y };
}

static inline float vec2_dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

static inline float vec2_cross(Vec2 a, Vec2 b) {
    return a.x * b.y - a.y * b.x;
}

static inline float vec2_length(Vec2 a) {
    return sqrtf(vec2_dot(a, a));
}

static inline Vec2 vec2_normalize(Vec2 a) {
    float len = vec2_length(a);
    if (len < 1e-8f) return (Vec2){ 0, 0 };
    return vec2_scale(a, 1.0f / len);
}

// Vec3

static inline Vec3 vec3_add(Vec3 a, Vec3 b) {
    return (Vec3){ a.x + b.x, a.y + b.y, a.z + b.z };
}

static inline Vec3 vec3_sub(Vec3 a, Vec3 b) {
    return (Vec3){ a.x - b.x, a.y - b.y, a.z - b.z };
}

static inline Vec3 vec3_scale(Vec3 a, float s) {
    return (Vec3){ a.x * s, a.y * s, a.z * s };
}

static inline Vec3 vec3_neg(Vec3 a) {
    return (Vec3){ -a.x, -a.y, -a.z };
}

static inline float vec3_dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline Vec3 vec3_cross(Vec3 a, Vec3 b) {
    return (Vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

static inline float vec3_length(Vec3 a) {
    return sqrtf(vec3_dot(a, a));
}

static inline Vec3 vec3_normalize(Vec3 a) {
    float len = vec3_length(a);
    if (len < 1e-8f) return (Vec3){ 0, 0, 0 };
    return vec3_scale(a, 1.0f / len);
}

// Vec4

static inline Vec4 vec4_add(Vec4 a, Vec4 b) {
    return (Vec4){ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

static inline Vec4 vec4_sub(Vec4 a, Vec4 b) {
    return (Vec4){ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

static inline Vec4 vec4_scale(Vec4 a, float s) {
    return (Vec4){ a.x * s, a.y * s, a.z * s, a.w * s };
}

static inline Vec4 vec4_neg(Vec4 a) {
    return (Vec4){ -a.x, -a.y, -a.z, -a.w };
}

static inline float vec4_dot(Vec4 a, Vec4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static inline float vec4_length(Vec4 a) {
    return sqrtf(vec4_dot(a, a));
}

static inline Vec4 vec4_normalize(Vec4 a) {
    float len = vec4_length(a);
    if (len < 1e-8f) return (Vec4){ 0, 0, 0, 0 };
    return vec4_scale(a, 1.0f / len);
}

// Conversions

static inline Vec2 vec3_to_vec2(Vec3 a) {
    return (Vec2){ a.x, a.y };
}

static inline Vec3 vec2_to_vec3(Vec2 a, float z) {
    return (Vec3){ a.x, a.y, z };
}

// drops w
static inline Vec3 vec4_to_vec3(Vec4 a) {
    return (Vec3){ a.x, a.y, a.z };
}

static inline Vec4 vec3_to_vec4(Vec3 a, float w) {
    return (Vec4){ a.x, a.y, a.z, w };
}

// perspective divide: (x/w, y/w, z/w)
static inline Vec3 vec4_to_vec3_persp(Vec4 a) {
    return (Vec3){ a.x / a.w, a.y / a.w, a.z / a.w };
}