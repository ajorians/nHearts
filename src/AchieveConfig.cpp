#include "AchieveConfig.h"
#include "Config.h"
#include "Defines.h"

AchieveConfig::AchieveConfig(Config* pConfig)
: m_pConfig(pConfig)
{
}

AchieveConfig::~AchieveConfig()
{
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

//bool AchieveConfig::LookForAchievements(HeartsLib hearts);
bool AchieveConfig::JustPlayedAGame()
{
   return false;
}

