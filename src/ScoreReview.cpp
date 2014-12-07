#include "ScoreReview.h"
#include "MouseHandling.h"
#include "Defines.h"

extern "C"
{
#include "SDL/SDL_gfxPrimitives.h"
}

ScoreReview::ScoreReview(SDL_Surface* pScreen, HeartsLib* pHeartsLib, MouseHandling* pMouse)
: m_pScreen(pScreen), m_pHeartsLib(pHeartsLib), m_pMouse(pMouse)
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

	int nMX = -1, nMY = -1;
        if( m_pMouse->PollMouse(nMX, nMY) ) {
                MouseButton eMouseButton = m_pMouse->GetMouseButton();
                if( eMouseButton == CenterButton ) {
                    return false;
                }
        }

	return true;
}

int GetScoreXPos(int nScore, int nPlayerIndex)
{
   int nOffset;
   switch(nPlayerIndex)
   {
      default:
      case 0:
         nOffset = 58;
         break;
      case 1:
         nOffset = 109;
         break;
      case 2:
         nOffset = 159;
         break;
      case 3:
         nOffset = 211;
         break;
   }

   int nNumberOffset = (nScore>9 ? 8 : 10);
   return 35 + nOffset + nNumberOffset;
}

void ScoreReview::UpdateDisplay()
{
	boxRGBA(m_pScreen, 35/*Left*/, 25/*Top*/, 35 + (SCREEN_WIDTH-35*2)/*Right*/, 25 + (SCREEN_HEIGHT-25*2)/*Bottom*/, 100, 149, 237, 70);

	int nTop = 25 + 10;
	nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, nTop, "Score Review:");
	nTop += 15;

	nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, nTop, "     --1-- | --2-- | --3-- | --4--");
	nTop += 10;

	int nRounds = GetNumberOfRounds(*m_pHeartsLib);
        int nStartRound = nRounds > 12 ? nRounds - 12 : 0;
	for(int i=nStartRound; i<nRounds; i++) {
		int n1,n2,n3,n4;
		n1 = GetHeartsRoundScore(*m_pHeartsLib, 0, i);
		n2 = GetHeartsRoundScore(*m_pHeartsLib, 1, i);
		n3 = GetHeartsRoundScore(*m_pHeartsLib, 2, i);
		n4 = GetHeartsRoundScore(*m_pHeartsLib, 3, i);
		nSDL_DrawString(m_pScreen, m_pFont, 35 + 15 + (i>9 ? 8 : 10), nTop, "%d", i+1);
		nSDL_DrawString(m_pScreen, m_pFont, GetScoreXPos(n1, 0), nTop, "%d", n1);
		nSDL_DrawString(m_pScreen, m_pFont, GetScoreXPos(n2, 1), nTop, "%d", n2);
		nSDL_DrawString(m_pScreen, m_pFont, GetScoreXPos(n3, 2), nTop, "%d", n3);
		nSDL_DrawString(m_pScreen, m_pFont, GetScoreXPos(n4, 3), nTop, "%d", n4);
		nTop += 10;
	}

	nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, nTop, "--------------------------------------");
	nTop += 10;

	nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, nTop, "Total:     |      |      |");
	int n1 = GetHeartsPlayerScore(*m_pHeartsLib, 0);
	int n2 = GetHeartsPlayerScore(*m_pHeartsLib, 1);
	int n3 = GetHeartsPlayerScore(*m_pHeartsLib, 2);
	int n4 = GetHeartsPlayerScore(*m_pHeartsLib, 3);
        nSDL_DrawString(m_pScreen, m_pFont, GetScoreXPos(n1, 0), nTop, "%d", n1);
        nSDL_DrawString(m_pScreen, m_pFont, GetScoreXPos(n2, 1), nTop, "%d", n2);
        nSDL_DrawString(m_pScreen, m_pFont, GetScoreXPos(n3, 2), nTop, "%d", n3);
        nSDL_DrawString(m_pScreen, m_pFont, GetScoreXPos(n4, 3), nTop, "%d", n4);
	nTop += 15;

	int nPlayerShotMoon = -1;
	if( HEARTSLIB_SHOT_THE_MOON == GetPlayerShotMoon(*m_pHeartsLib, &nPlayerShotMoon) ) {
                nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, nTop, "Player %d shot the moon!", nPlayerShotMoon+1/*index to human readable*/);
		nTop += 10;
        }
	if( GetHeartsGameOver(*m_pHeartsLib) == 1 ) {
                nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, nTop, "Score limit reached!");
		nTop += 10;
                nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, nTop, "Game over!");
		nTop += 10;
        }

	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}




