#include "HeartsAI.h"

HeartsAI::HeartsAI()
: m_pHeartsLib(NULL), m_nPlayerIndex(-1)
{
}

void HeartsAI::SetHeartsLib(HeartsLib* pHeartsLib, int nPlayerIndex)
{
   m_pHeartsLib = pHeartsLib;
   m_nPlayerIndex = nPlayerIndex;
}

void HeartsAI::PassCards()
{
   int nHearts = 0, nDiamonds = 0, nSpades = 0, nClubs = 0;
   GetCountOfSuits(nHearts, nDiamonds, nSpades, nClubs);

   double dDesirePass[13];
   for(int i=0; i<13; i++) {
      dDesirePass[i] = GetDesirePass(i, nHearts, nDiamonds, nSpades, nClubs);
   }

   for(int i=0; i<3; i++) {
      int nHighest = -1;
      double dHighest = -1.0;
      for(int nCard = 0; nCard < 13; nCard++) {
         if( dDesirePass[nCard] > dHighest ) {
            dHighest = dDesirePass[nCard];
            nHighest = nCard;
         }
      }

      if( nHighest != -1 ) {
         dDesirePass[nHighest] = 0.0;
         ToggleSelectedCard(*m_pHeartsLib, m_nPlayerIndex, nHighest);
      }
   }

   //Just in case act randomly
   while(GetNumberSelectedCards(*m_pHeartsLib, m_nPlayerIndex) != 3 ) {
      printf("Had to act randomly\n");
      ToggleSelectedCard(*m_pHeartsLib, m_nPlayerIndex, rand() % 13);
   }
   PassSelectedCards(*m_pHeartsLib, m_nPlayerIndex);
}

void HeartsAI::PlayACard()
{
   int nCards = GetNumberOfCardsInHand(*m_pHeartsLib, m_nPlayerIndex);
   bool arrCardsCanPlay[13];
   int nCountPlayableCards = 0;

   for(int i=0; i<13; i++) {
      if( i >= nCards ) {
         arrCardsCanPlay[i] = false;
      }
      else {
         if( CanPlayCard(*m_pHeartsLib, m_nPlayerIndex, i) == HEARTSLIB_CAN_PLAY_CARD ) {
            arrCardsCanPlay[i] = true;
            nCountPlayableCards++;
         }
         else {
            arrCardsCanPlay[i] = false;
         }
      }
   }

   if( nCountPlayableCards == 0 ) {
      printf("Playing randomly\n");
      while(true) {
         int nCardPossibleToPlay = rand() % nCards;
         if( CanPlayCard(*m_pHeartsLib, m_nPlayerIndex, nCardPossibleToPlay) == HEARTSLIB_CAN_PLAY_CARD ) {
            PlayCard(*m_pHeartsLib, m_nPlayerIndex, nCardPossibleToPlay);
            return;
         }
      }
   }

   //Can only play one card so play that
   if( nCountPlayableCards == 1 ) {
      for(int i=0; i<13; i++) {
         if( arrCardsCanPlay[i] == true ) {
            PlayCard(*m_pHeartsLib, m_nPlayerIndex, i);
            return;
         }
      }
   }

   //TODO: Determine if pretty much would take card if so don't play that one :)

   while(true) {
      int nCardsInHand = GetNumberOfCardsInHand(*m_pHeartsLib, m_nPlayerIndex);
      int nCardPossibleToPlay = rand() % nCardsInHand;
      if( CanPlayCard(*m_pHeartsLib, m_nPlayerIndex, nCardPossibleToPlay) == HEARTSLIB_CAN_PLAY_CARD ) {
         PlayCard(*m_pHeartsLib, m_nPlayerIndex, nCardPossibleToPlay);
         break;
      }
   }
}

void HeartsAI::GetCountOfSuits(int& nHearts, int& nDiamonds, int& nSpades, int& nClubs)
{
   nHearts = 0, nDiamonds = 0, nSpades = 0, nClubs = 0;
   int nCards = GetNumberOfCardsInHand(*m_pHeartsLib, m_nPlayerIndex);
   for(int i=0; i<nCards; i++) {
      Card c;
      GetCardInHand(*m_pHeartsLib, &c, m_nPlayerIndex, i);

      int nSuit = GetSuit(c);
      if( nSuit == HEARTS )
         nHearts++;
      else if( nSuit == DIAMONDS )
         nDiamonds++;
      else if( nSuit == SPADES )
         nSpades++;
      else if( nSuit == CLUBS )
         nClubs++;
   }
}

double HeartsAI::GetDesirePass(int nCardIndex, int nHearts, int nDiamonds, int nSpades, int nClubs)
{
   double dRet = 0.0;

   Card c;
   GetCardInHand(*m_pHeartsLib, &c, m_nPlayerIndex, nCardIndex);

   int nSuit = GetSuit(c);
   int nValue = GetCardValue(c);

   if( nSuit == SPADES ) {
      if( nValue == QUEEN || nValue == KING || nValue == ACE ) {
         if( nSpades > 4 ) {
            dRet = 0.2;
         }
         else if( nSpades == 3 ) {
            dRet = 0.4;
         }
         else if( nSpades == 2 ) {
            dRet = 0.6;
         }
         else {
            dRet = 0.8;
         }
      }
      else {//If it isn't Queen or higher I always hold on to it in case I get passed a queen
         dRet = 0.0;
      }
   }
   else {
      if( nValue == 2 || nValue == 3 ) {
         dRet = 0.0;
      }
      else if( nValue >= 4 && nValue < 8 ) {
         dRet = 0.1;
      }
      else if( nValue >= 8 && nValue <= JACK ) {
         dRet = 0.2;
      }
      else {
         dRet = 0.3;
      }
   }

   return dRet;
}
