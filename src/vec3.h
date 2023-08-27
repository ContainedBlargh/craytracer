#ifndef VEC3_H
#define VEC3_H
/**
 * @file vec3.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A vector module. Yay.
 * @version 0.1
 * @date 2023-08-21
 *
 * @copyright WingCorp (c) 2023
 * 
 */

#include "defs.h"

typedef struct __attribute__((packed)) _Vec3
{
    f32 x;
    f32 y;
    f32 z;
} Vec3;

Vec3 vec3(f32 x, f32 y, f32 z);

f32 mag(Vec3 v);

Vec3 norm(Vec3 v);

f32 dist(Vec3 a, Vec3 b);

f32 dot(Vec3 a, Vec3 b);

Vec3 smul(Vec3 v, f32 s);

Vec3 vadd(Vec3 a, Vec3 b);

Vec3 vsub(Vec3 a, Vec3 b);

Vec3 cross(Vec3 a, Vec3 b);

Vec3 vabs(Vec3 v);

#endif