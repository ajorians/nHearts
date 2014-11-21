#ifndef HEARTSLIB_H_INCLUDED
#define HEARTSLIB_H_INCLUDED

#include "CardLib/CardLib.h"

typedef void* HeartsLib;

#define HEARTSLIB_OK			(0)
#define HEARTSLIB_BADARGUMENT		(-1)
#define HEARTSLIB_OUT_OF_MEMORY		(-2)
#define HEARTSLIB_CARD_FAILURE		(-3)
#define HEARTSLIB_FAIL_PLAY_CARD	(-4)

#define HEARTSLIB_CARD_SELECTED		(1)
#define HEARTSLIB_CARD_NOT_SELECTED	(0)

#define HEARTSLIB_PASSED_CARDS		(1)
#define HEARTSLIB_NOT_PASSED_CARDS	(0)

#define HEARTSLIB_CAN_PLAY_CARD		(1)
#define HEARTSLIB_CANNOT_PLAY_CARD	(0)

#define HEARTSLIB_GAME_OVER		(1)
#define HEARTSLIB_STILL_PLAYING		(0)

#define HEARTSLIB_PLAYERS_PLAYED_CARDS	(1)
#define HEARTSLIB_TURN_IN_PLAY		(0)

#define HEARTSLIB_SHOT_THE_MOON		(1)
#define HEARTSLIB_DID_NOT_SHOOT		(0)

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
int HeartsLibCreate(HeartsLib* api, int nScoreLimit, int nJackDiamonds);
int HeartsLibCopy(HeartsLib* copyapi, HeartsLib orig);
int HeartsLibFree(HeartsLib* api);

int GetHeartsLibError(HeartsLib api);
void ClearHeartsLibError(HeartsLib api);

//////////////////////////////////////////////
//HeartsLib related functions
//////////////////////////////////////////////
Pass_Direction_t GetHeartsPassDirection(HeartsLib api);
int GetNumberOfCardsInHand(HeartsLib api, int nPlayerIndex);
int GetCardInHand(HeartsLib api, Card* pCard, int nPlayerIndex, int nCard);
int GetHeartsGameOver(HeartsLib api);
int ToggleSelectedCard(HeartsLib api, int nPlayerIndex, int nCard);
int IsCardSelected(HeartsLib api, int nPlayerIndex, int nCardIndex);
int GetNumberSelectedCards(HeartsLib api, int nPlayerIndex);
int PassSelectedCards(HeartsLib api, int nPlayerIndex);
int HasPassedCards(HeartsLib api);
int GetPlayersTurn(HeartsLib api);
int CanPlayCard(HeartsLib api, int nPlayerIndex, int nCardIndex);
int PlayCard(HeartsLib api, int nPlayerIndex, int nCardIndex);
int HasEverybodyPlayedTheirCard(HeartsLib api);
int GiveTrickToPlayer(HeartsLib api);
int GetNumberOfCardsInMiddle(HeartsLib api);
int FigureOutWhoTakesTrick(HeartsLib api, int* pPlayerIndex);
int DoHeartsNextHand(HeartsLib api);
int GetMiddleCard(HeartsLib api, Card* pCard, int nCardIndex, int* pPlayerIndex);
int ScoreOfCardsTaken(HeartsLib api, int nPlayerIndex);
int GetPlayerShotMoon(HeartsLib api, int* pPlayerIndex);
int GetHeartsPlayerScore(HeartsLib api, int nPlayerIndex);

#endif
