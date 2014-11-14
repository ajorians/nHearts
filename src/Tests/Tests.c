#include <nspireio.h>
#include "HeartsLib/HeartsLib.h"

#define PRINT_FUNC nio_printf("%s", __func__); /*printf(__func__); printf("\n");*/

#define TEST_SUCCEEDED	(0)
#define TEST_FAILED	(1)

#define TEST_PERSISTING

int g_nRelocatedGlobals = 0;

int TestConstruction()
{
   PRINT_FUNC;
   HeartsLib api;
   if( HEARTSLIB_OK != HeartsLibCreate(&api, 100, 0) )
      return TEST_FAILED;

   int i;
   for(i=0; i<4; i++)
      if( 13 != GetNumberOfCardsInHand(api, i) )
         return TEST_FAILED;

   if( HEARTSLIB_OK != HeartsLibFree(&api) )
      return TEST_FAILED;

   return TEST_SUCCEEDED;
}

int TestCopy()
{
   PRINT_FUNC;
   HeartsLib api;
   if( HEARTSLIB_OK != HeartsLibCreate(&api, 100, 0) )
      return TEST_FAILED;

   int i;
   for(i=0; i<4; i++)
      if( 13 != GetNumberOfCardsInHand(api, i) )
         return TEST_FAILED;

   HeartsLib copy;
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

typedef int (*testfunc)();
   testfunc g_Tests[] =
   {
      TestConstruction,
      TestCopy
   };

void RunTests()
{
   if( !g_nRelocatedGlobals ) {
      nl_relocdata((unsigned*)g_Tests, sizeof(g_Tests)/sizeof(g_Tests[0]));
      g_nRelocatedGlobals = 1;
   }

   unsigned i;
   for(i=0; i<sizeof(g_Tests)/sizeof(testfunc); i++) {

      int nRet = g_Tests[i]();
      if( nRet == TEST_SUCCEEDED ) {
         nio_printf("........OK\n");
      } else {
         nio_printf("........FAIL\n");
      }
   }
}


