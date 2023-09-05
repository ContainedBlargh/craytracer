#include "sphere.h"
#include <math.h>

Sphere *new_sphere(Vec3 center, f32 radius, Color color)
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
    f32 radius2 = (sphere->radius * sphere->radius);
    Vec3 center_dir = vsub(sphere->center, ray->origin);
    f32 origin_to_center = dot(center_dir, ray->direction);
    
    if (origin_to_center < 0.0f) return no_Hit();

    f32 offset = dot(center_dir, center_dir) - (origin_to_center * origin_to_center);
    if (offset > (radius2 + eps)) return no_Hit();
    
    f32 hit_to_center = sqrtf(radius2 - offset);
    f32 distance = min(origin_to_center - hit_to_center, origin_to_center + hit_to_center);
    if (distance < 0.0f)
    {
        return no_Hit();
    }

    Vec3 hit_point = vadd(ray->origin, smul(ray->direction, distance));
    Vec3 hit_norm = norm(vsub(hit_point, sphere->center));
    Vec3 hit_to_center_vec = norm(vsub(sphere->center, hit_point));
    if (dot(hit_norm, hit_to_center_vec) < 0.1) {
        hit_norm = smul(hit_norm, -1.0f);
    }
    return some_Hit((Hit) {
        .color = sphere->color,
        .distance = dist(ray->origin, hit_point),
        .position = hit_point,
        .norm = hit_norm
    });
}
