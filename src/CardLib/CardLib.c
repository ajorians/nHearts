//Public domain :)

#include <os.h>
#include "include/CardLib.h"
#include "Defines.h"

struct CardNode
{
   int m_nValue;
   int m_nSuit;
   void* m_pExtraData;
   struct CardNode* m_pNext;
};

struct CardDeck
{
   int m_nLastError;
   struct CardNode* m_pCards;
};

int CardLibCreate(CardLib* api)
{
   DEBUG_FUNC_NAME;

   struct CardDeck* pCD = malloc(sizeof(struct CardDeck));
   if( pCD == NULL ){//Out of memory
      return CARDLIB_OUT_OF_MEMORY;
   }

   pCD->m_nLastError = CARDLIB_OK;
   pCD->m_pCards = NULL;

   *api = pCD; 

   return CARDLIB_OK;
}

int CardLibFree(CardLib* api)
{
   DEBUG_FUNC_NAME;

   RemoveAllCards(*api, CARDLIB_FREE_CARD);
   struct CardDeck* pCD = *api;
   free(pCD);
   *api = NULL;
   return CARDLIB_OK;
}

int GetCardError(CardLib api)
{
   DEBUG_FUNC_NAME;

   struct CardDeck* pCD = (struct CardDeck*)api;
   return pCD->m_nLastError;
}

void ClearCardError(CardLib api)
{
   DEBUG_FUNC_NAME;

   struct CardDeck* pCD = (struct CardDeck*)api;
   pCD->m_nLastError = CARDLIB_OK;
}

//CardLib related functions
int IsCardsEmpty(CardLib api)
{
   DEBUG_FUNC_NAME;

   struct CardDeck* pCD = (struct CardDeck*)api;

   struct CardNode* pCurrent = pCD->m_pCards;

   return pCurrent == NULL ? CARDLIB_DECK_EMPTY : CARDLIB_DECK_HAS_CARDS;
}

int GetNumberOfCards(CardLib api)
{
   DEBUG_FUNC_NAME;

   struct CardDeck* pCD = (struct CardDeck*)api;

   int nRet = 0;
   struct CardNode* pCurrent = pCD->m_pCards;

   while(pCurrent != NULL) {
      pCurrent = pCurrent->m_pNext;
      nRet++;
   }

   return nRet;
}

int GetCard(CardLib api, Card* pCard, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct CardDeck* pCD = (struct CardDeck*)api;

   int n = 0;
   struct CardNode* pCurrent = pCD->m_pCards;

   while(pCurrent != NULL && n != nIndex) {
      pCurrent = pCurrent->m_pNext;
      n++;
   }

   if( pCurrent == NULL ){ //Indexed itemed not found
      DEBUG_MSG("Indexed item not found");
      pCD->m_nLastError = CARDLIB_INDEX_NOT_EXIST;
      return CARDLIB_INDEX_NOT_EXIST;
   }

   *pCard = pCurrent;

   return CARDLIB_OK;
}

int AddStandardCards(CardLib api, int nJokers)
{
   DEBUG_FUNC_NAME;

   int nSuit;
   int nValue;
   for(nSuit=SUIT_START; nSuit<=SUIT_END; nSuit++) {
      for(nValue = VALUE_START; nValue<=VALUE_END; nValue++) {
         Card c;
         CreateCard(&c, nSuit, nValue);
         AddCard(api, c);
      }
   }

   int n;
   for(n=0; n<nJokers; n++) {
      Card c;
      CreateCard(&c, JOKER, -1);
      AddCard(api, c);
   }

   return CARDLIB_OK;
}

int AddCard(CardLib api, Card c)
{
   DEBUG_FUNC_NAME;

   struct CardDeck* pCD = (struct CardDeck*)api;
   struct CardNode* pNode = (struct CardNode*)c;

   struct CardNode* pCurrent = pCD->m_pCards;
   struct CardNode* pPrevious = NULL;

   while(pCurrent != NULL) {
      pPrevious = pCurrent;
      pCurrent = pCurrent->m_pNext;
   }

   //Add node into linked list
   pNode->m_pNext = pCurrent;
   if( pCD->m_pCards == NULL ) {
      pCD->m_pCards = pNode;
   }
   else
   {
      pPrevious->m_pNext = pNode;
   }

   return CARDLIB_OK;
}

int AddCardToBeginning(CardLib api, Card c)
{
   DEBUG_FUNC_NAME;

   struct CardDeck* pCD = (struct CardDeck*)api;
   struct CardNode* pNode = (struct CardNode*)c;

   pNode->m_pNext = pCD->m_pCards;
   pCD->m_pCards = pNode;

   return CARDLIB_OK;
}

int Shuffle(CardLib api)
{
   struct CardDeck* pCD = (struct CardDeck*)api;

   if( pCD->m_pCards == NULL ){
      return CARDLIB_DECK_EMPTY;
   }

   int nNumberOfCards = GetNumberOfCards(api);

   struct CardNode* pCurrent = pCD->m_pCards;

   while(pCurrent != NULL) {
      int nSwapWith = rand() % nNumberOfCards;
      struct CardNode* pNodeSwappingDataWith = pCurrent;
      int n;
      for(n=0; n<nSwapWith; n++) {
         pNodeSwappingDataWith = pNodeSwappingDataWith->m_pNext;
         if( pNodeSwappingDataWith == NULL ) { //End; so go to beginning
            pNodeSwappingDataWith = pCD->m_pCards;
         }
      }

     int nValue = pCurrent->m_nValue;
     int nSuit = pCurrent->m_nSuit;

     pCurrent->m_nValue = pNodeSwappingDataWith->m_nValue;
     pCurrent->m_nSuit = pNodeSwappingDataWith->m_nSuit;
     
     pNodeSwappingDataWith->m_nValue = nValue;
     pNodeSwappingDataWith->m_nSuit = nSuit;

      pCurrent = pCurrent->m_pNext;
   }

   return CARDLIB_OK;
}

int TakeNextCard(CardLib api, Card* pCard)
{
   DEBUG_FUNC_NAME;

   struct CardDeck* pCD = (struct CardDeck*)api;

   if( pCD->m_pCards == NULL ){
      return CARDLIB_DECK_EMPTY;
   }

   struct CardNode* pNode = pCD->m_pCards;

   if( pNode == NULL ) {//Should never happen
      return CARDLIB_DECK_EMPTY;
   }

   pCD->m_pCards = pNode->m_pNext;

   pNode->m_pNext = NULL;//Just in case!

   Card c = (Card)pNode;
   *pCard = c;//Now callers responsibilty to clean it up

   return CARDLIB_OK;
}

int RemoveAllCards(CardLib api, int nFreeCard)
{
   DEBUG_FUNC_NAME;

   struct CardDeck* pCD = (struct CardDeck*)api;

   struct CardNode* pCurrent = pCD->m_pCards;

   while(pCurrent != NULL) {
      pCD->m_pCards = pCurrent->m_pNext;
      Card card = (Card*)pCurrent;
      if( nFreeCard == CARDLIB_FREE_CARD )
         DestroyCard(&card);
      pCurrent = pCD->m_pCards;
   }

   return CARDLIB_OK;
}

//Card related functions
int CreateCard(Card* pCard, int nSuit, int nValue)
{
   DEBUG_FUNC_NAME;

   struct CardNode* pC = malloc(sizeof(struct CardNode));
   if( pC == NULL ){//Out of memory
      return CARDLIB_OUT_OF_MEMORY;
   }

   pC->m_nSuit = nSuit;
   pC->m_nValue = nValue;
   pC->m_pExtraData = NULL;
   pC->m_pNext = NULL;

   *pCard = pC;
   return CARDLIB_OK;
}

int DestroyCard(Card* pCard)
{
   DEBUG_FUNC_NAME;

   struct Card* card = *pCard;

   free(card);
   *pCard = NULL;
   return CARDLIB_OK;
}

int GetSuit(Card c)
{
   DEBUG_FUNC_NAME;

   struct CardNode* pNode = (struct CardNode*)c;

   return pNode->m_nSuit;
}

int GetValue(Card c)
{
   DEBUG_FUNC_NAME;

   struct CardNode* pNode = (struct CardNode*)c;

   return pNode->m_nValue;
}

int SetCardExtraData(Card c, void* pExtraData)
{
   DEBUG_FUNC_NAME;

   struct CardNode* pNode = (struct CardNode*)c;

   pNode->m_pExtraData = pExtraData;

   return CARDLIB_OK;
}

void* GetCardExtraData(Card c)
{
   DEBUG_FUNC_NAME;

   struct CardNode* pNode = (struct CardNode*)c;

   return pNode->m_pExtraData;
}


