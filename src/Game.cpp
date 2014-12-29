#include "Game.h"
#include "BoardBackground.h"
#include "ScoreReview.h"
#include "Config.h"
#include "AchieveConfig.h"
#include "MouseHandling.h"

extern "C"
{
#include "SDL/SDL_gfxPrimitives.h"
}

Game::Game(SDL_Surface* pScreen, MouseHandling* pMouse, Config* pConfig, AchieveConfig* pAchieve, CardImages* pCardImages)
: m_pScreen(pScreen), m_pMouse(pMouse), m_pConfig(pConfig), m_pAchieve(pAchieve), m_pCardImages(pCardImages), m_Pieces(pScreen, &m_Metrics, m_pCardImages, pConfig)/*, m_ShotMoonMessage(pScreen)*/, m_nCurrentCard(-1)
{
	HeartsLibCreate(&m_Hearts, m_pConfig->GetScoreLimit(), m_pConfig->GetJackDiamondsAmount(), m_pConfig->GetGameMode() == 1 ? Reduced3Players : Normal4Players);

	m_pFont = nSDL_LoadFont(NSDL_FONT_THIN, 0/*R*/, 0/*G*/, 0/*B*/);

	m_pBackground = nSDL_LoadImage(image_cardbackground);

	m_Metrics.SetCardDimensions(DISPCARD_WIDTH, DISPCARD_HEIGHT);

	m_uLastAction = SDL_GetTicks();

	RebuildPieces();
	ConstructAIs();
}

Game::~Game()
{
	for(int i=0; i<GetNumHeartsPlayers(m_Hearts); i++)
		HeartsAIFree(&m_aAIs[i]);
	nSDL_FreeFont(m_pFont);
	SDL_FreeSurface(m_pBackground);
}

bool Game::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;

	//Update screen
	UpdateDisplay();

	DoGamePlay();
	
	SDL_Delay(30);
	if( GetHeartsGameOver(m_Hearts) == 1 ) {
		return false;
	}
	
	return true;
}

bool Game::PollEvents()
{
	SDL_Event event;
	
	/* Poll for events. SDL_PollEvent() returns 0 when there are no  */
	/* more events on the event queue, our while loop will exit when */
	/* that occurs.                                                  */
	while( SDL_PollEvent( &event ) )
	{
		/* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
		switch( event.type )
		{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) 
				{
					case SDLK_ESCAPE:
						fprintf(stderr, "Hit Escape!n");
					return false;
					break;

					case SDLK_RETURN:
					case SDLK_LCTRL:
					case SDLK_RCTRL:
						SelectCard();
					break;
					
					case SDLK_RIGHT:
					case SDLK_6:
						Move(Right);
					break;
					
					case SDLK_LEFT:
					case SDLK_4:
						Move(Left);
					break;

					case SDLK_CLEAR:
					case SDLK_BACKSPACE:
						RemovedSelectedPieces();
					break;

					default:
						break;
				}
				break;
			
			//Called when the mouse moves
			case SDL_MOUSEMOTION:
				break;
			
			case SDL_KEYUP:
				break;
			
			default:
				break;
		}
	}

	int nMX = -1, nMY = -1;
        if( m_pMouse->PollMouse(nMX, nMY) ) {
                MouseButton eMouseButton = m_pMouse->GetMouseButton();
                if( eMouseButton == CenterButton ) {
			SelectCard();
		}
	}

	return true;
}

void Game::UpdateDisplay()
{
	m_Pieces.Animate();

	/*if( m_ShotMoonMessage.HasMessage() && ((!m_ShotMoonMessage.IsAnimating() && !m_ShotMoonMessage.IsStayDuration()) || m_ShotMoonMessage.FinishFast()) ) {
		m_ShotMoonMessage.ClearMessage();
		return;
	}

	if( m_ShotMoonMessage.HasMessage() && m_ShotMoonMessage.Animate() ) {
		 return; }*/

	//Update cards in hand
	bool bHasPassedCards = HasPassedCards(m_Hearts) == HEARTSLIB_PASSED_CARDS;
	for(int nPlayerIndex = 0; nPlayerIndex<GetNumHeartsPlayers(m_Hearts); nPlayerIndex++) {
		int nNumCards = GetNumberOfCardsInHand(m_Hearts, nPlayerIndex);
		int nNumSelected = GetNumberSelectedCards(m_Hearts, nPlayerIndex);
		if( !bHasPassedCards ) {
			m_Metrics.SetNumCards(nNumCards - nNumSelected);
		}
		else {
			m_Metrics.SetNumCards(nNumCards);
		}
		int nNonSelectedCard = 0, nSelectedCard = 0;
		for(int i=0; i<nNumCards; i++ ) {
			Card c;
			GetCardInHand(m_Hearts, &c, nPlayerIndex, i);
			if( nPlayerIndex == 0 && !bHasPassedCards && IsCardSelected(m_Hearts, nPlayerIndex, i) == HEARTSLIB_CARD_SELECTED ) {
				m_Pieces.MoveCard(c, m_Metrics.GetSelectedXPos(nSelectedCard++), m_Metrics.GetSelectedTop());
			 }
			else {
				int nX = m_Metrics.GetXPos(GetCardLocation(nPlayerIndex), nNonSelectedCard);
				int nY = m_Metrics.GetYPos(GetCardLocation(nPlayerIndex), nNonSelectedCard);
				if( nPlayerIndex == 0 && m_nCurrentCard == i )
					nY-=10;
				m_Pieces.MoveCard(c, nX, nY);
				nNonSelectedCard++;
			}

			bool bCanPlayCard = true;
			if( bHasPassedCards == false ) {
				bCanPlayCard = true;
			}
			else {
				if( nPlayerIndex == 0 )
					bCanPlayCard = CanPlayCard(m_Hearts, nPlayerIndex, i) == HEARTSLIB_CAN_PLAY_CARD;
			}
			m_Pieces.MakeVisible(c, nPlayerIndex == 0, bCanPlayCard);
		}
	}

	//Updates cards being played
	int nNumCardsInMiddle = GetNumberOfCardsInMiddle(m_Hearts);
	for(int i=0; i<nNumCardsInMiddle; i++) {
		Card c;
		int nPlayerIndex;
		GetMiddleCard(m_Hearts, &c, i, &nPlayerIndex);
		m_Pieces.MoveCard(c, m_Metrics.GetMiddleCardX(GetCardLocation(nPlayerIndex)), m_Metrics.GetMiddleCardY(GetCardLocation(nPlayerIndex)));
		m_Pieces.MakeVisible(c, true, true);
	}

	//Draw background
#if 0
	SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, GAME_BACKGROUND_R, GAME_BACKGROUND_G, GAME_BACKGROUND_B));
#endif
	SDL_BlitSurface(m_pBackground, NULL, m_pScreen, NULL);
	
	//Display scores
	/*nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH-30, SCREEN_HEIGHT-20, "%d|%d", ScoreOfCardsTaken(m_Hearts, 0), GetPlayerScore(m_Hearts, 0) );
	nSDL_DrawString(m_pScreen, m_pFont, 0, SCREEN_HEIGHT/2, "%d|%d", ScoreOfCardsTaken(m_Hearts, 1), GetPlayerScore(m_Hearts, 1) );
	nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH/2, 0, "%d|%d", ScoreOfCardsTaken(m_Hearts, 2), GetPlayerScore(m_Hearts, 2) );
	nSDL_DrawString(m_pScreen, m_pFont, SCREEN_WIDTH-30, SCREEN_HEIGHT/2, "%d|%d", ScoreOfCardsTaken(m_Hearts, 3), GetPlayerScore(m_Hearts, 3) );*/

	if( HasPassedCards(m_Hearts) == HEARTSLIB_NOT_PASSED_CARDS ) {
		Pass_Direction_t ePass = GetHeartsPassDirection(m_Hearts);
		if( ePass == PassLeft ) {
			 nSDL_DrawString(m_pScreen, m_pFont, 30, SCREEN_HEIGHT-20, "Pass cards left");
		}
		else if( ePass == PassRight ) {
                         nSDL_DrawString(m_pScreen, m_pFont, 30, SCREEN_HEIGHT-20, "Pass cards right");
                }
		else if( ePass == PassAcross ) {
                         nSDL_DrawString(m_pScreen, m_pFont, 30, SCREEN_HEIGHT-20, "Pass cards across");
                }
	}
	else {
		if( GetPlayersTurn(m_Hearts) == 0 ) {
			nSDL_DrawString(m_pScreen, m_pFont, 30, SCREEN_HEIGHT-20, "It is your turn");
		}
	}
}

void Game::Move(Direction eDirection)
{
   int nNumCards = GetNumberOfCardsInHand(m_Hearts, 0);
   if( eDirection == Left ) {
      m_nCurrentCard--;
   }
   else if( eDirection == Right ) {
      m_nCurrentCard++;
   }

   if( m_nCurrentCard < 0 ) {
      m_nCurrentCard = nNumCards-1;
   }
   else if( m_nCurrentCard >= nNumCards ) {
      m_nCurrentCard = 0;
   }
}

void Game::SelectCard()
{
   if( HasPassedCards(m_Hearts) == HEARTSLIB_NOT_PASSED_CARDS ) {
      ToggleSelectedCard(m_Hearts, 0, m_nCurrentCard);
      if( GetNumberSelectedCards(m_Hearts, 0) == 3 ) {
         PassSelectedCards(m_Hearts, 0);

         for(int i=1; i<GetNumHeartsPlayers(m_Hearts); i++) {
            int nIndex1, nIndex2, nIndex3;
            HeartsAIDesiredPassIndexes(m_aAIs[i-1], &nIndex1, &nIndex2, &nIndex3);
            ToggleSelectedCard(m_Hearts, i, nIndex1);
            ToggleSelectedCard(m_Hearts, i, nIndex2);
            ToggleSelectedCard(m_Hearts, i, nIndex3);

            PassSelectedCards(m_Hearts, i);
         }
      }
   }
   else {
      PlayCard(m_Hearts, 0, m_nCurrentCard);

      if( GetNumberOfCardsInHand(m_Hearts, 0) == 1 )//If only one card select it
         m_nCurrentCard = 0;
   }
}

void Game::DoGamePlay()
{
   unsigned int nDelay = (m_pConfig->GetPieceMovePerStep() > 8) ? 400 : (m_pConfig->GetPieceMovePerStep() < 5) ? 1500 : 1000;
   if( (SDL_GetTicks() - m_uLastAction) < nDelay )
      return;

   if( HasPassedCards(m_Hearts) == HEARTSLIB_PASSED_CARDS ) {
      if( HasEverybodyPlayedTheirCard(m_Hearts ) ) {
         int nPlayerIndex;
         FigureOutWhoTakesTrick(m_Hearts, &nPlayerIndex);
         int nNumCardsInMiddle = GetNumberOfCardsInMiddle(m_Hearts);
         for(int nCard = 0; nCard < nNumCardsInMiddle; nCard++) {
            Card c;
            GetMiddleCard(m_Hearts, &c, nCard, NULL);
            m_Pieces.MoveCard(c, m_Metrics.GetPlayerSideX(GetCardLocation(nPlayerIndex)), m_Metrics.GetPlayerSideY(GetCardLocation(nPlayerIndex)));
         }
         int nNumInitialCards = 1;
         for(int i=0; i<nNumInitialCards; i++) {
            Card c;
            GetInitialCard(m_Hearts, &c, i);
            m_Pieces.MakeVisible(c, true, true);
            m_Pieces.MoveCard(c, m_Metrics.GetPlayerSideX(GetCardLocation(nPlayerIndex)), m_Metrics.GetPlayerSideY(GetCardLocation(nPlayerIndex)));
         }
         GiveTrickToPlayer(m_Hearts);
      }
      else {
        int nPlayersTurn = GetPlayersTurn(m_Hearts);
         if( nPlayersTurn != 0 ) {
            int nIndex;
            HeartsAIDesiredPlayIndex(m_aAIs[nPlayersTurn-1], &nIndex);
            PlayCard(m_Hearts, nPlayersTurn, nIndex);
            m_uLastAction = SDL_GetTicks();
         }
         return;//return so will check if everyone played their cards again and will give trick to player and update score
      }
   }

   if( HasPassedCards(m_Hearts) == HEARTSLIB_PASSED_CARDS ) {
      for(int i=0; i<GetNumHeartsPlayers(m_Hearts); i++)
         if( GetNumberOfCardsInHand(m_Hearts, i) != 0 )
            return;

      ScoreReview r(m_pScreen, &m_Hearts, m_pMouse);
      while( r.Loop() ){}
      m_pAchieve->LookForAchievements(m_Hearts);
      DoHeartsNextHand(m_Hearts);
      RebuildPieces();
   }
}

void Game::RemovedSelectedPieces()
{
   if( HasPassedCards(m_Hearts) != HEARTSLIB_PASSED_CARDS ) {
      int nCardsInHand = GetNumberOfCardsInHand(m_Hearts, 0);
      for(int i=0; i<nCardsInHand; i++) {
         if( IsCardSelected(m_Hearts, 0, i) == HEARTSLIB_CARD_SELECTED )
            ToggleSelectedCard(m_Hearts, 0, i);
      }
   }
}

void Game::RebuildPieces()
{
   m_Pieces.ClearPieces();
   for(int nPlayerIndex=0; nPlayerIndex<GetNumHeartsPlayers(m_Hearts); nPlayerIndex++) {
      int nNumCards = GetNumberOfCardsInHand(m_Hearts, nPlayerIndex);
      m_Metrics.SetNumCards(nNumCards);
      for(int i=0; i<nNumCards; i++) {
         Card c;
         GetCardInHand(m_Hearts, &c, nPlayerIndex, i);
         bool bHorizontal = nPlayerIndex == 1 || nPlayerIndex == 3;
         if( GetNumHeartsPlayers(m_Hearts) == 3 && nPlayerIndex == 2 )
            bHorizontal = true;
         m_Pieces.CreateCard(c, GetCardLocation(nPlayerIndex), i, bHorizontal);
      }
   }

   int nNumInitialCards = GetNumHeartsPlayers(m_Hearts)==3 ? 1 : 0;
   for(int i=0; i<nNumInitialCards; i++) {
      Card c;
      GetInitialCard(m_Hearts, &c, i);
      m_Pieces.CreateInitialCard(c, m_Metrics.GetInitialCardX(), m_Metrics.GetInitialCardY());
   }
}

void Game::ConstructAIs()
{
   for(int i=0; i<GetNumHeartsPlayers(m_Hearts); i++) {
      HeartsAICreate(&m_aAIs[i], m_Hearts, i+1);
   }
}

CardLocation Game::GetCardLocation(int nPlayerIndex) const
{
   if( nPlayerIndex == 1 )
      return LocLeft;
   if( nPlayerIndex == 2 && GetNumHeartsPlayers(m_Hearts) == 4 )
      return LocTop;
   if( nPlayerIndex == 2 && GetNumHeartsPlayers(m_Hearts) == 3 )
      return LocRight;
   if( nPlayerIndex == 3 )
      return LocRight;
   return LocBottom;
}
