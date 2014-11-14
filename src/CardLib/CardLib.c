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

void SwapCardNodes(struct CardNode **head, struct CardNode **a, struct CardNode **b);

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

int CardLibCopy(CardLib* copy, CardLib orig)
{
   DEBUG_FUNC_NAME;

   struct CardDeck* pOrig = (struct CardDeck*)orig;

   struct CardDeck* pCD = malloc(sizeof(struct CardDeck));
   if( pCD == NULL ){//Out of memory
      return CARDLIB_OUT_OF_MEMORY;
   }

   pCD->m_nLastError = pOrig->m_nLastError;
   pCD->m_pCards = NULL;

   *copy = pCD;

   int nCards = GetNumberOfCards(orig);
   for(int i=0; i<nCards; i++) {
      Card cOrig;
      GetCard(orig, &cOrig, i);
      Card cCopy;
      CopyCard(&cCopy, cOrig);
      AddCard(*copy, cCopy);
   }

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

   int nCard;
   for(nCard = 0; nCard<nNumberOfCards; nCard++) {
      Card a, b;
      GetCard(api, &a, nCard);
      int nSwapWith = rand() % nNumberOfCards;
      GetCard(api, &b, nSwapWith);

      struct CardNode* pA = (struct CardNode*)a;
      struct CardNode* pB = (struct CardNode*)b;

      SwapCardNodes(&(pCD->m_pCards), &pA, &pB);
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

int RemoveCard(CardLib api, int nIndex, int nFreeCard)
{
   DEBUG_FUNC_NAME;

   struct CardDeck* pCD = (struct CardDeck*)api;

   struct CardNode* pCurrent = pCD->m_pCards;

   struct CardNode* pLastNode = NULL;
   int i=0;
   while(pCurrent != NULL) {
      if( i == nIndex ) {
         if( pLastNode ) {
            pLastNode->m_pNext = pCurrent->m_pNext;
         }
         else {
            pCD->m_pCards = pCurrent->m_pNext;
         }

         pCurrent->m_pNext = NULL;
         Card card = (Card*)pCurrent;
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
   if(head == a){
      // node[a] is first node 
      return NULL;
   }
   struct CardNode* temp = head; // temp is current node
   struct CardNode* pre_a = NULL; 
	
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
   if( (*head) == NULL ||               // Empty list         
      (*a) == NULL || (*b) == NULL){     // one node is null  
      // Nothing to swap, just return 
      printf("\n Nothing to swap, just return \n");
      return;
   }

   struct CardNode* pre_a = get_prevnd(*head, *a);
   struct CardNode* pre_b = get_prevnd(*head, *b);
		
   //Now swap previous node's next
   if(pre_a) { pre_a->m_pNext = (*b); } // a's previous become b's previous, and 
   if(pre_b) { pre_b->m_pNext = (*a); } // b's previous become a's previous
 
   //Now swap next fields of candidate nodes 	
   struct CardNode* temp = NULL;  
   temp = (*a)->m_pNext;
   (*a)->m_pNext = (*b)->m_pNext;
   (*b)->m_pNext = temp;
  
   //change head: if any node was a head 
   if((*head)==(*a)) { *head = *b; }
   else if((*head)==(*b)) { *head = *a; }
}

int SwapCards(CardLib api, int nIndex1, int nIndex2)
{
   DEBUG_FUNC_NAME;

   struct CardDeck* pCD = (struct CardDeck*)api;

   if( nIndex1 == nIndex2 )
      CARDLIB_OK;

   int n = 0;
   struct CardNode* pCurrent = pCD->m_pCards;
   struct CardNode* pFirst = NULL;
   struct CardNode* pSecond = NULL;

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

int CopyCard(Card* pCard, Card cOrig)
{
   DEBUG_FUNC_NAME;

   struct CardNode* pOrig = (struct CardNode*)cOrig;

   struct CardNode* pC = malloc(sizeof(struct CardNode));
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

int GetCardValue(Card c)
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

int SetSuit(Card c, int nSuit)
{
   DEBUG_FUNC_NAME;

   struct CardNode* pNode = (struct CardNode*)c;

   pNode->m_nSuit = nSuit;

   return CARDLIB_OK;
}

int SetCardValue(Card c, int nValue)
{
   DEBUG_FUNC_NAME;

   struct CardNode* pNode = (struct CardNode*)c;

   pNode->m_nValue = nValue;

   return CARDLIB_OK;
}

int SwapCardValues(Card c1, Card c2)
{
   int nSuit = GetSuit(c1);
   int nValue = GetCardValue(c1);
   void* ptr = GetCardExtraData(c1);
   int nSuit2 = GetSuit(c2);
   int nValue2 = GetCardValue(c2);
   void* ptr2 = GetCardExtraData(c2);
   DEBUG_MSG("Card 1: %d,%d; Card2: %d,%d\n", nSuit, nValue, nSuit2, nValue2);

   SetSuit(c1, nSuit2); SetSuit(c2, nSuit);
   SetCardValue(c1, nValue2); SetCardValue(c2, nValue);
   SetCardExtraData(c1, ptr2); SetCardExtraData(c2, ptr);

   return CARDLIB_OK;
}

