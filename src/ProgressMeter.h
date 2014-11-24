#ifndef PROGRESSMETER_H
#define PROGRESSMETER_H

extern "C"
{
        #include <os.h>
        #include "SDL/SDL.h"
}

class ProgressMeter
{
public:
   ProgressMeter(SDL_Surface* pScreen, int nX, int nY, int nWidth, int nHeight);

   void SetDoneAmount(int nNumerator, int nDemoniator);
   void UpdateDisplay();

protected:
   SDL_Surface* m_pScreen;
   int m_nX;
   int m_nY;
   int m_nWidth;
   int m_nHeight;
   int m_nNumerator;
   int m_nDemoniator;
};

#endif

