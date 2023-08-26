#include "scene.h"
#include "fail.h"

typedef struct _Scene
{
    Camera *camera;
    Sphere **spheres;
    u16 count;
    u16 capacity;
} Scene;

Scene *new_scene(Camera *camera)
{
    Scene *s = malloc(sizeof(Scene));
    s->camera = camera;
    s->count = 0;
    s->capacity = 16;
    s->spheres = calloc(s->capacity, sizeof(Sphere*));
    return s;
}

void scene_add_sphere(Scene *scene, Sphere *sphere)
{
    scene->spheres[scene->count] = sphere;
    scene->count++;
    if (scene->count >= scene->capacity) {
        scene->capacity *= 2;
        Sphere **new_mem = realloc(scene->spheres, sizeof(Sphere*) * scene->capacity);
        if (new_mem == NULL) {
            failwith("Failed to allocate more space for spheres... you're probably out of memory!\n");
        }
        scene->spheres = new_mem;
    }
}

HitOption scene_intersect(Scene *scene, Ray *ray)
{
    HitOption current_, closest_ = no_Hit();
    f32 closest_dist = 999999.0f;
    for (u16 i = 0; i < scene->count; i++) {
        Sphere *s = scene->spheres[i];
        current_ = sphere_intersect(s, ray);
        if (is_some(current_) && current_.value.distance < closest_dist) {
            closest_ = current_;
        }
    }
    return closest_;
}

void scene_free(Scene *scene)
{
    for (u16 i = 0; i < scene->count; i++) {
        free(scene->spheres[i]);
    }
    free(scene->spheres);
    free(scene);
}
