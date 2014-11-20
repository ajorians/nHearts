#ifndef CARDLIB_H_INCLUDED
#define CARDLIB_H_INCLUDED

typedef void* CardLib;
typedef void* Card;

#define CARDLIB_OK			(0)
#define CARDLIB_BADARGUMENT		(-1)
#define CARDLIB_OUT_OF_MEMORY		(-2)
#define CARDLIB_INDEX_NOT_EXIST		(-3)

#define CARDLIB_DECK_HAS_CARDS		(0)
#define CARDLIB_DECK_EMPTY		(1)

#define CARDLIB_REMOVE_CARD_ONLY	(0)
#define CARDLIB_FREE_CARD		(1)

#define ACE				(1)
#define JACK				(11)
#define QUEEN				(12)
#define KING				(13)

#define HEARTS				(0)
#define DIAMONDS			(1)
#define SPADES				(2)
#define CLUBS				(3)

#define JOKER				(4)

#define SUIT_START			HEARTS
#define SUIT_END			CLUBS

#define VALUE_START			ACE
#define VALUE_END			KING

typedef enum Suits
{
   Hearts,
   Diamonds,
   Spades,
   Clubs
} Suit_t;

//////////////////////////////////////////////
//Initalization/Error checking/Mode functions
//////////////////////////////////////////////
int CardLibCreate(CardLib* api);
int CardLibCopy(CardLib* copy, CardLib orig);
int CardLibFree(CardLib* api);

int GetCardLibError(CardLib api);
void ClearCardLibError(CardLib api);

//////////////////////////////////////////////
//CardLib related functions
//////////////////////////////////////////////
int IsCardsEmpty(CardLib api);
int GetNumberOfCards(CardLib api);
int GetCard(CardLib api, Card* pCard, int nIndex);
int AddStandardCards(CardLib api, int nJokers);
int AddCard(CardLib api, Card c);
int AddCardToBeginning(CardLib api, Card c);
int Shuffle(CardLib api);
int TakeNextCard(CardLib api, Card* pCard);//Need to Destroy pCard when done
int RemoveAllCards(CardLib api, int nFreeCard);
int RemoveCard(CardLib api, int nIndex, int nFreeCard);
int SwapCards(CardLib api, int nIndex1, int nIndex2);

//////////////////////////////////////////////
//Card related functions
//////////////////////////////////////////////
int CreateCard(Card* pCard, Suit_t eSuit, int nValue);
int CopyCard(Card* pCard, Card cOrig);
int DestroyCard(Card* pCard);
Suit_t GetSuit(Card c);
int GetCardValue(Card c);
int IsTwoCardsSame(Card c1, Card c2);//Suit and Value
int SetCardExtraData(Card c, void* pExtraData);
void* GetCardExtraData(Card c);
int SetSuit(Card c, Suit_t eSuit);
int SetCardValue(Card c, int nValue);
int SwapCardValues(Card c1, Card c2);

#endif //CARDLIB_H_INCLUDED
