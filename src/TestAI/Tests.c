#ifdef _TINSPIRE
#include <nspireio.h>
#else
#include <stdio.h>
#include <stdlib.h>
//#include <time.h>
#endif
#include "HeartsAI/HeartsAI.h"

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
   HeartsAILib ai;
   PRINT_FUNC;
   if( HEARTSLIB_OK != HeartsLibCreate(&api, 100, 0) )
      return TEST_FAILED;

   HeartsAICreate(&ai, api, 0);

   HeartsAIFree(&ai);

   if( HEARTSLIB_OK != HeartsLibFree(&api) )
      return TEST_FAILED;

   return TEST_SUCCEEDED;
}

int TestPassing()
{
   HeartsLib api;
   int i;
   PRINT_FUNC;
   printf("\n");
   if( HEARTSLIB_OK != HeartsLibCreate(&api, 100, 0) )
      return TEST_FAILED;

   for(i=0; i<4; i++) {
      HeartsAILib ai;
      int nIndex1, nIndex2, nIndex3;
      HeartsAICreate(&ai, api, i);

      HeartsAIDesiredPassIndexes(ai, &nIndex1, &nIndex2, &nIndex3);

      HeartsAIFree(&ai);
   }

   if( HEARTSLIB_OK != HeartsLibFree(&api) )
      return TEST_FAILED;

   return TEST_SUCCEEDED;
}

int TestPlaying()
{
   HeartsLib api;
   int i;
   HeartsAILib ai[4];
   PRINT_FUNC;
   printf("\n");
   if( HEARTSLIB_OK != HeartsLibCreate(&api, 100, 0) )
      return TEST_FAILED;

   for(i=0; i<4; i++) {
      HeartsAICreate(&ai[i], api, i);
   }

   while( HEARTSLIB_STILL_PLAYING == GetHeartsGameOver(api) ) {
      Pass_Direction_t ePass;
      int nPlayersTurn;

      ePass = GetHeartsPassDirection(api);

      if( NoPass != ePass ) {
         for(i=0; i<4; i++) {
            int nIndex1, nIndex2, nIndex3;
            HeartsAIDesiredPassIndexes(ai[i], &nIndex1, &nIndex2, &nIndex3);
            ToggleSelectedCard(api, i, nIndex1);
            ToggleSelectedCard(api, i, nIndex2);
            ToggleSelectedCard(api, i, nIndex3);

            if( 3 != GetNumberSelectedCards(api, i) )
               return TEST_FAILED;

            PassSelectedCards(api, i);
         }
      }

      if( HEARTSLIB_PASSED_CARDS != HasPassedCards(api) )
         return TEST_FAILED;

      while( GetNumberOfCardsInHand(api, 0)>0 && GetNumberOfCardsInHand(api, 1)>0 && GetNumberOfCardsInHand(api, 2)>0 && GetNumberOfCardsInHand(api, 3)>0 ) {
         printf("--------------------------------------\n");

         while( HEARTSLIB_PLAYERS_PLAYED_CARDS != HasEverybodyPlayedTheirCard(api) ) {
            int nIndex;
            nPlayersTurn = GetPlayersTurn(api);
            HeartsAIDesiredPlayIndex(ai[nPlayersTurn], &nIndex);
            if( HEARTSLIB_CAN_PLAY_CARD != CanPlayCard(api, nPlayersTurn, nIndex) )
               return TEST_FAILED;

            if( HEARTSLIB_OK != PlayCard(api, nPlayersTurn, nIndex) )
               return TEST_FAILED;
         }

         GiveTrickToPlayer(api);
      }

      if( HEARTSLIB_STILL_PLAYING == GetHeartsGameOver(api) )
         DoHeartsNextHand(api);
   }

   for(i=0; i<4; i++) {
      HeartsAIFree(&ai[i]);
   }

   if( HEARTSLIB_OK != HeartsLibFree(&api) )
      return TEST_FAILED;

   return TEST_SUCCEEDED;
}

typedef int (*testfunc)();
   testfunc g_Tests[] =
   {
      TestConstruction,
      TestPassing,
      TestPlaying
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


