#ifndef PLANE_H
#define PLANE_H
/**
 * @file plane.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief
 * @version 0.1
 * @date 2023-09-05
 *
 * @copyright WingCorp (c) 2023
 *
 */

#include "color.h"
#include "hit.h"
#include "ray.h"
#include "vec3.h"

typedef struct _Plane {
    Vec3 pivot;
    Vec3 normal;
    Color color;
} Plane;

Plane *new_plane(Vec3 pivot, Vec3 normal, Color color);

HitOption plane_intersect(Plane *plane, Ray *ray);
#endif