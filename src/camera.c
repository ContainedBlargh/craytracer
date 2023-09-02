#include "camera.h"
#include <math.h>

Camera *new_camera(Vec3 pos, Vec3 dir)
{
    Camera *camera = malloc(sizeof(Camera));
    camera->position = pos;
    camera->direction = dir;
    camera->right = norm(cross(camera->direction, vec3(0.0, 1.0, 0.0)));
    camera->up = norm(cross(camera->right, camera->direction));
    return camera;
}

#define PI 3.14159

Ray *setup_perspective_rays(Camera *camera, u32 canvas_width, u32 canvas_height)
{
    printf("Allocating a lot of rays, actually %u\n", canvas_height * canvas_width);
    Ray *rays = calloc(canvas_width * canvas_height, sizeof(Ray));
    f32 aspect_ratio = (f32)canvas_width / (f32)canvas_height;
    f32 fov = 90.0; // Field of view in degrees

    // Calculate the height and width of the image plane at distance 1
    f32 half_height = tanf(fov * 0.5 * PI / 180.0);
    f32 half_width = aspect_ratio * half_height;

    // Calculate the step sizes for moving between pixels on the image plane
    f32 step_x = 2.0 * half_width / (f32)(canvas_width - 1);
    f32 step_y = 2.0 * half_height / (f32)(canvas_height - 1);

    // Starting position on the top-left corner of the image plane
    // Vec3 image_plane_top_left =
    //     vsub(camera->position, vadd(smul(camera->direction, 1.0), smul(camera->up, half_height)));

    for (u32 y = 0; y < canvas_height; y++)
    {
        for (u32 x = 0; x < canvas_width; x++)
        {
            f32 sx = half_width - x * step_x;
            f32 sy = half_height - y * step_y;

            Vec3 ray_direction = norm(vadd(vadd(camera->direction, smul(camera->right, sx)), smul(camera->up, sy)));

            u32 ray_index = y * canvas_width + x;
            rays[ray_index].origin = camera->position;
            rays[ray_index].direction = ray_direction;
        }
    }
    return rays;
}
