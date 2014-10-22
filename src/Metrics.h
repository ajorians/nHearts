#ifndef METRICS_H
#define METRICS_H

extern "C"
{
        #include <os.h>
}

class Metrics
{
public:
   Metrics();
   bool SetCardDimensions(int nWidth, int nHeight);
   void SetNumCards(int nNumCards);
   int GetNumCards() const;
   int GetXPos(int nPlayerIndex, int nCardIndex) const;
   int GetYPos(int nPlayerIndex, int nCardIndex) const;
   int GetSelectedXPos(int nSelectedIndex) const;
   int GetSelectedTop() const;
   int GetPlayerSideX(int nPlayerIndex) const;
   int GetPlayerSideY(int nPlayerIndex) const;

   int GetMiddleCardX(int nPlayerIndex) const;
   int GetMiddleCardY(int nPlayerIndex) const;

   int GetCardWidth() const;
   int GetCardHeight() const;
   int GetLeft() const;
   int GetTop() const;

protected:
   int m_nNumCards;
   int m_nCardWidth;
   int m_nCardHeight;
   int m_nTop;
};

#endif

