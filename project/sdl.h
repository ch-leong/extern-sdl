#pragma once
#include "../extern/SDL2/include/SDL.h"

class SDL
{
public:
    bool Load();
    bool Unload();

    /* SDL init flags */
    char** argv;
    Uint32 flags;
    Uint32 verbose;

    /* Video info */
    const char* videodriver;
    int display;
    const char* window_title;
    const char* window_icon;
    Uint32 window_flags;
    int window_x;
    int window_y;
    int window_w;
    int window_h;
    int window_minW;
    int window_minH;
    int window_maxW;
    int window_maxH;
    int logical_w;
    int logical_h;
    float scale;
    int depth;
    int refresh_rate;
    int num_windows;
    SDL_Window** windows;

    /* Renderer info */
    const char* renderdriver;
    Uint32 render_flags;
    SDL_bool skip_renderer;
    SDL_Renderer** renderers;
    SDL_Texture** targets;

    /* Audio info */
    const char* audiodriver;
    SDL_AudioSpec audiospec;

    /* GL settings */
    int gl_red_size;
    int gl_green_size;
    int gl_blue_size;
    int gl_alpha_size;
    int gl_buffer_size;
    int gl_depth_size;
    int gl_stencil_size;
    int gl_double_buffer;
    int gl_accum_red_size;
    int gl_accum_green_size;
    int gl_accum_blue_size;
    int gl_accum_alpha_size;
    int gl_stereo;
    int gl_multisamplebuffers;
    int gl_multisamplesamples;
    int gl_retained_backing;
    int gl_accelerated;
    int gl_major_version;
    int gl_minor_version;
    int gl_debug;
    int gl_profile_mask;
};
