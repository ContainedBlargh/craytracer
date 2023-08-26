
#pragma region OPTION_H
#ifndef OPTION_H
#define OPTION_H
/**
 * @file option.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module for defining and using Options made from pure macros 👌
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 *
 */
typedef enum __attribute__((packed)) _OptionType
{
    NONE,
    SOME
} OptionType;

#define option_type(T)                                                                                                 \
    typedef struct __attribute__((packed)) _##T##Option                                                                \
    {                                                                                                                  \
        OptionType type;                                                                                               \
        T value;                                                                                                       \
    } T##Option;                                                                                                       \
    T##Option no_##T()                                                                                                 \
    {                                                                                                                  \
        return (T##Option){.type = NONE};                                                                              \
    }                                                                                                                  \
    T##Option some_##T(T value)                                                                                        \
    {                                                                                                                  \
        return (T##Option){.type = SOME, .value = value};                                                              \
    }

#define is_some(option) ((option.type) == SOME)
#define is_none(option) ((option.type) == NONE)

#endif

#pragma endregion OPTION_H

#pragma region FAIL_H
#ifndef FAIL_H
#define FAIL_H

#include <stdbool.h>
#include <stdlib.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include <stdio.h>
#include <stdarg.h>

/**
 * @brief The fail-module.
 * 
 * @file fail.h
 * @author WingCorp (https://github.com/WingCorp)
 * @date 2018-06-19
 */

/**
 * @brief Exit the program with an error value.
 */
void fail();

#define FAIL_RED "\x1b[31m"
#define FAIL_RESET "\x1b[0m"

void trace_stack();

#define failwith(message)            \
    do {                             \
        fprintf(stderr, FAIL_RED);   \
        fprintf(stderr, message);    \
        fprintf(stderr, FAIL_RESET); \
        trace_stack();               \
        exit(EXIT_FAILURE);          \
    } while (0)

/**
 * @brief Print the error cause and exit the program with an error value.
 * 
 * @param cause to print before exiting. Use this like you would use printf.
 * @param VAR_ARGS, whatever you want it to say in your failure output.
 */
#define failwithf(format, ...)                      \
    do {                                            \
        fprintf(stderr, FAIL_RED);                  \
        fprintf(stderr, format, ##__VA_ARGS__);     \
        fprintf(stderr, FAIL_RESET);                \
        trace_stack();                              \
        exit(EXIT_FAILURE);                         \
    } while(0)                          

#endif

void* callstack[1024];
void trace_stack()
{
    #ifdef __linux__
    int frames = backtrace(callstack, 1024);
    char** strs = backtrace_symbols(callstack, frames);
    int i;
    fprintf(stderr, FAIL_RED);
    for (i = 0; i < frames; i++)
    {
        fprintf(stderr, "%s\n", strs[i]);
    }
    fprintf(stderr, FAIL_RESET);
    free(strs);
    #else
    fprintf(stderr, FAIL_RED);
    fprintf(stderr, "No backtrace available on mingw :<\n");
    fprintf(stderr, FAIL_RESET);
    #endif
}

void fail()
{
    trace_stack();
    exit(EXIT_FAILURE);
}
#pragma endregion FAIL_H

#pragma region DEFS_H
#ifndef DEFS_H
#define DEFS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux
#include <unistd.h>
#endif

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;


typedef float f32;
typedef double f64;

#endif

#pragma endregion DEFS_H

#pragma region COLOR_H
#ifndef COLOR_H
#define COLOR_H
/**
 * @file color.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module for using colors. Useful when you want to compose colors later on I guess.
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 * 
 */

#define rgb

#endif

#pragma endregion COLOR_H

#pragma region VEC3_H
#ifndef VEC3_H
#define VEC3_H
/**
 * @file vec3.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A vector module. Yay.
 * @version 0.1
 * @date 2023-08-21
 *
 * @copyright WingCorp (c) 2023
 * 
 */


typedef struct _Vec3
{
    f32 x;
    f32 y;
    f32 z;
} Vec3;

Vec3 vec3(f32 x, f32 y, f32 z);

f32 mag(Vec3 v);

Vec3 norm(Vec3 v);

f32 dist(Vec3 a, Vec3 b);

f32 dot(Vec3 a, Vec3 b);

Vec3 smul(Vec3 v, f32 s);

Vec3 vadd(Vec3 a, Vec3 b);

Vec3 vsub(Vec3 a, Vec3 b);

Vec3 cross(Vec3 a, Vec3 b);

#endif
#include <math.h>

#define sq(expr) (expr) * (expr)

Vec3 vec3(const f32 x, const f32 y, const f32 z)
{
    return (Vec3){.x = x, .y = y, .z = z};
}

f32 mag(const Vec3 v) {
    return sqrtf(sq(v.x) + sq(v.y) + sq(v.z));
}

Vec3 norm(const Vec3 v)
{
    const f32 m = mag(v);
    return vec3(v.x / m, v.y / m, v.z / m);
}

f32 dist(const Vec3 a, const Vec3 b) {
    return sqrtf(sq(b.x - a.x) + sq(b.y - a.y) + sq(b.z - a.z));
}

f32 dot(const Vec3 a, const Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 smul(Vec3 v, f32 s)
{
    return vec3(v.x * s, v.y * s, v.z * s);
}

Vec3 vadd(Vec3 a, Vec3 b)
{
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 vsub(Vec3 a, Vec3 b)
{
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 cross(Vec3 a, Vec3 b) {
    return vec3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

#pragma endregion VEC3_H

#pragma region RAY_H
#ifndef RAY_H
#define RAY_H
/**
 * @file ray.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module for rays.
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 * 
 */


typedef struct _Ray
{
    Vec3 origin;
    Vec3 direction;
} Ray;

#endif

#pragma endregion RAY_H

#pragma region CAMERA_H
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

#pragma endregion CAMERA_H

#pragma region HIT_H
#ifndef HIT_H
#define HIT_H
/**
 * @file hit.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module for 'Hit' structs.
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 * 
 */


typedef struct __attribute__((packed)) _Hit
{
    u32 color;
    f32 distance;
    Vec3 position;
    Vec3 norm;
} Hit;

option_type(Hit);

#endif


#pragma endregion HIT_H

#pragma region SPHERE_H
#ifndef SPHERE_H
#define SPHERE_H
/**
 * @file sphere.h
 * @author Jon Voigt Tøttrup (jvoi@itu.dk)
 *
 * @brief A module for spheres.
 * @version 0.1
 * @date 2023-08-23
 *
 * @copyright WingCorp (c) 2023
 * 
 */


typedef struct _Sphere
{
    Vec3 center;
    f32 radius;
    u32 color;
} Sphere;

Sphere* new_sphere(Vec3 center, f32 radius, u32 color);

HitOption sphere_intersect(Sphere *sphere, Ray *ray);

#endif


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

#pragma endregion SPHERE_H

#pragma region SCENE_H
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


typedef struct _Scene Scene;

Scene* new_scene(Camera *camera);
void scene_add_sphere(Scene *scene, Sphere *sphere);
HitOption scene_intersect(Scene *scene, Ray *ray);
void scene_free(Scene *scene);

#endif

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

#pragma endregion SCENE_H

#pragma region _\MAIN_C
#define SDL_MAIN_HANDLED
#include <math.h>
#include <SDL2/SDL.h>

#define CPU_COUNT 6

typedef struct _RayWorkerArgs
{
    u16 id;
    SDL_Surface *canvas;
    u32 offset;
    u32 amount;
    Ray *rays;
    Scene *scene;
} RayWorkerArgs;

int fire_rays(void *args) {
    RayWorkerArgs *wargs = (RayWorkerArgs *)args;

    SDL_Surface *canvas = wargs->canvas;

    Ray *rays = wargs->rays;
    Scene *scene = wargs->scene;
    
    u32 offset = wargs->offset;
    u32 amount = wargs->amount;

    u32 *raster = canvas->pixels;
    
    HitOption hit_;
    printf("rw[%hu]: Ready.\n", wargs->id);
    for (u32 i = offset; i < offset + amount; i += 3) {
        SDL_LockSurface(canvas);
        for (u32 j = 0; j < 3; j++) {
            hit_ = scene_intersect(scene, rays + i + j);
            if (is_some(hit_)) {
                raster[i + j] = hit_.value.color;
            }
        }
        SDL_UnlockSurface(canvas);
    }
    printf("rw[%hu]: Done!\n", wargs->id);
    return 0;
}

typedef struct _RenderArgs
{
    SDL_atomic_t *running;
    SDL_atomic_t *buffer_switched;
    SDL_Surface *canvas;
    SDL_Surface *window_surface;
    SDL_Window *window;
} RenderArgs;


int render(void *args)
{
    RenderArgs *rargs = (RenderArgs *)args;

    SDL_atomic_t *running = rargs->running;
    SDL_atomic_t *buffer_switched = rargs->buffer_switched;
    SDL_Window *window = rargs->window;
    SDL_Surface *window_surface = rargs->window_surface;
    SDL_Surface *canvas = rargs->canvas;
    SDL_PixelFormat *fmt = canvas->format;

    Camera *camera = new_camera(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0));
    Ray *rays = setup_perspective_rays(camera, canvas->w, canvas->h);
    Scene *scene = new_scene(camera);
    scene_add_sphere(scene, new_sphere(vec3(-0.4, 0.0, 2.0), 0.5, SDL_MapRGB(fmt, 0, 0, 255)));
    scene_add_sphere(scene, new_sphere(vec3(0.4, 0.0, 2.2), 0.5, SDL_MapRGB(fmt, 0, 255, 0)));
    scene_add_sphere(scene, new_sphere(vec3(0.0, 0.4, 2.1), 0.5, SDL_MapRGB(fmt, 255, 0, 0)));

    SDL_Thread **workers = calloc(CPU_COUNT, sizeof(SDL_Thread*));
    RayWorkerArgs *wargs = calloc(CPU_COUNT, sizeof(RayWorkerArgs));
    u32 canvas_size = canvas->w * canvas->h;
    u32 parallelism = canvas_size / CPU_COUNT;
    printf("Parallelism: %u rays per thread.\n", parallelism);
    char **names = calloc(CPU_COUNT, sizeof(char*));
    for (u32 i = 0; i < CPU_COUNT; i++) {
        wargs[i].id = i;
        wargs[i].offset = i * parallelism;
        wargs[i].amount = i < CPU_COUNT - 1 ? parallelism : canvas_size - i * parallelism;
        wargs[i].canvas = canvas;
        wargs[i].rays = rays;
        wargs[i].scene = scene;
        names[i] = malloc(sizeof(char) * 32);
        sprintf_s(names[i], 32, "RayWorker %u", i);
        workers[i] = SDL_CreateThread(fire_rays, names[i], &wargs[i]);
    }

    while(SDL_AtomicGet(running))
    {
        if (SDL_AtomicGet(buffer_switched)) {
            window_surface = SDL_GetWindowSurface(window);
            SDL_BlitScaled(canvas, NULL, window_surface, NULL);
            SDL_UpdateWindowSurface(window);
            SDL_AtomicSet(buffer_switched, false);
        }
    }
    for (u32 i = 0; i < CPU_COUNT; i++) {
        SDL_WaitThread(workers[i], NULL);
        free(names[i]);
    }
    scene_free(scene);
    free(names);
    free(rays);
    free(workers);

    return 0;
}

int main(int argc, char *argv[])
{
    SDL_atomic_t *running = malloc(sizeof(SDL_atomic_t));
    running->value = true;
    SDL_atomic_t *buffer_switched = malloc(sizeof(SDL_atomic_t));
    buffer_switched->value = false;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        failwith("Could not initialize SDL2 library!\n");
    }
    SDL_Window *window = SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
    {
        failwith("Window not opened, failing...\n");
    }
    SDL_Surface *window_surface = SDL_GetWindowSurface(window);
    if (window_surface == NULL)
    {
        failwith("Failed to retrieve window surface!\n");
    }
    SDL_Surface *canvas = SDL_CreateRGBSurfaceWithFormat(
        0, 
        1920, 
        1080, 
        32, 
        SDL_PIXELFORMAT_RGB888
    );
    RenderArgs *rargs = malloc(sizeof(RenderArgs));
    rargs->buffer_switched = buffer_switched;
    rargs->running = running;
    rargs->canvas = canvas;
    rargs->window_surface = window_surface;
    rargs->window = window;
    SDL_Thread *render_thread = SDL_CreateThread(render, "RENDER", rargs);
    SDL_Event e;
    while (SDL_AtomicGet(running))
    {
        while (SDL_PollEvent(&e) > 0)
        {
            if (e.type == SDL_QUIT)
            {
                SDL_AtomicSet(running, false);
                break;
            }
            SDL_AtomicSet(buffer_switched, true);
        }
    }
    SDL_WaitThread(render_thread, NULL);
    free(rargs);
    return 0;
}


#pragma endregion _\MAIN_C
