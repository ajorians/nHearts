#ifndef MENU_H
#define MENU_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
}

#include "Background.h"
#include "Config.h"

class AchieveConfig;

class MainMenu
{
public:
	MainMenu(SDL_Surface* pScreen, Config* pConfig, AchieveConfig* pAchieveConfig);
	~MainMenu();

	bool Loop();

	bool ShouldQuit() const;
	bool ShowShowOptions() const;
	bool ShouldShowHelp() const;
	bool ShouldShowAchievements() const;
	
protected:
	bool PollEvents();
	void UpdateDisplay();

protected:
	SDL_Surface	*m_pScreen;//Does not own
	Background	m_Background;
	SDL_Surface	*m_pTitleGraphic;
	SDL_Surface	*m_pPlayGraphic;
	SDL_Surface	*m_pOptionsGraphic;
	nSDL_Font 	*m_pFont;
	enum MenuChoice{Play, Options, Help, Achieve, Quit};
	MenuChoice 	m_eChoice;
	Config		*m_pConfig;
	AchieveConfig	*m_pAchieve;
};

#endif
