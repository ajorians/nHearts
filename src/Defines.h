#ifndef DEFINES_H
#define DEFINES_H

extern "C"
{
        #include <os.h>
        #include "SDL/SDL.h"
}

#ifndef DEBUG_MSG
#define DEBUG_MSG//     printf
#endif

#define CARD_WIDTH      (42)
#define CARD_HEIGHT     (62)

#define GAME_BACKGROUND_R       (153)
#define GAME_BACKGROUND_G       (153)
#define GAME_BACKGROUND_B       (102)

int Puz_Min(int a, int b);
int Puz_Max(int a, int b);
int Puz_NumberOfDigits(int n);
void Puz_itoa(int n, char buffer[], int nBufferSize);

void draw_rectangle(SDL_Surface* Surface, Uint32 color, Uint16 x, Uint16 y, Uint16 width, Uint16 height, Uint8 lnpx );

Uint16 get_pixel16( SDL_Surface *surface, int x, int y );
void put_pixel16( SDL_Surface *surface, int x, int y, Uint16 pixel );

#endif