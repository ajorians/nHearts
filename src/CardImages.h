#ifndef CARD_IMAGES_H
#define CARD_IMAGES_H

extern "C"
{
        #include <os.h>
        #include "SDL/SDL.h"
	#include "CardLib/CardLib.h"
}

#include "Defines.h"

class CardImages
{
public:
   CardImages();
   ~CardImages();

   bool GetImageForCard(SDL_Surface* pSurface, Card c, bool bEnabled, int nWidth = CARD_WIDTH, int nHeight = CARD_HEIGHT);
   bool GetImageForDeckStyle(SDL_Surface* pSurface, bool bHorizontal);

protected:
   void Tint(SDL_Surface* pSurface, Uint16 clrBlend);

protected:
   SDL_Surface* m_pCards;
};

#endif


