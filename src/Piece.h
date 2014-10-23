#ifndef PIECE_H
#define PIECE_H

extern "C"
{
        #include <os.h>
        #include "SDL/SDL.h"
	#include "HeartsLib/HeartsLib.h"
}

#include "Metrics.h"
//#include "Config.h"

class CardImages;

struct PieceSprite
{
   SDL_Surface* img;
   SDL_Surface* replace;
   Card c;
   bool horizontal;
   bool visible;//Probably rename
   int x,y;
   int toX, toY;
   bool toFade;
   struct PieceSprite* next;
};

class PieceControl
{
public:
   PieceControl(SDL_Surface* pScreen, Metrics* pBoardMetrics, CardImages* pCardImages);
   ~PieceControl();

   void ClearPieces();
   bool CreateCard(Card c, int nPlayerIndex, int nCardIndex, bool bHorizontal);
   bool MoveCard(Card c, int nX, int nY);
   bool MakeVisible(Card c, bool bVisible);
   bool IsAnimating() const;
   bool Animate();

protected:
   void ShowPiece(PieceSprite* pSprite);

protected:
   SDL_Surface* m_pScreen;//Does NOT own
   Metrics* m_pMetrics;//Does NOT own
//   Config* m_pConfig;//Does NOT own
   CardImages* m_pCardImages;//Does NOT own
   PieceSprite* m_pRoot;
   SDL_Surface* m_pCardImageNormal;
   SDL_Surface* m_pCardImageFlipped;
};

#endif


