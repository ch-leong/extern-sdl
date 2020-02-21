#include <cstring>
#include "../extern/SDL2/include/SDL_test_common.h"
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#include "sdl.h"


int main(int argc, char* argv[])
{
    SDL sdl;
    memset(&sdl, 0, sizeof(sdl));
    sdl.argv = argv;
    sdl.Load();

    /* Main render loop */
    int done = 0;

    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    done = 1;
                }
            }
        }
        for (int i = 0; i < sdl.num_windows; ++i)
        {
            SDL_Renderer* renderer = sdl.renderers[i];
            if (sdl.windows[i] == NULL)
                continue;
            SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
            SDL_RenderClear(renderer);
            // Draw stuffs.
            SDL_RenderPresent(renderer);
        }
    }

    sdl.Unload();
    return 0;
}
