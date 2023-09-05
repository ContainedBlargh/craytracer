#include "scene.h"
#include "fail.h"
#include "list.h"

#ifndef SpherePtrList_T
typedef Sphere *SpherePtr;
list_type(SpherePtr);
#define SpherePtrList_T
#endif
#ifndef LightList_T
list_type(Light);
#define LightList_T
#endif
typedef Plane* PlanePtr;
#ifndef PlanePtrList_T
#define PlanePtrList_T
list_type(PlanePtr);
#endif

typedef struct _Scene
{
    Camera *camera;
    LightList *lights;
    SpherePtrList *spheres;
    PlanePtrList *planes;
} Scene;

Scene *new_scene()
{
    Scene *s = malloc(sizeof(Scene));
    s->camera = NULL;
    s->lights = new_LightList(4);
    s->spheres = new_SpherePtrList(8);
    s->planes = new_PlanePtrList(3);
    return s;
}

void scene_set_camera(Scene *scene, Camera *camera)
{
    scene->camera = camera;
}

void scene_add_sphere(Scene *scene, Sphere *sphere)
{
    SpherePtrList_add(scene->spheres, sphere);
}

void scene_add_light(Scene *scene, Light light)
{
    LightList_add(scene->lights, light);
}

void scene_add_plane(Scene *scene, Plane *plane) {
    PlanePtrList_add(scene->planes, plane);
}

static HitOption cast_ray(Scene *scene, Ray *ray)
{
    if (scene->camera == NULL) {
        failwith("No camera set in scene, cannot cast rays!\n");
    }
    HitOption closest_ = no_Hit();
    f32 closest_dist = 999999.0f;
    for (u16 i = 0; i < scene->planes->size; i++)
    {
        Plane *p = scene->planes->elements[i];
        HitOption current_ = plane_intersect(p, ray);
        if (is_some(current_) && current_.value.distance < closest_dist) {
            closest_ = current_;
        }
    }
    for (u16 i = 0; i < scene->spheres->size; i++)
    {
        Sphere *s = scene->spheres->elements[i];
        HitOption current_ = sphere_intersect(s, ray);
        if (is_some(current_) && current_.value.distance < closest_dist)
        {
            closest_ = current_;
        }
    }
    
    return closest_;
}

HitOption trace_ray(Scene *scene, Ray *ray)
{
    if (scene->camera == NULL) {
        failwith("No camera set in scene, cannot cast rays!\n");
    }
    HitOption closest_ = cast_ray(scene, ray);
    if (is_some(closest_))
    {
        // Compute the color depending on whether we are in darkness or not.
        bool reached_by_light = false;
        Ray shadow_ray;
        shadow_ray.origin = vadd(closest_.value.position, smul(closest_.value.norm, 0.01));
        for (u16 i = 0; i < scene->lights->size; i++)
        {
            Light l = scene->lights->elements[i];
            shadow_ray.direction = norm(vsub(l.position, shadow_ray.origin));
            HitOption shadow_hit_ = cast_ray(scene, &shadow_ray);
            if (is_none(shadow_hit_))
            {
                reached_by_light = true;
                closest_.value.color = color_mix(closest_.value.color, l.color);
            }
        }
        if (!reached_by_light)
        {
            closest_.value.color = smul(closest_.value.color, 0.5);
        }
    }
    return closest_;
}

void scene_free(Scene *scene)
{
    destroy_SpherePtrList(scene->spheres);
    destroy_LightList(scene->lights);
    free(scene);
}

void scene_debug_print(Scene *scene)
{
    printf("Scene debug:\n");
    printf(
        "\tCamera: { p: (%.2f, %.2f, %.2f), d: (%.2f, %.2f, %.2f) }\n",
        scene->camera->position.x,
        scene->camera->position.y,
        scene->camera->position.z,
        scene->camera->direction.x,
        scene->camera->direction.y,
        scene->camera->direction.z
    );
    for(u32 i = 0; i < scene->lights->size; i++) {
        printf(
            "\tLight: { o: (%.2f, %.2f, %.2f), c: (%.2f, %.2f, %.2f) }\n",
            scene->lights->elements[i].position.x,
            scene->lights->elements[i].position.y,
            scene->lights->elements[i].position.z,
            scene->lights->elements[i].color.x,
            scene->lights->elements[i].color.y,
            scene->lights->elements[i].color.z
        );
    }
    printf("Scene has %u spheres:\n", scene->spheres->size);
    for(u32 i = 0; i < scene->spheres->size; i++) {
        printf(
            "\tSphere: { o: (%.2f, %.2f, %.2f), r: %.2f, c: (%.2f, %.2f, %.2f) }\n",
            scene->spheres->elements[i]->center.x,
            scene->spheres->elements[i]->center.y,
            scene->spheres->elements[i]->center.z,
            scene->spheres->elements[i]->radius,
            scene->spheres->elements[i]->color.x,
            scene->spheres->elements[i]->color.y,
            scene->spheres->elements[i]->color.z
        );
    }
}

Camera *scene_get_camera(Scene *scene)
{
    if(scene->camera != NULL) {
        return scene->camera;
    }
    failwith("Tried to get camera from scene, but none was available :(\n");
}
