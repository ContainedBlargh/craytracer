#ifndef RAY_H
#define RAY_H
/**
 * @file ray.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module for rays.
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 * 
 */

#include "vec3.h"

typedef struct _Ray
{
    Vec3 origin;
    Vec3 direction;
} Ray;

#endif