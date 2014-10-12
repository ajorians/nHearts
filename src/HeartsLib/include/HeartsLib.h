#ifndef HEARTSLIB_H_INCLUDED
#define HEARTSLIB_H_INCLUDED

#include "CardLib/CardLib.h"

typedef void* HeartsLib;

#define HEARTSLIB_OK			(0)
#define HEARTSLIB_BADARGUMENT		(-1)
#define HEARTSLIB_OUT_OF_MEMORY		(-2)
#define HEARTSLIB_CARD_FAILURE		(-3)

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

#endif
