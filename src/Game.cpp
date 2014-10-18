#include "Game.h"
#include "ScoreReview.h"

extern "C"
{
#include "SDL/SDL_gfxPrimitives.h"
}

Game::Game(SDL_Surface* pScreen, CardImages* pCardImages)
: m_pScreen(pScreen), m_pCardImages(pCardImages), m_Selector(pScreen, &m_Metrics)
{
	HeartsLibCreate(&m_Hearts);

	m_pFont = nSDL_LoadFont(NSDL_FONT_THIN, 0/*R*/, 0/*G*/, 0/*B*/);

	m_Metrics.SetCardDimensions(DISPCARD_WIDTH, DISPCARD_HEIGHT);

	m_uLastAction = SDL_GetTicks();
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
	//Draw background
	SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, GAME_BACKGROUND_R, GAME_BACKGROUND_G, GAME_BACKGROUND_B));
	
	//Draw score stuff
	int nNumCards = GetNumberOfCardsInHand(m_Hearts, 0);
	m_Metrics.SetNumCards(nNumCards);
	int nLeft = (SCREEN_WIDTH - nNumCards*DISPCARD_WIDTH)/2;
	for(int i=0; i<nNumCards; i++) {
		Card c;
		GetCardInHand(m_Hearts, &c, 0, i);
		SDL_Surface* pSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, m_Metrics.GetCardWidth(), m_Metrics.GetCardHeight(), 16, 0, 0, 0, 0);
		m_pCardImages->GetImageForCard(pSurface, c, DISPCARD_WIDTH, DISPCARD_HEIGHT);

		SDL_Rect rectDest;
	        rectDest.x = m_Metrics.GetXPos(i);
	        rectDest.y = m_Metrics.GetTop();
		if( IsCardSelected(m_Hearts, 0, i) == HEARTSLIB_CARD_SELECTED )
			rectDest.y -= 10;
	        rectDest.w = m_Metrics.GetCardWidth();
	        rectDest.h = m_Metrics.GetCardHeight();

		SDL_BlitSurface(pSurface, NULL, m_pScreen, &rectDest);
		SDL_FreeSurface(pSurface);
	}

	if( HasPassedCards(m_Hearts) == HEARTSLIB_NOT_PASSED_CARDS || GetPlayersTurn(m_Hearts) == 0 )
		m_Selector.DrawSelector();

	//Draw cards in middle
	int nNumCardsInMiddle = GetNumberOfCardsInMiddle(m_Hearts);
	for(int i=0; i<nNumCardsInMiddle; i++) {
		Card c;
		int nPlayer;
		GetMiddleCard(m_Hearts, &c, i, &nPlayer);
		SDL_Surface* pSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, m_Metrics.GetCardWidth(), m_Metrics.GetCardHeight(), 16, 0, 0, 0, 0);
		m_pCardImages->GetImageForCard(pSurface, c, DISPCARD_WIDTH, DISPCARD_HEIGHT);

		SDL_Rect rectDest;
		if( nPlayer == 0 ) {
			rectDest.x = SCREEN_WIDTH/2 - DISPCARD_WIDTH/2;
	                rectDest.y = 35+DISPCARD_HEIGHT;
		}
		else if( nPlayer == 1 ) {
			rectDest.x = SCREEN_WIDTH/2 - 3*DISPCARD_WIDTH/2;
                        rectDest.y = 35+DISPCARD_HEIGHT/2;
		}
		else if( nPlayer == 2 ) {
			rectDest.x = SCREEN_WIDTH/2 - DISPCARD_WIDTH/2;
                        rectDest.y = 35;
		}
		else {
			rectDest.x = SCREEN_WIDTH/2 + DISPCARD_WIDTH/2;
                        rectDest.y = 35+DISPCARD_HEIGHT/2;
		}
		rectDest.w = m_Metrics.GetCardWidth();
                rectDest.h = m_Metrics.GetCardHeight();

		SDL_BlitSurface(pSurface, NULL, m_pScreen, &rectDest);
                SDL_FreeSurface(pSurface);
	}

	//Display scores
	nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH-30, SCREEN_HEIGHT-20, "%d|%d", ScoreOfCardsTaken(m_Hearts, 0), GetPlayerScore(m_Hearts, 0) );
	nSDL_DrawString(m_pScreen, m_pFont, 0, SCREEN_HEIGHT/2, "%d|%d", ScoreOfCardsTaken(m_Hearts, 1), GetPlayerScore(m_Hearts, 1) );
	nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH/2, 0, "%d|%d", ScoreOfCardsTaken(m_Hearts, 2), GetPlayerScore(m_Hearts, 2) );
	nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH-30, SCREEN_HEIGHT/2, "%d|%d", ScoreOfCardsTaken(m_Hearts, 3), GetPlayerScore(m_Hearts, 3) );

	nSDL_DrawString(m_pScreen, m_pFont, 0, SCREEN_HEIGHT-20, "It is player %d turn", GetPlayersTurn(m_Hearts) );

	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Game::Move(Direction eDirection)
{
   m_Selector.Move(eDirection);
}

void Game::SelectCard()
{
   if( HasPassedCards(m_Hearts) == HEARTSLIB_NOT_PASSED_CARDS ) {
      ToggleSelectedCard(m_Hearts, 0, m_Selector.GetCurrentX());
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
      PlayCard(m_Hearts, 0, m_Selector.GetCurrentX());
   }
}

void Game::DoGamePlay()
{
   if( SDL_GetTicks() - m_uLastAction < 1000 )
      return;

   if( HasPassedCards(m_Hearts) == HEARTSLIB_PASSED_CARDS ) {
      if( HasEverybodyPlayedTheirCard(m_Hearts ) ) {
         printf("Everybody played their card\n");
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
