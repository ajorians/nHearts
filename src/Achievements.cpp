#include "Achievements.h"
#include "AchieveConfig.h"
#include "Config.h"
//#include "MouseHandling.h"
#include "PlayOneGraphic.h"
#include "PlayTenGraphic.h"
#include "WinGraphic.h"
#include "Win5Graphic.h"
#include "ShootGraphic.h"
#include "NoPointsGraphic.h"
#include "Defines.h"

#define ACHIEVEMENT_ADJUST_AMOUNT      (75)

void AdjustAchievementImage(SDL_Surface* pSurface, bool bAchieved)
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
         if( r == 0 && g == 0 && b == 0 ) continue;
         if( bAchieved ) {
                 r = Puz_Min(255, r+ACHIEVEMENT_ADJUST_AMOUNT);
                 g = Puz_Min(255, g+ACHIEVEMENT_ADJUST_AMOUNT);
                 b = Puz_Min(255, b+ACHIEVEMENT_ADJUST_AMOUNT);
        }
        else {
                r = Puz_Max(0, r-ACHIEVEMENT_ADJUST_AMOUNT);
                g = Puz_Max(0, g-ACHIEVEMENT_ADJUST_AMOUNT);
                b = Puz_Max(0, b-ACHIEVEMENT_ADJUST_AMOUNT);
        }

         nSDL_SetPixel(pSurface, x, y, SDL_MapRGB(pSurface->format, r, g, b));
      }
   }

   //Unlock surface 
   if( SDL_MUSTLOCK( pSurface ) )
   {
      SDL_UnlockSurface( pSurface );
   }
}

SDL_Surface* LoadImage(Uint16 *data, bool bColorBlack = true)
{
   SDL_Surface* pSurface = nSDL_LoadImage(data);
   if( bColorBlack )
      SDL_SetColorKey(pSurface, SDL_SRCCOLORKEY, SDL_MapRGB(pSurface->format, 0, 0, 0));
   else
      SDL_SetColorKey(pSurface, SDL_SRCCOLORKEY, SDL_MapRGB(pSurface->format, 255, 255, 255));
   return pSurface;
}

Achievements::Achievements(SDL_Surface* pScreen, AchieveConfig* pAchieveConfig/*, MouseHandling* pMouse*/)
: m_pScreen(pScreen), m_eChoice(PlayedAGame), m_pAchieveConfig(pAchieveConfig), /*m_pMouse(pMouse),*/ m_Progress(pScreen, 40, 190, SCREEN_WIDTH-40-40, 15)
{
	m_pAchieveConfig->ResetNewAchievements();

	m_pFont = nSDL_LoadFont(NSDL_FONT_THIN, 0, 0, 0);
	nSDL_SetFontSpacing(m_pFont, 0, 3);

	m_pFontRed = nSDL_LoadFont(NSDL_FONT_THIN, 255, 0, 0);

	m_imgPlayedAGame = LoadImage(image_Play1);
	m_imgPlayed10Games = LoadImage(image_Play10);
	m_imgWonAGame = LoadImage(image_Win);
	m_imgWon5Games = LoadImage(image_Win5_2);
	m_imgShotTheMoon = LoadImage(image_shoot, false);
	m_imgWonWith0Points = LoadImage(image_Zero);

	m_imgDkPlayedAGame = LoadImage(image_Play1);
	m_imgDkPlayed10Games = LoadImage(image_Play10);
	m_imgDkWonAGame = LoadImage(image_Win);
        m_imgDkWon5Games = LoadImage(image_Win5_2);
        m_imgDkShotTheMoon = LoadImage(image_shoot, false);
        m_imgDkWonWith0Points = LoadImage(image_Zero);
	/*AdjustAchievementImage(m_imgDkPlayedAGame, false);
	AdjustAchievementImage(m_imgDkPlayed10Games, false);
	AdjustAchievementImage(m_imgDkWonAGame, false);
	AdjustAchievementImage(m_imgDkWon5Games, false);
	AdjustAchievementImage(m_imgDkShotTheMoon, false);
	AdjustAchievementImage(m_imgDkWonWith0Points, false);*/

	UpdateDoneAmounts();
}

Achievements::~Achievements()
{
	nSDL_FreeFont(m_pFont);

	SDL_FreeSurface(m_imgPlayedAGame);
	SDL_FreeSurface(m_imgPlayed10Games);
	SDL_FreeSurface(m_imgWonAGame);
	SDL_FreeSurface(m_imgWon5Games);
	SDL_FreeSurface(m_imgShotTheMoon);
	SDL_FreeSurface(m_imgWonWith0Points);

	SDL_FreeSurface(m_imgDkPlayedAGame);
	SDL_FreeSurface(m_imgDkPlayed10Games);
	SDL_FreeSurface(m_imgDkWonAGame);
        SDL_FreeSurface(m_imgDkWon5Games);
        SDL_FreeSurface(m_imgDkShotTheMoon);
        SDL_FreeSurface(m_imgDkWonWith0Points);
}

bool Achievements::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();
	
	return true;
}

bool Achievements::PollEvents()
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
						return false;
					break;

					case SDLK_SPACE:
                                        case SDLK_RETURN:
                                        case SDLK_LCTRL:
                                                break;

					case SDLK_UP:
                                        case SDLK_8:
						Move(A_Up);
                                                break;

                                        case SDLK_DOWN:
                                        case SDLK_2:
						Move(A_Down);
                                                break;

					case SDLK_LEFT:
					case SDLK_4:
						Move(A_Left);
						break;

					case SDLK_RIGHT:
					case SDLK_6:
						Move(A_Right);
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

	/*int nMX = -1, nMY = -1;
        if( m_pMouse->PollMouse(nMX, nMY) ) {
                MouseButton eMouseButton = m_pMouse->GetMouseButton();
                if( eMouseButton == LeftButton ) {
                    Move(A_Left);
                }
                else if( eMouseButton == RightButton ) {
                    Move(A_Right);
                }

//                if( nMX != -1 && nMY != -1 )
//                        Move(nMX, nMY);
        }*/

	return true;
}

void Achievements::Move(Achievement_Direction eDirection)
{
	if( m_eChoice == None ) {
		m_eChoice = PlayedAGame;
		goto Exit;
	}
	if( eDirection == A_Up ) {
		if( m_eChoice == ShotTheMoon ) {
			m_eChoice = PlayedAGame;
		}
		else if( m_eChoice == WonWith0Points ) {
			m_eChoice = Played10Games;
		}
	}
	else if( eDirection == A_Down ) {
		if( m_eChoice == PlayedAGame ) {
			m_eChoice = ShotTheMoon;
		}
		else {
			m_eChoice = WonWith0Points;
		}
	}
	else if( eDirection == A_Left ) {
		if( m_eChoice == Played10Games ) {
			m_eChoice = PlayedAGame;
		}
		else if( m_eChoice == WonAGame ) {
			m_eChoice = Played10Games;
		}
		else if( m_eChoice == Won5Games ) {
			m_eChoice = WonAGame;
		}
		else if( m_eChoice == ShotTheMoon ) {
			m_eChoice = Won5Games;
		}
		else if( m_eChoice == WonWith0Points ) {
			m_eChoice = ShotTheMoon;
		}
	}
	else if( eDirection == A_Right ) {
		if( m_eChoice == PlayedAGame ) {
			m_eChoice = Played10Games;
		}
		else if( m_eChoice == Played10Games ) {
			m_eChoice = WonAGame;
		}
		else if( m_eChoice == WonAGame ) {
			m_eChoice = Won5Games;
		}
		else if( m_eChoice == Won5Games ) {
			m_eChoice = ShotTheMoon;
		}
		else if( m_eChoice == ShotTheMoon ) {
			m_eChoice = WonWith0Points;
		}
	}

Exit:
	UpdateDoneAmounts();
}

void Achievements::UpdateDoneAmounts()
{
	Config* pConfig = m_pAchieveConfig->GetConfig();
	switch(m_eChoice) {
	default:
	case None:
		printf("None\n");
		m_Progress.SetDoneAmount(0,0);
		break;
	case PlayedAGame:
		m_Progress.SetDoneAmount(Puz_Min(1, pConfig->GetGamesPlayed()), 1);
		break;
	case Played10Games:
		m_Progress.SetDoneAmount(Puz_Min(10, pConfig->GetGamesPlayed()), 10);
		break;
	case WonAGame:
		m_Progress.SetDoneAmount(Puz_Min(1, pConfig->GetGamesWon()), 1);
		break;
	case Won5Games:
		m_Progress.SetDoneAmount(Puz_Min(5, pConfig->GetGamesWon()), 5);
		break;
	case ShotTheMoon:
		m_Progress.SetDoneAmount(Puz_Min(1, pConfig->GetTimesShotMoon()), 1);
		break;
	case WonWith0Points:
		m_Progress.SetDoneAmount(Puz_Min(1, pConfig->GetWonWith0Points()), 1);
		break;
	}
}

void Achievements::UpdateDisplay()
{
	//Draw background
	SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, 153, 153, 255/*I don't know a better color :( */));
	nSDL_DrawString(m_pScreen, m_pFont, (SCREEN_WIDTH-nSDL_GetStringWidth(m_pFont, "Achievements:"))/2, 15, "Achievements:");

	SDL_Rect rect;
	rect.w = 32;
	rect.h = 32;

	rect.x = 30;
        rect.y = 30;
        SDL_BlitSurface(m_pAchieveConfig->PlayedAGame() ? m_imgPlayedAGame : m_imgDkPlayedAGame, NULL, m_pScreen, &rect);

        rect.x = 80;
        rect.y = 30;
        SDL_BlitSurface(m_pAchieveConfig->Played10Games() ? m_imgPlayed10Games : m_imgDkPlayed10Games, NULL, m_pScreen, &rect);

	rect.x = 130;
        rect.y = 30;
	SDL_BlitSurface(m_pAchieveConfig->WonAGame() ? m_imgWonAGame : m_imgDkWonAGame, NULL, m_pScreen, &rect);

	rect.x = 180;
        rect.y = 30;
	SDL_BlitSurface(m_pAchieveConfig->Won5Games() ? m_imgWon5Games : m_imgDkWon5Games, NULL, m_pScreen, &rect);

	rect.x = 30;
        rect.y = 80;
        SDL_BlitSurface(m_pAchieveConfig->ShotTheMoon() ? m_imgShotTheMoon : m_imgDkShotTheMoon, NULL, m_pScreen, &rect);

	rect.x = 80;
        rect.y = 80;
        SDL_BlitSurface(m_pAchieveConfig->WonWith0Points() ? m_imgWonWith0Points : m_imgDkWonWith0Points, NULL, m_pScreen, &rect);

	char buffer[256];

	int nTop = 120;
	int nLeft = 12;
	nSDL_DrawString(m_pScreen, m_pFont, nLeft, nTop, "Description:");

	bool bAchieved = false;
	if( m_eChoice == PlayedAGame ) {
		strcpy(buffer, "Play a game:\n\
Simply play 1 game!");
                draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 30, 30, 32, 32, 1);
                bAchieved = m_pAchieveConfig->PlayedAGame();
        }
	else if( m_eChoice == Played10Games ) {
                strcpy(buffer, "Play 10 games:\n\
Play at least 10 games!");
                draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 80, 30, 32, 32, 1);
                bAchieved = m_pAchieveConfig->Played10Games();
        }
	else if( m_eChoice == WonAGame ) {
		strcpy(buffer, "Win a game:\n\
Simply win 1 game!");
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 130, 30, 32, 32, 1);
		bAchieved = m_pAchieveConfig->WonAGame();
	}
	else if( m_eChoice == Won5Games ) {
		strcpy(buffer, "Win 5 games:\n\
Win at least 5 games!");
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 180, 30, 32, 32, 1);
		bAchieved = m_pAchieveConfig->Won5Games();
	}
	else if( m_eChoice == ShotTheMoon ) {
                strcpy(buffer, "Shoot the Moon:\n\
Take all Hearts and the Queen of Spades\n\
in one hand and you shoot the moon!");
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 30, 80, 32, 32, 1);
		bAchieved = m_pAchieveConfig->ShotTheMoon();
	}
	else if( m_eChoice == WonWith0Points ) {
                strcpy(buffer, "Win with 0 points:\n\
Win a game where you don't have a single\n\
point!  (Shooting the moon is ok)");
		draw_rectangle(m_pScreen, SDL_MapRGB(m_pScreen->format, 255, 0, 0), 80, 80, 32, 32, 1);
		bAchieved = m_pAchieveConfig->WonWith0Points();
	}
        nSDL_DrawString(m_pScreen, m_pFont, nLeft, nTop + 20, buffer);

	if( bAchieved ) {
		strcpy(buffer, "Achievement completed!");
		nSDL_DrawString(m_pScreen, m_pFontRed, SCREEN_WIDTH/2-nSDL_GetStringWidth(m_pFontRed, buffer)/2, nTop + 54, buffer);
	}
	else {
		strcpy(buffer, "Not yet achieved!");
		nSDL_DrawString(m_pScreen, m_pFontRed, SCREEN_WIDTH/2-nSDL_GetStringWidth(m_pFontRed, buffer)/2, nTop + 54, buffer);
	}

	m_Progress.UpdateDisplay();

	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

}




