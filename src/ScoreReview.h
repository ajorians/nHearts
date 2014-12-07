#ifndef REVIEW_H
#define REVIEW_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "HeartsLib/HeartsLib.h"
}

class MouseHandling;

class ScoreReview
{
public:
	ScoreReview(SDL_Surface* pScreen, HeartsLib* pHeartsLib, MouseHandling* pMouse);
	~ScoreReview();

	bool Loop();
	
protected:
	bool PollEvents();
	void UpdateDisplay();

protected:
	SDL_Surface	*m_pScreen;//Does not own
	MouseHandling	*m_pMouse;//Does not own
	nSDL_Font *m_pFont;
	HeartsLib	*m_pHeartsLib;
	int m_nOffer;
	int m_nCaseContained;
        bool m_bAcceptedOffer;
};

#endif
