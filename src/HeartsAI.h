#ifndef AI_H
#define AI_H

extern "C"
{
        #include <os.h>
        #include "HeartsLib/HeartsLib.h"
}

class HeartsAI
{
public:
   HeartsAI();
   void SetHeartsLib(HeartsLib* pHeartsLib, int nPlayerIndex);
   void PassCards();
   void PlayACard();

protected:
   void GetCountOfSuits(int& nHearts, int& nDiamonds, int& nSpades, int& nClubs);
   double GetDesirePass(int nCardIndex, int nHearts, int nDiamonds, int nSpades, int nClubs);

protected:
   HeartsLib *m_pHeartsLib;
   int m_nPlayerIndex;
};

#endif

