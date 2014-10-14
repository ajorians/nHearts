#include "Game.h"

Game::Game(SDL_Surface* pScreen, CardImages* pCardImages)
: m_pScreen(pScreen), m_pCardImages(pCardImages)
{
	HeartsLibCreate(&m_Hearts);

	m_pFont = nSDL_LoadFont(NSDL_FONT_THIN, 0/*R*/, 0/*G*/, 0/*B*/);
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
					break;
					
					case SDLK_RIGHT:
					case SDLK_6:
					break;
					
					case SDLK_LEFT:
					case SDLK_4:
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
	//nSDL_DrawString(m_pScreen, m_pFont, 0, SCREEN_HEIGHT-20, "Bonus:         %d", GetBonus() );
	int nNumCards = GetNumberOfCardsInHand(m_Hearts, 0);
	for(int i=0; i<nNumCards; i++) {
		Card c;
		GetCardInHand(m_Hearts, &c, 0, i);
		SDL_Surface* pSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, CARD_WIDTH, CARD_HEIGHT, 16, 0, 0, 0, 0);
		m_pCardImages->GetImageForCard(pSurface, c);

		SDL_Rect rectDest;
	        rectDest.x = i*(CARD_WIDTH*2/3);
	        rectDest.y = SCREEN_HEIGHT-CARD_HEIGHT;
	        rectDest.w = CARD_WIDTH;
	        rectDest.h = CARD_HEIGHT;

		SDL_BlitSurface(pSurface, NULL, m_pScreen, &rectDest);
		SDL_FreeSurface(pSurface);
	}

	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}


