#define SDL_MAIN_HANDLED
#include "camera.h"
#include "color.h"
#include "defs.h"
#include "fail.h"
#include "parser.h"
#include "scene.h"
#include "sphere.h"
#include "vec3.h"
#include <SDL2/SDL.h>
#include <math.h>
#ifdef __linux__
#include <unistd.h>
#else
#include <getopt.h>
#endif

static u8 cpu_count = 12;

typedef struct _RayWorkerArgs
{
    u16 id;
    SDL_Surface *canvas;
    u64 offset;
    u64 amount;
    Ray *rays;
    Scene *scene;
} RayWorkerArgs;

int fire_rays(void *args)
{
    RayWorkerArgs *wargs = (RayWorkerArgs *)args;

    SDL_Surface *canvas = wargs->canvas;

    Ray *rays = wargs->rays;
    Scene *scene = wargs->scene;

    u64 offset = wargs->offset;
    u64 amount = wargs->amount;

    u32 *raster = canvas->pixels;

    HitOption hit_;
    printf("rw[%hu]: Firing %llu rays.\n", wargs->id, amount);
    for (u32 i = offset; i < offset + amount; i += 3)
    {
        SDL_LockSurface(canvas);
        for (u32 j = 0; j < 3; j++)
        {
            hit_ = trace_ray(scene, rays + i + j);
            if (is_some(hit_))
            {
                raster[i + j] = color_to_pixel(hit_.value.color);
            }
        }
        SDL_UnlockSurface(canvas);
    }
    printf("rw[%hu]: Done!\n", wargs->id);
    return 0;
}

typedef struct _RenderArgs
{
    Scene *scene;
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

    Scene *scene = rargs->scene;
    Camera *camera = scene_get_camera(scene); // new_camera(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0));
    Ray *rays = setup_perspective_rays(camera, canvas->w, canvas->h);

    SDL_Thread **workers = calloc(cpu_count, sizeof(SDL_Thread *));
    RayWorkerArgs **wargs = calloc(cpu_count, sizeof(RayWorkerArgs *));
    u64 canvas_size = canvas->w * canvas->h;
    u64 parallelism = canvas_size / ((u64)cpu_count);
    printf("Parallelism: %llu rays per thread, %llu rays in total.\n", parallelism, canvas_size);
    for (u32 i = 0; i < cpu_count; i++)
    {
        RayWorkerArgs *rwargs = malloc(sizeof(RayWorkerArgs));
        rwargs->id = i;
        rwargs->offset = i * parallelism;
        rwargs->amount = i < (cpu_count - 1) ? parallelism : canvas_size - (parallelism * i);
        rwargs->canvas = canvas;
        rwargs->rays = rays;
        rwargs->scene = scene;
        wargs[i] = rwargs;
        workers[i] = SDL_CreateThread(fire_rays, NULL, rwargs);
    }

#define render_surface()                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        if (SDL_AtomicGet(buffer_switched))                                                                            \
        {                                                                                                              \
            window_surface = SDL_GetWindowSurface(window);                                                             \
            SDL_BlitScaled(canvas, NULL, window_surface, NULL);                                                        \
            SDL_AtomicSet(buffer_switched, false);                                                                     \
        }                                                                                                              \
        SDL_BlitScaled(canvas, NULL, window_surface, NULL);                                                            \
        SDL_UpdateWindowSurface(window);                                                                               \
    } while (0)

    while (SDL_AtomicGet(running))
    {
        render_surface();
    }
    for (u32 i = 0; i < cpu_count; i++)
    {
        SDL_WaitThread(workers[i], NULL);
    }
    free(rays);
    free(workers);
    for (u32 i = 0; i < cpu_count; i++)
    {
        free(wargs[i]);
    }
    free(wargs);
    render_surface();
#undef render_surface
    return 0;
}

int main(int argc, char *argv[])
{
    int opt;
    u32 window_w = 1792, w = 1792, window_h = 768, h = 768;
    char *input_file = NULL;

    while ((opt = getopt(argc, argv, "w:h:c:i:")) != -1)
    {
        switch (opt)
        {
        case 'w':
            w = atoi(optarg);
            break;
        case 'h':
            h = atoi(optarg);
            break;
        case 'c':
            cpu_count = atoi(optarg);
            break;
        case 'i':
            input_file = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s [-w width] [-h height] [-c cpu_count] -i <input_scene.json>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (input_file == NULL)
    {
        fprintf(stderr, "An input file path is required.\n");
        fprintf(stderr, "Usage: %s [-w width] [-h height] [-c cpu_count] -i <input_scene.json>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    Scene *scene = parse_scene(input_file);
    scene_debug_print(scene);
    SDL_atomic_t *running = malloc(sizeof(SDL_atomic_t));
    running->value = true;
    SDL_atomic_t *buffer_switched = malloc(sizeof(SDL_atomic_t));
    buffer_switched->value = false;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        failwith("Could not initialize SDL2 library!\n");
    }
    SDL_Window *window = SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_w,
                                          window_h, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
    {
        failwith("Window not opened, failing...\n");
    }
    SDL_Surface *window_surface = SDL_GetWindowSurface(window);
    if (window_surface == NULL)
    {
        failwith("Failed to retrieve window surface!\n");
    }
    SDL_Surface *canvas = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGB888);
    SDL_PixelFormat *fmt = canvas->format;
    // Awful casting, but silences the warnings.
    color_register_format(fmt, (u32(*)(void *, u8, u8, u8)) & SDL_MapRGB);
    printf("color (1.0, 1.0, 1.0) -> %u\n", color_to_pixel(vec3(1.0, 1.0, 1.0)));
    RenderArgs *rargs = malloc(sizeof(RenderArgs));
    rargs->buffer_switched = buffer_switched;
    rargs->running = running;
    rargs->canvas = canvas;
    rargs->window_surface = window_surface;
    rargs->window = window;
    rargs->scene = scene;
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
    scene_free(scene);
    return 0;
}
