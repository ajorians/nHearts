#ifndef ACHIEVEMENTS_H
#define ACHIEVEMENTS_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
}

#include "ProgressMeter.h"

class AchieveConfig;
class MouseHandling;

enum Achievement_Direction{A_Up, A_Down, A_Left, A_Right};

class Achievements
{
public:
	Achievements(SDL_Surface* pScreen, AchieveConfig* pAchieveConfig/*, MouseHandling* pMouse*/);
	~Achievements();

	bool Loop();
	
protected:
	bool PollEvents();
	void UpdateDisplay();
	void Move(Achievement_Direction eDirection);

protected:
	SDL_Surface	*m_pScreen;//Does not own
	enum AchievementChoice{None, WonAGame, Won5Games, ShotTheMoon, WonWith0Points};
	AchievementChoice m_eChoice;
	nSDL_Font	*m_pFont;
	nSDL_Font	*m_pFontRed;
	AchieveConfig	*m_pAchieveConfig;
	//MouseHandling   *m_pMouse;
	SDL_Surface	*m_imgWonAGame;
	SDL_Surface	*m_imgWon5Games;
	SDL_Surface	*m_imgShotTheMoon;
	SDL_Surface	*m_imgWonWith0Points;

	SDL_Surface     *m_imgDkWonAGame;
	SDL_Surface     *m_imgDkWon5Games;
        SDL_Surface     *m_imgDkShotTheMoon;
        SDL_Surface     *m_imgDkWonWith0Points;

	ProgressMeter	m_Progress;
};

#endif
