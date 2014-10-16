#include "Metrics.h"

Metrics::Metrics()
{
}

bool Metrics::SetCardDimensions(int nWidth, int nHeight)
{
   m_nCardWidth = nWidth;
   m_nCardHeight = nHeight;

   m_nTop = SCREEN_HEIGHT - nHeight - 35;
   m_nLeft = 45;//TODO: Fix

   return true;
}

int Metrics::GetNumItems() const
{
   return 13;
}

int Metrics::GetXPos(int nCardX) const
{
   return m_nLeft + nCardX*m_nCardWidth;
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
   return m_nLeft;
}

int Metrics::GetTop() const
{
   return m_nTop;
}

