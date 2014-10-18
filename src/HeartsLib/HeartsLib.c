//Public domain :)

#include <os.h>
#include "include/HeartsLib.h"
#include "Defines.h"

#define NUMBER_OF_HEARTS_PLAYERS	4
#define HEARTS_NUMBER_OF_JOKERS		0

struct HeartsPlayer
{
   CardLib m_cardsHand;//Current cards in hand
   CardLib m_cardsTaken;//Cards received in round
   CardLib m_cardsQueued;//Cards passed
   int m_nScore;
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

   pH->m_bPassedCards = 0;

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
      if( CARDLIB_OK != CardLibCreate(&pH->m_Players[nPlayerIndex].m_cardsQueued) ) {
         return HEARTSLIB_OUT_OF_MEMORY;//Assuming
      }
   }

   pH->m_bHeartsBroken = 0;
   pH->m_nLastTrumpPlayer = -1;
   if( CARDLIB_OK != CardLibCreate(&pH->m_cardsMiddle) )
      return HEARTSLIB_OUT_OF_MEMORY;//Assuming

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

int ToggleSelectedCard(HeartsLib api, int nPlayerIndex, int nCard)
{
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   if( nCard < 0 || nCard > GetNumberOfCardsInHand(api, nPlayerIndex) )
      return HEARTSLIB_BADARGUMENT;

   struct Hearts* pH = (struct Hearts*)api;

   Card c;
   GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &c, nCard);

   void* ptr = GetCardExtraData(c);
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
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   if( nCardIndex < 0 || nCardIndex > GetNumberOfCardsInHand(api, nPlayerIndex) )
      return HEARTSLIB_BADARGUMENT;

   struct Hearts* pH = (struct Hearts*)api;

   Card c;
   GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &c, nCardIndex);

   if( GetCardExtraData(c) != NULL )
      return HEARTSLIB_CARD_SELECTED;

   return HEARTSLIB_CARD_NOT_SELECTED;
}

int GetNumberSelectedCards(HeartsLib api, int nPlayerIndex)
{
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   struct Hearts* pH = (struct Hearts*)api;

   int nNumCards = GetNumberOfCardsInHand(api, nPlayerIndex);
   int nNumSelected = 0;
   int i;
   for(i=0; i<nNumCards; i++) {
      Card c;
      GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &c, i);
      if( GetCardExtraData(c) != NULL )
         nNumSelected++;
   }

   printf("Number of selected cards: %d\n", nNumSelected);
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
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   struct Hearts* pH = (struct Hearts*)api;

   if( GetHeartsPassDirection(api) == NoPass ) {
      pH->m_bPassedCards = 1;
      return HEARTSLIB_OK;
   }

   if( GetNumberSelectedCards(api, nPlayerIndex) != 3 )
      return HEARTSLIB_CARD_FAILURE;

   int nNumCards = GetNumberOfCardsInHand(api, nPlayerIndex);
   printf("NumCardsInHand: %d\n", nNumCards);
   int i;
   for(i=nNumCards; i-->0;) {
      Card c;
      GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &c, i);
      if( GetCardExtraData(c) != NULL ) {
         printf("Passing card index: %d\n", i);
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
   DEBUG_FUNC_NAME;

   struct Hearts* pH = (struct Hearts*)api;

   return pH->m_bPassedCards == 1 ? HEARTSLIB_PASSED_CARDS : HEARTSLIB_NOT_PASSED_CARDS;
}

int GetPlayersTurn(HeartsLib api)
{
   DEBUG_FUNC_NAME;

   struct Hearts* pH = (struct Hearts*)api;

   if( pH->m_nLastTrumpPlayer == -1 ) {//Whomever has the 2 of Clubs
      int nPlayerIndex;
      for(nPlayerIndex = 0; nPlayerIndex < NUMBER_OF_HEARTS_PLAYERS; nPlayerIndex++) {
         int nNumCards = GetNumberOfCards(pH->m_Players[nPlayerIndex].m_cardsHand);
         int nCard;
         for(nCard = 0; nCard < nNumCards; nCard++) {
            Card c;
            GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &c, nCard);
            int nSuit = GetSuit(c);
            if( nSuit == CLUBS ) {
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
   DEBUG_FUNC_NAME;

   if( nPlayerIndex < 0 || nPlayerIndex >= NUMBER_OF_HEARTS_PLAYERS )
      return HEARTSLIB_BADARGUMENT;

   if( nCardIndex < 0 || nCardIndex > GetNumberOfCardsInHand(api, nPlayerIndex) )
      return HEARTSLIB_BADARGUMENT;

   struct Hearts* pH = (struct Hearts*)api;

   if( pH->m_bPassedCards == 0 && GetHeartsPassDirection(api) != NoPass )
      return HEARTSLIB_BADARGUMENT;

   int nPlayersTurn = GetPlayersTurn(api);
   if( nPlayerIndex != nPlayersTurn )
      return HEARTSLIB_BADARGUMENT;//Not your turn

   Card c;
   GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &c, nCardIndex);

   if( pH->m_nLastTrumpPlayer == -1 ) {//Can only be passing the 2 of clubs
      int nSuit = GetSuit(c);
      if( nSuit != CLUBS ) {
         printf("Need to play 2club and this isn't a club\n");
         return HEARTSLIB_CANNOT_PLAY_CARD;
      }

      int nValue = GetCardValue(c);
      if( nValue != 2 ) {
         printf("Need to play 2club and this isn't a 2\n");
         return HEARTSLIB_CANNOT_PLAY_CARD;
      }

      printf("2club is ok card to play\n");
      return HEARTSLIB_CAN_PLAY_CARD;
   }
   else {
      int nSuit = GetSuit(c);
      if( GetNumberOfCards(pH->m_cardsMiddle) == 0 ) {//Can play "any" suit
         //Can't start a heart unless that is all you have or hearts have been broken
         if( nSuit == HEARTS && pH->m_bHeartsBroken != 1 ) {
            printf("Trying to play a heart\n");
            //Check player only have Hearts
            int nNumCards = GetNumberOfCards(pH->m_Players[nPlayerIndex].m_cardsHand);
            int nCard;
            for(nCard = 0; nCard < nNumCards; nCard++) {
               Card cardInHand;
               GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &cardInHand, nCard);
               int nSuitInHand = GetSuit(cardInHand);
               if( nSuitInHand != HEARTS ) {
                  printf("You have other cards and hearts hasn't been broken\n");
                  return HEARTSLIB_CANNOT_PLAY_CARD;
               }
            }
         }
      }
      else {//Has to match suit unless don't have that suit.
         Card cardFirst;
         GetCard(pH->m_cardsMiddle, &cardFirst, 0);
         int nCurrentSuit = GetSuit(cardFirst);
         if( nSuit != nCurrentSuit ) {
            //Player can only do this if they don't have any of that suit
            int nNumCards = GetNumberOfCards(pH->m_Players[nPlayerIndex].m_cardsHand);
            int nCard;
            for(nCard = 0; nCard < nNumCards; nCard++) {
               Card cardInHand;
               GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &cardInHand, nCard);
               int nSuitInHand = GetSuit(cardInHand);
               if( nSuitInHand == nCurrentSuit ) {
                  printf("Cards were played and you are not matching suit and you can\n");
                  return HEARTSLIB_CANNOT_PLAY_CARD;
               }
            }
         }
      }
   }

   printf("Seems fine to play this card\n");
   return HEARTSLIB_CAN_PLAY_CARD;
}

int PlayCard(HeartsLib api, int nPlayerIndex, int nCardIndex)
{
   DEBUG_FUNC_NAME;

   struct Hearts* pH = (struct Hearts*)api;

   int nRet = CanPlayCard(api, nPlayerIndex, nCardIndex);
   if( nRet != HEARTSLIB_CAN_PLAY_CARD ) {
      if( nRet == HEARTSLIB_CANNOT_PLAY_CARD )
         return HEARTSLIB_FAIL_PLAY_CARD;
      return nRet;
   }

   printf("Can play card player: %d, cardindex: %d\n", nPlayerIndex, nCardIndex);

   int nPlayersTurn = GetPlayersTurn(api);
   printf("Players turn: %d\n", nPlayersTurn);

   Card c;
   GetCard(pH->m_Players[nPlayerIndex].m_cardsHand, &c, nCardIndex);

   if( pH->m_nLastTrumpPlayer == -1 ) {//Can only be passing the 2 of clubs
      pH->m_nLastTrumpPlayer = nPlayersTurn;
   }

   RemoveCard(pH->m_Players[nPlayerIndex].m_cardsHand, nCardIndex, CARDLIB_REMOVE_CARD_ONLY);
   AddCard(pH->m_cardsMiddle, c);
   printf("Put card in middle\n");

   //After all players have played their cards
   if( GetNumberOfCards(pH->m_cardsMiddle) == NUMBER_OF_HEARTS_PLAYERS ) {
      printf("All players have played their cards\n");
      //Figure out who had the highest
      Card cardFirst;
      GetCard(pH->m_cardsMiddle, &cardFirst, 0);
      int nCurrentSuit = GetSuit(cardFirst);
      int nCurrentValue = GetCardValue(cardFirst);
      printf("First played card is suit: %d, value %d\n", nCurrentSuit, nCurrentValue);

      //So the player who just went was the last to go lets set nPlayersTurn to the first to play
      nPlayersTurn = (nPlayersTurn+1) % NUMBER_OF_HEARTS_PLAYERS;
      printf("First card play by player: %d\n", nPlayersTurn);
      int nHighest = nPlayersTurn;
      int nCard;
      for(nCard = 1; nCard < NUMBER_OF_HEARTS_PLAYERS; nCard++) {
         Card cardPlayed;
         GetCard(pH->m_cardsMiddle, &cardPlayed, nCard);
         int nSuit = GetSuit(cardPlayed);
         if( nSuit != nCurrentSuit )
            continue;

         int nValue = GetCardValue(cardPlayed);
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

      printf("Player with highest card was: %d\n", nHighest);

      //Give the cards to the player
      pH->m_nLastTrumpPlayer = nHighest;
      for(nCard = 0; nCard < NUMBER_OF_HEARTS_PLAYERS; nCard++) {
         printf("Taking card and giving it to player\n");
         Card cardPlayed;
         TakeNextCard(pH->m_cardsMiddle, &cardPlayed);
         AddCard(pH->m_Players[nHighest].m_cardsTaken, cardPlayed);
      }
      printf("All set giving cards to player\n");
   }

   return HEARTSLIB_OK;
}

int GetNumberOfCardsInMiddle(HeartsLib api)
{
   DEBUG_MSG;

   struct Hearts* pH = (struct Hearts*)api;

   return GetNumberOfCards(pH->m_cardsMiddle);
}

int GetMiddleCard(HeartsLib api, Card* pCard, int nCardIndex, int* pPlayerIndex)
{
   DEBUG_MSG;

   if( nCardIndex < 0 || nCardIndex > GetNumberOfCardsInMiddle(api) )
      return HEARTSLIB_BADARGUMENT;

   struct Hearts* pH = (struct Hearts*)api;

   GetCard(pH->m_cardsMiddle, pCard, nCardIndex);

   int nNumCards = GetNumberOfCardsInMiddle(api);
   int nPlayer = (pH->m_nLastTrumpPlayer + nCardIndex) % NUMBER_OF_HEARTS_PLAYERS;
   if( pPlayerIndex )
      *pPlayerIndex = nPlayer;

   return HEARTSLIB_OK;
}



