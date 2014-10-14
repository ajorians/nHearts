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

