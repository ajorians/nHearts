#ifndef GAME_H
#define GAME_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "HeartsLib/HeartsLib.h"
}

#include "Defines.h"
#include "CardImages.h"
#include "Metrics.h"
#include "Selector.h"
#include "Direction.h"

class Game
{
public:
	Game(SDL_Surface* pScreen, CardImages* pCardImages);
	~Game();

	bool Loop();
	
protected:
	bool PollEvents();
	void UpdateDisplay();
	void Move(Direction eDirection);
	void SelectCard();
	void DoGamePlay();

protected:
	SDL_Surface	*m_pScreen;//Does not own
	nSDL_Font *m_pFont;
	CardImages* m_pCardImages;
	HeartsLib m_Hearts;
	Metrics		m_Metrics;
	Selector	m_Selector;
	Uint32		m_uLastAction;
};

#endif
