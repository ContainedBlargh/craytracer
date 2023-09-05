#include "color.h"
#include <math.h>
#include "fail.h"

static void *global_format = NULL;
// Uint32 SDL_MapRGB(const SDL_PixelFormat *format, Uint8 r, Uint8 g, Uint8 b)
static u32 (*pixel_fun)(void *fmt, u8 r, u8 g, u8 b) = NULL;

void color_register_format(void *fmt, u32 (*fun)(void *fmt, u8 r, u8 g, u8 b))
{
    global_format = fmt;
    pixel_fun = fun;
}

u32 color_to_pixel(Color color)
{
    if (global_format == NULL) failwith("Color pixelformat not set!");
    if (pixel_fun == NULL) failwith("Color pixelformat not set!");
    Color nc = vabs(norm(color));
    return pixel_fun(global_format, (u8)truncf(nc.x * 255.0f), (u8)truncf(nc.y * 255.0f), (u8)truncf(nc.z * 255.0f));
}

Color color_mix(Color a, Color b)
{
    return vec3(
        (1.0 - b.x) * a.x + b.x,
        (1.0 - b.y) * a.y + b.y,
        (1.0 - b.z) * a.z + b.z
    );
}
