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
#include "plane.h"
#include "camera.h"
#include "light.h"

typedef struct _Scene Scene;

Scene* new_scene();
Camera *scene_get_camera(Scene *scene);
void scene_set_camera(Scene *scene, Camera *camera);
void scene_add_sphere(Scene *scene, Sphere *sphere);
void scene_add_plane(Scene *scene, Plane *plane);
void scene_add_light(Scene *scene, Light light);
HitOption trace_ray(Scene *scene, Ray *ray);
void scene_free(Scene *scene);
void scene_debug_print(Scene *scene);
#endif