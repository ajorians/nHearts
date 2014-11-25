#include "HeartsAI/HeartsAI.h"

#ifdef _TINSPIRE
#include <os.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#endif

struct HeartsAI
{
   HeartsLib m_hearts;
   int m_nPlayerIndex;
};

int HeartsAICreate(HeartsAILib* ai, HeartsLib hearts, int nPlayerIndex)
{
   struct HeartsAI* pH;

   pH = malloc(sizeof(struct HeartsAI));
   if( pH == NULL ){//Out of memory
      return -2;
   }

   pH->m_hearts = hearts;
   pH->m_nPlayerIndex = nPlayerIndex;

   //pH->m_nLastError = HEARTSLIB_OK;

   *ai = pH;

   return 0;
}

int HeartsAIFree(HeartsLib* ai)
{
   struct HeartsAI* pH;

   pH = *ai;

   free(pH);
   *ai = NULL;
   return 0;
}

void PrintCard(Card c)
{
   int nCardValue;
   Suit_t eSuit;
   char chValue, chSuit;

   nCardValue = GetCardValue(c);
   eSuit = GetSuit(c);

   if( nCardValue == ACE ) {
      chValue = 'A';
   }
   else if( nCardValue == JACK ) {
      chValue = 'J';
   }
   else if( nCardValue == QUEEN ) {
      chValue = 'Q';
   }
   else if( nCardValue == KING ) {
      chValue = 'K';
   }
   else {
      chValue = nCardValue + '0';
   }

   if( eSuit == Clubs ) {
      chSuit = 5;
   }
   else if( eSuit == Diamonds ) {
      chSuit = 4;
   }
   else if( eSuit == Spades ) {
      chSuit = 6;
   }
   else if( eSuit == Hearts ) {
      chSuit = 3;
   }

   if( nCardValue == 10 ) {
      printf("10 of %c\n", chSuit);
   }
   else {
      printf("%c of %c\n", chValue, chSuit);
   }
}

int GetNumberOfCardsLower(Card c)
{
   int nCardValue;
   nCardValue = GetCardValue(c);

   if( nCardValue == ACE )
      return 12;
   if( nCardValue == KING )
      return 11;
   if( nCardValue == QUEEN )
      return 10;
   if( nCardValue == JACK )
      return 9;

   return nCardValue - 2;
}

int GetNumberOfCardsHigher(Card c)
{
   return 12/*13-1(this card)*/ - GetNumberOfCardsLower(c);
}

int IsCardValueHigher(int nValue1, int nValue2)
{
   if( nValue1 == ACE && nValue2 != ACE )
      return 1;
   if( nValue1 != ACE && nValue2 == ACE )
      return 0;
   return nValue1 > nValue2 ? 1 : 0;
}

int IsCardHigher(Card c1, Card c2)
{
   int nValue1, nValue2;
   Suit_t eSuit1, eSuit2;

   eSuit1 = GetSuit(c1);
   eSuit2 = GetSuit(c2);
   nValue1 = GetCardValue(c1);
   nValue2 = GetCardValue(c2);

   if( eSuit1 != eSuit2 ) {
      //assert(0);//Wasn't expecting that
      return 0;
   }

   return IsCardValueHigher(nValue1, nValue2);
}

//The difference between this and GetNumberOfCardsLower; is it looks to see what cards do others have that would beat this
int GetNumberOfCardsBeatenBy(HeartsAILib ai, Card c)
{
   struct HeartsAI* pH;
   int nNumCards;
   int i;
   Suit_t eSuit;
   int nCardValue;
   int nCardsBeatenBy;
   pH = ai;

   eSuit = GetSuit(c);
   nCardValue = GetCardValue(c);

   nNumCards = GetNumberOfCardsInHand(pH->m_hearts, pH->m_nPlayerIndex);

   nCardsBeatenBy = GetNumberOfCardsHigher(c);

   for(i=0; i<nNumCards; i++) {
      Card cardInHand;
      Suit_t eSuitInHand;
      int nCardValueInHand;

      GetCardInHand(pH->m_hearts, &cardInHand, pH->m_nPlayerIndex, i);
      if( 1 == IsTwoCardsSame(c, cardInHand) )
         continue;

      eSuitInHand = GetSuit(cardInHand);
      nCardValueInHand = GetCardValue(cardInHand);

      if( eSuit != eSuitInHand )
         continue;

      if( 1 == IsCardHigher(cardInHand, c) )
         nCardsBeatenBy--;
   }

   return nCardsBeatenBy;
}

int GetNumberOfCardsBeats(HeartsAILib ai, Card c)
{
   struct HeartsAI* pH;
   int nNumCards;
   int i;
   Suit_t eSuit;
   int nCardValue;
   int nCardsBeats;
   pH = ai;

   eSuit = GetSuit(c);
   nCardValue = GetCardValue(c);

   nNumCards = GetNumberOfCardsInHand(pH->m_hearts, pH->m_nPlayerIndex);

   nCardsBeats = GetNumberOfCardsLower(c);

   for(i=0; i<nNumCards; i++) {
      Card cardInHand;
      Suit_t eSuitInHand;
      int nCardValueInHand;

      GetCardInHand(pH->m_hearts, &cardInHand, pH->m_nPlayerIndex, i);
      if( 1 == IsTwoCardsSame(c, cardInHand) )
         continue;

      eSuitInHand = GetSuit(cardInHand);
      nCardValueInHand = GetCardValue(cardInHand);

      if( eSuit != eSuitInHand )
         continue;

      if( 1 == IsCardHigher(c, cardInHand) )
         nCardsBeats--;
   }

   return nCardsBeats;
}

int GetNumberUnplayedBeatenBy(HeartsAILib ai, Suit_t eSuit, int nCardValue)
{
   struct HeartsAI* pH;
   int nNumCards;
   int nPlayerIndex;
   int nCardsBeatenBy = 0;
   pH = ai;

   for(nPlayerIndex=0; nPlayerIndex<4; nPlayerIndex++) {
      int i;
      if( pH->m_nPlayerIndex == nPlayerIndex )
         continue;

      nNumCards = GetNumberOfCardsInHand(pH->m_hearts, nPlayerIndex);

      for(i=0; i<nNumCards; i++) {
         Card cardInHand;
         Suit_t eSuitInHand;
         int nCardValueInHand;

         GetCardInHand(pH->m_hearts, &cardInHand, nPlayerIndex, i);

         eSuitInHand = GetSuit(cardInHand);
         nCardValueInHand = GetCardValue(cardInHand);

         if( eSuit != eSuitInHand )
            continue;

         if( 1 == IsCardValueHigher(nCardValueInHand, nCardValue) )
            nCardsBeatenBy++;
      }
   }

   return nCardsBeatenBy;
}

int GetNumberUnplayedCardsBeatenBy(HeartsAILib ai, Card c)
{
   struct HeartsAI* pH;
   Suit_t eSuit;
   int nCardValue;
   pH = ai;

   eSuit = GetSuit(c);
   nCardValue = GetCardValue(c);

   return GetNumberUnplayedBeatenBy(ai, eSuit, nCardValue);
}

int GetNumberUnplayedBeats(HeartsAILib ai, Suit_t eSuit, int nCardValue)
{
   struct HeartsAI* pH;
   int nNumCards;
   int nPlayerIndex;
   int nCardsBeats = 0;
   pH = ai;

   for(nPlayerIndex=0; nPlayerIndex<4; nPlayerIndex++) {
      int i;
      if( pH->m_nPlayerIndex == nPlayerIndex )
         continue;

      nNumCards = GetNumberOfCardsInHand(pH->m_hearts, nPlayerIndex);

      for(i=0; i<nNumCards; i++) {
         Card cardInHand;
         Suit_t eSuitInHand;
         int nCardValueInHand;

         GetCardInHand(pH->m_hearts, &cardInHand, nPlayerIndex, i);

         eSuitInHand = GetSuit(cardInHand);
         nCardValueInHand = GetCardValue(cardInHand);

         if( eSuit != eSuitInHand )
            continue;

         if( 1 == IsCardValueHigher(nCardValue, nCardValueInHand) )
            nCardsBeats++;
      }
   }

   return nCardsBeats;
}

int GetNumberUnplayedCardsBeats(HeartsAILib ai, Card c)
{
   struct HeartsAI* pH;
   Suit_t eSuit;
   int nCardValue;
   pH = ai;

   eSuit = GetSuit(c);
   nCardValue = GetCardValue(c);

   return GetNumberUnplayedBeats(ai, eSuit, nCardValue);
}

int HaveQueenOfSpades(HeartsAILib ai)
{
   struct HeartsAI* pH;
   int nNumCards;
   int i;

   pH = ai;

   nNumCards = GetNumberOfCardsInHand(pH->m_hearts, pH->m_nPlayerIndex);

   for(i=0; i<nNumCards; i++) {
      Card cardInHand;
      Suit_t eSuitInHand;
      int nCardValueInHand;

      GetCardInHand(pH->m_hearts, &cardInHand, pH->m_nPlayerIndex, i);

      eSuitInHand = GetSuit(cardInHand);
      nCardValueInHand = GetCardValue(cardInHand);

      if( eSuitInHand == Spades && nCardValueInHand == QUEEN )
         return 1;
   }

   return 0;
}

//Return 0. to 1. with 0. as undesirable, and 1. and fully desirable
double GetPassDesirability(HeartsAILib ai, Card c)
{
   struct HeartsAI* pH;
   int nCardValue;
   Suit_t eSuit;
   int nClubs, nDiamonds, nSpades, nHearts;
   int nRemainingOfSuit;

   int nNumCardsLower, nNumCardsHigher;
   int nNumCardsBeatenBy, nNumCardsBeats;
   double dEstRemaingSuitPerPlayer;
   double dSomeOneElseTakesTrick, dTakeTrickWithCard;

   double dDesirability;

   pH = ai;

   nCardValue = GetCardValue(c);
   eSuit = GetSuit(c);

   nClubs = nDiamonds = nSpades = nHearts = 0;
   HeartsAIGetCountOfSuits(ai, &nClubs, &nDiamonds, &nSpades, &nHearts);

   nRemainingOfSuit = 13;
   if( eSuit == Clubs )
      nRemainingOfSuit -= nClubs;
   if( eSuit == Diamonds )
      nRemainingOfSuit -= nDiamonds;
   if( eSuit == Spades )
      nRemainingOfSuit -= nSpades;
   if( eSuit == Hearts )
      nRemainingOfSuit -= nHearts;
   dEstRemaingSuitPerPlayer = nRemainingOfSuit/3.;

   nNumCardsLower = GetNumberOfCardsLower(c);
   nNumCardsHigher = GetNumberOfCardsHigher(c);

   nNumCardsBeatenBy = GetNumberOfCardsBeatenBy(ai, c);
   nNumCardsBeats = GetNumberOfCardsBeats(ai, c);

   dSomeOneElseTakesTrick = nNumCardsBeatenBy/12.;
   dTakeTrickWithCard = nNumCardsBeats/12.;

   dDesirability = 1.-dTakeTrickWithCard;

   if( eSuit == CLUBS) {
      //Because Clubs you can get rid of a clubs first trick it makes it a tad ok to have slightly higher clubs
      double dValue = dTakeTrickWithCard;
      dValue /= 1.1;
      dDesirability = 1.-dValue;

      if( nCardValue == 2 ) {
         //The 2 of Clubs is special; it can be desirable to keep and could be annoying to have.  As is it is pretty
         //desirable lets make it less desirable
         double dValue = dSomeOneElseTakesTrick;
         dValue /= 1.;
         dDesirability = dValue;
      }
   }
   if( eSuit == Diamonds ) {

   }
   if( eSuit == Spades ) {
      //It is pretty desirable to have 2-J of Spades; though if you have a lot of Spades it can be a bad thing
      //But I'll weight the scale so 2-J of Spades are more desirable.
      if( nCardValue != ACE && nCardValue <= JACK ) {
         double dValue = dTakeTrickWithCard;
         dValue /= 4.;
         dDesirability = 1.-dValue;
      }
      //For the Q of Spades and higher it gets tough to deside whether or not to keep it as the whole round is
      //pretty dependent on whom takes the Q of Spades
      else {
         //Having the Queen of Spades does factor in
         if( 1 == HaveQueenOfSpades(ai) ) {
            int nMoreSpadesThanOthers = nSpades - (int)(dEstRemaingSuitPerPlayer + 0.5);

            //If I have 2 more spades than others there is a good chance that I have enough Spades (maybe)
            if( nMoreSpadesThanOthers >= 2 ) {
               double dValue = dTakeTrickWithCard;
               dValue /= 4.;
               dDesirability = 1.-dValue;
            }
            //Somebody could have one more Spades than me and if they are searching for the Queen could be bad
            //So just weighting it a little differently
            else if( nMoreSpadesThanOthers == 1 ) {
               double dValue = dTakeTrickWithCard;
               dValue /= 2.;
               dDesirability = 1.-dValue;
            }
         }
         else {
            //If I don't have the Queen; I could be passed it.  Hard to say but let's try to get rid of high spades
            double dValue = dTakeTrickWithCard;
            dValue *= 2.;
            if( dValue >= 1. )
               dValue = 1.;
            dDesirability = 1.-dValue;
         }
      }
   }
   if( eSuit == Hearts ) {
      //Though a 7 of Hearts, for example, isn't bad; and an Ace of Hearts isn't bad until Hearts is broken.
      //I want to weight Hearts slightly favorable for some of the lower values
      if( nCardValue <= 10 ) {
         double dValue = dTakeTrickWithCard;
         dValue *= 1.4;
         if( dValue >= 1. )
            dValue = dTakeTrickWithCard;
         dDesirability = 1.-dValue;
      }
   }

   return dDesirability;
}

int HeartsAIDesiredPassIndexes(HeartsAILib ai, int* pnIndex1, int* pnIndex2, int* pnIndex3)
{
   struct HeartsAI* pH;
   int i;
   int nNumCards;
   double* paDesirability;

   pH = ai;

   if( HEARTSLIB_PASSED_CARDS == HasPassedCards(pH->m_hearts) )
      return 1;

   nNumCards = GetNumberOfCardsInHand(pH->m_hearts, pH->m_nPlayerIndex);

   //Until I change things lets pretend we are trying to not shoot the moon
   paDesirability = malloc(sizeof(double)*nNumCards);
   for(i=0; i<nNumCards; i++)
      paDesirability[i] = 0.;

   //printf("--------------------------\n");
   for(i=0; i<nNumCards; i++) {
      Card c;

      GetCardInHand(pH->m_hearts, &c, pH->m_nPlayerIndex, i);

      //PrintCard(c);

      paDesirability[i] = GetPassDesirability(ai, c);
   }

   //printf("Passing:\n");

   for(i=0; i<3; i++) {
      //Card c;
      int n;
      int nLowestIndex;
      double dLowest = 1.;

      for(n=0; n<nNumCards; n++) {
         if( paDesirability[n] < dLowest ) {
            dLowest = paDesirability[n];
            nLowestIndex = n;
         }
      }

      //GetCardInHand(pH->m_hearts, &c, pH->m_nPlayerIndex, nLowestIndex);
      //PrintCard(c);
      if( i==0 ) {
         *pnIndex1 = nLowestIndex;
      }
      else if( i==1 ) {
         *pnIndex2 = nLowestIndex;
      }
      else if( i==2 ) {
         *pnIndex3 = nLowestIndex;
      }
      paDesirability[nLowestIndex] = 1.;//Gone value
   }

   free(paDesirability);

   return 0;
}

int GetPointsOfCardsInMiddle(HeartsLib api)
{
   int nPoints;
   int nCards;
   int i;
   nPoints = 0;
   nCards = GetNumberOfCardsInMiddle(api);

   for(i=0; i<nCards; i++) {
      Card c;
      Suit_t eSuit;
      int nCardValue;
      GetMiddleCard(api, &c, i, NULL);

      eSuit = GetSuit(c);
      nCardValue = GetCardValue(c);

      if( eSuit == Hearts )
         nPoints++;
      if( eSuit == Spades && nCardValue == QUEEN )
         nPoints+=13;
   }

   return nPoints;
}

int HighestValueInMiddle(HeartsLib api)
{
   int nCardsInMiddle;
   Card cardMiddle;
   Suit_t eSuit;
   int nHighestValue;
   int i;

   nCardsInMiddle = GetNumberOfCardsInMiddle(api);
   //assert(nCardsInMiddle>0);

   GetMiddleCard(api, &cardMiddle, 0, NULL);
   eSuit = GetSuit(cardMiddle);
   nHighestValue = GetCardValue(cardMiddle);

   for(i=1; i<nCardsInMiddle; i++) {
      Card c;
      Suit_t eMiddleSuit;
      int nMiddleValue;
      GetMiddleCard(api, &c, i, NULL);

      eMiddleSuit = GetSuit(c);
      nMiddleValue = GetCardValue(c);

      if( eMiddleSuit != eSuit )
         continue;

      if( IsCardValueHigher(nMiddleValue, nHighestValue) ){
         nHighestValue = nMiddleValue;
      }
   }

   return nHighestValue;
}

int HighestCardOfSuit(HeartsAILib ai, Suit_t eSuit)
{
   struct HeartsAI* pH;
   int nNumCards;
   int i;

   int nHighest = 0;
   pH = ai;

   nNumCards = GetNumberOfCardsInHand(pH->m_hearts, pH->m_nPlayerIndex);

   for(i=0; i<nNumCards; i++) {
      Card cardHand;
      int eSuitHand;
      int nValueHand;
      GetCardInHand(pH->m_hearts, &cardHand, pH->m_nPlayerIndex, i);

      eSuitHand = GetSuit(cardHand);
      nValueHand = GetCardValue(cardHand);

      if( eSuitHand != eSuit )
         continue;

      if( IsCardValueHigher(nValueHand, nHighest) ) {
         nHighest = nValueHand;
      }
   }

   return nHighest;
}

int DoIHaveAHigherCardOfPlayedSuit(HeartsAILib ai, Card c)
{
   struct HeartsAI* pH;
   int nCardValue;
   Card cardMiddle;
   Suit_t eSuitMiddle;
   int nCardsInMiddle;
   pH = ai;

   nCardValue = GetCardValue(c);

   nCardsInMiddle = GetNumberOfCardsInMiddle(pH->m_hearts);
   //assert(nCardsInMiddle>0);

   GetMiddleCard(pH->m_hearts, &cardMiddle, 0, NULL);
   eSuitMiddle = GetSuit(cardMiddle);

   return IsCardValueHigher(HighestCardOfSuit(ai, eSuitMiddle), nCardValue) ? 1 : 0;
}

int DoIHaveAHigherCardThatWillNotTakeTrick(HeartsAILib ai, Card c)
{
   struct HeartsAI* pH;
   Suit_t eSuit;
   int nCardValue;
   Card cardMiddle;
   Suit_t eSuitMiddle;
   int nCardsInMiddle;
   int nNumCards;
   int i;
   int nHighestValue;
   pH = ai;

   nCardValue = GetCardValue(c);
   eSuit = GetSuit(c);

   nCardsInMiddle = GetNumberOfCardsInMiddle(pH->m_hearts);
   //assert(nCardsInMiddle>0);

   GetMiddleCard(pH->m_hearts, &cardMiddle, 0, NULL);
   eSuitMiddle = GetSuit(cardMiddle);
   nHighestValue = HighestValueInMiddle(pH->m_hearts);

   nNumCards = GetNumberOfCardsInHand(pH->m_hearts, pH->m_nPlayerIndex);

   for(i=0; i<nNumCards; i++) {
      Card cardHand;
      Suit_t eSuitHand;
      int nValueHand;
      GetCardInHand(pH->m_hearts, &cardHand, pH->m_nPlayerIndex, i);

      eSuitHand = GetSuit(cardHand);
      nValueHand = GetCardValue(cardHand);

	  if (eSuitHand != eSuit)
         continue;

	  if (IsCardValueHigher(nValueHand, nCardValue)) {
        if(eSuit != eSuitMiddle)
           return 1;

        if(IsCardValueHigher(nHighestValue, nValueHand))
           return 1;
	  }
   }

   return 0;
}

Suit_t GetLeadSuit(HeartsLib api) {
   Card c;
   Suit_t eSuitMiddle;
   GetMiddleCard(api, &c, 0, NULL);
   eSuitMiddle = GetSuit(c);
   return eSuitMiddle;
}

int HowManyOfSuit(HeartsLib api, int nPlayer, Suit_t eSuit)
{
   int nNumCards;
   int i;
   int nCount = 0;
   nNumCards = GetNumberOfCardsInHand(api, nPlayer);

   for(i=0; i<nNumCards; i++) {
      Card c;
      GetCardInHand(api, &c, nPlayer, i);
      if( GetSuit(c) == eSuit )
         nCount++;
   }
   return nCount;
}

//Return 0. to 1. with 0. as undesirable to play, and 1. and fully desirable
double GetPlayDesirability(HeartsAILib ai, int nCardIndex)
{
   struct HeartsAI* pH;
   int nNumCards;
   Card c;
   int nCardValue;
   Suit_t eSuit;
   int nClubs, nDiamonds, nSpades, nHearts;
   int nMyClubs, nMyDiamonds, nMySpades, nMyHearts;
   int nRemainingOfSuit;
   int nMyOfSuit;
   int nCardsInMiddle;

   int nNumCardsLower, nNumCardsHigher;
   int nNumCardsBeatenBy, nNumCardsBeats;
   double dEstRemaingSuitPerPlayer;
   double dSomeOneElseTakesTrick, dTakeTrickWithCard;

   double dDesirability;

   pH = ai;

   if( HEARTSLIB_CANNOT_PLAY_CARD == CanPlayCard(pH->m_hearts, pH->m_nPlayerIndex, nCardIndex) )
      return -1.;

   nNumCards = GetNumberOfCardsInHand(pH->m_hearts, pH->m_nPlayerIndex);

   GetCardInHand(pH->m_hearts, &c, pH->m_nPlayerIndex, nCardIndex);
   nCardValue = GetCardValue(c);
   eSuit = GetSuit(c);

   if( eSuit == Clubs && nCardValue == 2 )//2 of Clubs leads
      return 1.;

   nCardsInMiddle = GetNumberOfCardsInMiddle(pH->m_hearts);

   nClubs = nDiamonds = nSpades = nHearts = 0;
   HeartsAIGetCountOfSuitsNotPlayed(ai, &nClubs, &nDiamonds, &nSpades, &nHearts);

   nMyClubs = nMyDiamonds = nMySpades = nMyHearts = 0;
   HeartsGetCountOfSuits(pH->m_hearts, pH->m_nPlayerIndex, &nMyClubs, &nMyDiamonds, &nMySpades, &nMyHearts);

   if( eSuit == Clubs ) {
      nRemainingOfSuit = nClubs - nMyClubs;
      nMyOfSuit = nMyClubs;
   }
   if( eSuit == Diamonds ) {
      nRemainingOfSuit = nDiamonds - nMyDiamonds;
      nMyOfSuit = nMyDiamonds;
   }
   if( eSuit == Spades ) {
      nRemainingOfSuit = nSpades - nMySpades;
      nMyOfSuit = nMySpades;
   }
   if( eSuit == Hearts ) {
      nRemainingOfSuit = nHearts - nMyClubs;
      nMyOfSuit = nMyHearts;
   }
   dEstRemaingSuitPerPlayer = nRemainingOfSuit/3.;

   nNumCardsLower = GetNumberOfCardsLower(c);
   nNumCardsHigher = GetNumberOfCardsHigher(c);

   nNumCardsBeatenBy = GetNumberUnplayedCardsBeatenBy(ai, c);
   nNumCardsBeats = GetNumberUnplayedCardsBeats(ai, c);

   dSomeOneElseTakesTrick = nRemainingOfSuit!=0 ? nNumCardsBeatenBy/(double)nRemainingOfSuit : 0.;
   dTakeTrickWithCard = nRemainingOfSuit!=0 ? nNumCardsBeats/(double)nRemainingOfSuit : 1.;

   dDesirability = dSomeOneElseTakesTrick;

   //If somebody has to take the Queen of Spades then give it!
   if( nNumCards != 13 && HaveQueenOfSpades(ai) ) {
      int nBeats, nBeatenBy;
      nBeats = GetNumberUnplayedBeats(ai, Spades, QUEEN);
      nBeatenBy = GetNumberUnplayedBeatenBy(ai, Spades, QUEEN);

      if( nCardsInMiddle == 0 && nBeatenBy > 0 && nBeats <= 0 ) {
         return (eSuit == Spades && nCardValue == QUEEN) ? 1. : 0.;
      }

      if( nCardsInMiddle != 0 ) {
         //If I can play a Queen of Spades and not take the card lets play it
         if( GetLeadSuit(pH->m_hearts) != Spades && HowManyOfSuit(pH->m_hearts, pH->m_nPlayerIndex, GetLeadSuit(pH->m_hearts)) == 0 ) {
            return (eSuit == Spades && nCardValue == QUEEN) ? 1. : 0.;
         }

         for( int i = 0; i < nCardsInMiddle; i++ ) {
            Card cMiddle;
            Suit_t eSuitMiddle;
            int nValueMiddle;
            GetMiddleCard(pH->m_hearts, &cMiddle, i, NULL);
            eSuitMiddle = GetSuit(cMiddle);
            nValueMiddle = GetCardValue(cMiddle);
            if( eSuitMiddle == Spades && (nValueMiddle == KING || nValueMiddle == ACE) ) {
               //Somebody played the Ace or King of spades and I have the Queen; give it
               return(eSuit == Spades && nCardValue == QUEEN) ? 1. : 0.;
            }
         }
      }

   }

   if( nNumCards == 13 ) {
      if( eSuit == Spades && HaveQueenOfSpades(ai) ) {

      }
      else {
         //Because I cannot take any hearts on first turn lets play the least desirable
         dDesirability = dTakeTrickWithCard;
      }
   }

   //If I don't have the Queen,King,Ace of Spades and it isn't found and I just have 1 Spade it might makes sence to look for it?
   if( nNumCards != 13 && !HaveQueenOfSpades(ai) && nCardsInMiddle == 0 && nMySpades == 1 ) {
      int nHighestSpade = HighestCardOfSuit(ai, Spades);
      if( !IsCardValueHigher(nHighestSpade, QUEEN) ) {
         return eSuit == Spades ? 1. : 0.;
      }
   }

   if( nMyOfSuit == 1 && dEstRemaingSuitPerPlayer >= 2. ) {
      double dValue = dDesirability;
      dValue *= 4;
      if( dValue >= 1. )
         dValue = 1.;
      dDesirability = dValue;
   }

   else if( nCardsInMiddle > 0 && DoIHaveAHigherCardThatWillNotTakeTrick(ai, c) ) {
      //If there is a better card play that instead
      dDesirability = 0.;
   }

   else if( nCardsInMiddle == 3 && GetPointsOfCardsInMiddle(pH->m_hearts) == 0 && DoIHaveAHigherCardOfPlayedSuit(ai, c) ) {
      //Why don't I take it?
      dDesirability = 0.;
   }

   else if( nNumCards != 13 && nCardsInMiddle == 3 && GetPointsOfCardsInMiddle(pH->m_hearts) == 0 && !DoIHaveAHigherCardOfPlayedSuit(ai, c) ) {
      //Take it with the highest
      dDesirability = 1.;
   }

   else if( nNumCards != 13 && nCardsInMiddle > 0 && GetLeadSuit(pH->m_hearts) != eSuit ) {
      dDesirability = 1. - dDesirability;
   }

   else if( dEstRemaingSuitPerPlayer >= 2. ) {
   }

   if( dDesirability <= 0. )
      dDesirability = 0.1;

   return dDesirability;
}

int HeartsAIDesiredPlayIndex(HeartsAILib ai, int* pnIndex)
{
   struct HeartsAI* pH;
   int nNumCards;
   double* paDesirability;
   int i;
   pH = ai;

   nNumCards = GetNumberOfCardsInHand(pH->m_hearts, pH->m_nPlayerIndex);
   paDesirability = malloc(sizeof(double)*nNumCards);
   for(i=0; i<nNumCards; i++)
      paDesirability[i] = 0.;

   //printf("---------------------------\n");
   for(i=0; i<nNumCards; i++) {
      Card c;

      GetCardInHand(pH->m_hearts, &c, pH->m_nPlayerIndex, i);

      PrintCard(c);
   }

   for(i=0; i<nNumCards; i++) {
      paDesirability[i] = GetPlayDesirability(ai, i);
   }

   {
      Card c;
      int n;
      int nHighestIndex;
      double dHighest = 0.;

      for(n=0; n<nNumCards; n++) {
         if( paDesirability[n] >= dHighest ) {
            dHighest = paDesirability[n];
            nHighestIndex = n;
         }
      }

      GetCardInHand(pH->m_hearts, &c, pH->m_nPlayerIndex, nHighestIndex);
      printf("Playing card: ");
      PrintCard(c);
      *pnIndex = nHighestIndex;
   }

   free(paDesirability);

   return 0;
}

int HeartsAIGetCountOfSuits(HeartsAILib ai, int* pnClubs, int* pnDiamonds, int* pnSpades, int* pnHearts)
{
   struct HeartsAI* pH;
   pH = ai;

   return HeartsGetCountOfSuits(pH->m_hearts, pH->m_nPlayerIndex, pnClubs, pnDiamonds, pnSpades, pnHearts);
}

int HeartsGetCountOfSuits(HeartsLib hearts, int nPlayerIndex, int* pnClubs, int* pnDiamonds, int* pnSpades, int* pnHearts)
{
   int i;
   int nNumCards;
   int nClubs, nDiamonds, nSpades, nHearts;
   nClubs = nDiamonds = nSpades = nHearts = 0;

   nNumCards = GetNumberOfCardsInHand(hearts, nPlayerIndex);
   for(i=0; i<nNumCards; i++) {
      Card c;
      Suit_t eSuit;

      GetCardInHand(hearts, &c, nPlayerIndex, i);
      eSuit = GetSuit(c);

      if( eSuit == Clubs )
         nClubs++;
      if( eSuit == Diamonds )
         nDiamonds++;
      if( eSuit == Spades )
         nSpades++;
      if( eSuit == Hearts )
         nHearts++;
   }

   if( pnClubs )
      *pnClubs = nClubs;
   if( pnDiamonds )
      *pnDiamonds = nDiamonds;
   if( pnSpades )
      *pnSpades = nSpades;
   if( pnHearts )
      *pnHearts = nHearts;

   return 0;
}

int HeartsAIGetCountOfSuitsNotPlayed(HeartsAILib ai, int* pnClubs, int* pnDiamonds, int* pnSpades, int* pnHearts)
{
   struct HeartsAI* pH;
   int nPlayerIndex;
   int nTotalClubs, nTotalDiamonds, nTotalSpades, nTotalHearts;
   nTotalClubs = nTotalDiamonds = nTotalSpades = nTotalHearts = 0;

   pH = ai;

   for(nPlayerIndex=0; nPlayerIndex<4; nPlayerIndex++) {
      int nClubs, nDiamonds, nSpades, nHearts;
      nClubs = nDiamonds = nSpades = nHearts = 0;

      HeartsGetCountOfSuits(pH->m_hearts, nPlayerIndex, &nClubs, &nDiamonds, &nSpades, &nHearts);
      nTotalClubs += nClubs;
      nTotalDiamonds += nDiamonds;
      nTotalSpades += nSpades;
      nTotalHearts += nHearts;
   }

   if( pnClubs )
      *pnClubs = nTotalClubs;
   if( pnDiamonds )
      *pnDiamonds = nTotalDiamonds;
   if( pnSpades )
      *pnSpades = nTotalSpades;
   if( pnHearts )
      *pnHearts = nTotalHearts;

   return 0;
}

