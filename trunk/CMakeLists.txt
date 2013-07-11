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

message( STATUS "CMAKE_CL_64: ${CMAKE_CL_64}")

set(CMAKE_DEBUG_POSTFIX  "_d" CACHE STRING "add a postfix, usually d on windows")
set(GASS_INSTALL_LIB_DIR "lib")
set(GASS_INSTALL_PLUGIN_LIB_DIR  "lib")
set(GASS_INSTALL_INCLUDE_DIR  "include")
set(GASS_INSTALL_SAMPLES_DIR  "samples")

set(GASS_COMMON_INCLUDE_DIRS "${BOOST_DIR}"  "${PROJECT_SOURCE_DIR}/Source" 	"${GASS_DEPENDENCIES_DIR}/tinyxml"	"${GASS_DEPENDENCIES_DIR}/tbb/include")
set(GASS_COMMON_LIB_DIRS "${BOOST_DIR}/lib/" "${GASS_DEPENDENCIES_DIR}/lib/")
set(GASS_COMMON_LINK_LIBRARIES optimized tinyxml debug tinyxml_d)
	
add_subdirectory( Source )

option( BUILD_SAMPLES "Build Samples." ON )
if(BUILD_SAMPLES)
	add_subdirectory( Samples )
endif()