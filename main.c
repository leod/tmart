#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include <SDL/SDL.h>

#include "tm.h"
#include "draw.h"

#define WIDTH 1920
#define HEIGHT 1080

#undef main
int main(int argc, char* argv[]) {
    srand(time(NULL));

    size_t numStates = 4;
    size_t numSymbols = 3;

    TM_Def tmDef;
    if (argc == 1) {
        tmDef = tm_def_create_random(numStates, numSymbols);
    } else {
        if (!tm_def_load(argv[1], &tmDef)) {
            fprintf(stderr, "Couldn't open TM file \"%s\"", argv[1]);
            return 1;
        }
    }
        
    TM_Config tmConfig = tm_config_create(&tmDef, WIDTH, HEIGHT);

    tm_def_print(&tmDef);

    bool running = true;
    float nSteps = 50000;

    size_t zoomLevel = 1;
    int startX = 0, startY = 0;

    Uint32 deltaTime = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return 1;

    SDL_Surface *screen;

    int flags = SDL_FULLSCREEN | SDL_HWSURFACE;
    if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, flags))) {
        SDL_Quit();
        return 1;
    }

    int mouseX = 0, mouseY = 0;

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
                    tm_config_free(&tmConfig);
                    tmConfig = tm_config_create(&tmDef, WIDTH, HEIGHT);
                    break;
                case SDLK_s: {
                    char filename[100];
                    time_t now = time(0);
                    strftime(filename, 100, "%Y%m%d_%H%M%S.tm",
                             localtime(&now));
                    tm_def_save(&tmDef, filename);
                    break;
                }

                case SDLK_SPACE:
                    tm_def_free(&tmDef);
                    tmDef = tm_def_create_random(numStates, numSymbols);

                    tm_config_free(&tmConfig);
                    tmConfig = tm_config_create(&tmDef, WIDTH, HEIGHT);

                    tm_def_print(&tmDef);
                    break;
                case SDLK_UP:
                    nSteps *= 1.5;
                    if (nSteps >= 100000000)
                        nSteps = 100000000;
                    printf("Steps/Second: %d\n", (int) nSteps);
                    break;
                case SDLK_DOWN:
                    nSteps /= 1.5;
                    if (nSteps <= 1) nSteps = 1;
                    printf("Steps/Second: %d\n", (int) nSteps);
                    break;
                case SDLK_ESCAPE:
                    running = false;
                    break;
                }

                break;
            case SDL_MOUSEBUTTONDOWN: {
                size_t prevZoom = zoomLevel;
                switch (event.button.button) {
                case 4:
                    zoomLevel *= 2;
                    goto Lrecalc;
                case 5:
                    if (zoomLevel > 1) zoomLevel /= 2;
                    goto Lrecalc;
                Lrecalc:
                    startX += mouseX / prevZoom - WIDTH / (zoomLevel * 2);
                    startY += mouseY / prevZoom - HEIGHT / (zoomLevel * 2);

                    if (startX < 0) startX = 0;
                    if (startY < 0) startY = 0;
                    if (startX >= WIDTH) startX = WIDTH - 1;
                    if (startY >= HEIGHT) startY = HEIGHT - 1;
                }

                break;
            }
            case SDL_MOUSEMOTION:
                mouseX = event.motion.x;
                mouseY = event.motion.y;

                break;
            }
        }

        {
            const int speed = 10;
            if (mouseX < 10 && startX > 0)
                startX -= speed;
            else if (mouseX > WIDTH - 10 && startX < WIDTH)
                startX += speed;
            if (mouseY < 10 && startY > 0)
                startY -= speed;
            else if (mouseY > HEIGHT - 10 && startY < HEIGHT)
                startY += speed;

            if (zoomLevel == 1) {
                startX = 0;
                startY = 0;
            }
        }

        //tm_config_print(&tmConfig);
        
        if (deltaTime / 1000.0f * nSteps >= 1) {
            tm_config_step(&tmConfig, deltaTime / 1000.0f * nSteps);
            deltaTime = 0;
        }

        draw_tm_config(screen, &tmConfig, zoomLevel, startX, startY);

        SDL_Flip(screen);
        SDL_FillRect(screen, NULL, 0);
        SDL_Delay(10);

        deltaTime += SDL_GetTicks() - startTime;
    }

    tm_def_free(&tmDef);
    tm_config_free(&tmConfig);

    SDL_Quit();
    return 0;
}
