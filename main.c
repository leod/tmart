#include <stdbool.h>
#include <stdio.h>

#include <SDL/SDL.h>

#include "tm.h"
#include "draw.h"

#define WIDTH 1920
#define HEIGHT 1200

#undef main
int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return 1;

    SDL_Surface *screen;

    int flags = SDL_FULLSCREEN | SDL_HWSURFACE;
    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, flags))) {
        SDL_Quit();
        return 1;
    }

    srand(time(NULL));

    size_t numStates = 5;
    size_t numSymbols = 4;

    TM_Def tmDef = tm_def_create_random(numStates, numSymbols);
    TM_Config tmConfig = tm_config_create(&tmDef, WIDTH, HEIGHT);

    tm_def_print(&tmDef);

    bool running = true;
    float nSteps = 50000;

    Uint32 deltaTime = 0;

    while (running) {
        Uint32 startTime = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_r:
                    tm_def_free(&tmDef);
                    tmDef = tm_def_create_random(numStates, numSymbols);

                    tm_config_free(&tmConfig);
                    tmConfig = tm_config_create(&tmDef, WIDTH, HEIGHT);

                    tm_def_print(&tmDef);
                    break;
                case SDLK_UP:
                    nSteps *= 1.5;
                    if (nSteps >= 2000000)
                        nSteps = 2000000;
                    printf("Steps/Second: %d\n", (int) nSteps);
                    break;
                case SDLK_DOWN:
                    nSteps /= 1.5;
                    if (nSteps <= 1) nSteps = 1;
                    printf("Steps/Second: %d", (int) nSteps);
                    break;
                case SDLK_ESCAPE:
                    running = false;
                    break;
                }

                break;
            }
        }

        //tm_config_print(&tmConfig);
        
        if (deltaTime / 1000.0f * nSteps >= 1) {
            tm_config_step(&tmConfig, deltaTime / 1000.0f * nSteps);
            deltaTime = 0;
        }

        draw_tm_config(screen, &tmConfig);

        SDL_Flip(screen);
        SDL_Delay(10);

        deltaTime += SDL_GetTicks() - startTime;
    }

    tm_def_free(&tmDef);
    tm_config_free(&tmConfig);

    SDL_Quit();
    return 0;
}
