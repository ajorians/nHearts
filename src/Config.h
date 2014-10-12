#ifndef CONFIG_H
#define CONFIG_H

extern "C"
{
	#include <os.h>
	#include "ArchiveLib/ArchiveLib.h"
}

#define MAKE_CONFIG_ITEM(type, name) \
public: \
   type Get##name() const { return m_##name; }\
   void Set##name(type val) { m_##name = val; }\
protected: \
   type m_##name;

class Config
{
public:
   Config();
   ~Config();

   MAKE_CONFIG_ITEM(bool, BackgroundMoves);

public:

protected:
   ArchiveLib	m_Archive;
};

#endif

