# Find OIS includes and library
#
# This module defines
#  OIS_INCLUDE_DIRS
#  OIS_LIBRARIES, the libraries to link against to use OIS.
#  OIS_FOUND, If false, do not try to use OIS

IF (OIS_LIBRARIES AND OIS_INCLUDE_DIRS)
	SET(OIS_FIND_QUIETLY TRUE) # Already in cache, be silent
ELSEIF (NOT OIS_FIND_QUIETLY)
	MESSAGE(STATUS "Looking for OIS")
ENDIF ()

SET(OIS_INCLUDE_SEARCH_DIRS
	${OIS_INCLUDE_SEARCH_DIRS}
	${CMAKE_LIBRARY_PATH}
	/usr/include
	/usr/local/include
	/opt/include
	/opt/OIS/include
)

SET(OIS_LIBRARY_SEARCH_DIRS
	${OIS_LIBRARY_SEARCH_DIRS}
	${CMAKE_LIBRARY_PATH}
	/usr/lib
	/usr/local/lib
	/opt/lib
	/opt/CEGUI/lib
)

FIND_PATH(OIS_INCLUDE_DIRS OIS.h PATHS ${OIS_INCLUDE_SEARCH_DIRS} PATH_SUFFIXES "OIS")
FIND_LIBRARY(OIS_LIBRARY_REL OIS PATHS ${OIS_LIBRARY_SEARCH_DIRS})
FIND_LIBRARY(OIS_LIBRARY_DBG OIS_d PATHS ${OIS_LIBRARY_SEARCH_DIRS})
if (OIS_LIBRARY_REL AND OIS_LIBRARY_DBG)
  set(OIS_LIBRARIES optimized ${OIS_LIBRARY_REL} debug ${OIS_LIBRARY_DBG})
elseif (OIS_LIBRARY_REL)
  set(OIS_LIBRARIES ${OIS_LIBRARY_REL})
elseif (OIS_LIBRARY_DBG)
  set(OIS_LIBRARIES ${OIS_LIBRARY_DBG})
endif ()  


MARK_AS_ADVANCED(OIS_INCLUDE_DIRS OIS_LIBRARIES OIS_LIBRARY_REL OIS_LIBRARY_DBG)

IF (OIS_INCLUDE_DIRS AND OIS_LIBRARIES)
	SET(OIS_FOUND TRUE)
ENDIF ()

IF (OIS_FOUND)
	IF (NOT OIS_FIND_QUIETLY)
		MESSAGE(STATUS "  libraries : ${OIS_LIBRARIES}")
		MESSAGE(STATUS "  includes  : ${OIS_INCLUDE_DIRS}")
	ENDIF ()
ELSE ()
	IF (OIS_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find OIS")
	ENDIF ()
ENDIF ()