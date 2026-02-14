#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "useful.h"
#include "libretro.h"

#define WIDTH 640
#define HEIGHT 480
#define FPS 60.0
#define SAMPLE_RATE 44100.0
#define ASPECT_RATIO 4.0 / 3.0
#define PITCH WIDTH * 4

static struct retro_system_av_info g_av_info;
static struct retro_framebuffer g_framebuffer;
static retro_video_refresh_t g_video_refresh = NULL;
static uint32_t buf[WIDTH * HEIGHT] = {0};

void retro_init(void)
{
    srand((unsigned int)time(NULL));
    g_av_info.geometry.base_width = WIDTH;
    g_av_info.geometry.base_height = HEIGHT;
    g_av_info.geometry.max_width = WIDTH;
    g_av_info.geometry.max_height = HEIGHT;
    g_av_info.geometry.aspect_ratio = ASPECT_RATIO;
    g_av_info.timing.fps = FPS;
    g_av_info.timing.sample_rate = SAMPLE_RATE;

    g_framebuffer.width = WIDTH;
    g_framebuffer.height = HEIGHT;
    g_framebuffer.pitch = WIDTH * 4;
    g_framebuffer.format = RETRO_PIXEL_FORMAT_XRGB8888;
    g_framebuffer.data = buf;
    return;
}

void retro_deinit(void)
{
    return;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
    *info = g_av_info;
}

void retro_run(void)
{
    if (!g_video_refresh) return;
    for (size_t i = 0; i < ARRARY_LEN(buf); i++)
    {
        uint8_t r = rand(); // 0–255
        uint8_t g = rand(); // 0–255
        uint8_t b = rand();
        buf[i] = 0xFF << 24 | r << 16 | g << 8 | b;
    }

    g_video_refresh(buf, WIDTH, HEIGHT, PITCH);
    return;
}

void retro_reset(void)
{
    return;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
    g_video_refresh = cb;
}
