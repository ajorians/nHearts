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

int Metrics::GetXPos(CardLocation eLocation, int nCardIndex) const
{
   if( eLocation == LocLeft ) {
      return 0;
   }
   else if( eLocation == LocRight ) {
      return SCREEN_WIDTH-m_nCardWidth;
   }
   return GetLeft() + nCardIndex*GetCardOffsetX();
}

int Metrics::GetYPos(CardLocation eLocation, int nCardIndex) const
{
   if( eLocation == LocBottom ) {
      return GetTop();
   }
   else if( eLocation == LocLeft || eLocation == LocRight ) {
      return 30 + nCardIndex*(m_nCardHeight/3);
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
   return GetTop() - GetCardHeight() - 10;
}

int Metrics::GetPlayerSideX(CardLocation eLocation) const
{
   if( eLocation == LocBottom || eLocation == LocTop )
      return SCREEN_WIDTH/2;
   else if( eLocation == LocLeft )
      return -50;
   return SCREEN_WIDTH + 50;
}

int Metrics::GetPlayerSideY(CardLocation eLocation) const
{
   if( eLocation == LocBottom )
      return SCREEN_HEIGHT + 50;
   else if ( eLocation == LocLeft || eLocation == LocRight )
      return SCREEN_HEIGHT/2;
   return -50;
}

int Metrics::GetMiddleCardX(CardLocation eLocation) const
{
   int nWidthRemaining = SCREEN_WIDTH - m_nCardWidth*3 - 3*10;
   int nLeft = nWidthRemaining/2;
   if( eLocation == LocLeft ) {
      return nLeft;
   }
   else if( eLocation == LocBottom || eLocation == LocTop ) {
      return nLeft + m_nCardWidth + 10;
   }
   return nLeft + 2*m_nCardWidth + 20;
}

int Metrics::GetMiddleCardY(CardLocation eLocation) const
{
   int nTop = 30;
   if( eLocation == LocTop ) {
      return nTop;
   }
   else if( eLocation == LocLeft || eLocation == LocRight ) {
      return nTop + m_nCardHeight;
   }
   return nTop + 2*m_nCardHeight;
}

int Metrics::GetInitialCardX() const
{
   return GetMiddleCardX(LocLeft) + m_nCardWidth + 10;
}

int Metrics::GetInitialCardY() const
{
   return GetMiddleCardY(LocTop) + m_nCardHeight;
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
   int nSpaceRequired = GetCardOffsetX()*m_nNumCards;
   int nWidthRemaining = SCREEN_WIDTH - nSpaceRequired - 6/*padding*/;
   return nWidthRemaining/2;
}

int Metrics::GetTop() const
{
   return m_nTop;
}

int Metrics::GetCardOffsetX() const
{
   int nSpaceAllocated = m_nCardWidth*13;
   int nSpacePerCard = nSpaceAllocated / m_nNumCards;
   if( nSpacePerCard < m_nCardWidth )
      return nSpacePerCard;
   return m_nCardWidth;
}
