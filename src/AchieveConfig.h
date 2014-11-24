#ifndef ACHIEVECONFIG_H
#define ACHIEVECONFIG_H

extern "C"
{
	#include <os.h>
}

class Config;

class AchieveConfig
{
public:
   AchieveConfig(Config* pConfig);
   ~AchieveConfig();

   bool WonAGame() const;
   bool Won5Games() const;
   bool ShotTheMoon() const;
   bool WonWith0Points() const;

   //bool LookForAchievements(HeartsLib hearts);
   bool JustPlayedAGame();

   Config* GetConfig() { return m_pConfig; }

protected:
   Config*	m_pConfig;
};

#endif

