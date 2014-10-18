#include "Metrics.h"

Metrics::Metrics()
{
}

bool Metrics::SetCardDimensions(int nWidth, int nHeight)
{
   m_nCardWidth = nWidth;
   m_nCardHeight = nHeight;

   m_nTop = SCREEN_HEIGHT - nHeight - 35;

   return true;
}

void Metrics::SetNumCards(int nNumCards)
{
   m_nNumCards = nNumCards;
}

int Metrics::GetNumCards() const
{
   return m_nNumCards;
}

int Metrics::GetXPos(int nCardX) const
{
   return GetLeft() + nCardX*m_nCardWidth;
}

int Metrics::GetCardWidth() const
{
   return m_nCardWidth;
}

int Metrics::GetCardHeight() const
{
   return m_nCardHeight;
}

int Metrics::GetLeft() const
{
   int nWidthRemaining = SCREEN_WIDTH - m_nCardWidth*m_nNumCards;
   return nWidthRemaining/2;
}

int Metrics::GetTop() const
{
   return m_nTop;
}

