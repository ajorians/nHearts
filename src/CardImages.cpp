#include "CardImages.h"

#include "CardGraphics.h"

CardImages::CardImages()
{
   m_pCards = nSDL_LoadImage(image_carddata);
}

CardImages::~CardImages()
{
   SDL_FreeSurface(m_pCards);
}

bool CardImages::GetImageForCard(SDL_Surface* pSurface, Card c, int nWidth /*= CARD_WIDTH*/, int nHeight /*= CARD_HEIGHT*/)
{
   int nSuit = GetSuit(c);
   int nValue = GetCardValue(c);

   int nImgPos = 0;
   if( nValue == ACE ){
      nImgPos = 0;
   }
   else if( nValue == JACK ){
      nImgPos = 10;
   }
   else if( nValue == QUEEN ){
      nImgPos = 11;
   }
   else if( nValue == KING ){
      nImgPos = 12;
   }
   else {
      nImgPos = nValue - 1;
   }

   int nSuitPos = 0;
   if( nSuit == DIAMONDS ) {
      nSuitPos = 0;
   }
   else if( nSuit == CLUBS ) {
      nSuitPos = 3;
   }
   else if( nSuit == HEARTS ) {
      nSuitPos = 1;
   }
   else if( nSuit == SPADES ) {
      nSuitPos = 2;
   }

   if( nSuit == JOKER ) {
      nSuitPos = 1;
      nImgPos = 15;
   }

   SDL_Rect rectSrc;
   rectSrc.x = nImgPos * 42;
   rectSrc.y = nSuitPos * 62;
   rectSrc.w = 42;
   rectSrc.h = 62;

   SDL_Rect rectDst;
   rectDst.x = 0;
   rectDst.y = 0;
   rectDst.w = CARD_WIDTH;
   rectDst.h = CARD_HEIGHT;

   if( nWidth == CARD_WIDTH && nHeight == CARD_HEIGHT ) {
      SDL_BlitSurface(m_pCards, &rectSrc, pSurface, &rectDst);
   } else
   {
      rectDst.w = nWidth;
      rectDst.h = nHeight;
      SDL_SoftStretch(m_pCards, &rectSrc, pSurface, &rectDst);
   }
   
   SDL_SetColorKey(pSurface, SDL_SRCCOLORKEY, SDL_MapRGB(pSurface->format, 0, 255, 0));

   return true;
}

bool CardImages::GetImageForDeckStyle(SDL_Surface* pSurface)
{
   int nImgPos = 15;
   int nSuitPos = 0;

   SDL_Rect rectSrc;
   rectSrc.x = nImgPos * 42;
   rectSrc.y = nSuitPos * 62;
   rectSrc.w = 42;
   rectSrc.h = 62;

   SDL_Rect rectDst;
   rectDst.x = 0;
   rectDst.y = 0;
   rectDst.w = DISPCARD_WIDTH;
   rectDst.h = DISPCARD_HEIGHT;

   SDL_SoftStretch(m_pCards, &rectSrc, pSurface, &rectDst);
   //SDL_BlitSurface(m_pCards, &rectSrc, pSurface, &rectDst);

   SDL_SetColorKey(pSurface, SDL_SRCCOLORKEY, SDL_MapRGB(pSurface->format, 0, 255, 0));

   Tint(pSurface, SDL_MapRGB(pSurface->format, 255, 0, 0));

   return true;
}

void CardImages::Tint(SDL_Surface* pSurface, Uint16 clrBlend)
{
   Uint16 clrWhite = SDL_MapRGB(pSurface->format, 255, 255, 255);
   //If the surface must be locked 
   if( SDL_MUSTLOCK( pSurface ) )
   {
      //Lock the surface 
      SDL_LockSurface( pSurface );
   }

   //Go through columns 
   for( int x = 0; x < pSurface->w; x++ )
   {
      //Go through rows 
      for( int y = 0; y < pSurface->h; y++ )
      {
         //Get pixel 
         Uint16 pixel = get_pixel16( pSurface, x, y );
         if( pixel == clrWhite ) {
	    put_pixel16( pSurface, x, y, clrBlend);
         }
      }
   }

   //Unlock surface 
   if( SDL_MUSTLOCK( pSurface ) )
   {
      SDL_UnlockSurface( pSurface );
   }
}

