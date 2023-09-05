#include "plane.h"
#include <math.h>

Plane * new_plane(Vec3 pivot, Vec3 normal, Color color) {
    Plane *plane = malloc(sizeof(Plane));
    plane->pivot = pivot;
    plane->normal = norm(normal);
    plane->color = color;
    return plane;
}

HitOption plane_intersect(Plane * plane, Ray * ray) {
    f32 d = dot(plane->normal, ray->direction);
    if (fabsf(d) < eps) {
        return no_Hit();
    }

    Vec3 pivot_minus_origin = vsub(plane->pivot, ray->origin);
    f32 t = dot(pivot_minus_origin, plane->normal) / d;

    if (t < 0) {
        return no_Hit();
    }

    Vec3 hit_position = vadd(ray->origin, smul(ray->direction, t));
    return some_Hit((Hit) {
        .distance = dist(ray->origin, hit_position),
        .position = hit_position,
        .norm = plane->normal,
        .color = plane->color
    });
}
