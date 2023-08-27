#ifndef COLOR_H
#define COLOR_H
/**
 * @file color.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module for using colors. Useful when you want to compose colors later on I guess.
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 * 
 */
#include "defs.h"
#include "vec3.h"

typedef Vec3 Color;

Color color_mix(Color a, Color b);
void color_register_format(void *fmt, u32 (*fun)(void *fmt, u8 r, u8 g, u8 b));
u32 color_to_pixel(Color color);

#endif