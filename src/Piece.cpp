#include "Piece.h"
#include "CardImages.h"
#include "Defines.h"

PieceControl::PieceControl(SDL_Surface* pScreen, Metrics* pMetrics, CardImages* pCardImages, Config* pConfig)
: m_pScreen(pScreen), m_pMetrics(pMetrics), m_pConfig(pConfig), m_pCardImages(pCardImages), m_pRoot(NULL)
{
   m_pCardImageNormal = SDL_CreateRGBSurface(SDL_SWSURFACE, DISPCARD_WIDTH, DISPCARD_HEIGHT, 16, 0, 0, 0, 0);
   m_pCardImages->GetImageForDeckStyle(m_pCardImageNormal, false);

   m_pCardImageFlipped = SDL_CreateRGBSurface(SDL_SWSURFACE, DISPCARD_WIDTH, DISPCARD_HEIGHT, 16, 0, 0, 0, 0);
   m_pCardImages->GetImageForDeckStyle(m_pCardImageFlipped, true);
}

PieceControl::~PieceControl()
{
   ClearPieces();
   SDL_FreeSurface(m_pCardImageNormal);
   SDL_FreeSurface(m_pCardImageFlipped);
}

void PieceControl::ClearPieces()
{
   PieceSprite* pPiece = m_pRoot;
   while(pPiece != NULL) {
      PieceSprite* pTemp = pPiece;
      SDL_FreeSurface(pPiece->img);
      SDL_FreeSurface(pPiece->imgDisabled);
      SDL_FreeSurface(pPiece->replace);
      pPiece = pPiece->next;
      free(pTemp);
   }
   m_pRoot = NULL;
}

bool PieceControl::CreateCard(Card c, CardLocation eLocation, int nCardIndex, bool bHorizontal)
{
   struct PieceSprite* pPiece = (PieceSprite*)malloc(sizeof(struct PieceSprite));
   pPiece->img = NULL;
//   pPiece->imgDisabled = NULL;
   pPiece->replace = NULL;

   pPiece->c = c;
   pPiece->horizontal = false;//bHorizontal;

#ifdef PIECE_ANIMATE_RANDOM_START
   pPiece->x = rand() % 320;//I do random start position; I suppose it is alright :)
   pPiece->y = rand() % 240;
#else
   pPiece->x = m_pMetrics->GetXPos(eLocation, nCardIndex);
   pPiece->y = m_pMetrics->GetYPos(eLocation, nCardIndex);
#endif
   pPiece->toX = m_pMetrics->GetXPos(eLocation, nCardIndex);
   pPiece->toY = m_pMetrics->GetYPos(eLocation, nCardIndex);
   pPiece->m_eSpeed = SpeedNormal;

   pPiece->next = m_pRoot;
   m_pRoot = pPiece;

   int nCardWidth = m_pMetrics->GetCardWidth(), nCardHeight = m_pMetrics->GetCardHeight();
   pPiece->img = SDL_CreateRGBSurface(SDL_SWSURFACE, nCardWidth, nCardHeight, 16, 0, 0, 0, 0);
   pPiece->imgDisabled = SDL_CreateRGBSurface(SDL_SWSURFACE, nCardWidth, nCardHeight, 16, 0, 0, 0, 0);

   if( eLocation != LocBottom && m_pConfig->GetShowAllCards() != 1 ) {
      pPiece->visible = false;
   }
   else {
      pPiece->visible = true;
   }
   pPiece->enabled = true;
   m_pCardImages->GetImageForCard(pPiece->img, c, true, nCardWidth, nCardHeight);
   m_pCardImages->GetImageForCard(pPiece->imgDisabled, c, false, nCardWidth, nCardHeight);
   pPiece->replace = SDL_CreateRGBSurface(SDL_SWSURFACE, nCardWidth, nCardHeight, 16, 0, 0, 0, 0);

   return true;
}

bool PieceControl::CreateInitialCard(Card c, int nX, int nY)
{
   struct PieceSprite* pPiece = (PieceSprite*)malloc(sizeof(struct PieceSprite));
   pPiece->img = NULL;
   pPiece->replace = NULL;

   pPiece->c = c;
   pPiece->horizontal = false;

   pPiece->x = nX;
   pPiece->y = nY;
   pPiece->toX = nX;
   pPiece->toY = nY;
   pPiece->m_eSpeed = SpeedSlower;

   pPiece->next = m_pRoot;
   m_pRoot = pPiece;

   int nCardWidth = m_pMetrics->GetCardWidth(), nCardHeight = m_pMetrics->GetCardHeight();
   pPiece->img = SDL_CreateRGBSurface(SDL_SWSURFACE, nCardWidth, nCardHeight, 16, 0, 0, 0, 0);
   pPiece->imgDisabled = SDL_CreateRGBSurface(SDL_SWSURFACE, nCardWidth, nCardHeight, 16, 0, 0, 0, 0);

   pPiece->visible = false;
   pPiece->enabled = true;
   m_pCardImages->GetImageForCard(pPiece->img, c, true, nCardWidth, nCardHeight);
   m_pCardImages->GetImageForCard(pPiece->imgDisabled, c, false, nCardWidth, nCardHeight);
   pPiece->replace = SDL_CreateRGBSurface(SDL_SWSURFACE, nCardWidth, nCardHeight, 16, 0, 0, 0, 0);

   return true;
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

bool PieceControl::MakeVisible(Card c, bool bVisible, bool bEnabled)
{
   PieceSprite* pCurrent = NULL;
   for(pCurrent = m_pRoot; pCurrent != NULL; pCurrent = pCurrent->next) {
      if( pCurrent->c == c ) {
         if( bVisible && !pCurrent->visible ) {
            pCurrent->visible = bVisible;
         }
         else if( !bVisible && pCurrent->visible && m_pConfig->GetShowAllCards() != 1) {
            pCurrent->visible = bVisible;
         }
         pCurrent->enabled = bEnabled;
         return true;
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

void PieceControl::ShowPiece(PieceSprite* pSprite)
{
   //if( pSprite->visible == false )
      //return;

   SDL_Rect rect;
   rect.x = pSprite->x;
   rect.y = pSprite->y;
   rect.w = pSprite->horizontal ? m_pMetrics->GetCardHeight() : m_pMetrics->GetCardWidth();
   rect.h = pSprite->horizontal ? m_pMetrics->GetCardWidth() : m_pMetrics->GetCardHeight();

   if( pSprite->visible ) {
      if( pSprite->enabled ) {
         SDL_BlitSurface(pSprite->img, NULL, m_pScreen, &rect);
      }
      else {
         SDL_BlitSurface(pSprite->imgDisabled, NULL, m_pScreen, &rect);
      }
   }
   else {
      if( pSprite->horizontal ) {
         SDL_BlitSurface(m_pCardImageFlipped, NULL, m_pScreen, &rect);
      }
      else {
         SDL_BlitSurface(m_pCardImageNormal, NULL, m_pScreen, &rect);
      }
   }
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
      MovePiece(m_pScreen, pCurrent, m_pMetrics, pCurrent->m_eSpeed == SpeedNormal ? m_pConfig->GetPieceMovePerStep() : m_pConfig->GetPieceMovePerStep()/2);
      pCurrent = pCurrent->next;
   }

   pCurrent = m_pRoot;
   while(pCurrent != NULL) {
      ShowPiece(pCurrent);
      pCurrent = pCurrent->next;
   }

   SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   pCurrent = m_pRoot;
   while(pCurrent != NULL) {
      ResetBackground(m_pScreen, pCurrent, m_pMetrics);
      pCurrent = pCurrent->next;
   }

   return true;
}



