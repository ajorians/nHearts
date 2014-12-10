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
	void Scroll(bool bDown);
	nSDL_Font* GetClrFont(int n1, int n2, int n3, int n4);

protected:
	SDL_Surface	*m_pScreen;//Does not own
	MouseHandling	*m_pMouse;//Does not own
	nSDL_Font *m_pFont;
	nSDL_Font *m_pFontGood;
	nSDL_Font *m_pFontBad;
	HeartsLib	*m_pHeartsLib;
	int m_nEndOffset;
};

#endif
