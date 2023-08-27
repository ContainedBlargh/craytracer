#ifndef LIGHT_H
#define LIGHT_H
/**
 * @file light.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A point light implementation
 * @version 0.1
 * @date 2023-08-27
 *
 * @copyright WingCorp (c) 2023
 * 
 */

#include "vec3.h"
#include "color.h"

typedef struct __attribute__((packed)) _Light
{
    Vec3 position;
    Color color;
} Light;

#endif