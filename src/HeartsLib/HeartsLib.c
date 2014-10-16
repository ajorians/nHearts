//Public domain :)

#include <os.h>
#include "include/HeartsLib.h"
#include "Defines.h"

#define NUMBER_OF_HEARTS_PLAYERS	4
#define HEARTS_NUMBER_OF_JOKERS		0

struct HeartsPlayer
{
   CardLib m_cardsHand;
   CardLib m_cardsTaken;
   int m_nScore;
};

struct Hearts
{
   int m_nLastError;
   struct HeartsPlayer m_Players[NUMBER_OF_HEARTS_PLAYERS];
   int m_nScoreLimit;
   Pass_Direction_t m_ePassDirection;
};

void SortCards(CardLib cards)
{
   //First let's sort by suit
   int nMadeChange = 1;
   while( nMadeChange == 1 ) {
      int nNumCards = GetNumberOfCards(cards);
      DEBUG_MSG("NumCards: %d\n", nNumCards);
      nMadeChange = 0;
      int i;
      for(i=1; i<nNumCards; i++) {
         Card c, cPrevious;
         GetCard(cards, &cPrevious, i-1);
         GetCard(cards, &c, i);

         int nSuit = GetSuit(c), nSuitPrevious = GetSuit(cPrevious);
         DEBUG_MSG("Suit1: %d; Suit2: %d\n", nSuit, nSuitPrevious);

         int nSwap = 0;
         if( nSuit != nSuitPrevious ) {
            //Order: Clubs(3) Diamonds(1) Spades(2) Hearts(0)
            if( nSuit == CLUBS ) {
               nSwap = 1;
            }
            else if ( nSuit == DIAMONDS && (nSuitPrevious == SPADES || nSuitPrevious == HEARTS) ) {
               nSwap = 1;
            }
            else if( nSuit == SPADES && nSuitPrevious == HEARTS ){
               nSwap = 1;
            }

         }

         if( nSwap == 1 ) {
            DEBUG_MSG("Swapping 2 cards\n");
            SwapCardValues(c, cPrevious);
            nMadeChange = 1;
         }
      }
   }

   nMadeChange = 1;
   while( nMadeChange == 1 ) {
      int nNumCards = GetNumberOfCards(cards);
      DEBUG_MSG("NumCards: %d\n", nNumCards);
      nMadeChange = 0;
      int i;
      for(i=1; i<nNumCards; i++) {
         Card c, cPrevious;
         GetCard(cards, &cPrevious, i-1);
         GetCard(cards, &c, i);

         int nSuit = GetSuit(c), nSuitPrevious = GetSuit(cPrevious);
         DEBUG_MSG("Suit1: %d; Suit2: %d\n", nSuit, nSuitPrevious);

         int nSwap = 0;
         if( nSuit == nSuitPrevious ) {
            //Order: 2-10, J,K,Q,A
            int nValue = GetCardValue(c), nPreviousValue = GetCardValue(cPrevious);
            if( nPreviousValue == ACE ) {
               nSwap = 1;
            }
            else if ( nPreviousValue != ACE && nValue != ACE && nPreviousValue > nValue ) {
               nSwap = 1;
            }

         }

         if( nSwap == 1 ) {
            DEBUG_MSG("Swapping 2 cards\n");
            SwapCardValues(c, cPrevious);
            nMadeChange = 1;
         }
      }
   }
}

int DealHands(HeartsLib api)
{
   struct Hearts* pH = (struct Hearts*)api;

   int nPlayerIndex;
   for(nPlayerIndex = 0; nPlayerIndex < NUMBER_OF_HEARTS_PLAYERS; nPlayerIndex++) {
      RemoveAllCards(pH->m_Players[nPlayerIndex].m_cardsHand, 1/*Free card*/);
   }

   CardLib cardDeck;
   if( CARDLIB_OK != CardLibCreate(&cardDeck) ) {
      return HEARTSLIB_OUT_OF_MEMORY;//Assuming
   }

   if( CARDLIB_OK != AddStandardCards(cardDeck, HEARTS_NUMBER_OF_JOKERS) ) {
      CardLibFree(&cardDeck);
      return HEARTSLIB_OUT_OF_MEMORY;
   }

   if( CARDLIB_OK != Shuffle(cardDeck) ) {
      CardLibFree(&cardDeck);
      return HEARTSLIB_CARD_FAILURE;
   }

   int nNumCards = GetNumberOfCards(cardDeck);//Better be 52 :)
   int nCard;
   for(nCard = 0; nCard < nNumCards; nCard++) {
      Card c;
      TakeNextCard(cardDeck, &c);
      nPlayerIndex = nCard % NUMBER_OF_HEARTS_PLAYERS;
      AddCard(pH->m_Players[nPlayerIndex].m_cardsHand, c);
   }

   for(nPlayerIndex = 0; nPlayerIndex < NUMBER_OF_HEARTS_PLAYERS; nPlayerIndex++) {
      SortCards(pH->m_Players[nPlayerIndex].m_cardsHand);
   }

   CardLibFree(&cardDeck);

   return HEARTSLIB_OK;
}

int HeartsLibCreate(HeartsLib* api)
{
   DEBUG_FUNC_NAME;

   struct Hearts* pH = malloc(sizeof(struct Hearts));
   if( pH == NULL ){//Out of memory
      return HEARTSLIB_OUT_OF_MEMORY;
   }

   int nPlayerIndex;
   for(nPlayerIndex = 0; nPlayerIndex < NUMBER_OF_HEARTS_PLAYERS; nPlayerIndex++) {
      if( CARDLIB_OK != CardLibCreate(&pH->m_Players[nPlayerIndex].m_cardsHand) ) {
         return HEARTSLIB_OUT_OF_MEMORY;//Assuming
      }
      if( CARDLIB_OK != CardLibCreate(&pH->m_Players[nPlayerIndex].m_cardsTaken) ) {
         return HEARTSLIB_OUT_OF_MEMORY;//Assuming
      }
   }

   pH->m_nLastError = HEARTSLIB_OK;

   *api = pH;

   int nDealHands = DealHands(*api);
   if( HEARTSLIB_OK != nDealHands ) {
      free(pH);
      return nDealHands;
   }

   return HEARTSLIB_OK;
}

int HeartsLibFree(HeartsLib* api)
{
   DEBUG_FUNC_NAME;

   struct Hearts* pH = *api;

   int nPlayerIndex;
   for(nPlayerIndex = 0; nPlayerIndex < NUMBER_OF_HEARTS_PLAYERS; nPlayerIndex++) {
      CardLibFree(&pH->m_Players[nPlayerIndex].m_cardsHand);
      CardLibFree(&pH->m_Players[nPlayerIndex].m_cardsTaken);
   }

   free(pH);
   *api = NULL;
   return HEARTSLIB_OK;
}

int GetHeartsLibError(HeartsLib api)
{
   DEBUG_FUNC_NAME;

   struct Hearts* pH = (struct Hearts*)api;
   return pH->m_nLastError;
}

void ClearHeartsLibError(HeartsLib api)
{
   DEBUG_FUNC_NAME;

   struct Hearts* pH = (struct Hearts*)api;
   pH->m_nLastError = HEARTSLIB_OK;
}

//HeartsLib related functions
int GetHeartsPlayerScore(HeartsLib api, int nPlayerIndex)
{
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   struct Hearts* pH = (struct Hearts*)api;
   return pH->m_Players[nPlayerIndex].m_nScore;
}

Pass_Direction_t GetHeartsPassDirection(HeartsLib api)
{
   DEBUG_FUNC_NAME;

   struct Hearts* pH = (struct Hearts*)api;
   return pH->m_ePassDirection;
}

int GetNumberOfCardsInHand(HeartsLib api, int nPlayerIndex)
{
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   struct Hearts* pH = (struct Hearts*)api;
   return GetNumberOfCards(pH->m_Players[nPlayerIndex].m_cardsHand);
}

int GetCardInHand(HeartsLib api, Card* pCard, int nPlayerIndex, int nCard)
{
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   if( nCard < 0 || nCard > GetNumberOfCardsInHand(api, nPlayerIndex) )
      return HEARTSLIB_BADARGUMENT;

   struct Hearts* pH = (struct Hearts*)api;

   GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, pCard, nCard);
   return HEARTSLIB_OK;
}

int GetHeartsGameOver(HeartsLib api)
{
   return 0;
}

