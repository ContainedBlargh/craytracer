#include "vec3.h"
#include <math.h>

#define sq(expr) (expr) * (expr)

Vec3 vec3(const f32 x, const f32 y, const f32 z) {
    return (Vec3){.x = x, .y = y, .z = z};
}

f32 mag(const Vec3 v) {
    return sqrtf(sq(v.x) + sq(v.y) + sq(v.z));
}

Vec3 norm(const Vec3 v) {
    const f32 m = mag(v);
    return vec3(v.x / m, v.y / m, v.z / m);
}

f32 dist(const Vec3 a, const Vec3 b) {
    return sqrtf(sq(b.x - a.x) + sq(b.y - a.y) + sq(b.z - a.z));
}

f32 dot(const Vec3 a, const Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 smul(Vec3 v, f32 s) {
    return vec3(v.x * s, v.y * s, v.z * s);
}

Vec3 vmul(Vec3 a, Vec3 b) {
    return vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vec3 vadd(Vec3 a, Vec3 b) {
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 vsub(Vec3 a, Vec3 b) {
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 cross(Vec3 a, Vec3 b) {
    return vec3(
        a.y * b.z - a.z * b.y, 
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

Vec3 vabs(Vec3 v) {
    return vec3(fabsf(v.x), fabsf(v.y), fabsf(v.z));
}
