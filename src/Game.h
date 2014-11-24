#ifndef GAME_H
#define GAME_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "HeartsLib/HeartsLib.h"
	#include "HeartsAI/HeartsAI.h"
}

#include "Defines.h"
#include "CardImages.h"
#include "Metrics.h"
#include "Piece.h"
#include "Message.h"
#include "Direction.h"

class Config;
class AchieveConfig;

class Game
{
public:
	Game(SDL_Surface* pScreen, Config* pConfig, AchieveConfig* pArchieve, CardImages* pCardImages);
	~Game();

	bool Loop();
	
protected:
	bool PollEvents();
	void UpdateDisplay();
	void Move(Direction eDirection);
	void SelectCard();
	void DoGamePlay();
	void RemovedSelectedPieces();
	void RebuildPieces();
	void ConstructAIs();

protected:
	SDL_Surface	*m_pScreen;//Does not own
	Config		*m_pConfig;//Does not own
	AchieveConfig	*m_pAchieve;
	nSDL_Font *m_pFont;
	CardImages* m_pCardImages;
	HeartsLib m_Hearts;
	PieceControl	m_Pieces;
	Metrics		m_Metrics;
	HeartsAILib	m_aAIs[3];
	//Message		m_ShotMoonMessage;
	int		m_nCurrentCard;
	Uint32		m_uLastAction;
};

#endif
