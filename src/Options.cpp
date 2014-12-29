#include "Options.h"
#include "MouseHandling.h"
#include "Defines.h"

Options::Options(SDL_Surface* pScreen, MouseHandling* pMouse, Config* pConfig)
: m_pScreen(pScreen), m_pMouse(pMouse), m_nOptionsIndex(0), m_pConfig(pConfig)
{
	m_pFont = nSDL_LoadFont(NSDL_FONT_TINYTYPE, 0, 0, 0);
	nSDL_SetFontSpacing(m_pFont, 0, 2);
}

Options::~Options()
{
	nSDL_FreeFont(m_pFont);
}

bool Options::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();
	
	return true;
}

bool Options::PollEvents()
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
						return false;
					break;

					case SDLK_SPACE:
                                        case SDLK_RETURN:
                                        case SDLK_LCTRL:
						ToggleCurrentOption();
                                                break;

					case SDLK_UP:
                                        case SDLK_8:
                                                Move(OP_Up);
                                                break;

                                        case SDLK_DOWN:
                                        case SDLK_2:
                                                Move(OP_Down);
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

	int nMX = -1, nMY = -1;
        if( m_pMouse->PollMouse(nMX, nMY) ) {
                MouseButton eMouseButton = m_pMouse->GetMouseButton();
                if( eMouseButton == CenterButton ) {
			ToggleCurrentOption();
                }
	}

	return true;
}

void Options::ToggleCurrentOption()
{
	if( m_nOptionsIndex == 0 ) {
		m_pConfig->SetGameMode(!m_pConfig->GetGameMode());
	} else if( m_nOptionsIndex == 1 ) {
		m_pConfig->SetJackDiamondsAmount(m_pConfig->GetJackDiamondsAmount() == 0 ? -10 : 0);
	} else if( m_nOptionsIndex == 2 ) {
		int nPointLimit = m_pConfig->GetScoreLimit();
		//25, 50, 100, 200, 500
		int nNewPoints = 100;
		switch(nPointLimit)
		{
			default:
			case 100:
				nNewPoints = 200;
				break;
			case 25:
				nNewPoints = 50;
				break;
			case 50:
				nNewPoints = 100;
				break;
			case 200:
				nNewPoints = 500;
				break;
			case 500:
				nNewPoints = 25;
				break;
		}
		m_pConfig->SetScoreLimit(nNewPoints);
        } else if( m_nOptionsIndex == 3 ) {
		int nStep = m_pConfig->GetPieceMovePerStep();
		int nNewStep = 7;
		switch(nStep) {
			default:
			case 7:
				nNewStep = 11;
				break;
			case 11:
				nNewStep = 4;
				break;
			case 4:
				nNewStep = 7;
				break;
		}
		m_pConfig->SetPieceMovePerStep(nNewStep);
	}
}

void Options::Move(Option_Direction eDirection)
{
	if( eDirection == OP_Down && m_nOptionsIndex < 3 ) {
		m_nOptionsIndex++;
	} else if( eDirection == OP_Up && m_nOptionsIndex > 0 ) {
		m_nOptionsIndex--;
	}
}

#define GAME_MODE_Y	     (45)
#define JACK_DIAMONDS_Y      (105)
#define POINT_LIMIT_Y	     (125)
#define GAME_SPEED_Y	     (155)
void Options::UpdateDisplay()
{
	//Draw background
	SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, 153, 153, 255));
	nSDL_DrawString(m_pScreen, m_pFont, (SCREEN_WIDTH-nSDL_GetStringWidth(m_pFont, "Options:"))/2, 15, "Options:");

	if( m_pConfig->GetGameMode() == 1 ) {
		nSDL_DrawString(m_pScreen, m_pFont, 12, GAME_MODE_Y, "Game mode: 3 Player");
                nSDL_DrawString(m_pScreen, m_pFont, 12, GAME_MODE_Y+12, 
"3 player mode where each player gets 17\n\
cards.  There is a mystery card in that the\n\
first player to take a trick gets.  It won't\n\
break hearts though.");
	} else {
		nSDL_DrawString(m_pScreen, m_pFont, 12, GAME_MODE_Y, "Game mode: Normal");
                nSDL_DrawString(m_pScreen, m_pFont, 12, GAME_MODE_Y+12, 
"Normal 4 player game where each player\n\
starts with 13 cards.");
	}

	if( m_nOptionsIndex == 0 ) {
                if( is_classic ) {
                        draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 255, 255), 7, GAME_MODE_Y-5, 312, 59, 1);
                } else {
                        draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 7, GAME_MODE_Y-5, 312, 59, 1);
                }
        }

	char buffer[256] = "Jack of Diamonds is -10 points: ";

	if( m_pConfig->GetJackDiamondsAmount() ) {
		strcat(buffer, "Yes");
	} else {
		strcat(buffer, "No");
	}

        if( m_nOptionsIndex == 1 ) {
                if( is_classic ) {
                        draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 255, 255), 7, JACK_DIAMONDS_Y-5, 312, 25, 1);
                } else {
                        draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 7, JACK_DIAMONDS_Y-5, 312, 25, 1);
                }
        }

	nSDL_DrawString(m_pScreen, m_pFont, 12, JACK_DIAMONDS_Y, buffer);

	nSDL_DrawString(m_pScreen, m_pFont, 12, POINT_LIMIT_Y, "Game point limit: %d", m_pConfig->GetScoreLimit());

	if( m_nOptionsIndex == 2 ) {
		if( is_classic ) {
                        draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 255, 255), 7, POINT_LIMIT_Y-5, 312, 25, 1);
                } else {
                        draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 7, POINT_LIMIT_Y-5, 312, 25, 1);
                }
	}

	if( m_pConfig->GetPieceMovePerStep() < 5 ) {	
		nSDL_DrawString(m_pScreen, m_pFont, 12, GAME_SPEED_Y, "Game speed: Slow");
	}
	else if( m_pConfig->GetPieceMovePerStep() >=5 && m_pConfig->GetPieceMovePerStep() < 8 ) {
		nSDL_DrawString(m_pScreen, m_pFont, 12, GAME_SPEED_Y, "Game speed: Normal");
	}
	else {
		nSDL_DrawString(m_pScreen, m_pFont, 12, GAME_SPEED_Y, "Game speed: Fast");
	}

	if( m_nOptionsIndex == 3 ) {
                if( is_classic ) {
                        draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 255, 255), 7, GAME_SPEED_Y-5, 312, 25, 1);
                } else {
                        draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 7, GAME_SPEED_Y-5, 312, 25, 1);
                }
        }

	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

}




