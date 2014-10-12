//Public domain :)

#include <os.h>
#include "include/HeartsLib.h"
#include "Defines.h"

#define NUMBER_OF_HEARTS_PLAYERS	4
#define HEARTS_NUMBER_OF_JOKERS		0

struct HeartsPlayer
{
   CardLib m_cardsHand;
   CardLib m_cardCardTook;
   int m_nScore;
};

struct Hearts
{
   int m_nLastError;
   CardLib m_Deck;
   struct HeartsPlayer m_Players[NUMBER_OF_HEARTS_PLAYERS];
   int m_nScoreLimit;
};

int HeartsLibCreate(HeartsLib* api)
{
   DEBUG_FUNC_NAME;

   struct Hearts* pH = malloc(sizeof(struct Hearts));
   if( pH == NULL ){//Out of memory
      return HEARTSLIB_OUT_OF_MEMORY;
   }

   if( CARDLIB_OK != CardLibCreate(&pH->m_Deck) ) {
      free(pH);
      return HEARTSLIB_OUT_OF_MEMORY;//Assuming
   }

   if( CARDLIB_OK != AddStandardCards(pH->m_Deck, HEARTS_NUMBER_OF_JOKERS) ) {
      CardLibFree(&pH->m_Deck);
      free(pH);
      return HEARTSLIB_OUT_OF_MEMORY;
   }

   if( CARDLIB_OK != Shuffle(pH->m_Deck) ) {
      CardLibFree(&pH->m_Deck);
      free(pH);
      return HEARTSLIB_CARD_FAILURE;
   }

   pH->m_nLastError = HEARTSLIB_OK;

   *api = pH; 

   return HEARTSLIB_OK;
}

int HeartsLibFree(HeartsLib* api)
{
   DEBUG_FUNC_NAME;

   struct Hearts* pH = *api;

   CardLibFree(&pH->m_Deck);

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

