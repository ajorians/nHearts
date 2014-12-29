#ifndef METRICS_H
#define METRICS_H

extern "C"
{
        #include <os.h>
}

typedef enum {
   LocLeft,
   LocTop,
   LocRight,
   LocBottom
} CardLocation;

class Metrics
{
public:
   Metrics();
   bool SetCardDimensions(int nWidth, int nHeight);
   void SetNumCards(int nNumCards);
   int GetNumCards() const;
   int GetXPos(CardLocation eLocation, int nCardIndex) const;
   int GetYPos(CardLocation eLocation, int nCardIndex) const;
   int GetSelectedXPos(int nSelectedIndex) const;
   int GetSelectedTop() const;
   int GetPlayerSideX(CardLocation eLocation) const;
   int GetPlayerSideY(CardLocation eLocation) const;

   int GetMiddleCardX(CardLocation eLocation) const;
   int GetMiddleCardY(CardLocation eLocation) const;

   int GetInitialCardX() const;
   int GetInitialCardY() const;

   int GetCardWidth() const;
   int GetCardHeight() const;
   int GetLeft() const;
   int GetTop() const;

   int GetCardOffsetX() const;

protected:
   int m_nNumCards;
   int m_nCardWidth;
   int m_nCardHeight;
   int m_nTop;
};

#endif

