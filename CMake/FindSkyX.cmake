if (NOT "$ENV{SKYX_HOME}" STREQUAL "")
	set (SKYX_DIR $ENV{SKYX_HOME})
endif()

find_path(SKYX_INCLUDE_DIRS SkyX.h PATHS ${SKYX_DIR}/include/SKYX)
set(SKYX_LIBRARY_DIRS ${SKYX_DIR}/lib)

find_library(SKYX_LIBRARY_RELEASE SkyX HINTS ${SKYX_LIBRARY_DIRS} ${SKYX_LIBRARY_DIRS}/release)
find_library(SKYX_LIBRARY_DEBUG SkyX_d SkyX HINTS ${SKYX_LIBRARY_DIRS} ${SKYX_LIBRARY_DIRS}/debug)
set(SKYX_LIBRARIES optimized ${SKYX_LIBRARY_RELEASE}
	 debug ${SKYX_LIBRARY_DEBUG})

if (WIN32)
	  set(SKYX_BINARY_DIRS ${SKYX_DIR}/bin ${SKYX_DIR}/bin/release ${SKYX_DIR}/bin/debug)
	  find_file(SKYX_BINARY_REL NAMES "SkyX.dll" HINTS ${SKYX_BINARY_DIRS})
	  find_file(SKYX_BINARY_DBG NAMES "SkyX_d.dll" HINTS ${SKYX_BINARY_DIRS})
endif()
