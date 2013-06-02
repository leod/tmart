#include "draw.h"

typedef struct Color {
    Uint8 r;
    Uint8 g;
    Uint8 b;
} Color;

Color colorscheme[] = {
    { 0, 0, 0},
    { 255, 255, 255 },
    { 255, 0, 0 },
    { 0, 255, 0 },
    { 0, 0, 255 },
    { 255, 255, 0 },
    { 0, 255, 255 },
};

Color colorscheme2[] = {
    { 0, 0, 0 },
    { 10, 122, 166 },
    { 255, 137, 0 },
    { 242, 48, 5 },
    { 255, 184, 0 }
};

Color colorscheme3[] = {
    { 0, 0, 0 },
    { 115, 138, 2 },
    { 201, 5, 2 },
    { 255, 163, 13 },
    { 242, 85, 22 },
    { 102, 11, 85 }
};

void draw_tm_config(SDL_Surface *surface, const TM_Config *config,
                    size_t zoomLevel, int startX, int startY) {
    if (SDL_MUSTLOCK(surface)) {
        if (SDL_LockSurface(surface) < 0)
            return;
    }

    int x, y, u, v;
    for (y = startY; y < config->tapeHeight; y++) {
        for (x = startX; x < config->tapeWidth; x++) {
            size_t tapeIndex = config->tapeWidth * y + x;
            TM_Symbol symbol = config->tape[tapeIndex];
            if (symbol > 5) symbol = 5;

            const Color *color = &colorscheme2[symbol];
            Uint32 colorInt = SDL_MapRGB(surface->format,
                                         color->r, color->g, color->b);

            int y0 = (y - startY) * zoomLevel;
            int x0 = (x - startX) * zoomLevel;
            for (v = 0; v < zoomLevel && y0 + v < surface->h; v++) {
                for (u = 0; u < zoomLevel && x0 + u < surface->w; u++) {
                    Uint8* pixel = (Uint8*)surface->pixels;
                    pixel += ((y0 + v) * surface->pitch) +
                              (x0 + u) * sizeof(Uint32);
                    *((Uint32*)pixel) = colorInt;
                }
            }
        }
    }

    if (SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);

    return;
}
