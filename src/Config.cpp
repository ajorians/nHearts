#include "Config.h"
#include "Defines.h"

#define SCORE_LIMIT_DEFAULT        (100)
#define JACK_DIAMONDS_DEFAULT      (0)
#define PIECE_STEP_DEFAULT	   (7)
#define SHOW_ALL_CARDS_DEFAULT	   (0)

#define GAMES_PLAYED_DEFAULT		(0)
#define GAMES_WON_DEFAULT		(0)
#define TIMES_SHOT_MOON_DEFAULT		(0)
#define WON_WITH_0_POINTS_DEFAULT	(0)

#define STRINGIFY_CONFIG_ITEM(x) #x

#define READ_BOOL_CONFIG_VAR(identifier) if( strcmp(strName, STRINGIFY_CONFIG_ITEM(Use##identifier) ) == 0 ) {\
   if( strcmp("0", GetValue(m_Archive, "Settings", i)) == 0 ) {\
      m_##identifier = false;\
   } else {\
      m_##identifier = true;\
  }\
}

#define WRITE_BOOL_CONFIG_VAR(identifier) UpdateArchiveEntry(m_Archive, "Settings", STRINGIFY_CONFIG_ITEM(Use##identifier), m_##identifier ? "1" : "0", NULL);

#define READ_INT_CONFIG_VAR(identifier) if( strcmp(strName, STRINGIFY_CONFIG_ITEM(Use##identifier) ) == 0 ) {\
   m_##identifier = atoi( GetValue(m_Archive, "Settings", i));\
}

#define WRITE_INT_CONFIG_VAR(identifier) sprintf(buffer, "%d", m_##identifier);\
   UpdateArchiveEntry(m_Archive, "Settings", STRINGIFY_CONFIG_ITEM(Use##identifier), buffer, NULL);

Config::Config()
: m_ScoreLimit(SCORE_LIMIT_DEFAULT), m_JackDiamondsAmount(JACK_DIAMONDS_DEFAULT), m_PieceMovePerStep(PIECE_STEP_DEFAULT), m_ShowAllCards(SHOW_ALL_CARDS_DEFAULT), m_GamesPlayed(GAMES_PLAYED_DEFAULT), m_GamesWon(GAMES_WON_DEFAULT), m_TimesShotMoon(TIMES_SHOT_MOON_DEFAULT), m_WonWith0Points(WON_WITH_0_POINTS_DEFAULT)
{
   ArchiveCreate(&m_Archive);

   OpenArchiveFile(m_Archive, "HeartsSettings.tns");

   int nSettings = GetNumberArchives(m_Archive, "Settings");
   char strName[MAX_NAME_LENGTH];
   for(int i=0; i<nSettings; i++) {
      strcpy(strName, GetName(m_Archive, "Settings", i));

      READ_INT_CONFIG_VAR(ScoreLimit);
      READ_INT_CONFIG_VAR(JackDiamondsAmount);
      READ_INT_CONFIG_VAR(PieceMovePerStep);
      READ_INT_CONFIG_VAR(ShowAllCards);

//Achievements
      READ_INT_CONFIG_VAR(GamesPlayed);
      READ_INT_CONFIG_VAR(GamesWon);
      READ_INT_CONFIG_VAR(TimesShotMoon);
      READ_INT_CONFIG_VAR(WonWith0Points);
   }
}

Config::~Config()
{
   char buffer[8];
   ArchiveSetBatchMode(m_Archive, ARCHIVE_ENABLE_BATCH);
   WRITE_INT_CONFIG_VAR(ScoreLimit);
   WRITE_INT_CONFIG_VAR(JackDiamondsAmount);
   WRITE_INT_CONFIG_VAR(PieceMovePerStep);
   WRITE_INT_CONFIG_VAR(ShowAllCards);

//Achievements
   WRITE_INT_CONFIG_VAR(GamesPlayed);
   WRITE_INT_CONFIG_VAR(GamesWon);
   WRITE_INT_CONFIG_VAR(TimesShotMoon);
   WRITE_INT_CONFIG_VAR(WonWith0Points);
   ArchiveSetBatchMode(m_Archive, ARCHIVE_DISABLE_BATCH);

   ArchiveFree(&m_Archive);
}


