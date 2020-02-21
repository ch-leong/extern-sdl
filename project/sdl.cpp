#include "sdl.h"
#include <iostream>
#include "../extern/SDL2/include/SDL.h"

#define DEFAULT_WINDOW_WIDTH  640
#define DEFAULT_WINDOW_HEIGHT 480

#define VERBOSE_VIDEO   0x00000001
#define VERBOSE_MODES   0x00000002
#define VERBOSE_RENDER  0x00000004
#define VERBOSE_EVENT   0x00000008
#define VERBOSE_AUDIO   0x00000010

bool SDL::Load()
{
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    /* Initialize some defaults */
    flags = SDL_INIT_VIDEO;
    window_title = "title";
    window_flags = 0;
    window_x = SDL_WINDOWPOS_UNDEFINED;
    window_y = SDL_WINDOWPOS_UNDEFINED;
    window_w = DEFAULT_WINDOW_WIDTH;
    window_h = DEFAULT_WINDOW_HEIGHT;
    num_windows = 1;
    audiospec.freq = 22050;
    audiospec.format = AUDIO_S16;
    audiospec.channels = 2;
    audiospec.samples = 2048;

    /* Set some very sane GL defaults */
    gl_red_size = 3;
    gl_green_size = 3;
    gl_blue_size = 2;
    gl_alpha_size = 0;
    gl_buffer_size = 0;
    gl_depth_size = 16;
    gl_stencil_size = 0;
    gl_double_buffer = 1;
    gl_accum_red_size = 0;
    gl_accum_green_size = 0;
    gl_accum_blue_size = 0;
    gl_accum_alpha_size = 0;
    gl_stereo = 0;
    gl_multisamplebuffers = 0;
    gl_multisamplesamples = 0;
    gl_retained_backing = 1;
    gl_accelerated = -1;
    gl_debug = 0;

    int i, j, m, n, w, h;
    SDL_DisplayMode fullscreen_mode;
    char text[1024];

    if (flags & SDL_INIT_VIDEO) {
        if (verbose & VERBOSE_VIDEO) {
            n = SDL_GetNumVideoDrivers();
            if (n == 0) {
                SDL_Log("No built-in video drivers\n");
            }
        }
        if (SDL_VideoInit(videodriver) < 0) {
            SDL_Log("Couldn't initialize video driver: %s\n",
                SDL_GetError());
            return SDL_FALSE;
        }
        if (verbose & VERBOSE_VIDEO) {
            SDL_Log("Video driver: %s\n",
                SDL_GetCurrentVideoDriver());
        }

        /* Upload GL settings */
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, gl_red_size);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, gl_green_size);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, gl_blue_size);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, gl_alpha_size);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, gl_double_buffer);
        SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, gl_buffer_size);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, gl_depth_size);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, gl_stencil_size);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, gl_accum_red_size);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, gl_accum_green_size);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, gl_accum_blue_size);
        SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, gl_accum_alpha_size);
        SDL_GL_SetAttribute(SDL_GL_STEREO, gl_stereo);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, gl_multisamplebuffers);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, gl_multisamplesamples);
        if (gl_accelerated >= 0) {
            SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,
                gl_accelerated);
        }
        SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, gl_retained_backing);
        if (gl_major_version) {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_version);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_version);
        }
        if (gl_debug) {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
        }
        if (gl_profile_mask) {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl_profile_mask);
        }

        if (verbose & VERBOSE_MODES) {
            SDL_Rect bounds, usablebounds;
            float hdpi = 0;
            float vdpi = 0;
            SDL_DisplayMode mode;
            int bpp;
            Uint32 Rmask, Gmask, Bmask, Amask;
#if SDL_VIDEO_DRIVER_WINDOWS
            int adapterIndex = 0;
            int outputIndex = 0;
#endif
            n = SDL_GetNumVideoDisplays();
            SDL_Log("Number of displays: %d\n", n);
            for (i = 0; i < n; ++i) {
                SDL_Log("Display %d: %s\n", i, SDL_GetDisplayName(i));

                SDL_zero(bounds);
                SDL_GetDisplayBounds(i, &bounds);

                SDL_zero(usablebounds);
                SDL_GetDisplayUsableBounds(i, &usablebounds);

                SDL_GetDisplayDPI(i, NULL, &hdpi, &vdpi);

                SDL_Log("Bounds: %dx%d at %d,%d\n", bounds.w, bounds.h, bounds.x, bounds.y);
                SDL_Log("Usable bounds: %dx%d at %d,%d\n", usablebounds.w, usablebounds.h, usablebounds.x, usablebounds.y);
                SDL_Log("DPI: %fx%f\n", hdpi, vdpi);

                SDL_GetDesktopDisplayMode(i, &mode);
                SDL_PixelFormatEnumToMasks(mode.format, &bpp, &Rmask, &Gmask,
                    &Bmask, &Amask);
                SDL_Log("  Current mode: %dx%d@%dHz, %d bits-per-pixel (%s)\n",
                    mode.w, mode.h, mode.refresh_rate, bpp,
                    SDL_GetPixelFormatName(mode.format));
                if (Rmask || Gmask || Bmask) {
                    SDL_Log("      Red Mask   = 0x%.8x\n", Rmask);
                    SDL_Log("      Green Mask = 0x%.8x\n", Gmask);
                    SDL_Log("      Blue Mask  = 0x%.8x\n", Bmask);
                    if (Amask)
                        SDL_Log("      Alpha Mask = 0x%.8x\n", Amask);
                }

                /* Print available fullscreen video modes */
                m = SDL_GetNumDisplayModes(i);
                if (m == 0) {
                    SDL_Log("No available fullscreen video modes\n");
                }
                else {
                    SDL_Log("  Fullscreen video modes:\n");
                    for (j = 0; j < m; ++j) {
                        SDL_GetDisplayMode(i, j, &mode);
                        SDL_PixelFormatEnumToMasks(mode.format, &bpp, &Rmask,
                            &Gmask, &Bmask, &Amask);
                        SDL_Log("    Mode %d: %dx%d@%dHz, %d bits-per-pixel (%s)\n",
                            j, mode.w, mode.h, mode.refresh_rate, bpp,
                            SDL_GetPixelFormatName(mode.format));
                        if (Rmask || Gmask || Bmask) {
                            SDL_Log("        Red Mask   = 0x%.8x\n",
                                Rmask);
                            SDL_Log("        Green Mask = 0x%.8x\n",
                                Gmask);
                            SDL_Log("        Blue Mask  = 0x%.8x\n",
                                Bmask);
                            if (Amask)
                                SDL_Log("        Alpha Mask = 0x%.8x\n",
                                    Amask);
                        }
                    }
                }

#if SDL_VIDEO_DRIVER_WINDOWS
                /* Print the D3D9 adapter index */
                adapterIndex = SDL_Direct3D9GetAdapterIndex(i);
                SDL_Log("D3D9 Adapter Index: %d", adapterIndex);

                /* Print the DXGI adapter and output indices */
                SDL_DXGIGetOutputInfo(i, &adapterIndex, &outputIndex);
                SDL_Log("DXGI Adapter Index: %d  Output Index: %d", adapterIndex, outputIndex);
#endif
            }
        }

        if (verbose & VERBOSE_RENDER) {
            SDL_RendererInfo info;

            n = SDL_GetNumRenderDrivers();
            if (n == 0) {
                SDL_Log("No built-in render drivers\n");
            }
            else {
                SDL_Log("Built-in render drivers:\n");
                for (i = 0; i < n; ++i) {
                    SDL_GetRenderDriverInfo(i, &info);
                }
            }
        }

        SDL_zero(fullscreen_mode);
        switch (depth) {
        case 8:
            fullscreen_mode.format = SDL_PIXELFORMAT_INDEX8;
            break;
        case 15:
            fullscreen_mode.format = SDL_PIXELFORMAT_RGB555;
            break;
        case 16:
            fullscreen_mode.format = SDL_PIXELFORMAT_RGB565;
            break;
        case 24:
            fullscreen_mode.format = SDL_PIXELFORMAT_RGB24;
            break;
        default:
            fullscreen_mode.format = SDL_PIXELFORMAT_RGB888;
            break;
        }
        fullscreen_mode.refresh_rate = refresh_rate;

        windows =
            (SDL_Window**)SDL_calloc(num_windows,
                sizeof(*windows));
        renderers =
            (SDL_Renderer**)SDL_calloc(num_windows,
                sizeof(*renderers));
        targets =
            (SDL_Texture**)SDL_calloc(num_windows,
                sizeof(*targets));
        if (!windows || !renderers) {
            SDL_Log("Out of memory!\n");
            return SDL_FALSE;
        }
        for (i = 0; i < num_windows; ++i) {
            char title[1024];

            if (num_windows > 1) {
                SDL_snprintf(title, SDL_arraysize(title), "%s %d",
                    window_title, i + 1);
            }
            else {
                SDL_strlcpy(title, window_title, SDL_arraysize(title));
            }
            windows[i] =
                SDL_CreateWindow(title, window_x, window_y,
                    window_w, window_h,
                    window_flags);
            if (!windows[i]) {
                SDL_Log("Couldn't create window: %s\n",
                    SDL_GetError());
                return SDL_FALSE;
            }
            if (window_minW || window_minH) {
                SDL_SetWindowMinimumSize(windows[i], window_minW, window_minH);
            }
            if (window_maxW || window_maxH) {
                SDL_SetWindowMaximumSize(windows[i], window_maxW, window_maxH);
            }
            SDL_GetWindowSize(windows[i], &w, &h);
            if (!(window_flags & SDL_WINDOW_RESIZABLE) &&
                (w != window_w || h != window_h)) {
                printf("Window requested size %dx%d, got %dx%d\n", window_w, window_h, w, h);
                window_w = w;
                window_h = h;
            }
            if (SDL_SetWindowDisplayMode(windows[i], &fullscreen_mode) < 0) {
                SDL_Log("Can't set up fullscreen display mode: %s\n",
                    SDL_GetError());
                return SDL_FALSE;
            }

            /* Add resize/drag areas for windows that are borderless and resizable */
            //if ((window_flags & (SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS)) ==
            //    (SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS)) {
            //    SDL_SetWindowHitTest(windows[i], SDLTest_ExampleHitTestCallback, NULL);
            //}

            //if (window_icon) {
            //    SDL_Surface* icon = SDLTest_LoadIcon(window_icon);
            //    if (icon) {
            //        SDL_SetWindowIcon(windows[i], icon);
            //        SDL_FreeSurface(icon);
            //    }
            //}

            SDL_ShowWindow(windows[i]);

            if (!skip_renderer
                && (renderdriver
                    || !(window_flags & (SDL_WINDOW_OPENGL | SDL_WINDOW_VULKAN)))) {
                m = -1;
                if (renderdriver) {
                    SDL_RendererInfo info;
                    n = SDL_GetNumRenderDrivers();
                    for (j = 0; j < n; ++j) {
                        SDL_GetRenderDriverInfo(j, &info);
                        if (SDL_strcasecmp(info.name, renderdriver) ==
                            0) {
                            m = j;
                            break;
                        }
                    }
                    if (m == -1) {
                        SDL_Log("Couldn't find render driver named %s",
                            renderdriver);
                        return SDL_FALSE;
                    }
                }
                renderers[i] = SDL_CreateRenderer(windows[i],
                    m, render_flags);
                if (!renderers[i]) {
                    SDL_Log("Couldn't create renderer: %s\n",
                        SDL_GetError());
                    return SDL_FALSE;
                }
                if (logical_w && logical_h) {
                    SDL_RenderSetLogicalSize(renderers[i], logical_w, logical_h);
                }
                else if (scale != 0.) {
                    SDL_RenderSetScale(renderers[i], scale, scale);
                }
                if (verbose & VERBOSE_RENDER) {
                    SDL_RendererInfo info;

                    SDL_Log("Current renderer:\n");
                    SDL_GetRendererInfo(renderers[i], &info);
                }
            }
        }
    }

    if (flags & SDL_INIT_AUDIO) {
        if (verbose & VERBOSE_AUDIO) {
            n = SDL_GetNumAudioDrivers();
            if (n == 0) {
                SDL_Log("No built-in audio drivers\n");
            }
        }
        if (SDL_AudioInit(audiodriver) < 0) {
            SDL_Log("Couldn't initialize audio driver: %s\n",
                SDL_GetError());
            return SDL_FALSE;
        }
        if (verbose & VERBOSE_VIDEO) {
            SDL_Log("Audio driver: %s\n",
                SDL_GetCurrentAudioDriver());
        }

        if (SDL_OpenAudio(&audiospec, NULL) < 0) {
            SDL_Log("Couldn't open audio: %s\n", SDL_GetError());
            return SDL_FALSE;
        }
    }


    /* Create the windows and initialize the renderers */
    for (int i = 0; i < num_windows; ++i) {
        SDL_Renderer* renderer = renderers[i];
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
        SDL_RenderClear(renderer);
    }

    return true;
}

bool SDL::Unload()
{
    int i;

    SDL_free(windows);
    if (targets) {
        for (i = 0; i < num_windows; ++i) {
            if (targets[i]) {
                SDL_DestroyTexture(targets[i]);
            }
        }
        SDL_free(targets);
    }
    if (renderers) {
        for (i = 0; i < num_windows; ++i) {
            if (renderers[i]) {
                SDL_DestroyRenderer(renderers[i]);
            }
        }
        SDL_free(renderers);
    }
    if (flags & SDL_INIT_VIDEO) {
        SDL_VideoQuit();
    }
    if (flags & SDL_INIT_AUDIO) {
        SDL_AudioQuit();
    }
    SDL_Quit();

    return true;
}
