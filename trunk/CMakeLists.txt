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

if(EXISTS "${GASS_DEPENDENCIES_DIR}/boost-v1.54")
	set(BOOST_DIR  "${GASS_DEPENDENCIES_DIR}/boost-v1.54" CACHE PATH "BOOST folder")
else()
	set(BOOST_DIR $ENV{BOOST_HOME} CACHE PATH "BOOST folder")
endif()

message( STATUS "CMAKE_CL_64: ${CMAKE_CL_64}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) #static libs
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) #dynnamic libs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) #dlls and exe

set(CMAKE_DEBUG_POSTFIX  "_d" CACHE STRING "add a postfix, usually d on windows")

set(GASS_INSTALL_LIB_DIR_DEBUG "lib/debug")
set(GASS_INSTALL_BIN_DIR_DEBUG "bin/debug")

set(GASS_INSTALL_LIB_DIR_RELEASE "lib/release")
set(GASS_INSTALL_BIN_DIR_RELEASE "bin/release")

#set(GASS_PLUGIN_INSTALL_LIB_DIR_DEBUG "${GASS_INSTALL_LIB_DIR_DEBUG}/plugins")
#set(GASS_PLUGIN_INSTALL_BIN_DIR_DEBUG "${GASS_INSTALL_BIN_DIR_DEBUG}/plugins")

#set(GASS_PLUGIN_INSTALL_LIB_DIR_RELEASE "${GASS_INSTALL_LIB_DIR_RELEASE}/plugins")
#set(GASS_PLUGIN_INSTALL_BIN_DIR_RELEASE "${GASS_INSTALL_BIN_DIR_RELEASE}/plugins")

set(GASS_PLUGIN_INSTALL_LIB_DIR_DEBUG "${GASS_INSTALL_LIB_DIR_DEBUG}")
set(GASS_PLUGIN_INSTALL_BIN_DIR_DEBUG "${GASS_INSTALL_BIN_DIR_DEBUG}")

set(GASS_PLUGIN_INSTALL_LIB_DIR_RELEASE "${GASS_INSTALL_LIB_DIR_RELEASE}")
set(GASS_PLUGIN_INSTALL_BIN_DIR_RELEASE "${GASS_INSTALL_BIN_DIR_RELEASE}")

set(GASS_INSTALL_PLUGIN_LIB_DIR  "lib")
set(GASS_INSTALL_INCLUDE_DIR  "include")
set(GASS_INSTALL_SAMPLES_DIR  "samples")

set(GASS_COMMON_INCLUDE_DIRS "${BOOST_DIR}"  "${PROJECT_SOURCE_DIR}/Source" 	"${GASS_DEPENDENCIES_DIR}/tinyxml"	"${GASS_DEPENDENCIES_DIR}/tbb/include")
set(GASS_COMMON_LIB_DIRS "${BOOST_DIR}/lib/" "${GASS_DEPENDENCIES_DIR}/lib/")
set(GASS_COMMON_LINK_LIBRARIES optimized tinyxml debug tinyxml_d)
#set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")

add_subdirectory( Source )

option( BUILD_SAMPLES "Build Samples." ON )
if(BUILD_SAMPLES)
	add_subdirectory( Samples )
endif()


#-- Add an Option to toggle the generation of the API documentation
option(BUILD_DOCUMENTATION "Use Doxygen to create the HTML based API documentation" ON)
if(BUILD_DOCUMENTATION)
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