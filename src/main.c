#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <math.h>
#include <time.h>
#include "camera.h"
#include "color.h"
#include "defs.h"
#include "fail.h"
#include "parser.h"
#include "scene.h"
#include "sphere.h"
#include "vec3.h"
#ifdef __linux__
#include <unistd.h>
#else
#include <getopt.h>
#endif

static bool debug = false;
static clock_t start;
static u8 cpu_count = 12;
static u8 batch_size = 3;

typedef struct _RayWorkerArgs {
    u16 id;
    SDL_Surface *canvas;
    u64 offset;
    u64 amount;
    Ray *rays;
    Scene *scene;
    SDL_atomic_t *done;
} RayWorkerArgs;

int fire_rays(void *args) {
    RayWorkerArgs *wargs = (RayWorkerArgs *)args;

    SDL_Surface *canvas = wargs->canvas;
    SDL_atomic_t *done = wargs->done;

    Ray *rays = wargs->rays;
    Scene *scene = wargs->scene;

    u64 offset = wargs->offset;
    u64 amount = wargs->amount;

    u32 *raster = canvas->pixels;

    HitOption hit_;
    if (debug) {
        printf("rw[%hu]: Firing %llu rays.\n", wargs->id, amount);
    }
    u32 hit_queue[batch_size][3];
    u32 hits = 0;
    for (u32 i = offset; i < offset + amount; i += batch_size) {
        for (u32 j = 0; j < batch_size; j++) {
            hit_ = trace_ray(scene, rays + i + j);
            if (is_some(hit_)) {
                hit_queue[hits][0] = i;
                hit_queue[hits][1] = j;
                hit_queue[hits++][2] = color_to_pixel(hit_.value.color);

            }
        }
        for (u32 h = 0; h < hits; h++) {
            u32 *hit_data = hit_queue[h];
            raster[hit_data[0] + hit_data[1]] = hit_data[2];
        }
        hits = 0;
    }
    SDL_AtomicAdd(done, 1);
    if (debug) {
        printf("rw[%hu]: Done!\n", wargs->id);
    }
    return 0;
}

typedef struct _RenderArgs {
    Scene *scene;
    SDL_atomic_t *running;
    SDL_atomic_t *buffer_switched;
    SDL_Surface *canvas;
    SDL_Surface *window_surface;
    SDL_Window *window;
} RenderArgs;

int render(void *args) {
    RenderArgs *rargs = (RenderArgs *)args;

    SDL_atomic_t *running = rargs->running;
    SDL_atomic_t *buffer_switched = rargs->buffer_switched;
    SDL_Window *window = rargs->window;
    SDL_Surface *window_surface = rargs->window_surface;
    SDL_Surface *canvas = rargs->canvas;

#define render_surface()                                        \
    do {                                                        \
        if (SDL_AtomicGet(buffer_switched)) {                   \
            window_surface = SDL_GetWindowSurface(window);      \
            SDL_BlitScaled(canvas, NULL, window_surface, NULL); \
            SDL_AtomicSet(buffer_switched, false);              \
        }                                                       \
        SDL_BlitScaled(canvas, NULL, window_surface, NULL);     \
        SDL_UpdateWindowSurface(window);                        \
    } while (0);

    Scene *scene = rargs->scene;
    Camera *camera = scene_get_camera(
        scene);  // new_camera(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0));
    Ray *rays = setup_perspective_rays(camera, canvas->w, canvas->h);

    SDL_Thread **workers = calloc(cpu_count, sizeof(SDL_Thread *));
    RayWorkerArgs **wargs = calloc(cpu_count, sizeof(RayWorkerArgs *));
    u64 canvas_size = canvas->w * canvas->h;
    u64 parallelism = canvas_size / ((u64)cpu_count);
    if (debug) {
        printf("Parallelism: %llu rays per thread, %llu rays in total.\n",
            parallelism, canvas_size);
    }
    SDL_atomic_t *done = malloc(sizeof(SDL_atomic_t));
    SDL_AtomicSet(done, 0);
    for (u32 i = 0; i < cpu_count; i++) {
        RayWorkerArgs *rwargs = malloc(sizeof(RayWorkerArgs));
        rwargs->id = i;
        rwargs->offset = i * parallelism;
        rwargs->amount =
            i < (cpu_count - 1) ? parallelism : canvas_size - (parallelism * i);
        rwargs->canvas = canvas;
        rwargs->rays = rays;
        rwargs->scene = scene;
        rwargs->done = done;
        wargs[i] = rwargs;
        workers[i] = SDL_CreateThread(fire_rays, NULL, rwargs);
        render_surface();
    }

    while (SDL_AtomicGet(running)) {
        render_surface();
        if (SDL_AtomicGet(done) == cpu_count) {
            clock_t elapsed = clock() - start;
            i64 msec = elapsed * 1000 / CLOCKS_PER_SEC;
            printf("Render completed: %lld seconds, %lld milliseconds\n",
                msec / 1000, msec % 1000);
            SDL_AtomicSet(done, -1);
        }
    }
    render_surface();
    render_surface();

    for (u32 i = 0; i < cpu_count; i++) {
        render_surface();
        SDL_WaitThread(workers[i], NULL);
    }

    free(done);
    free(rays);
    free(workers);
    for (u32 i = 0; i < cpu_count; i++) {
        free(wargs[i]);
    }
    free(wargs);
    render_surface();
#undef render_surface
    return 0;
}

int main(int argc, char *argv[]) {
    int opt;
    u32 window_w = 1792, w = 1792, window_h = 768, h = 768;
    char *input_file = NULL;
    bool fullscreen = false;

    while ((opt = getopt(argc, argv, "w:h:c:b:i:df")) != -1) {
        switch (opt) {
            case 'w':
                w = atoi(optarg);
                break;
            case 'h':
                h = atoi(optarg);
                break;
            case 'c':
                cpu_count = atoi(optarg);
                break;
            case 'b':
                batch_size = atoi(optarg);
                break;
            case 'd':
                debug = true;
                break;
            case 'f':
                fullscreen = true;
                break;
            case 'i':
                input_file = optarg;
                break;
            default:
                fprintf(stderr,
                    "Usage: %s [-w width] [-h height] [-c cpu_count] [-b "
                    "batch_size] [-d] [-f] -i <input_scene.json>\n",
                    argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (input_file == NULL) {
        fprintf(stderr, "An input file path is required.\n");
        fprintf(stderr,
            "Usage: %s [-w width] [-h height] [-c cpu_count] [-b batch_size] "
            "[-d] [-f] -i <input_scene.json>\n",
            argv[0]);
        exit(EXIT_FAILURE);
    }

    Scene *scene = parse_scene(input_file);
    if (debug) {
        scene_debug_print(scene);
    }
    start = clock();
    SDL_atomic_t *running = malloc(sizeof(SDL_atomic_t));
    running->value = true;
    SDL_atomic_t *buffer_switched = malloc(sizeof(SDL_atomic_t));
    buffer_switched->value = false;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        failwith("Could not initialize SDL2 library!\n");
    }
    SDL_Window *window =
        fullscreen
            ? SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_CENTERED,
                  SDL_WINDOWPOS_CENTERED, window_w, window_h,
                  SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP)
            : SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_CENTERED,
                  SDL_WINDOWPOS_CENTERED, window_w, window_h,
                  SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        failwith("Window not opened, failing...\n");
    }
    SDL_Surface *window_surface = SDL_GetWindowSurface(window);
    if (window_surface == NULL) {
        failwith("Failed to retrieve window surface!\n");
    }
    SDL_Surface *canvas =
        SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGB888);
    SDL_PixelFormat *fmt = canvas->format;
    // Awful casting, but silences the warnings.
    color_register_format(fmt, (u32(*)(void *, u8, u8, u8)) & SDL_MapRGB);
    RenderArgs *rargs = malloc(sizeof(RenderArgs));
    rargs->buffer_switched = buffer_switched;
    rargs->running = running;
    rargs->canvas = canvas;
    rargs->window_surface = window_surface;
    rargs->window = window;
    rargs->scene = scene;

    SDL_Thread *render_thread = SDL_CreateThread(render, "RENDER", rargs);
    SDL_Event e;
    while (SDL_AtomicGet(running)) {
        while (SDL_PollEvent(&e) > 0) {
            if (e.type == SDL_QUIT) {
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
