#ifndef SCENE_H
#define SCENE_H
/**
 * @file scene.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief Contains a scene.
 * @version 0.1
 * @date 2023-08-27
 *
 * @copyright WingCorp (c) 2023
 * 
 */

#include "sphere.h"
#include "camera.h"

typedef struct _Scene Scene;

Scene* new_scene(Camera *camera);
void scene_add_sphere(Scene *scene, Sphere *sphere);
HitOption scene_intersect(Scene *scene, Ray *ray);
void scene_free(Scene *scene);

#endif