#include "ScoreReview.h"
#include "MouseHandling.h"
#include "Defines.h"

extern "C"
{
#include "SDL/SDL_gfxPrimitives.h"
}

#define NUMBER_ENTRIES_VISIBLE	(10)

ScoreReview::ScoreReview(SDL_Surface* pScreen, HeartsLib* pHeartsLib, MouseHandling* pMouse)
: m_pScreen(pScreen), m_pMouse(pMouse), m_pHeartsLib(pHeartsLib), m_nEndOffset(0)
{
	m_pFont = nSDL_LoadFont(NSDL_FONT_VGA, 0, 0, 0);
	m_pFontGood = nSDL_LoadFont(NSDL_FONT_VGA, 0, 255, 0);
	m_pFontBad = nSDL_LoadFont(NSDL_FONT_VGA, 255, 0, 0);
}

ScoreReview::~ScoreReview()
{
	nSDL_FreeFont(m_pFont);
	nSDL_FreeFont(m_pFontGood);
	nSDL_FreeFont(m_pFontBad);
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

					case SDLK_LEFTPAREN:
					case SDLK_UP:
						Scroll(false);
						break;

					case SDLK_RIGHTPAREN:
					case SDLK_DOWN:
						Scroll(true);
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

int ScoreReview::GetScoreXPos(int nScore, int nPlayerIndex) const
{
   int nOffset;
   //int nNumPlayers = GetNumHeartsPlayers(*m_pHeartsLib);
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

nSDL_Font* ScoreReview::GetClrFont(int n1, int n2, int n3, int n4)
{
   if( n1 <= n2 && n1 <= n3 && (GetNumHeartsPlayers(*m_pHeartsLib)==3 ? true : n1 <= n4) )
      return m_pFontGood;
   if( n1 >= n2 && n1 >= n3 && (GetNumHeartsPlayers(*m_pHeartsLib)==3 ? true : n1 >= n4) )
      return m_pFontBad;
   return m_pFont;
}

void ScoreReview::UpdateDisplay()
{
	boxRGBA(m_pScreen, 35/*Left*/, 25/*Top*/, 35 + (SCREEN_WIDTH-35*2)/*Right*/, 25 + (SCREEN_HEIGHT-25*2)/*Bottom*/, 100, 149, 237, 70);

	int nTop = 25 + 10;
	nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, nTop, "Score Review:");
	nTop += 15;

	int nNumPlayers = GetNumHeartsPlayers(*m_pHeartsLib);
	if( nNumPlayers == 3 ) {
		nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, nTop, "     --1-- | --2-- | --3--");
	} else {
		nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, nTop, "     --1-- | --2-- | --3-- | --4--");
	}
	nTop += 10;

	int nRounds = GetNumberOfRounds(*m_pHeartsLib);
        int nStartRoundIndex = nRounds > NUMBER_ENTRIES_VISIBLE ? (nRounds - NUMBER_ENTRIES_VISIBLE + m_nEndOffset) : 0;
	int nEndRoundIndex = nRounds - 1;
	if( nEndRoundIndex > nStartRoundIndex + NUMBER_ENTRIES_VISIBLE - 1 )
		nEndRoundIndex = nStartRoundIndex + NUMBER_ENTRIES_VISIBLE - 1;
	if( nStartRoundIndex != 0 ) {
		nSDL_DrawString(m_pScreen, m_pFont, 35 + 15 + 8, nTop, "...");
                nSDL_DrawString(m_pScreen, m_pFont, GetScoreXPos(10, 0), nTop, "...");
                nSDL_DrawString(m_pScreen, m_pFont, GetScoreXPos(10, 1), nTop, "...");
                nSDL_DrawString(m_pScreen, m_pFont, GetScoreXPos(10, 2), nTop, "...");
		if( nNumPlayers > 3 )
	                nSDL_DrawString(m_pScreen, m_pFont, GetScoreXPos(10, 3), nTop, "...");
                nTop += 10;
	}
	for(int i=nStartRoundIndex; i<=nEndRoundIndex; i++) {
		int n1,n2,n3,n4;
		n1 = GetHeartsRoundScore(*m_pHeartsLib, 0, i);
		n2 = GetHeartsRoundScore(*m_pHeartsLib, 1, i);
		n3 = GetHeartsRoundScore(*m_pHeartsLib, 2, i);
		n4 = nNumPlayers > 3 ? GetHeartsRoundScore(*m_pHeartsLib, 3, i) : 0;
		nSDL_DrawString(m_pScreen, m_pFont, 35 + 15 + (i>9 ? 8 : 10), nTop, "%d", i+1);
		nSDL_DrawString(m_pScreen, GetClrFont(n1,n2,n3,n4), GetScoreXPos(n1, 0), nTop, "%d", n1);
		nSDL_DrawString(m_pScreen, GetClrFont(n2,n1,n3,n4), GetScoreXPos(n2, 1), nTop, "%d", n2);
		nSDL_DrawString(m_pScreen, GetClrFont(n3,n2,n1,n4), GetScoreXPos(n3, 2), nTop, "%d", n3);
		if( nNumPlayers > 3 )
			nSDL_DrawString(m_pScreen, GetClrFont(n4,n2,n3,n1), GetScoreXPos(n4, 3), nTop, "%d", n4);
		nTop += 10;
	}
	if( nEndRoundIndex != (nRounds-1) ) {
                nSDL_DrawString(m_pScreen, m_pFont, 35 + 15 + 8, nTop, "...");
                nSDL_DrawString(m_pScreen, m_pFont, GetScoreXPos(10, 0), nTop, "...");
                nSDL_DrawString(m_pScreen, m_pFont, GetScoreXPos(10, 1), nTop, "...");
                nSDL_DrawString(m_pScreen, m_pFont, GetScoreXPos(10, 2), nTop, "...");
		if( nNumPlayers > 3 )
	                nSDL_DrawString(m_pScreen, m_pFont, GetScoreXPos(10, 3), nTop, "...");
                nTop += 10;
        }

	nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, nTop, "--------------------------------------");
	nTop += 10;

	if( nNumPlayers > 3 ) {
		nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, nTop, "Total:     |      |      |");
	} else {
		 nSDL_DrawString(m_pScreen, m_pFont, 35 + 15, nTop, "Total:     |      |");
	}
	int n1 = GetHeartsPlayerScore(*m_pHeartsLib, 0);
	int n2 = GetHeartsPlayerScore(*m_pHeartsLib, 1);
	int n3 = GetHeartsPlayerScore(*m_pHeartsLib, 2);
	int n4 = GetHeartsPlayerScore(*m_pHeartsLib, 3);
        nSDL_DrawString(m_pScreen, GetClrFont(n1,n2,n3,n4), GetScoreXPos(n1, 0), nTop, "%d", n1);
        nSDL_DrawString(m_pScreen, GetClrFont(n2,n1,n3,n4), GetScoreXPos(n2, 1), nTop, "%d", n2);
        nSDL_DrawString(m_pScreen, GetClrFont(n3,n2,n1,n4), GetScoreXPos(n3, 2), nTop, "%d", n3);
	if( nNumPlayers > 3 )
	        nSDL_DrawString(m_pScreen, GetClrFont(n4,n2,n3,n1), GetScoreXPos(n4, 3), nTop, "%d", n4);
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

void ScoreReview::Scroll(bool bDown)
{
   int nRounds = GetNumberOfRounds(*m_pHeartsLib);
   if( nRounds <= NUMBER_ENTRIES_VISIBLE )
      return;

   m_nEndOffset = bDown ? (m_nEndOffset+1) : (m_nEndOffset-1);
   if( m_nEndOffset > 0 )
      m_nEndOffset = 0;
   if( m_nEndOffset < (-nRounds+NUMBER_ENTRIES_VISIBLE) )
      m_nEndOffset = (-nRounds+NUMBER_ENTRIES_VISIBLE);
}


