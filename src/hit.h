#ifndef HIT_H
#define HIT_H
/**
 * @file hit.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module for 'Hit' structs.
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 * 
 */

#include "vec3.h"
#include "color.h"
#include "option.h"

typedef struct __attribute__((packed)) _Hit
{
    Color color;
    f32 distance;
    Vec3 position;
    Vec3 norm;
} Hit;

option_type(Hit);

#endif
