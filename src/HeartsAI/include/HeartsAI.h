#ifndef AI_H
#define AI_H

#ifdef _TINSPIRE
#include <os.h>
#endif
#include "HeartsLib/HeartsLib.h"

typedef void* HeartsAILib;

int HeartsAICreate(HeartsAILib* ai, HeartsLib hearts, int nPlayerIndex);
int HeartsAIFree(HeartsAILib* ai);

int HeartsAIDesiredPassIndexes(HeartsAILib ai, int* pnIndex1, int* pnIndex2, int* pnIndex3);
int HeartsAIDesiredPlayIndex(HeartsAILib ai, int* pnIndex);
int HeartsAIGetCountOfSuits(HeartsAILib ai, int* pnClubs, int* pnDiamonds, int* pnSpades, int* pnHearts);
int HeartsGetCountOfSuits(HeartsLib hearts, int nPlayerIndex, int* pnClubs, int* pnDiamonds, int* pnSpades, int* pnHearts);
int HeartsAIGetCountOfSuitsNotPlayed(HeartsAILib ai, int* pnClubs, int* pnDiamonds, int* pnSpades, int* pnHearts);

#endif

