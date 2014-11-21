#include "ScoreReview.h"
//#include "MouseHandling.h"
#include "Defines.h"

extern "C"
{
#include "SDL/SDL_gfxPrimitives.h"
}

ScoreReview::ScoreReview(SDL_Surface* pScreen, HeartsLib* pHeartsLib)
: m_pScreen(pScreen), m_pHeartsLib(pHeartsLib)
{
	m_pFont = nSDL_LoadFont(NSDL_FONT_VGA, 0, 0, 0);
}

ScoreReview::~ScoreReview()
{
	nSDL_FreeFont(m_pFont);
}

bool ScoreReview::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();
	
	return true;
}

bool ScoreReview::PollEvents()
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
					
					case SDLK_RETURN:
					case SDLK_LCTRL:
						return false;
						break;

					case SDLK_CLEAR:
					case SDLK_DELETE:
					case SDLK_BACKSPACE:
						return false;
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

	/*
	int nMX = -1, nMY = -1;
        if( m_pMouse->PollMouse(nMX, nMY) ) {
                MouseButton eMouseButton = m_pMouse->GetMouseButton();
                if( eMouseButton == CenterButton ) {
                    return false;
                }
        }*/

	return true;
}

void ScoreReview::UpdateDisplay()
{
	boxRGBA(m_pScreen, 35/*Left*/, 35/*Top*/, 35 + (SCREEN_WIDTH-35*2)/*Right*/, 35 + (SCREEN_HEIGHT-35*2)/*Bottom*/, 100, 149, 237, 70);

	nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, 35 + 10, "Score Review:\n\
Player 1: %d\n\
Player 2: %d\n\
Player 3: %d\n\
Player 4: %d",
GetHeartsPlayerScore(*m_pHeartsLib, 0),
GetHeartsPlayerScore(*m_pHeartsLib, 1),
GetHeartsPlayerScore(*m_pHeartsLib, 2),
GetHeartsPlayerScore(*m_pHeartsLib, 3));

	int nTop = 90;
	int nPlayerShotMoon = -1;
        if( HEARTSLIB_SHOT_THE_MOON == GetPlayerShotMoon(*m_pHeartsLib, &nPlayerShotMoon) ) {
                nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, nTop, "Player %d shot the moon!", nPlayerShotMoon+1/*index to human readable*/);
		nTop += 25;
        }

        if( GetHeartsGameOver(*m_pHeartsLib) == 1 ) {
		nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, nTop, "Score limit reached!");
		nTop += 15;
		nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, nTop, "Game over!"); 
        }

	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}




