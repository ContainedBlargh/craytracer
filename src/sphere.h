#ifndef SPHERE_H
#define SPHERE_H
/**
 * @file sphere.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module for spheres.
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 * 
 */

#include "vec3.h"
#include "hit.h"
#include "ray.h"
#include "color.h"

typedef struct _Sphere
{
    Vec3 center;
    f32 radius;
    Color color;
} Sphere;

Sphere* new_sphere(Vec3 center, f32 radius, Color color);

HitOption sphere_intersect(Sphere *sphere, Ray *ray);

#endif
