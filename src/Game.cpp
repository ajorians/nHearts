#include "Game.h"
#include "ScoreReview.h"

extern "C"
{
#include "SDL/SDL_gfxPrimitives.h"
}

Game::Game(SDL_Surface* pScreen, CardImages* pCardImages)
: m_pScreen(pScreen), m_pCardImages(pCardImages), m_Pieces(pScreen, &m_Metrics, m_pCardImages), m_nCurrentCard(-1)
{
	HeartsLibCreate(&m_Hearts);

	m_pFont = nSDL_LoadFont(NSDL_FONT_THIN, 0/*R*/, 0/*G*/, 0/*B*/);

	m_Metrics.SetCardDimensions(DISPCARD_WIDTH, DISPCARD_HEIGHT);

	m_uLastAction = SDL_GetTicks();

	for(int nPlayerIndex=0; nPlayerIndex<4; nPlayerIndex++) {
		int nNumCards = GetNumberOfCardsInHand(m_Hearts, nPlayerIndex);
		m_Metrics.SetNumCards(nNumCards);
	        for(int i=0; i<nNumCards; i++) {
	                Card c;
	                GetCardInHand(m_Hearts, &c, nPlayerIndex, i);
			m_Pieces.CreateCard(c, nPlayerIndex, i, nPlayerIndex == 1 || nPlayerIndex == 3);
		}
	}
}

Game::~Game()
{
	nSDL_FreeFont(m_pFont);
}

bool Game::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;

	m_Pieces.Animate();	

	//Update screen
	UpdateDisplay();

	DoGamePlay();
	
	//SDL_Delay(5);
	
	return true;
}

bool Game::PollEvents()
{
	SDL_Event event;
	
	/* Poll for events. SDL_PollEvent() returns 0 when there are no  */
	/* more events on the event queue, our while loop will exit when */
	/* that occurs.                                                  */
	while( SDL_PollEvent( &event ) )
	{
		/* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
		switch( event.type )
		{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) 
				{
					case SDLK_ESCAPE:
						fprintf(stderr, "Hit Escape!n");
					return false;
					break;

					case SDLK_RETURN:
					case SDLK_LCTRL:
					case SDLK_RCTRL:
						SelectCard();
					break;
					
					case SDLK_RIGHT:
					case SDLK_6:
						Move(Right);
					break;
					
					case SDLK_LEFT:
					case SDLK_4:
						Move(Left);
					break;

					default:
						break;
				}
				break;
			
			//Called when the mouse moves
			case SDL_MOUSEMOTION:
				break;
			
			case SDL_KEYUP:
				break;
			
			default:
				break;
		}
	}
	return true;
}

void Game::UpdateDisplay()
{
	//Update cards in hand
	for(int nPlayerIndex = 0; nPlayerIndex<4; nPlayerIndex++) {
		bool bHasPassedCards = HasPassedCards(m_Hearts) == HEARTSLIB_PASSED_CARDS;
		int nNumCards = GetNumberOfCardsInHand(m_Hearts, nPlayerIndex);
		int nNumSelected = GetNumberSelectedCards(m_Hearts, nPlayerIndex);
		if( !bHasPassedCards ) {
			m_Metrics.SetNumCards(nNumCards - nNumSelected);
		}
		else {
			m_Metrics.SetNumCards(nNumCards);
		}
		int nNonSelectedCard = 0, nSelectedCard = 0;
		for(int i=0; i<nNumCards; i++ ) {
			Card c;
			GetCardInHand(m_Hearts, &c, nPlayerIndex, i);
			if( nPlayerIndex == 0 && !bHasPassedCards && IsCardSelected(m_Hearts, nPlayerIndex, i) == HEARTSLIB_CARD_SELECTED ) {
				m_Pieces.MoveCard(c, m_Metrics.GetSelectedXPos(nSelectedCard++), m_Metrics.GetSelectedTop());
			}
			else {
				int nX = m_Metrics.GetXPos(nPlayerIndex, nNonSelectedCard);
				int nY = m_Metrics.GetYPos(nPlayerIndex, nNonSelectedCard);
				if( nPlayerIndex == 0 && m_nCurrentCard == i )
					nY-=10;
				m_Pieces.MoveCard(c, nX, nY);
				nNonSelectedCard++;
			}
			m_Pieces.MakeVisible(c, nPlayerIndex == 0);
		}
	}

	//Updates cards being played
	int nNumCardsInMiddle = GetNumberOfCardsInMiddle(m_Hearts);
	for(int i=0; i<nNumCardsInMiddle; i++) {
		Card c;
		int nPlayerIndex;
		GetMiddleCard(m_Hearts, &c, i, &nPlayerIndex);
		m_Pieces.MoveCard(c, m_Metrics.GetMiddleCardX(nPlayerIndex), m_Metrics.GetMiddleCardY(nPlayerIndex));
		m_Pieces.MakeVisible(c, true);
	}


	//Draw background
	SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, GAME_BACKGROUND_R, GAME_BACKGROUND_G, GAME_BACKGROUND_B));
	
	//Display scores
	nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH-30, SCREEN_HEIGHT-20, "%d|%d", ScoreOfCardsTaken(m_Hearts, 0), GetPlayerScore(m_Hearts, 0) );
	nSDL_DrawString(m_pScreen, m_pFont, 0, SCREEN_HEIGHT/2, "%d|%d", ScoreOfCardsTaken(m_Hearts, 1), GetPlayerScore(m_Hearts, 1) );
	nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH/2, 0, "%d|%d", ScoreOfCardsTaken(m_Hearts, 2), GetPlayerScore(m_Hearts, 2) );
	nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH-30, SCREEN_HEIGHT/2, "%d|%d", ScoreOfCardsTaken(m_Hearts, 3), GetPlayerScore(m_Hearts, 3) );

	nSDL_DrawString(m_pScreen, m_pFont, 0, SCREEN_HEIGHT-20, "It is player %d turn", GetPlayersTurn(m_Hearts) );

	//SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Game::Move(Direction eDirection)
{
   int nNumCards = GetNumberOfCardsInHand(m_Hearts, 0);
   if( eDirection == Left ) {
      m_nCurrentCard--;
   }
   else if( eDirection == Right ) {
      m_nCurrentCard++;
   }

   if( m_nCurrentCard < 0 ) {
      m_nCurrentCard = nNumCards-1;
   }
   else if( m_nCurrentCard >= nNumCards ) {
      m_nCurrentCard = 0;
   }
}

void Game::SelectCard()
{
   if( HasPassedCards(m_Hearts) == HEARTSLIB_NOT_PASSED_CARDS ) {
      ToggleSelectedCard(m_Hearts, 0, m_nCurrentCard);
      if( GetNumberSelectedCards(m_Hearts, 0) == 3 ) {
         PassSelectedCards(m_Hearts, 0);

         for(int i=1; i<4; i++) {
            while(GetNumberSelectedCards(m_Hearts, i) != 3 ) {
                ToggleSelectedCard(m_Hearts, i, rand() % 13);
            }
            PassSelectedCards(m_Hearts, i);
         }
      }
   }
   else {
      PlayCard(m_Hearts, 0, m_nCurrentCard);
   }
}

void Game::DoGamePlay()
{
   if( SDL_GetTicks() - m_uLastAction < 1000 )
      return;

   if( HasPassedCards(m_Hearts) == HEARTSLIB_PASSED_CARDS ) {
      if( HasEverybodyPlayedTheirCard(m_Hearts ) ) {
         printf("Everybody played their card\n");
         int nPlayerIndex;
         FigureOutWhoTakesTrick(m_Hearts, &nPlayerIndex);
         int nNumCardsInMiddle = GetNumberOfCardsInMiddle(m_Hearts);
         for(int nCard = 0; nCard < nNumCardsInMiddle; nCard++) {
            Card c;
            GetMiddleCard(m_Hearts, &c, nCard, NULL);
            m_Pieces.MoveCard(c, m_Metrics.GetPlayerSideX(nPlayerIndex), m_Metrics.GetPlayerSideY(nPlayerIndex));
         }
         GiveTrickToPlayer(m_Hearts);
         
         if( GetNumberOfCardsInHand(m_Hearts, 0) == 0 ) {
            ScoreReview r(m_pScreen, &m_Hearts);
            while( r.Loop() ){}
            printf("DoHeartsNextHand\n");
            DoHeartsNextHand(m_Hearts);
            printf("After DoHeartsNextHand\n");
         }
      }
      else {
        int nPlayersTurn = GetPlayersTurn(m_Hearts);
         if( nPlayersTurn != 0 ) {
            while(true) {
               int nCardsInHand = GetNumberOfCardsInHand(m_Hearts, nPlayersTurn);
               int nCardPossibleToPlay = rand() % nCardsInHand;
               if( CanPlayCard(m_Hearts, nPlayersTurn, nCardPossibleToPlay) == HEARTSLIB_CAN_PLAY_CARD ) {
                  PlayCard(m_Hearts, nPlayersTurn, nCardPossibleToPlay);
                  m_uLastAction = SDL_GetTicks();
                  break;
               }
            }
         }
      }
   }
}
