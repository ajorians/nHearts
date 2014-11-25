#include "Menu.h"
#include "HeartsGraphic.h"
#include "PlayGraphic.h"
#include "OptionsHelpGraphic.h"
#include "AchievementGraphic.h"
#include "AchieveConfig.h"
#include "Defines.h"

MainMenu::MainMenu(SDL_Surface* pScreen, Config* pConfig, AchieveConfig* pAchieve)
: m_pScreen(pScreen), m_Background(pScreen, pConfig), m_eChoice(Play), m_pConfig(pConfig), m_pAchieve(pAchieve)
{
	m_pTitleGraphic 	= nSDL_LoadImage(image_nHeartsText);
	m_pPlayGraphic		= nSDL_LoadImage(image_Play);
	m_pOptionsGraphic	= nSDL_LoadImage(image_OptionsAndHelp);
	m_pAchievementGraphic	= nSDL_LoadImage(image_AchievementGraphic);
	SDL_SetColorKey(m_pTitleGraphic, SDL_SRCCOLORKEY, SDL_MapRGB(m_pTitleGraphic->format, 255, 255, 255));
	SDL_SetColorKey(m_pPlayGraphic, SDL_SRCCOLORKEY, SDL_MapRGB(m_pPlayGraphic->format, 255, 255, 255));
	SDL_SetColorKey(m_pOptionsGraphic, SDL_SRCCOLORKEY, SDL_MapRGB(m_pOptionsGraphic->format, 255, 255, 255));
	SDL_SetColorKey(m_pAchievementGraphic, SDL_SRCCOLORKEY, SDL_MapRGB(m_pAchievementGraphic->format, 255, 255, 255));

	m_pFont = nSDL_LoadFont(NSDL_FONT_THIN, 0/*R*/, 0/*G*/, 0/*B*/);
}

MainMenu::~MainMenu()
{
	SDL_FreeSurface(m_pTitleGraphic);
	SDL_FreeSurface(m_pPlayGraphic);
	SDL_FreeSurface(m_pOptionsGraphic);
	SDL_FreeSurface(m_pAchievementGraphic);
	nSDL_FreeFont(m_pFont);
}

bool MainMenu::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();

	rand();

	SDL_Delay(30);
	
	return true;
}

bool MainMenu::ShouldQuit() const
{
	return m_eChoice == Quit;
}

bool MainMenu::ShowShowOptions() const
{
	return m_eChoice == Options;
}

bool MainMenu::ShouldShowHelp() const
{
	return m_eChoice == Help;
}

bool MainMenu::ShouldShowAchievements() const
{
	return m_eChoice == Achieve;
}

bool MainMenu::PollEvents()
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
				printf( "Key press detected\n" );
				switch (event.key.keysym.sym) 
				{
					case SDLK_ESCAPE:
						fprintf(stderr, "Hit Escape!n");
						m_eChoice = Quit;
					return false;
					break;
					
					case SDLK_UP:
					case SDLK_8:
						if( m_eChoice == Help )
							m_eChoice = Options;
						else if( m_eChoice == Options )
							m_eChoice = Achieve;
						else if( m_eChoice == Achieve )
							m_eChoice = Play;
						break;
					
					case SDLK_DOWN:
					case SDLK_2:
						if( m_eChoice == Play )
							m_eChoice = Achieve;
						else if( m_eChoice == Achieve )
							m_eChoice = Options;
						else if( m_eChoice == Options )
							m_eChoice = Help;
						break;

					case SDLK_SPACE:
					case SDLK_RETURN:
					case SDLK_LCTRL:
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
				printf( "Key release detected\n" );
				break;
			
			default:
				break;
		}
	}
	return true;
}

void MainMenu::UpdateDisplay()
{
	//Draw background
	if( is_classic ) {
		SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, 255, 255, 255));
		nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH/2-30, 15, "nHearts");
		nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH/2-30, 75, "Play");
		nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH/2-30, 162, "Achievements");
		nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH/2-30, 182, "Options");
		nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH/2-30, 205, "Help");
	} else {
		m_Background.DrawBackground();
		SDL_Rect rectTitle;
		rectTitle.x = (SCREEN_WIDTH - m_pTitleGraphic->w)/2;
		rectTitle.y = 15;
		rectTitle.w = m_pTitleGraphic->w;
		rectTitle.h = m_pTitleGraphic->h;
		SDL_BlitSurface(m_pTitleGraphic, NULL, m_pScreen, &rectTitle);

		SDL_Rect rectPlay;
                rectPlay.x = (SCREEN_WIDTH - m_pPlayGraphic->w)/2;
                rectPlay.y = 76;
                rectPlay.w = m_pPlayGraphic->w;
                rectPlay.h = m_pPlayGraphic->h;
                SDL_BlitSurface(m_pPlayGraphic, NULL, m_pScreen, &rectPlay);

		SDL_Rect rectAchievements;
                rectAchievements.x = (SCREEN_WIDTH - m_pAchievementGraphic->w)/2;
                rectAchievements.y = 159;
                rectAchievements.w = m_pAchievementGraphic->w;
                rectAchievements.h = m_pAchievementGraphic->h;
                SDL_BlitSurface(m_pAchievementGraphic, NULL, m_pScreen, &rectAchievements);

		SDL_Rect rectOptions;
                rectOptions.x = (SCREEN_WIDTH - m_pOptionsGraphic->w)/2;
                rectOptions.y = 179;
                rectOptions.w = m_pOptionsGraphic->w;
                rectOptions.h = m_pOptionsGraphic->h;
                SDL_BlitSurface(m_pOptionsGraphic, NULL, m_pScreen, &rectOptions);
	}

	if( m_eChoice == Play )
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), SCREEN_WIDTH/2-60, 70, 120, 65
	, 1);
	else if( m_eChoice == Achieve )
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 90, 152, 142, 24, 1);
	else if( m_eChoice == Options )
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 114, 172, 96, 24, 1);
	if( m_eChoice == Help )
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 130, 195, 60, 25, 1);
	
	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}



