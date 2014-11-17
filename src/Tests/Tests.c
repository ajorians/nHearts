#ifdef _TINSPIRE
#include <nspireio.h>
#else
#include <stdio.h>
#include <stdlib.h>
//#include <time.h>
#endif
#include "HeartsLib/HeartsLib.h"

#ifdef _TINSPIRE
#define PRINT  nio_printf
#else
#define PRINT  printf
#endif

#ifdef _TINSPIRE
#define PRINT_FUNC PRINT("%s", __func__);
#else
#define PRINT_FUNC PRINT("%s", __FUNCTION__);
#endif

#define TEST_SUCCEEDED	(0)
#define TEST_FAILED	(1)

#define TEST_PERSISTING

#ifdef _TINSPIRE
int g_nRelocatedGlobals = 0;
#endif

int TestConstruction()
{
   HeartsLib api;
   int i;
   PRINT_FUNC;
   if( HEARTSLIB_OK != HeartsLibCreate(&api, 100, 0) )
      return TEST_FAILED;

   for(i=0; i<4; i++)
      if( 13 != GetNumberOfCardsInHand(api, i) )
         return TEST_FAILED;

   if( HEARTSLIB_OK != HeartsLibFree(&api) )
      return TEST_FAILED;

   return TEST_SUCCEEDED;
}

int TestCopy()
{
   HeartsLib api;
   int i;
   HeartsLib copy;
   PRINT_FUNC;
   if( HEARTSLIB_OK != HeartsLibCreate(&api, 100, 0) )
      return TEST_FAILED;

   for(i=0; i<4; i++)
      if( 13 != GetNumberOfCardsInHand(api, i) )
         return TEST_FAILED;

   if( HEARTSLIB_OK != HeartsLibCopy(&copy, api) )
      return TEST_FAILED;

   for(i=0; i<4; i++)
      if( 13 != GetNumberOfCardsInHand(copy, i) )
         return TEST_FAILED;

   if( HEARTSLIB_OK != HeartsLibFree(&copy) )
      return TEST_FAILED;

   if( HEARTSLIB_OK != HeartsLibFree(&api) )
      return TEST_FAILED;

   return TEST_SUCCEEDED;
}

int TestPassing()
{
   HeartsLib api;
   int i;
   Pass_Direction_t ePass;
   PRINT_FUNC;
   if( HEARTSLIB_OK != HeartsLibCreate(&api, 100, 0) )
      return TEST_FAILED;

   ePass = GetHeartsPassDirection(api);

   if( PassLeft != ePass )
      return TEST_FAILED;

   for(i=0; i<4; i++) {
      ToggleSelectedCard(api, i, 0);
      ToggleSelectedCard(api, i, 1);
      ToggleSelectedCard(api, i, 2);

      if( 3 != GetNumberSelectedCards(api, i) )
         return TEST_FAILED;

      PassSelectedCards(api, i);
   }

   if( HEARTSLIB_PASSED_CARDS != HasPassedCards(api) )
      return TEST_FAILED;

   if( HEARTSLIB_OK != HeartsLibFree(&api) )
      return TEST_FAILED;

   return TEST_SUCCEEDED;
}

int TestGame()
{
   HeartsLib api;
   int i;

   PRINT_FUNC;
   if( HEARTSLIB_OK != HeartsLibCreate(&api, 100, 0) )
      return TEST_FAILED;

   while( HEARTSLIB_STILL_PLAYING == GetHeartsGameOver(api) ) {
      Pass_Direction_t ePass;
      int nPlayersTurn;

      ePass = GetHeartsPassDirection(api);

      if( NoPass != ePass ) {
         for(i=0; i<4; i++) {
            ToggleSelectedCard(api, i, 0);
            ToggleSelectedCard(api, i, 1);
            ToggleSelectedCard(api, i, 2);

            if( 3 != GetNumberSelectedCards(api, i) )
               return TEST_FAILED;

            PassSelectedCards(api, i);
         }
      }

      if( HEARTSLIB_PASSED_CARDS != HasPassedCards(api) )
         return TEST_FAILED;

      while( GetNumberOfCardsInHand(api, 0)>0 && GetNumberOfCardsInHand(api, 1)>0 && GetNumberOfCardsInHand(api, 2)>0 && GetNumberOfCardsInHand(api, 3)>0 ) {
         while( HEARTSLIB_PLAYERS_PLAYED_CARDS != HasEverybodyPlayedTheirCard(api) ) {
            int nNumCards;
            int nCard;
            nPlayersTurn = GetPlayersTurn(api);
            nNumCards = GetNumberOfCardsInHand(api, nPlayersTurn);
            for(nCard=0; nCard<nNumCards; nCard++) {
               if( HEARTSLIB_CAN_PLAY_CARD == CanPlayCard(api, nPlayersTurn, nCard) ) {
                  PlayCard(api, nPlayersTurn, nCard);
                  break;
               }
            }
         }

         GiveTrickToPlayer(api);
      }

      if( HEARTSLIB_STILL_PLAYING == GetHeartsGameOver(api) )
         DoHeartsNextHand(api);
   }

   //PRINT("\n");
   for(i=0; i<4; i++) {
      //PRINT("Player %d: %d\n", i, GetPlayerScore(api, i));
   }

   if( HEARTSLIB_OK != HeartsLibFree(&api) )
      return TEST_FAILED;

   return TEST_SUCCEEDED;
}

//int TestManyGames()
//{
//   int i;
//   int nTime;
//   PRINT_FUNC;
//   nTime = time(NULL);
//   for(i=0; i<5000; i++) {
//      int nRet;
//      nRet = TestGame();
//      if( TEST_SUCCEEDED != nRet )
//         return nRet;
//   }
//   printf("Time taken: %d\n", time(NULL)-nTime);
//
//   return TEST_SUCCEEDED;
//}

typedef int (*testfunc)();
   testfunc g_Tests[] =
   {
      TestConstruction,
      TestCopy,
      TestPassing,
      TestGame//,
      //TestManyGames
   };

void RunTests()
{
#ifdef _TINSPIRE
   if( !g_nRelocatedGlobals ) {
      nl_relocdata((unsigned*)g_Tests, sizeof(g_Tests)/sizeof(g_Tests[0]));
      g_nRelocatedGlobals = 1;
   }
#endif

   unsigned i;
   for(i=0; i<sizeof(g_Tests)/sizeof(testfunc); i++) {

      int nRet = g_Tests[i]();
      if( nRet == TEST_SUCCEEDED ) {
         PRINT("........OK\n");
      } else {
         PRINT("........FAIL\n");
      }
   }
}


