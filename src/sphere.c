#include "sphere.h"

Sphere *new_sphere(Vec3 center, f32 radius, u32 color)
{
    Sphere *sphere = malloc(sizeof(Sphere));
    sphere->center = center;
    sphere->radius = radius;
    sphere->color = color;
    return sphere;
}

/**
 * @brief Compute the intersect between a sphere and a ray.
 * 
 * @param sphere A sphere.
 * @param ray A ray.
 * @return HitOption An option of the Hit-persuasion. 
 */
HitOption sphere_intersect(Sphere *sphere, Ray *ray)
{
    Vec3 center_dir = sphere->center;
    f32 extent = dot(ray->direction, center_dir);
    if (extent < 0)
        return no_Hit();
    Vec3 offset = smul(norm(ray->direction), extent);
    Vec3 moved = vadd(ray->origin, offset);
    f32 distance = dist(moved, sphere->center);

    return distance > sphere->radius
               ? no_Hit()
               : some_Hit((Hit){.color = sphere->color, .distance = distance, .norm = norm(moved), .position = moved});
}
