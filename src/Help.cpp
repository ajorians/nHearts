#include "Help.h"

HeartsHelp::HeartsHelp(SDL_Surface* pScreen)
: m_pScreen(pScreen)
{
	m_pFont = nSDL_LoadFont(NSDL_FONT_VGA, 0, 0, 0);

}

HeartsHelp::~HeartsHelp()
{
	nSDL_FreeFont(m_pFont);
}

bool HeartsHelp::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();
	
	return true;
}

bool HeartsHelp::PollEvents()
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
					case SDLK_RETURN:
					case SDLK_SPACE:
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
	return true;
}

void HeartsHelp::UpdateDisplay()
{
	SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, 153, 153, 255));

	nSDL_DrawString(m_pScreen, m_pFont, 15, 20, 
"Hearts is a card game. The object is to\n\
get the least amount of points as possible.\n\
\n\
Each heart is worth 1 point and the Queen\n\
of Spades is worth 13 points!\n\
\n\
You must follow suit if you can.\n\
The highest card of the played suit in each\n\
trick takes the trick.\n\
\n\
The passing order is pass left, pass right,\n\
pass across and non passing round.  When\n\
you pass you can pass any 3 cards.\n\
\n\
The 2 of Clubs starts each round.  On that\n\
first trick you cannot play any Hearts or\n\
Queen of Spades.\n\
\n\
You are player 1!\n\
\n\
Good luck!");

	//int nLeft = 55;

	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}




