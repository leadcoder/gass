cmake_minimum_required (VERSION 3.0)
project(GASS)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
endif()

# if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#    Override to an "Install" directory in the current build-dir
    # message( STATUS "Install-prefix was at default -> forcing it to the build-dir" )
    # message( STATUS "'set( CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/install )'" )
	# set(GASS_INTALL_DIR  ${CMAKE_CURRENT_BINARY_DIR}/install CACHE PATH "SDK install folder (CMAKE_INSTALL_PREFIX replacement)")
    # set( CMAKE_INSTALL_PREFIX ${GASS_INTALL_DIR} CACHE INTERNAL "prefix override")
# endif ( )

# The version number.
set (GASS_VERSION_MAJOR 1)
set (GASS_VERSION_MINOR 0)
set (GASS_VERSION_PATCH 0)
#set (GASS_VERSION ${GASS_VERSION_MAJOR}.${GASS_VERSION_MINOR}.${GASS_VERSION_PATCH})

set(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/CMake
  ${CMAKE_MODULE_PATH}
  )

include(CMakeDependentOption)
include(Common)

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # turn on -Wall and -Wextra, but skip  long-long and unused-parameter warnings
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-long-long -Wno-unused-parameter")
endif()

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GASS_GIT_SHA1)
set(GASS_MAJOR_VERSION 1)
  

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) #static libs
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) #dynamic libs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) #dlls and exe

set(CMAKE_DEBUG_POSTFIX  "_d" CACHE STRING "add a postfix, usually d on windows")

set(GASS_INSTALL_LIB_DIR_DEBUG "lib/debug")
set(GASS_INSTALL_BIN_DIR_DEBUG "bin/debug")

set(GASS_INSTALL_LIB_DIR_RELEASE "lib/release")
set(GASS_INSTALL_BIN_DIR_RELEASE "bin/release")

#set(GASS_PLUGIN_INSTALL_LIB_DIR_DEBUG "${GASS_INSTALL_LIB_DIR_DEBUG}")
#set(GASS_PLUGIN_INSTALL_BIN_DIR_DEBUG "${GASS_INSTALL_BIN_DIR_DEBUG}")

#set(GASS_PLUGIN_INSTALL_LIB_DIR_RELEASE "${GASS_INSTALL_LIB_DIR_RELEASE}")
#set(GASS_PLUGIN_INSTALL_BIN_DIR_RELEASE "${GASS_INSTALL_BIN_DIR_RELEASE}")

set(GASS_INSTALL_PLUGIN_LIB_DIR  "lib")
set(GASS_INSTALL_INCLUDE_DIR  "include")
set(GASS_INSTALL_SAMPLES_DIR  "bin")

option(GASS_BUILDTYPE_STATIC "Static Build" OFF)

if(MSVC)
	option(GASS_INSTALL_DEP_BINARIES "Install dependency binaries (and copy to build folder) " ON)
elseif()
	set(GASS_INSTALL_DEP_BINARIES 0)
endif()

option(GASS_BUILD_CORE_SAMPLES "Build Core Samples" ON)
option(GASS_BUILD_SIM "Build GASS Sim" ON )
CMAKE_DEPENDENT_OPTION(GASS_BUILD_PLUGIN_OGRE "Build Plugin." ON "GASS_BUILD_SIM" OFF)
CMAKE_DEPENDENT_OPTION(GASS_BUILD_PLUGIN_PAGED_GEOMETRY "Build Plugin." ON "GASS_BUILD_SIM;GASS_BUILD_PLUGIN_OGRE" OFF)
CMAKE_DEPENDENT_OPTION(GASS_BUILD_PLUGIN_ENVIRONMENT "Build Plugin." ON "GASS_BUILD_SIM;GASS_BUILD_PLUGIN_OGRE" OFF)
CMAKE_DEPENDENT_OPTION(GASS_BUILD_PLUGIN_OSG "Build Plugin." ON "GASS_BUILD_SIM" OFF)
CMAKE_DEPENDENT_OPTION(GASS_BUILD_PLUGIN_ODE "Build Plugin." ON "GASS_BUILD_SIM" OFF)
CMAKE_DEPENDENT_OPTION(GASS_BUILD_PLUGIN_OIS "Build Plugin." ON "GASS_BUILD_SIM" OFF)
CMAKE_DEPENDENT_OPTION(GASS_BUILD_PLUGIN_OPENAL "Build Plugin." ON "GASS_BUILD_SIM" OFF)
CMAKE_DEPENDENT_OPTION(GASS_BUILD_PLUGIN_RAKNET "Build Plugin." ON "GASS_BUILD_SIM" OFF)
CMAKE_DEPENDENT_OPTION(GASS_BUILD_PLUGIN_PHYSX "Build Plugin." ON "GASS_BUILD_SIM" OFF)
CMAKE_DEPENDENT_OPTION(GASS_BUILD_PLUGIN_MYGUI "Build Plugin." OFF "GASS_BUILD_SIM" OFF)
CMAKE_DEPENDENT_OPTION(GASS_BUILD_PLUGIN_OSGEARTH "Build Plugin" OFF "GASS_BUILD_SIM;GASS_BUILD_PLUGIN_OSG" OFF)
CMAKE_DEPENDENT_OPTION(GASS_BUILD_SIM_SAMPLES "Build Sim Samples" OFF "GASS_BUILD_SIM" OFF)

option(GASS_BUILD_DOCUMENTATION "Use Doxygen to create the HTML based API documentation" OFF)

#boost settings
if(WIN32)
  set (Boost_USE_STATIC_LIBS ON)
else()
  set (Boost_USE_STATIC_LIBS OFF CACHE BOOL "Use static boost libraries")
endif()

if(CMAKE_VERSION VERSION_GREATER "2.8.11") #suppress warnings
  CMAKE_POLICY(SET CMP0022 OLD)
endif()

set(GASS_BUILDTYPE SHARED)
if(GASS_BUILDTYPE_STATIC)
	set(GASS_BUILDTYPE STATIC)
	set(GASS_COMMON_DEFINITIONS GASS_STATIC_BUILD)
endif()

include(DepTargets)

add_subdirectory(source)
add_subdirectory(samples)

#-- Add an Option to toggle the generation of the API documentation
if(GASS_BUILD_DOCUMENTATION)
  FIND_PACKAGE(Doxygen)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR 
      "Doxygen is needed to build the documentation. Please install it correctly")
  endif()
  #-- Configure the Template Doxyfile for our specific project
  configure_file(Doxyfile.in 
                 ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)
  #-- Add a custom target to run Doxygen when ever the project is built
  add_custom_target (Docs ALL 
	COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
  										SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
  # IF you do NOT want the documentation to be generated EVERY time you build the project
  # then leave out the 'ALL' keyword from the above command.
  install(DIRECTORY ${CMAKE_BINARY_DIR}/Documentation/html/ DESTINATION doc)
endif()

# Install CMake files in order to re-use them in other projects
install( DIRECTORY CMake/ DESTINATION CMake/ PATTERN *.cmake)
# Install xsd files in order to re-use them in other projects
#file( GLOB_RECURSE files ${PROJECT_SOURCE_DIR}/source/ "*.xsd")
#install( FILES ${files} DESTINATION schema)

#mkcmakeconfig(GASS 1.5.1)