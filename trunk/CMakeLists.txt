cmake_minimum_required (VERSION 2.6)
project (GASS)

# The version number.
set (GASS_VERSION_MAJOR 1)
set (GASS_VERSION_MINOR 0)

set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
  )
include (Common)

set(GASS_DEPENDENCIES_DIR $ENV{GASS_DEPENDENCIES} CACHE PATH "GASS 3rd-party dependency folder")

if(EXISTS "${GASS_DEPENDENCIES_DIR}/boost")
	set(BOOST_DIR  "${GASS_DEPENDENCIES_DIR}/boost" CACHE PATH "BOOST folder")
else()
	set(BOOST_DIR $ENV{BOOST_HOME} CACHE PATH "BOOST folder")
endif()

SET(CMAKE_DEBUG_POSTFIX  "_d" CACHE STRING "add a postfix, usually d on windows")

add_subdirectory( Source )
