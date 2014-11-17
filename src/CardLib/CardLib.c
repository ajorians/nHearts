//Public domain :)
#ifdef _TINSPIRE
#include <os.h>
#else
#include <stdio.h>
#include <stdlib.h>
#endif
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

void SwapCardNodes(struct CardNode **head, struct CardNode **a, struct CardNode **b);

int CardLibCreate(CardLib* api)
{
   struct CardDeck* pCD;
   DEBUG_FUNC_NAME;

   pCD = malloc(sizeof(struct CardDeck));
   if( pCD == NULL ){//Out of memory
      return CARDLIB_OUT_OF_MEMORY;
   }

   pCD->m_nLastError = CARDLIB_OK;
   pCD->m_pCards = NULL;

   *api = pCD; 

   return CARDLIB_OK;
}

int CardLibCopy(CardLib* copy, CardLib orig)
{
   struct CardDeck* pOrig;
   struct CardDeck* pCD;
   int nCards;
   int i;

   DEBUG_FUNC_NAME;

   pOrig = (struct CardDeck*)orig;

   pCD = malloc(sizeof(struct CardDeck));
   if( pCD == NULL ){//Out of memory
      return CARDLIB_OUT_OF_MEMORY;
   }

   pCD->m_nLastError = pOrig->m_nLastError;
   pCD->m_pCards = NULL;

   *copy = pCD;

   nCards = GetNumberOfCards(orig);
   for(i=0; i<nCards; i++) {
      Card cOrig;
      Card cCopy;
      GetCard(orig, &cOrig, i);
      CopyCard(&cCopy, cOrig);
      AddCard(*copy, cCopy);
   }

   return CARDLIB_OK;
}

int CardLibFree(CardLib* api)
{
   struct CardDeck* pCD;
   DEBUG_FUNC_NAME;

   RemoveAllCards(*api, CARDLIB_FREE_CARD);
   pCD = *api;
   free(pCD);
   *api = NULL;
   return CARDLIB_OK;
}

int GetCardError(CardLib api)
{
   struct CardDeck* pCD;
   DEBUG_FUNC_NAME;

   pCD = (struct CardDeck*)api;
   return pCD->m_nLastError;
}

void ClearCardError(CardLib api)
{
   struct CardDeck* pCD;
   DEBUG_FUNC_NAME;

   pCD = (struct CardDeck*)api;
   pCD->m_nLastError = CARDLIB_OK;
}

//CardLib related functions
int IsCardsEmpty(CardLib api)
{
   struct CardDeck* pCD;
   struct CardNode* pCurrent;
   DEBUG_FUNC_NAME;

   pCD = (struct CardDeck*)api;

   pCurrent = pCD->m_pCards;

   return pCurrent == NULL ? CARDLIB_DECK_EMPTY : CARDLIB_DECK_HAS_CARDS;
}

int GetNumberOfCards(CardLib api)
{
   struct CardDeck* pCD;
   int nRet;
   struct CardNode* pCurrent;

   DEBUG_FUNC_NAME;

   pCD = (struct CardDeck*)api;

   nRet = 0;
   pCurrent = pCD->m_pCards;

   while(pCurrent != NULL) {
      pCurrent = pCurrent->m_pNext;
      nRet++;
   }

   return nRet;
}

int GetCard(CardLib api, Card* pCard, int nIndex)
{
   struct CardDeck* pCD;
   int n;
   struct CardNode* pCurrent;
   DEBUG_FUNC_NAME;

   pCD = (struct CardDeck*)api;

   n = 0;
   pCurrent = pCD->m_pCards;

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
   int nSuit;
   int nValue;
   int n;
   DEBUG_FUNC_NAME;

   for(nSuit=SUIT_START; nSuit<=SUIT_END; nSuit++) {
      for(nValue = VALUE_START; nValue<=VALUE_END; nValue++) {
         Card c;
         CreateCard(&c, nSuit, nValue);
         AddCard(api, c);
      }
   }

   for(n=0; n<nJokers; n++) {
      Card c;
      CreateCard(&c, JOKER, -1);
      AddCard(api, c);
   }

   return CARDLIB_OK;
}

int AddCard(CardLib api, Card c)
{
   struct CardDeck* pCD;
   struct CardNode* pNode;
   struct CardNode* pCurrent;
   struct CardNode* pPrevious;
   DEBUG_FUNC_NAME;

   pCD = (struct CardDeck*)api;
   pNode = (struct CardNode*)c;

   pCurrent = pCD->m_pCards;
   pPrevious = NULL;

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
   struct CardDeck* pCD;
   struct CardNode* pNode;
   DEBUG_FUNC_NAME;

   pCD = (struct CardDeck*)api;
   pNode = (struct CardNode*)c;

   pNode->m_pNext = pCD->m_pCards;
   pCD->m_pCards = pNode;

   return CARDLIB_OK;
}

int Shuffle(CardLib api)
{
   struct CardDeck* pCD;
   int nNumberOfCards;
   int nCard;

   pCD = (struct CardDeck*)api;

   if( pCD->m_pCards == NULL ){
      return CARDLIB_DECK_EMPTY;
   }

   nNumberOfCards = GetNumberOfCards(api);

   for(nCard = 0; nCard<nNumberOfCards; nCard++) {
      Card a, b;
      int nSwapWith;
      struct CardNode* pA;
      struct CardNode* pB;
      GetCard(api, &a, nCard);
      nSwapWith = rand() % nNumberOfCards;
      GetCard(api, &b, nSwapWith);

      pA = (struct CardNode*)a;
      pB = (struct CardNode*)b;

      SwapCardNodes(&(pCD->m_pCards), &pA, &pB);
   }

   return CARDLIB_OK;
}

int TakeNextCard(CardLib api, Card* pCard)
{
   struct CardDeck* pCD;
   struct CardNode* pNode;
   Card c;
   DEBUG_FUNC_NAME;

   pCD = (struct CardDeck*)api;

   if( pCD->m_pCards == NULL ){
      return CARDLIB_DECK_EMPTY;
   }

   pNode = pCD->m_pCards;

   if( pNode == NULL ) {//Should never happen
      return CARDLIB_DECK_EMPTY;
   }

   pCD->m_pCards = pNode->m_pNext;

   pNode->m_pNext = NULL;//Just in case!

   c = (Card)pNode;
   *pCard = c;//Now callers responsibilty to clean it up

   return CARDLIB_OK;
}

int RemoveAllCards(CardLib api, int nFreeCard)
{
   struct CardDeck* pCD;
   struct CardNode* pCurrent;
   DEBUG_FUNC_NAME;

   pCD = (struct CardDeck*)api;

   pCurrent = pCD->m_pCards;

   while(pCurrent != NULL) {
      Card card;
      pCD->m_pCards = pCurrent->m_pNext;
      card = (Card*)pCurrent;
      if( nFreeCard == CARDLIB_FREE_CARD )
         DestroyCard(&card);
      pCurrent = pCD->m_pCards;
   }

   return CARDLIB_OK;
}

int RemoveCard(CardLib api, int nIndex, int nFreeCard)
{
   struct CardDeck* pCD;
   struct CardNode* pCurrent;
   struct CardNode* pLastNode;
   int i;
   DEBUG_FUNC_NAME;

   pCD = (struct CardDeck*)api;

   pCurrent = pCD->m_pCards;

   pLastNode = NULL;
   i=0;
   while(pCurrent != NULL) {
      if( i == nIndex ) {
         Card card;
         if( pLastNode ) {
            pLastNode->m_pNext = pCurrent->m_pNext;
         }
         else {
            pCD->m_pCards = pCurrent->m_pNext;
         }

         pCurrent->m_pNext = NULL;
         card = (Card*)pCurrent;
         if( nFreeCard == CARDLIB_FREE_CARD )
            DestroyCard(&card);
         return CARDLIB_OK;
      }

      i++;
      pLastNode = pCurrent;
      pCurrent = pCurrent->m_pNext;
   }
   
   return CARDLIB_INDEX_NOT_EXIST;
}

struct CardNode* get_prevnd(struct CardNode* head, struct CardNode* a){
   struct CardNode* temp;
   struct CardNode* pre_a;
   if(head == a){
      // node[a] is first node 
      return NULL;
   }
   temp = head; // temp is current node
   pre_a = NULL; 
	
   while(temp && temp!=a){ //seach while not reach to end or 
      pre_a = temp;          // find previous node   
      temp = temp->m_pNext;
   }
   if( temp != a ) {
      printf("Couldn't find before node a!\n");
      return NULL;
   }
   return pre_a;	
}

void SwapCardNodes(struct CardNode **head, struct CardNode **a, struct CardNode **b)
{
   struct CardNode* pre_a;
   struct CardNode* pre_b;
   struct CardNode* temp;
   if( (*head) == NULL ||               // Empty list         
      (*a) == NULL || (*b) == NULL){     // one node is null  
      // Nothing to swap, just return 
      printf("\n Nothing to swap, just return \n");
      return;
   }

   pre_a = get_prevnd(*head, *a);
   pre_b = get_prevnd(*head, *b);

   //Now swap previous node's next
   if(pre_a) { pre_a->m_pNext = (*b); } // a's previous become b's previous, and 
   if(pre_b) { pre_b->m_pNext = (*a); } // b's previous become a's previous
 
   //Now swap next fields of candidate nodes 	
   temp = NULL;  
   temp = (*a)->m_pNext;
   (*a)->m_pNext = (*b)->m_pNext;
   (*b)->m_pNext = temp;
  
   //change head: if any node was a head 
   if((*head)==(*a)) { *head = *b; }
   else if((*head)==(*b)) { *head = *a; }
}

int SwapCards(CardLib api, int nIndex1, int nIndex2)
{
   struct CardDeck* pCD;
   int n;
   struct CardNode* pCurrent;
   struct CardNode* pFirst;
   struct CardNode* pSecond;
   DEBUG_FUNC_NAME;

   pCD = (struct CardDeck*)api;

   if( nIndex1 == nIndex2 )
      CARDLIB_OK;

   n = 0;
   pCurrent = pCD->m_pCards;
   pFirst = NULL;
   pSecond = NULL;

   while(pCurrent != NULL ) {
      if( n == nIndex1 )
         pFirst = pCurrent;
      if( n == nIndex2 ) {
         pSecond = pCurrent;
      }
      if( pFirst != NULL && pSecond != NULL )
         break;

      pCurrent = pCurrent->m_pNext;
      n++;
   }

   if( pFirst != NULL && pSecond != NULL ) {
      SwapCardNodes(&(pCD->m_pCards), &pFirst, &pSecond);
      return CARDLIB_OK;
   }

   return CARDLIB_INDEX_NOT_EXIST;
}

//Card related functions
int CreateCard(Card* pCard, int nSuit, int nValue)
{
   struct CardNode* pC;
   DEBUG_FUNC_NAME;

   pC = malloc(sizeof(struct CardNode));
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

int CopyCard(Card* pCard, Card cOrig)
{
   struct CardNode* pOrig;
   struct CardNode* pC;
   DEBUG_FUNC_NAME;

   pOrig = (struct CardNode*)cOrig;

   pC = malloc(sizeof(struct CardNode));
   if( pC == NULL ){//Out of memory
      return CARDLIB_OUT_OF_MEMORY;
   }

   pC->m_nValue = pOrig->m_nValue;
   pC->m_nSuit = pOrig->m_nSuit;
   pC->m_pExtraData = pOrig->m_pExtraData;
   pC->m_pNext = NULL;//Just copying the card; not the order

   *pCard = pC;
   return CARDLIB_OK;
}

int DestroyCard(Card* pCard)
{
   struct Card* card;
   DEBUG_FUNC_NAME;

   card = *pCard;

   free(card);
   *pCard = NULL;
   return CARDLIB_OK;
}

int GetSuit(Card c)
{
   struct CardNode* pNode;
   DEBUG_FUNC_NAME;

   pNode = (struct CardNode*)c;

   return pNode->m_nSuit;
}

int GetCardValue(Card c)
{
   struct CardNode* pNode;
   DEBUG_FUNC_NAME;

   pNode = (struct CardNode*)c;

   return pNode->m_nValue;
}

int SetCardExtraData(Card c, void* pExtraData)
{
   struct CardNode* pNode;
   DEBUG_FUNC_NAME;

   pNode = (struct CardNode*)c;

   pNode->m_pExtraData = pExtraData;

   return CARDLIB_OK;
}

void* GetCardExtraData(Card c)
{
   struct CardNode* pNode;
   DEBUG_FUNC_NAME;

   pNode = (struct CardNode*)c;

   return pNode->m_pExtraData;
}

int SetSuit(Card c, int nSuit)
{
   struct CardNode* pNode;
   DEBUG_FUNC_NAME;

   pNode = (struct CardNode*)c;

   pNode->m_nSuit = nSuit;

   return CARDLIB_OK;
}

int SetCardValue(Card c, int nValue)
{
   struct CardNode* pNode;
   DEBUG_FUNC_NAME;

   pNode = (struct CardNode*)c;

   pNode->m_nValue = nValue;

   return CARDLIB_OK;
}

int SwapCardValues(Card c1, Card c2)
{
   int nSuit, nValue;
   void* ptr;
   int nSuit2, nValue2;
   void* ptr2;
   nSuit = GetSuit(c1);
   nValue = GetCardValue(c1);
   ptr = GetCardExtraData(c1);
   nSuit2 = GetSuit(c2);
   nValue2 = GetCardValue(c2);
   ptr2 = GetCardExtraData(c2);
   DEBUG_MSG("Card 1: %d,%d; Card2: %d,%d\n", nSuit, nValue, nSuit2, nValue2);

   SetSuit(c1, nSuit2); SetSuit(c2, nSuit);
   SetCardValue(c1, nValue2); SetCardValue(c2, nValue);
   SetCardExtraData(c1, ptr2); SetCardExtraData(c2, ptr);

   return CARDLIB_OK;
}

