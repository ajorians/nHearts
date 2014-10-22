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

int Metrics::GetXPos(int nPlayerIndex, int nCardIndex) const
{
   if( nPlayerIndex == 1 ) {
      return 0;
   }
   else if( nPlayerIndex == 3 ) {
      return SCREEN_WIDTH-m_nCardWidth;
   }
   return GetLeft() + nCardIndex*m_nCardWidth;
}

int Metrics::GetYPos(int nPlayerIndex, int nCardIndex) const
{
   if( nPlayerIndex == 0 ) {
      return GetTop();
   }
   else if( nPlayerIndex == 1 || nPlayerIndex == 3 ) {
      return 30 + nCardIndex*(m_nCardHeight/2);
   }
   return 0;
}

int Metrics::GetSelectedXPos(int nSelectedIndex) const
{
   int nWidthRemaining = SCREEN_WIDTH - m_nCardWidth*3;
   int nLeft = nWidthRemaining/2;
   return nLeft + nSelectedIndex*m_nCardWidth;
}

int Metrics::GetSelectedTop() const
{
   return GetTop() - GetCardHeight() - 5;
}

int Metrics::GetPlayerSideX(int nPlayerIndex) const
{
   if( nPlayerIndex == 0 || nPlayerIndex == 2 )
      return SCREEN_WIDTH/2;
   else if( nPlayerIndex == 1 )
      return -50;
   return SCREEN_WIDTH + 50;
}

int Metrics::GetPlayerSideY(int nPlayerIndex) const
{
   if( nPlayerIndex == 0 )
      return SCREEN_HEIGHT + 50;
   else if ( nPlayerIndex == 1 || nPlayerIndex == 3 )
      return SCREEN_HEIGHT/2;
   return -50;
}

int Metrics::GetMiddleCardX(int nPlayerIndex) const
{
   int nWidthRemaining = SCREEN_WIDTH - m_nCardWidth*3 - 3*10;
   int nLeft = nWidthRemaining/2;
   if( nPlayerIndex == 1 ) {
      return nLeft;
   }
   else if( nPlayerIndex == 0 || nPlayerIndex == 2 ) {
      return nLeft + m_nCardWidth + 10;
   }
   return nLeft + 2*m_nCardWidth + 20;
}

int Metrics::GetMiddleCardY(int nPlayerIndex) const
{
   int nTop = 30;
   if( nPlayerIndex == 2 ) {
      return nTop;
   }
   else if( nPlayerIndex == 1 || nPlayerIndex == 3 ) {
      return nTop + m_nCardHeight;
   }
   return nTop + 2*m_nCardHeight;
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

