#include "AchieveConfig.h"
#include "Config.h"
#include "Defines.h"

AchieveConfig::AchieveConfig(Config* pConfig)
: m_pConfig(pConfig), m_bNewAchievement(false)
{
}

AchieveConfig::~AchieveConfig()
{
}

bool AchieveConfig::PlayedAGame() const
{
   return m_pConfig->GetGamesPlayed() > 0;
}

bool AchieveConfig::Played10Games() const
{
   return m_pConfig->GetGamesPlayed() >= 10;
}

bool AchieveConfig::WonAGame() const
{
   return m_pConfig->GetGamesWon() > 0;
}

bool AchieveConfig::Won5Games() const
{
   return m_pConfig->GetGamesWon() >= 5;
}

bool AchieveConfig::ShotTheMoon() const
{
   return m_pConfig->GetTimesShotMoon() > 0;
}

bool AchieveConfig::WonWith0Points() const
{
   return m_pConfig->GetWonWith0Points() > 0;
}

bool AchieveConfig::LookForAchievements(HeartsLib hearts)
{
   int nGamesPlayed = m_pConfig->GetGamesPlayed();
   int nGamesWon = m_pConfig->GetGamesWon();
   int TimesShotMoon = m_pConfig->GetTimesShotMoon();
   int TimesWon0Points = m_pConfig->GetWonWith0Points();
   bool bNewAchievement = false;

   if( GetHeartsGameOver(hearts) == HEARTSLIB_GAME_OVER ) {
      int nScore0 = GetHeartsPlayerScore(hearts, 0);
      int nScore1 = GetHeartsPlayerScore(hearts, 1);
      int nScore2 = GetHeartsPlayerScore(hearts, 2);
      int nScore3 = GetHeartsPlayerScore(hearts, 3);
      if( nScore0 < nScore1 && nScore0 < nScore2 && nScore0 < nScore3 ) {
         m_pConfig->SetGamesWon(m_pConfig->GetGamesWon()+1);
         if( nGamesWon==0 || nGamesWon == 4 )
            bNewAchievement = true;
      }

      if( nScore0 == 0 ) {
         m_pConfig->SetWonWith0Points(m_pConfig->GetWonWith0Points()+1);
         if( TimesWon0Points == 0 )
            bNewAchievement = true;
      }

      if( nGamesPlayed == 0 || nGamesPlayed == 9 )
         bNewAchievement = true;
      m_pConfig->SetGamesPlayed(m_pConfig->GetGamesPlayed()+1);
   }

   int nPlayerIndex = -1;
   if( GetPlayerShotMoon(hearts, &nPlayerIndex) == HEARTSLIB_SHOT_THE_MOON ) {
      m_pConfig->SetTimesShotMoon(m_pConfig->GetTimesShotMoon()+1);
      if( TimesShotMoon == 0 )
         bNewAchievement = true;
   }

   //Look for shot the moon, won with 0 points, won game
   if( bNewAchievement )
      m_bNewAchievement = bNewAchievement;
   return bNewAchievement;
}

bool AchieveConfig::GetNewAchievements() const
{
   return m_bNewAchievement;
}

void AchieveConfig::ResetNewAchievements()
{
   m_bNewAchievement = false;
}
