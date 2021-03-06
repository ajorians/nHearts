PROJECT (Tests)

cmake_minimum_required(VERSION 2.6)

# This is in case we ever need to know if we were creating a VS or Xcode project
IF(CMAKE_GENERATOR MATCHES "Visual Studio 8")
  SET(GENERATING_IDE_MAKEFILE 1)
ELSEIF(CMAKE_GENERATOR MATCHES "Xcode")
  SET(GENERATING_IDE_MAKEFILE 1)
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio 8")

IF(WIN32)
  ADD_DEFINITIONS( -D "_UNICODE")
  SET( CMAKE_MFC_FLAG 0 )
ENDIF(WIN32)

FILE(GLOB HEADERS *.h)
FILE(GLOB SOURCE_FILES *.c)

SOURCE_GROUP(Source FILES ${SOURCE_FILES})
SOURCE_GROUP(Headers FILES ${HEADERS})

SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
ADD_EXECUTABLE(Tests ${SOURCE_FILES} ${HEADERS})
TARGET_LINK_LIBRARIES(Tests CardLib HeartsLib)
INCLUDE_DIRECTORIES( ../include )

IF(WIN32)
   SET_TARGET_PROPERTIES(Tests PROPERTIES COMPILE_FLAGS /Zc:wchar_t-)
ENDIF(WIN32)
