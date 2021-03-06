#ifndef PIECE_H
#define PIECE_H

extern "C"
{
        #include <os.h>
        #include "SDL/SDL.h"
	#include "HeartsLib/HeartsLib.h"
}

#include "Metrics.h"
#include "Config.h"

class CardImages;

typedef enum {
  SpeedNormal,
  SpeedSlower
} SpeedCard;

struct PieceSprite
{
   SDL_Surface* img;
   SDL_Surface* imgDisabled;
   SDL_Surface* replace;
   Card c;
   bool horizontal;
   bool visible;//Probably rename
   bool enabled;
   int x,y;
   int toX, toY;
   SpeedCard m_eSpeed;
   bool toFade;
   struct PieceSprite* next;
};

class PieceControl
{
public:
   PieceControl(SDL_Surface* pScreen, Metrics* pBoardMetrics, CardImages* pCardImages, Config* pConfig);
   ~PieceControl();

   void ClearPieces();
   bool CreateCard(Card c, CardLocation eLocation, int nCardIndex, bool bHorizontal);
   bool CreateInitialCard(Card c, int nX, int nY);
   bool MoveCard(Card c, int nX, int nY);
   bool MakeVisible(Card c, bool bVisible, bool bEnabled);
   bool IsAnimating() const;
   bool Animate();

protected:
   void ShowPiece(PieceSprite* pSprite);

protected:
   SDL_Surface* m_pScreen;//Does NOT own
   Metrics* m_pMetrics;//Does NOT own
   Config* m_pConfig;//Does NOT own
   CardImages* m_pCardImages;//Does NOT own
   PieceSprite* m_pRoot;
   SDL_Surface* m_pCardImageNormal;
   SDL_Surface* m_pCardImageFlipped;
};

#endif


