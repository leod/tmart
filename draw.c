#include "draw.h"

typedef struct Color {
    Uint8 r;
    Uint8 g;
    Uint8 b;
} Color;

Color colorscheme[] = {
    { 0, 0, 0},
    { 255, 0, 0 },
    { 0, 255, 0 },
    { 0, 0, 255 },
    { 255, 255, 0 },
    { 0, 255, 255 },
    { 255, 255, 255 }
};

void draw_tm_config(SDL_Surface *surface, const TM_Config *config) {
    if (SDL_MUSTLOCK(surface)) {
        if (SDL_LockSurface(surface) < 0)
            return;
    }

    int x, y, yTimesW;
    for (y = 0; y < surface->h; y++) {
        yTimesW = y * surface->pitch / 4;

        for (x = 0; x < surface->w; x++) {
            if (x >= config->tapeWidth || y >= config->tapeHeight)
                return;

            size_t tapeIndex = config->tapeWidth * y + x;
            TM_Symbol symbol = config->tape[tapeIndex];
            if (symbol > 6) symbol = 6;

            const Color *color = &colorscheme[symbol];
            Uint32 colorInt = SDL_MapRGB(surface->format,
                                         color->r, color->g, color->b);
            {
                Uint8* pixel = (Uint8*)surface->pixels;
                pixel += (y * surface->pitch) + (x * sizeof(Uint32));
                *((Uint32*)pixel) = colorInt;
            }
        }
    }

    if (SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);

    return;
}
