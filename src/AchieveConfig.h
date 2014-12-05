#ifndef ACHIEVECONFIG_H
#define ACHIEVECONFIG_H

extern "C"
{
	#include <os.h>
	#include "HeartsLib/HeartsLib.h"
}

class Config;

class AchieveConfig
{
public:
   AchieveConfig(Config* pConfig);
   ~AchieveConfig();

   bool PlayedAGame() const;
   bool Played10Games() const;
   bool WonAGame() const;
   bool Won5Games() const;
   bool ShotTheMoon() const;
   bool WonWith0Points() const;

   bool LookForAchievements(HeartsLib hearts);

   Config* GetConfig() { return m_pConfig; }

   bool GetNewAchievements() const;
   void ResetNewAchievements();

protected:
   Config*	m_pConfig;
   bool		m_bNewAchievement;
};

#endif

