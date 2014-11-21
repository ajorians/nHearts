//Public domain :)

#ifdef _TINSPIRE
#include <os.h>
#else
#include <stdio.h>
#include <stdlib.h>
#endif
#include "HeartsLib/HeartsLib.h"
#include "Defines.h"

#define NUMBER_OF_HEARTS_PLAYERS	4
#define HEARTS_NUMBER_OF_JOKERS		0

struct ScoreNode
{
   int m_nPointsTaken;
   struct ScoreNode* m_pNext;
};

struct HeartsPlayer
{
   CardLib m_cardsHand;//Current cards in hand
   CardLib m_cardsTaken;//Cards received in round
   CardLib m_cardsQueued;//Cards passed
   struct ScoreNode* m_pScores;
};

struct Hearts
{
   int m_nLastError;
   struct HeartsPlayer m_Players[NUMBER_OF_HEARTS_PLAYERS];
   int m_bPassedCards;
   int m_bHeartsBroken;
   int m_nLastTrumpPlayer;//Last player to take the cards and the character who played the 2 club
   CardLib m_cardsMiddle;//Cards being played
   int m_nScoreLimit;
   int m_nJackDiamonds;
   Pass_Direction_t m_ePassDirection;
};

void AddScoreAmount(struct HeartsPlayer* pPlayer, int nAmount);

void SortCards(CardLib cards)
{
   //First let's sort by suit
   int nMadeChange = 1;
   while( nMadeChange == 1 ) {
      int nNumCards;
      int i;
      nNumCards = GetNumberOfCards(cards);
      DEBUG_MSG("NumCards: %d\n", nNumCards);
      nMadeChange = 0;
      for(i=1; i<nNumCards; i++) {
         Card c, cPrevious;
         Suit_t eSuit, eSuitPrevious;
         int nSwap;
         GetCard(cards, &cPrevious, i-1);
         GetCard(cards, &c, i);

         eSuit = GetSuit(c), eSuitPrevious = GetSuit(cPrevious);
         DEBUG_MSG("Suit1: %d; Suit2: %d\n", eSuit, eSuitPrevious);

         nSwap = 0;
         if( eSuit != eSuitPrevious ) {
            //Order: Clubs(3) Diamonds(1) Spades(2) Hearts(0)
            if( eSuit == Clubs ) {
               nSwap = 1;
            }
            else if ( eSuit == Diamonds && (eSuitPrevious == Spades || eSuitPrevious == Hearts) ) {
               nSwap = 1;
            }
            else if( eSuit == Spades && eSuitPrevious == Hearts ){
               nSwap = 1;
            }

         }

         if( nSwap == 1 ) {
            DEBUG_MSG("Swapping 2 cards\n");
            SwapCards(cards, i-1, i);
            nMadeChange = 1;
         }
      }
   }

   nMadeChange = 1;
   while( nMadeChange == 1 ) {
      int nNumCards;
      int i;
      nNumCards = GetNumberOfCards(cards);
      DEBUG_MSG("NumCards: %d\n", nNumCards);
      nMadeChange = 0;
      for(i=1; i<nNumCards; i++) {
         Card c, cPrevious;
         Suit_t eSuit, eSuitPrevious;
         int nSwap;
         GetCard(cards, &cPrevious, i-1);
         GetCard(cards, &c, i);

         eSuit = GetSuit(c), eSuitPrevious = GetSuit(cPrevious);
         DEBUG_MSG("Suit1: %d; Suit2: %d\n", eSuit, eSuitPrevious);

         nSwap = 0;
         if( eSuit == eSuitPrevious ) {
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
            SwapCards(cards, i-1, i);
            nMadeChange = 1;
         }
      }
   }
}

int DealHands(HeartsLib api)
{
   struct Hearts* pH;
   int nPlayerIndex;
   CardLib cardDeck;
   int nNumCards;
   int nCard;
   pH = (struct Hearts*)api;

   for(nPlayerIndex = 0; nPlayerIndex < NUMBER_OF_HEARTS_PLAYERS; nPlayerIndex++) {
      RemoveAllCards(pH->m_Players[nPlayerIndex].m_cardsHand, CARDLIB_FREE_CARD);
      RemoveAllCards(pH->m_Players[nPlayerIndex].m_cardsTaken, CARDLIB_FREE_CARD);
      RemoveAllCards(pH->m_Players[nPlayerIndex].m_cardsQueued, CARDLIB_FREE_CARD);
   }

   pH->m_bPassedCards = 0;
   pH->m_bHeartsBroken = 0;
   pH->m_nLastTrumpPlayer = -1;

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

   nNumCards = GetNumberOfCards(cardDeck);//Better be 52 :)
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

   if( pH->m_ePassDirection == NoPass )
     pH->m_bPassedCards = 1;

   return HEARTSLIB_OK;
}

int HeartsLibCreate(HeartsLib* api, int nScoreLimit, int nJackDiamonds)
{
   struct Hearts* pH;
   int nPlayerIndex;
   int nDealHands;
   DEBUG_FUNC_NAME;

   pH = malloc(sizeof(struct Hearts));
   if( pH == NULL ){//Out of memory
      return HEARTSLIB_OUT_OF_MEMORY;
   }

   for(nPlayerIndex = 0; nPlayerIndex < NUMBER_OF_HEARTS_PLAYERS; nPlayerIndex++) {
      if( CARDLIB_OK != CardLibCreate(&pH->m_Players[nPlayerIndex].m_cardsHand) ) {
         return HEARTSLIB_OUT_OF_MEMORY;//Assuming
      }
      if( CARDLIB_OK != CardLibCreate(&pH->m_Players[nPlayerIndex].m_cardsTaken) ) {
         return HEARTSLIB_OUT_OF_MEMORY;//Assuming
      }
      if( CARDLIB_OK != CardLibCreate(&pH->m_Players[nPlayerIndex].m_cardsQueued) ) {
         return HEARTSLIB_OUT_OF_MEMORY;//Assuming
      }
      pH->m_Players[nPlayerIndex].m_pScores = NULL;
   }

   if( CARDLIB_OK != CardLibCreate(&pH->m_cardsMiddle) )
      return HEARTSLIB_OUT_OF_MEMORY;//Assuming

   pH->m_ePassDirection = PassLeft;
   if( nScoreLimit <= 0 ) {
      pH->m_nScoreLimit = 100;
   }
   else {
      pH->m_nScoreLimit = nScoreLimit;
   }
   pH->m_nJackDiamonds = nJackDiamonds;

   pH->m_nLastError = HEARTSLIB_OK;

   *api = pH;

   nDealHands = DealHands(*api);
   if( HEARTSLIB_OK != nDealHands ) {
      free(pH);
      return nDealHands;
   }

   return HEARTSLIB_OK;
}

int HeartsLibCopy(HeartsLib* copyapi, HeartsLib orig)
{
   struct Hearts* pHOrig;
   struct Hearts* pH;
   int nPlayerIndex;
   DEBUG_FUNC_NAME;

   pHOrig = (struct Hearts*)orig;

   pH = malloc(sizeof(struct Hearts));
   if( pH == NULL ){//Out of memory
      return HEARTSLIB_OUT_OF_MEMORY;
   }

   for(nPlayerIndex = 0; nPlayerIndex < NUMBER_OF_HEARTS_PLAYERS; nPlayerIndex++) {
      struct ScoreNode* pNode;
      if( CARDLIB_OK != CardLibCopy(&pH->m_Players[nPlayerIndex].m_cardsHand, pHOrig->m_Players[nPlayerIndex].m_cardsHand) ) {
         return HEARTSLIB_OUT_OF_MEMORY;//Assuming
      }
      if( CARDLIB_OK != CardLibCopy(&pH->m_Players[nPlayerIndex].m_cardsTaken, pHOrig->m_Players[nPlayerIndex].m_cardsTaken) ) {
         return HEARTSLIB_OUT_OF_MEMORY;//Assuming
      }
      if( CARDLIB_OK != CardLibCopy(&pH->m_Players[nPlayerIndex].m_cardsQueued, pHOrig->m_Players[nPlayerIndex].m_cardsQueued) ) {
         return HEARTSLIB_OUT_OF_MEMORY;//Assuming
      }
      pH->m_Players[nPlayerIndex].m_pScores = NULL;
      pNode = pHOrig->m_Players[nPlayerIndex].m_pScores;
      while(pNode != NULL) {
         AddScoreAmount(&pHOrig->m_Players[nPlayerIndex], pNode->m_nPointsTaken);
         pNode = pNode->m_pNext;
      }
   }

   if( CARDLIB_OK != CardLibCopy(&pH->m_cardsMiddle, pHOrig->m_cardsMiddle) )
      return HEARTSLIB_OUT_OF_MEMORY;//Assuming

   pH->m_nLastError = pHOrig->m_nLastError;
   pH->m_bPassedCards = pHOrig->m_bPassedCards;
   pH->m_bHeartsBroken = pHOrig->m_bHeartsBroken;
   pH->m_nLastTrumpPlayer = pHOrig->m_nLastTrumpPlayer;
   pH->m_nScoreLimit = pHOrig->m_nScoreLimit;
   pH->m_nJackDiamonds = pHOrig->m_nJackDiamonds;
   pH->m_ePassDirection = pHOrig->m_ePassDirection;

   *copyapi = pH;
   return HEARTSLIB_OK;
}

int HeartsLibFree(HeartsLib* api)
{
   struct Hearts* pH;
   int nPlayerIndex;
   DEBUG_FUNC_NAME;

   pH = *api;

   for(nPlayerIndex = 0; nPlayerIndex < NUMBER_OF_HEARTS_PLAYERS; nPlayerIndex++) {
      struct ScoreNode *pNode, *pNext;

      CardLibFree(&pH->m_Players[nPlayerIndex].m_cardsHand);
      CardLibFree(&pH->m_Players[nPlayerIndex].m_cardsTaken);

      pNode = pH->m_Players[nPlayerIndex].m_pScores;
      while(pNode != NULL) {
         pNext = pNode->m_pNext;
         free(pNode);
         pNode = pNext;
      }
   }

   free(pH);
   *api = NULL;
   return HEARTSLIB_OK;
}

int GetHeartsLibError(HeartsLib api)
{
   struct Hearts* pH;
   DEBUG_FUNC_NAME;

   pH = (struct Hearts*)api;
   return pH->m_nLastError;
}

void ClearHeartsLibError(HeartsLib api)
{
   struct Hearts* pH;
   DEBUG_FUNC_NAME;

   pH = (struct Hearts*)api;
   pH->m_nLastError = HEARTSLIB_OK;
}

//HeartsLib related functions
Pass_Direction_t GetHeartsPassDirection(HeartsLib api)
{
   struct Hearts* pH;
   DEBUG_FUNC_NAME;

   pH = (struct Hearts*)api;
   return pH->m_ePassDirection;
}

int GetNumberOfCardsInHand(HeartsLib api, int nPlayerIndex)
{
   struct Hearts* pH;
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   pH = (struct Hearts*)api;
   return GetNumberOfCards(pH->m_Players[nPlayerIndex].m_cardsHand);
}

int GetCardInHand(HeartsLib api, Card* pCard, int nPlayerIndex, int nCard)
{
   struct Hearts* pH;
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   if( nCard < 0 || nCard > GetNumberOfCardsInHand(api, nPlayerIndex) )
      return HEARTSLIB_BADARGUMENT;

   pH = (struct Hearts*)api;

   GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, pCard, nCard);
   return HEARTSLIB_OK;
}

int GetHeartsGameOver(HeartsLib api)
{
   struct Hearts* pH;
   int nPlayerIndex;
   DEBUG_FUNC_NAME;

   pH = (struct Hearts*)api;

   for(nPlayerIndex=0; nPlayerIndex < NUMBER_OF_HEARTS_PLAYERS; nPlayerIndex++) {
      if( GetHeartsPlayerScore(api, nPlayerIndex) >= pH->m_nScoreLimit )
         return HEARTSLIB_GAME_OVER;
   }

   return HEARTSLIB_STILL_PLAYING;
}

int ToggleSelectedCard(HeartsLib api, int nPlayerIndex, int nCard)
{
   struct Hearts* pH;
   Card c;
   void* ptr;
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   if( nCard < 0 || nCard > GetNumberOfCardsInHand(api, nPlayerIndex) )
      return HEARTSLIB_BADARGUMENT;

   pH = (struct Hearts*)api;

   GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &c, nCard);

   ptr = GetCardExtraData(c);
   if( ptr == NULL ) {
      SetCardExtraData(c, (void*)1);
   }
   else {
      SetCardExtraData(c, (void*)NULL);
   }

   return HEARTSLIB_OK;
}

int IsCardSelected(HeartsLib api, int nPlayerIndex, int nCardIndex)
{
   struct Hearts* pH;
   Card c;
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   if( nCardIndex < 0 || nCardIndex > GetNumberOfCardsInHand(api, nPlayerIndex) )
      return HEARTSLIB_BADARGUMENT;

   pH = (struct Hearts*)api;

   GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &c, nCardIndex);

   if( GetCardExtraData(c) != NULL )
      return HEARTSLIB_CARD_SELECTED;

   return HEARTSLIB_CARD_NOT_SELECTED;
}

int GetNumberSelectedCards(HeartsLib api, int nPlayerIndex)
{
   struct Hearts* pH;
   int nNumCards;
   int nNumSelected;
   int i;
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   pH = (struct Hearts*)api;

   nNumCards = GetNumberOfCardsInHand(api, nPlayerIndex);
   nNumSelected = 0;
   for(i=0; i<nNumCards; i++) {
      Card c;
      GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &c, i);
      if( GetCardExtraData(c) != NULL )
         nNumSelected++;
   }

   return nNumSelected;
}

int GetPlayerIndexPassingTo(HeartsLib api, int nPlayerIndex)
{
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   if( GetHeartsPassDirection(api) == NoPass )
      return nPlayerIndex;

   if( GetHeartsPassDirection(api) == PassAcross ) {
      if( nPlayerIndex == 0 )
         return 2;
      if( nPlayerIndex == 1 )
         return 3;
      if( nPlayerIndex == 2 )
         return 0;
      if( nPlayerIndex == 3 )
         return 1;
   }

   if( GetHeartsPassDirection(api) == PassLeft ) {
      if( nPlayerIndex == 0 )
         return 3;
      return nPlayerIndex - 1;
   }

   if( GetHeartsPassDirection(api) == PassRight ) { 
      if( nPlayerIndex == 3 )
         return 0;
      return nPlayerIndex + 1;
   }

   return HEARTSLIB_BADARGUMENT;
}

int PassSelectedCards(HeartsLib api, int nPlayerIndex)
{
   struct Hearts* pH;
   int nNumCards;
   int i;
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   pH = (struct Hearts*)api;

   if( GetHeartsPassDirection(api) == NoPass ) {
      pH->m_bPassedCards = 1;
      return HEARTSLIB_OK;
   }

   if( GetNumberSelectedCards(api, nPlayerIndex) != 3 )
      return HEARTSLIB_CARD_FAILURE;

   nNumCards = GetNumberOfCardsInHand(api, nPlayerIndex);
   for(i=nNumCards; i-->0;) {
      Card c;
      GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &c, i);
      if( GetCardExtraData(c) != NULL ) {
         int nPassToPlayerIndex = GetPlayerIndexPassingTo(api, nPlayerIndex);
         RemoveCard(pH->m_Players[nPlayerIndex].m_cardsHand, i, CARDLIB_REMOVE_CARD_ONLY);
         //SetCardExtraData(c, NULL);So I know who sent what card
         AddCard(pH->m_Players[nPassToPlayerIndex].m_cardsQueued, c);
      }
   }

   //If everybody has passed then put the queued cards into everybodies hand.
   for(i=0; i<NUMBER_OF_HEARTS_PLAYERS; i++) {
      if( GetNumberOfCards(pH->m_Players[i].m_cardsQueued) != 3 )
         return HEARTSLIB_OK;
   }
   
   //If here that means all players have passed their cards
   for(i=0; i<NUMBER_OF_HEARTS_PLAYERS; i++) {
      int nCards = GetNumberOfCards(pH->m_Players[i].m_cardsQueued);
      int j;
      for(j = 0; j<nCards; j++ ) {
         Card c;
         TakeNextCard(pH->m_Players[i].m_cardsQueued, &c);
         AddCard(pH->m_Players[i].m_cardsHand, c);
      }
      SortCards(pH->m_Players[i].m_cardsHand);
   }
   pH->m_bPassedCards = 1;

   return HEARTSLIB_OK;
}

int HasPassedCards(HeartsLib api)
{
   struct Hearts* pH;
   DEBUG_FUNC_NAME;

   pH = (struct Hearts*)api;

   return pH->m_bPassedCards == 1 ? HEARTSLIB_PASSED_CARDS : HEARTSLIB_NOT_PASSED_CARDS;
}

int GetPlayersTurn(HeartsLib api)
{
   struct Hearts* pH;
   DEBUG_FUNC_NAME;

   pH = (struct Hearts*)api;

   if( pH->m_nLastTrumpPlayer == -1 ) {//Whomever has the 2 of Clubs
      int nPlayerIndex;
      for(nPlayerIndex = 0; nPlayerIndex < NUMBER_OF_HEARTS_PLAYERS; nPlayerIndex++) {
         int nNumCards = GetNumberOfCards(pH->m_Players[nPlayerIndex].m_cardsHand);
         int nCard;
         for(nCard = 0; nCard < nNumCards; nCard++) {
            Card c;
            Suit_t eSuit;
            GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &c, nCard);
            eSuit = GetSuit(c);
            if( eSuit == Clubs ) {
               int nValue = GetCardValue(c);
               if( nValue == 2 ) {
                  return nPlayerIndex;
               }
            }
         }
      }
   }
   else {
      int nCardsInMiddle = GetNumberOfCards(pH->m_cardsMiddle);
      int nPlayersTurn = pH->m_nLastTrumpPlayer+nCardsInMiddle;
      return nPlayersTurn % NUMBER_OF_HEARTS_PLAYERS;
   }

   return HEARTSLIB_BADARGUMENT;//TODO: New define
}

int CanPlayCard(HeartsLib api, int nPlayerIndex, int nCardIndex)
{
   struct Hearts* pH;
   Card c;
   Suit_t eSuit;
   int nValue;
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   if( nCardIndex < 0 || nCardIndex >= GetNumberOfCardsInHand(api, nPlayerIndex) )
      return HEARTSLIB_BADARGUMENT;

   pH = (struct Hearts*)api;

   if( pH->m_bPassedCards == 0 && GetHeartsPassDirection(api) != NoPass )
      return HEARTSLIB_BADARGUMENT;

   GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &c, nCardIndex);
   eSuit = GetSuit(c);
   nValue = GetCardValue(c);

   if( pH->m_nLastTrumpPlayer == -1 ) {//Can only be passing the 2 of clubs
      int nValue;
      if( eSuit != Clubs ) {
         return HEARTSLIB_CANNOT_PLAY_CARD;
      }

      nValue = GetCardValue(c);
      if( nValue != 2 ) {
         return HEARTSLIB_CANNOT_PLAY_CARD;
      }

      return HEARTSLIB_CAN_PLAY_CARD;
   }
   else {
      if( GetNumberOfCards(pH->m_cardsMiddle) == 0 ) {//Can play "any" suit
         //Can't start a heart unless that is all you have or hearts have been broken
         if( eSuit == Hearts && pH->m_bHeartsBroken != 1 ) {
            //Check player only have Hearts
            int nNumCards = GetNumberOfCards(pH->m_Players[nPlayerIndex].m_cardsHand);
            int nCard;
            for(nCard = 0; nCard < nNumCards; nCard++) {
               Card cardInHand;
               Suit_t eSuitInHand;
               GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &cardInHand, nCard);
               eSuitInHand = GetSuit(cardInHand);
               if( eSuitInHand != Hearts ) {
                  return HEARTSLIB_CANNOT_PLAY_CARD;
               }
            }
         }
      }
      else {//Has to match suit unless don't have that suit.
         //That is unless it is the first turn; then can't play hearts/Queen of Spades
         Card cardFirst;
         Suit_t eCurrentSuit;
         GetCard(pH->m_cardsMiddle, &cardFirst, 0);
         eCurrentSuit = GetSuit(cardFirst);
         if( eSuit != eCurrentSuit ) {
            int n2ClubsHand = (GetCardValue(cardFirst) == 2 && eCurrentSuit == Clubs) ? 1 : 0;
            //Player can only do this if they don't have any of that suit
            int nNumCards = GetNumberOfCards(pH->m_Players[nPlayerIndex].m_cardsHand);
            int nCard;
            for(nCard = 0; nCard < nNumCards; nCard++) {
               Card cardInHand;
               Suit_t eSuitInHand;
               GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &cardInHand, nCard);
               eSuitInHand = GetSuit(cardInHand);
               if( eSuitInHand == eCurrentSuit ) {
                  return HEARTSLIB_CANNOT_PLAY_CARD;
               }
            }
            if( n2ClubsHand == 1 ) {
               if( eSuit == Hearts || (eSuit == Spades && nValue == QUEEN) ) {
                  //If you have any card that is not a heart or Queen of spades then you need to play that
                  int nNumCards = GetNumberOfCards(pH->m_Players[nPlayerIndex].m_cardsHand);
                  int nCard;
                  for(nCard = 0; nCard < nNumCards; nCard++) {
                     Card cardInHand;
                     Suit_t eSuitInHand;
                     int nValueInHand;
                     GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &cardInHand, nCard);
                     eSuitInHand = GetSuit(cardInHand);
                     nValueInHand = GetCardValue(cardInHand);
                     if( eSuitInHand != Hearts && ! (eSuitInHand != Spades && nValueInHand != QUEEN) )
                        return HEARTSLIB_CANNOT_PLAY_CARD;
                  }
               }
            }
         }
      }
   }

   return HEARTSLIB_CAN_PLAY_CARD;
}

int PlayCard(HeartsLib api, int nPlayerIndex, int nCardIndex)
{
   struct Hearts* pH;
   int nRet;
   int nPlayersTurn;
   int nNumCards;
   int i;
   Card c;
   DEBUG_FUNC_NAME;

   pH = (struct Hearts*)api;

   nRet = CanPlayCard(api, nPlayerIndex, nCardIndex);
   if( nRet != HEARTSLIB_CAN_PLAY_CARD ) {
      if( nRet == HEARTSLIB_CANNOT_PLAY_CARD )
         return HEARTSLIB_FAIL_PLAY_CARD;
      return nRet;
   }

   nPlayersTurn = GetPlayersTurn(api);
   if( nPlayerIndex != nPlayersTurn )
      return HEARTSLIB_BADARGUMENT;//Not your turn

   //Remove the selected-ness of cards in hand
   nNumCards = GetNumberOfCardsInHand(api, nPlayerIndex);
   for(i=0; i<nNumCards; i++) {
      GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &c, i);
      if( GetCardExtraData(c) != NULL )
         SetCardExtraData(c, NULL);
   }

   GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &c, nCardIndex);

   if( pH->m_nLastTrumpPlayer == -1 ) {//Can only be the 2 of clubs
      pH->m_nLastTrumpPlayer = nPlayersTurn;
   }

   if( GetSuit(c) == HEARTS )
      pH->m_bHeartsBroken = 1;

   RemoveCard(pH->m_Players[nPlayerIndex].m_cardsHand, nCardIndex, CARDLIB_REMOVE_CARD_ONLY);
   AddCard(pH->m_cardsMiddle, c);

   return HEARTSLIB_OK;
}

void AddScoreAmount(struct HeartsPlayer* pPlayer, int nAmount)
{
   struct ScoreNode* pNode;
   struct ScoreNode* pScore = malloc(sizeof(struct ScoreNode));
   if( pScore == NULL )
      return;//!

   pScore->m_nPointsTaken = nAmount;
   pScore->m_pNext = NULL;

   if( pPlayer->m_pScores == NULL ) {
      pPlayer->m_pScores = pScore;
   }
   else {
      pNode = pPlayer->m_pScores;
      while( pNode->m_pNext != NULL ) {
         pNode = pNode->m_pNext;
      }
      pNode->m_pNext = pScore;
   }
}

int GiveTrickToPlayer(HeartsLib api)
{
   struct Hearts* pH;
   int nPlayerTakesTrick;
   int nCard;
   int nPlayer;
   int nPlayerWithJDCard, nPlayerShotMoon;
   DEBUG_FUNC_NAME;

   if( !HasEverybodyPlayedTheirCard(api) )
      return HEARTSLIB_BADARGUMENT;

   pH = (struct Hearts*)api;

   nPlayerTakesTrick = -1;
   FigureOutWhoTakesTrick(api, &nPlayerTakesTrick);

   //Give the cards to the player
   pH->m_nLastTrumpPlayer = nPlayerTakesTrick;
   for(nCard = 0; nCard < NUMBER_OF_HEARTS_PLAYERS; nCard++) {
      Card cardPlayed;
      TakeNextCard(pH->m_cardsMiddle, &cardPlayed);
      AddCard(pH->m_Players[nPlayerTakesTrick].m_cardsTaken, cardPlayed);
   }

   for(nPlayer = 0; nPlayer < NUMBER_OF_HEARTS_PLAYERS; nPlayer++) {
      if( GetNumberOfCardsInHand(api, nPlayer) != 0 )
      return HEARTSLIB_OK;
   }

   //Check to see who took the Jack of Diamonds
   nPlayerWithJDCard = -1;
   for(nPlayer = 0; nPlayer < NUMBER_OF_HEARTS_PLAYERS; nPlayer++) {
      int nNumCards;
      int i;
      if( nPlayerWithJDCard != -1 )
         break;
      nNumCards = GetNumberOfCards(pH->m_Players[nPlayer].m_cardsTaken);
      for(i=0; i<nNumCards; i++) {
         Card c;
         GetCard(pH->m_Players[nPlayer].m_cardsTaken, &c, i);
         if( GetSuit(c) == DIAMONDS && GetCardValue(c) == JACK ) {
            nPlayerWithJDCard = nPlayer;
            break;
         }
      }
   }

   //If here means all players have no cards so update score
   //Update scores
   nPlayerShotMoon = -1;
   if( HEARTSLIB_SHOT_THE_MOON == GetPlayerShotMoon(api, &nPlayerShotMoon) ) {
      int i;
      for(i=0; i<NUMBER_OF_HEARTS_PLAYERS; i++) {
         int nAmount = 0;
         nAmount += i != nPlayerShotMoon ? 26 : 0;
         if( i == nPlayerWithJDCard )
            nAmount += pH->m_nJackDiamonds;//Usually a negative value
         AddScoreAmount(&pH->m_Players[i], nAmount);
      }
   }
   else {
      for(nPlayer = 0; nPlayer < NUMBER_OF_HEARTS_PLAYERS; nPlayer++) {
         int nScore = ScoreOfCardsTaken(api, nPlayer);
         if( nPlayer == nPlayerWithJDCard )
            nScore += pH->m_nJackDiamonds;//Usually a negative value
         AddScoreAmount(&pH->m_Players[nPlayer], nScore);
      }
   }

   return HEARTSLIB_OK;
}

int DoHeartsNextHand(HeartsLib api)
{
   int nPlayer;
   struct Hearts* pH;
   DEBUG_FUNC_NAME;

   //The middle can be empty if called GiveTrickToPlayer

   for(nPlayer = 0; nPlayer < NUMBER_OF_HEARTS_PLAYERS; nPlayer++) {
      if( GetNumberOfCardsInHand(api, nPlayer) != 0 )
      return HEARTSLIB_BADARGUMENT;
   }

   pH = (struct Hearts*)api;

   //If have cards in middle then haven't Given Trick to player
   if( GetNumberOfCards(pH->m_cardsMiddle) != 0 )
      GiveTrickToPlayer(api);

   //Reset things
   if( pH->m_ePassDirection == PassLeft )
      pH->m_ePassDirection = PassRight;
   else if( pH->m_ePassDirection == PassRight )
      pH->m_ePassDirection = PassAcross;
   else if( pH->m_ePassDirection == PassAcross )
      pH->m_ePassDirection = NoPass;
   else
      pH->m_ePassDirection = PassLeft;

   DealHands(api);

   return HEARTSLIB_OK;
}

int FigureOutWhoTakesTrick(HeartsLib api, int* pPlayerIndex)
{
   struct Hearts* pH;
   Card cardFirst;
   Suit_t eCurrentSuit;
   int nCurrentValue;
   int nPlayersTurn;
   int nHighest;
   int nCard;
   DEBUG_FUNC_NAME;

   if( HasEverybodyPlayedTheirCard(api) != HEARTSLIB_PLAYERS_PLAYED_CARDS )
      return HEARTSLIB_BADARGUMENT;

   pH = (struct Hearts*)api;

   //Figure out who had the highest
   GetCard(pH->m_cardsMiddle, &cardFirst, 0);
   eCurrentSuit = GetSuit(cardFirst);
   nCurrentValue = GetCardValue(cardFirst);

   //So the player who just went was the last to go lets set nPlayersTurn to the first to play
   nPlayersTurn = GetPlayersTurn(api);
   nHighest = nPlayersTurn;
   for(nCard = 1; nCard < NUMBER_OF_HEARTS_PLAYERS; nCard++) {
      Card cardPlayed;
      Suit_t eSuit;
      int nValue;
      GetCard(pH->m_cardsMiddle, &cardPlayed, nCard);
      eSuit = GetSuit(cardPlayed);
      if( eSuit != eCurrentSuit )
         continue;

      nValue = GetCardValue(cardPlayed);
      if( nCurrentValue == ACE ){}
      else if( nValue == ACE ) {
         nCurrentValue = nValue;
         nHighest = (nPlayersTurn + nCard) % NUMBER_OF_HEARTS_PLAYERS;
      }
      else if( nValue > nCurrentValue ) {
         nCurrentValue = nValue;
         nHighest = (nPlayersTurn + nCard) % NUMBER_OF_HEARTS_PLAYERS;
      }
   }
   if( pPlayerIndex != NULL )
      *pPlayerIndex = nHighest;

   return HEARTSLIB_OK;
}

int HasEverybodyPlayedTheirCard(HeartsLib api)
{
   struct Hearts* pH;
   DEBUG_FUNC_NAME;

   pH = (struct Hearts*)api;

   return (GetNumberOfCards(pH->m_cardsMiddle) == NUMBER_OF_HEARTS_PLAYERS) ? HEARTSLIB_PLAYERS_PLAYED_CARDS : HEARTSLIB_TURN_IN_PLAY;
}

int GetNumberOfCardsInMiddle(HeartsLib api)
{
   struct Hearts* pH;
   DEBUG_FUNC_NAME;

   pH = (struct Hearts*)api;

   return GetNumberOfCards(pH->m_cardsMiddle);
}

int GetMiddleCard(HeartsLib api, Card* pCard, int nCardIndex, int* pPlayerIndex)
{
   struct Hearts* pH;
   int nNumCards;
   int nPlayer;
   DEBUG_FUNC_NAME;

   if( nCardIndex < 0 || nCardIndex > GetNumberOfCardsInMiddle(api) )
      return HEARTSLIB_BADARGUMENT;

   pH = (struct Hearts*)api;

   GetCard(pH->m_cardsMiddle, pCard, nCardIndex);

   nNumCards = GetNumberOfCardsInMiddle(api);
   nPlayer = (pH->m_nLastTrumpPlayer + nCardIndex) % NUMBER_OF_HEARTS_PLAYERS;
   if( pPlayerIndex )
      *pPlayerIndex = nPlayer;

   return HEARTSLIB_OK;
}

int ScoreOfCardsTaken(HeartsLib api, int nPlayerIndex)
{
   struct Hearts* pH;
   int nNumCards;
   int nSum;
   int i;
   DEBUG_FUNC_NAME;

   pH = (struct Hearts*)api;

   nNumCards = GetNumberOfCards(pH->m_Players[nPlayerIndex].m_cardsTaken);

   nSum = 0;
   for(i=0; i<nNumCards; i++) {
      Card c;
      Suit_t eSuit;
      GetCard(pH->m_Players[nPlayerIndex].m_cardsTaken, &c, i);
      eSuit = GetSuit(c);
      if( eSuit == Hearts ) {
         nSum++;
      }
      else if( eSuit == Spades ) {
         int nValue = GetCardValue(c);
         if( nValue == QUEEN )
            nSum += 13;
      }
   }

   return nSum;
}

int GetPlayerShotMoon(HeartsLib api, int* pPlayerIndex)
{
   struct Hearts* pH;
   int nPlayer;
   DEBUG_FUNC_NAME;

   pH = (struct Hearts*)api;

   for(nPlayer = 0; nPlayer < NUMBER_OF_HEARTS_PLAYERS; nPlayer++) {
      if( ScoreOfCardsTaken(api, nPlayer) == 26 ) {
         if( pPlayerIndex ) *pPlayerIndex = nPlayer;
         return HEARTSLIB_SHOT_THE_MOON;
      }
   }
   return HEARTSLIB_DID_NOT_SHOOT;
}

int GetNumberOfRounds(HeartsLib api)
{
   struct Hearts* pH;
   struct ScoreNode* pScore;
   int i;
   DEBUG_FUNC_NAME;

   pH = (struct Hearts*)api;

   //Gets from number of score entries
   i=0;
   pScore = pH->m_Players[0].m_pScores;
   while(pScore) {
      i++;
      pScore = pScore->m_pNext;
   }
   return i;
}

int GetHeartsRoundScore(HeartsLib api, int nPlayerIndex, int nRoundIndex)
{
   struct Hearts* pH;
   struct ScoreNode* pScore;
   int i;
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   pH = (struct Hearts*)api;

   i=0;
   pScore = pH->m_Players[nPlayerIndex].m_pScores;
   while(pScore) {
      if( i++ == nRoundIndex )
         return pScore->m_nPointsTaken;
      pScore = pScore->m_pNext;
   }
   return 0;
}

int GetHeartsPlayerScore(HeartsLib api, int nPlayerIndex)
{
   struct Hearts* pH;
   int nTotal;
   struct ScoreNode* pScore;
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   pH = (struct Hearts*)api;
   nTotal = 0;
   pScore = pH->m_Players[nPlayerIndex].m_pScores;
   while(pScore) {
      nTotal += pScore->m_nPointsTaken;
      pScore = pScore->m_pNext;
   }
   return nTotal;
}

