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
   int GetNumItems() const;
   int GetXPos(int nBoardX) const;
   int GetCardWidth() const;
   int GetCardHeight() const;
   int GetLeft() const;
   int GetTop() const;

protected:
   int m_nNumItems;
   int m_nCardWidth;
   int m_nCardHeight;
   int m_nLeft;
   int m_nTop;
};

#endif

