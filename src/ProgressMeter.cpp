#include "ProgressMeter.h"

extern "C"
{
#include "SDL/SDL_gfxPrimitives.h"
}

ProgressMeter::ProgressMeter(SDL_Surface* pScreen, int nX, int nY, int nWidth, int nHeight)
: m_pScreen(pScreen), m_nX(nX), m_nY(nY), m_nWidth(nWidth), m_nHeight(nHeight), m_nNumerator(0), m_nDemoniator(0)
{
}

void ProgressMeter::SetDoneAmount(int nNumerator, int nDemoniator)
{
   m_nNumerator = nNumerator;
   m_nDemoniator = nDemoniator;
}

void ProgressMeter::UpdateDisplay()
{
   if( m_nDemoniator <= 0 )
      return;

   boxRGBA(m_pScreen, m_nX, m_nY, m_nX+m_nWidth, m_nY+m_nHeight, 100, 149, 237, 70);

   SDL_Rect rect;
   rect.x = m_nX;
   rect.y = m_nY;
   rect.w = m_nWidth*m_nNumerator/m_nDemoniator;
   rect.h = m_nHeight;
   SDL_FillRect(m_pScreen, &rect, SDL_MapRGB(m_pScreen->format, 0, 255, 0));
}


