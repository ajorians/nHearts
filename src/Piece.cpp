#include "Piece.h"
#include "CardImages.h"
#include "Defines.h"

PieceControl::PieceControl(SDL_Surface* pScreen, Metrics* pMetrics, CardImages* pCardImages)
: m_pScreen(pScreen), m_pMetrics(pMetrics), m_pCardImages(pCardImages), m_pRoot(NULL)
{
}

PieceControl::~PieceControl()
{
   ClearPieces();
}

void PieceControl::ClearPieces()
{
   PieceSprite* pPiece = m_pRoot;
   while(pPiece != NULL) {
      PieceSprite* pTemp = pPiece;
      SDL_FreeSurface(pPiece->img);
      SDL_FreeSurface(pPiece->replace);
      pPiece = pPiece->next;
      free(pTemp);
   }
   m_pRoot = NULL;
}

bool PieceControl::CreateCard(Card c, int nPlayerIndex, int nCardIndex, bool bHorizontal)
{
   struct PieceSprite* pPiece = (PieceSprite*)malloc(sizeof(struct PieceSprite));
   pPiece->img = NULL;
   pPiece->replace = NULL;

   pPiece->c = c;
   pPiece->horizontal = false;//bHorizontal;

#ifdef PIECE_ANIMATE_RANDOM_START
   pPiece->x = rand() % 320;//I do random start position; I suppose it is alright :)
   pPiece->y = rand() % 240;
#else
   pPiece->x = m_pMetrics->GetXPos(nPlayerIndex, nCardIndex);
   pPiece->y = m_pMetrics->GetYPos(nPlayerIndex, nCardIndex);
#endif
   pPiece->toX = m_pMetrics->GetXPos(nPlayerIndex, nCardIndex);
   pPiece->toY = m_pMetrics->GetYPos(nPlayerIndex, nCardIndex);

   pPiece->next = m_pRoot;
   m_pRoot = pPiece;

   int nCardWidth = m_pMetrics->GetCardWidth(), nCardHeight = m_pMetrics->GetCardHeight();
   pPiece->img = SDL_CreateRGBSurface(SDL_SWSURFACE, nCardWidth, nCardHeight, 16, 0, 0, 0, 0);
   if( nPlayerIndex != 0 ) {
      pPiece->visible = false;
      m_pCardImages->GetImageForDeckStyle(pPiece->img);
   }
   else {
      pPiece->visible = true;
      m_pCardImages->GetImageForCard(pPiece->img, c, nCardWidth, nCardHeight);
   }
   pPiece->replace = SDL_CreateRGBSurface(SDL_SWSURFACE, nCardWidth, nCardHeight, 16, 0, 0, 0, 0);

   return true;
}

bool PieceControl::UpdateCardImage(Card c, bool bCardImage)
{
   PieceSprite* pCurrent = NULL;
   for(pCurrent = m_pRoot; pCurrent != NULL; pCurrent = pCurrent->next) {
      if( pCurrent->c == c ) {
         if( pCurrent->img != NULL ) {
            SDL_FreeSurface(pCurrent->img);
            SDL_FreeSurface(pCurrent->replace);
         }
         int nCardWidth = m_pMetrics->GetCardWidth(), nCardHeight = m_pMetrics->GetCardHeight();
         pCurrent->img = SDL_CreateRGBSurface(SDL_SWSURFACE, nCardWidth, nCardHeight, 16, 0, 0, 0, 0);
         if( bCardImage ) {
            m_pCardImages->GetImageForCard(pCurrent->img, c, nCardWidth, nCardHeight);
         }
         else
         {
            m_pCardImages->GetImageForDeckStyle(pCurrent->img);
         }
         pCurrent->replace = SDL_CreateRGBSurface(SDL_SWSURFACE, nCardWidth, nCardHeight, 16, 0, 0, 0, 0);
         return true;
      }
   }
   return false;
}

bool PieceControl::MoveCard(Card c, int nX, int nY)
{
   PieceSprite* pCurrent = NULL;
   for(pCurrent = m_pRoot; pCurrent != NULL; pCurrent = pCurrent->next) {
      if( pCurrent->c == c ) {
         pCurrent->toX = nX;
         pCurrent->toY = nY;
         return true;
      }
   }
   return false;
}

bool PieceControl::MakeVisible(Card c, bool bVisible)
{
   PieceSprite* pCurrent = NULL;
   for(pCurrent = m_pRoot; pCurrent != NULL; pCurrent = pCurrent->next) {
      if( pCurrent->c == c ) {
         if( bVisible && !pCurrent->visible ) {
            pCurrent->visible = bVisible;
            UpdateCardImage(c, bVisible);
         }
         else if( !bVisible && pCurrent->visible ) {
            pCurrent->visible = bVisible;
            UpdateCardImage(c, bVisible);
         }
      }
   }
   return false;
}

bool IsPieceMoving(PieceSprite* pSprite)
{
   return (pSprite->y != pSprite->toY) || (pSprite->x != pSprite->toX);
}

bool PieceControl::IsAnimating() const
{
   PieceSprite* pCurrent = NULL;
   for(pCurrent = m_pRoot; pCurrent != NULL; pCurrent = pCurrent->next) {
      if( IsPieceMoving(pCurrent) )
         return true;
   }

   return false;
}

void SetBackground(SDL_Surface* pScreen, PieceSprite* pSprite, Metrics* pMetrics)
{
   SDL_Rect rect;
   rect.x = pSprite->x;
   rect.y = pSprite->y;
   rect.w = pSprite->horizontal ? pMetrics->GetCardHeight() : pMetrics->GetCardWidth();
   rect.h = pSprite->horizontal ? pMetrics->GetCardWidth() : pMetrics->GetCardHeight();

   SDL_BlitSurface(pScreen, &rect, pSprite->replace, NULL);
}

void MovePiece(SDL_Surface* pScreen, PieceSprite* pSprite, Metrics* pMetrics, int nMoveStepAmount)
{
   if( (pSprite->x != pSprite->toX) || (pSprite->y != pSprite->toY) ) {

      for(int n=0; n<nMoveStepAmount; n++) {
         if( pSprite->x == pSprite->toX )
            break;
         if( pSprite->x < pSprite->toX ) {
            pSprite->x++;
         }
         if( pSprite->x > pSprite->toX ) {
            pSprite->x--;
         }
      }

      for(int n=0; n<nMoveStepAmount; n++) {
         if( pSprite->y == pSprite->toY )
            break;
         if( pSprite->y < pSprite->toY ) {
            pSprite->y++;
         }
         if( pSprite->y > pSprite->toY ) {
            pSprite->y--;
         }
      }
   }

   SetBackground(pScreen, pSprite, pMetrics);
}

void Brighten(SDL_Surface* pSurface, int nBrightenAmount)
{
   //If the surface must be locked 
   if( SDL_MUSTLOCK( pSurface ) )
   {
      //Lock the surface 
      SDL_LockSurface( pSurface );
   }

   //Go through columns 
   for( int x = 0; x < pSurface->w; x++ )
   {
      //Go through rows 
      for( int y = 0; y < pSurface->h; y++ )
      {
         Uint32 pixel = nSDL_GetPixel(pSurface, x, y);
         Uint8 r = 0, g = 0, b = 0;
         SDL_GetRGB(pixel, pSurface->format, &r, &g, &b);
         r = Puz_Min(255, r+nBrightenAmount);
         g = Puz_Min(255, g+nBrightenAmount);
         b = Puz_Min(255, b+nBrightenAmount);

         nSDL_SetPixel(pSurface, x, y, SDL_MapRGB(pSurface->format, r, g, b));
      }
   }

   //Unlock surface 
   if( SDL_MUSTLOCK( pSurface ) )
   {
      SDL_UnlockSurface( pSurface );
   }
}

void ShowPiece(SDL_Surface* pScreen, PieceSprite* pSprite, Metrics* pMetrics)
{
   //if( pSprite->visible == false )
      //return;

   SDL_Rect rect;
   rect.x = pSprite->x;
   rect.y = pSprite->y;
   rect.w = pSprite->horizontal ? pMetrics->GetCardHeight() : pMetrics->GetCardWidth();
   rect.h = pSprite->horizontal ? pMetrics->GetCardWidth() : pMetrics->GetCardHeight();

   SDL_BlitSurface(pSprite->img, NULL, pScreen, &rect);
}

void ResetBackground(SDL_Surface* pScreen, PieceSprite* pSprite, Metrics* pMetrics)
{
   SDL_Rect rect;
   rect.x = pSprite->x;
   rect.y = pSprite->y;
   rect.w = pSprite->horizontal ? pMetrics->GetCardHeight() : pMetrics->GetCardWidth();
   rect.h = pSprite->horizontal ? pMetrics->GetCardWidth() :pMetrics->GetCardHeight();

   SDL_BlitSurface(pSprite->replace, NULL, pScreen, &rect);
}

bool PieceControl::Animate()
{
   //if( !IsAnimating() ) {
      //return false;
   //}

   PieceSprite* pCurrent = m_pRoot;

   while(pCurrent != NULL) {
      MovePiece(m_pScreen, pCurrent, m_pMetrics, 5/*m_pConfig->GetPieceMovePerStep()*/);
      pCurrent = pCurrent->next;
   }

   pCurrent = m_pRoot;
   while(pCurrent != NULL) {
      ShowPiece(m_pScreen, pCurrent, m_pMetrics);
      pCurrent = pCurrent->next;
   }

   //m_pMovesLabel->DrawLabel();
   SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   pCurrent = m_pRoot;
   while(pCurrent != NULL) {
      ResetBackground(m_pScreen, pCurrent, m_pMetrics);
      pCurrent = pCurrent->next;
   }

   return true;
}



