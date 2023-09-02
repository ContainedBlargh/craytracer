#include "parser.h"
#include "cJSON.h"
#include "fail.h"
#include "hashmap.h"
#include "list.h"
#include "util.h"
#include <math.h>
#include <string.h>

Vec3 parse_vec3(cJSON *vector)
{
    if (vector == NULL) {
        failwith("Cannot parse vector, it was a null pointer!\n");
    }
    if (cJSON_HasObjectItem(vector, "x"))
    {
        return vec3(cJSON_GetObjectItem(vector, "x")->valuedouble, cJSON_GetObjectItem(vector, "y")->valuedouble,
                    cJSON_GetObjectItem(vector, "z")->valuedouble);
    }
    if (cJSON_HasObjectItem(vector, "r"))
    {
        return vec3(cJSON_GetObjectItem(vector, "r")->valuedouble, cJSON_GetObjectItem(vector, "g")->valuedouble,
                    cJSON_GetObjectItem(vector, "b")->valuedouble);
    }
    failwith("Could not parse vector!\n");
}

Camera *parse_camera(cJSON *root)
{
    cJSON *camera = cJSON_GetObjectItem(root, "camera");
    if (camera == NULL)
    {
        failwith("Failed to parse scene from JSON: No camera object found!\n");
    }
    cJSON *position = cJSON_GetObjectItem(camera, "position");
    if (position == NULL)
    {
        failwith("Camera had no position vector!\n");
    }
    cJSON *direction = cJSON_GetObjectItem(camera, "direction");
    if (direction == NULL)
    {
        failwith("Camera had no direction vector!\n");
    }
    return new_camera(parse_vec3(position), parse_vec3(direction));
}

void parse_lights(Scene *scene, cJSON *root)
{
    cJSON *lights = cJSON_GetObjectItem(root, "lights");
    if (lights == NULL)
    {
        fprintf(stderr, "Warning, no lights parsed from scene ...\n");
    }
    int lights_count = cJSON_GetArraySize(lights);
    for (u32 i = 0; i < lights_count; i++)
    {
        cJSON *light = cJSON_GetArrayItem(lights, i);
        cJSON *position = cJSON_GetObjectItem(light, "position");
        if (position == NULL)
            continue;
        cJSON *color = cJSON_GetObjectItem(light, "color");
        if (color == NULL)
            continue;
        scene_add_light(scene, (Light){.position = parse_vec3(position), .color = parse_vec3(color)});
    }
}

void parse_shapes(Scene *scene, cJSON *root)
{
    cJSON *shapes = cJSON_GetObjectItem(root, "shapes");
    if (shapes == NULL)
    {
        return;
    }
    u32 shapes_count = cJSON_GetArraySize(shapes);
    for (int i = 0; i < shapes_count; i++)
    {
        cJSON *shape = cJSON_GetArrayItem(shapes, i);

        cJSON *type = cJSON_GetObjectItem(shape, "type");

        if (strcasecmp(type->valuestring, "sphere") == 0) {
            cJSON *color = cJSON_GetObjectItem(shape, "color");
            cJSON *radius = cJSON_GetObjectItem(shape, "radius");
            cJSON *center = cJSON_GetObjectItem(shape, "center");

            Sphere *sphere = new_sphere(parse_vec3(center), radius->valuedouble, parse_vec3(color));
            scene_add_sphere(scene, sphere);
        }
    }
}

Scene *parse_scene(char *path)
{
    char *contents = read_file(path);
    cJSON *root = cJSON_Parse(contents);
    if (root == NULL)
    {
        const char *error = cJSON_GetErrorPtr();
        if (error != NULL)
        {
            cJSON_Delete(root);
            failwithf("Error parsing JSON: %s\n", error);
        }
    }
    Scene *scene = new_scene();
    Camera *camera = parse_camera(root);
    scene_set_camera(scene, camera);
    parse_lights(scene, root);
    parse_shapes(scene, root);
    free(contents);
    return scene;
}