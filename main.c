#define SDL_MAIN_HANDLED
#include <math.h>
#include "color.h"
#include "defs.h"
#include "fail.h"
#include "sphere.h"
#include "vec3.h"
#include "camera.h"
#include <SDL2/SDL.h>
#include "scene.h"

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
