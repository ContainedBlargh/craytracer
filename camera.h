#ifndef CAMERA_H
#define CAMERA_H
/**
 * @file camera.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module that represents a camera for looking at scenes.
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 * 
 */

#include "defs.h"
#include "vec3.h"
#include "ray.h"

typedef struct _Camera
{
    Vec3 position;
    Vec3 direction;
    Vec3 right;
    Vec3 up;
} Camera;

Camera *new_camera(Vec3 pos, Vec3 dir);

Ray *setup_perspective_rays(Camera *camera, u32 canvas_width, u32 canvas_height);

#endif