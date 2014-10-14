#ifndef HEARTSLIB_H_INCLUDED
#define HEARTSLIB_H_INCLUDED

#include "CardLib/CardLib.h"

typedef void* HeartsLib;

#define HEARTSLIB_OK			(0)
#define HEARTSLIB_BADARGUMENT		(-1)
#define HEARTSLIB_OUT_OF_MEMORY		(-2)
#define HEARTSLIB_CARD_FAILURE		(-3)

typedef enum
{
   PassLeft,
   PassRight,
   PassAcross,
   NoPass
} Pass_Direction_t;

//////////////////////////////////////////////
//Initalization/Error checking/Mode functions
//////////////////////////////////////////////
int HeartsLibCreate(HeartsLib* api);
int HeartsLibFree(HeartsLib* api);

int GetHeartsLibError(HeartsLib api);
void ClearHeartsLibError(HeartsLib api);

//////////////////////////////////////////////
//HeartsLib related functions
//////////////////////////////////////////////
int GetHeartsPlayerScore(HeartsLib api, int nPlayerIndex);
Pass_Direction_t GetHeartsPassDirection(HeartsLib api);
int GetNumberOfCardsInHand(HeartsLib api, int nPlayerIndex);
int GetCardInHand(HeartsLib api, Card* pCard, int nPlayerIndex, int nCard);
int GetHeartsGameOver(HeartsLib api);

#endif
