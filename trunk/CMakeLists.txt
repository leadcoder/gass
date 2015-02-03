cmake_minimum_required (VERSION 2.8)
project (GASS)

# The version number.
set (GASS_VERSION_MAJOR 1)
set (GASS_VERSION_MINOR 0)

set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
  )

#folder holding precompiled dependencies
set(GASS_DEPENDENCIES_DIR $ENV{GASS_DEPENDENCIES} CACHE PATH "3rd-party dependency folder")

include(CMakeDependentOption)
include(Common)

#tinyxml2, specific version used in gass, ie avoid system wide search 
set(TINYXML2_DIR "${GASS_DEPENDENCIES_DIR}/tinyxml2" CACHE PATH "tinyxml2 folder")
find_package(TinyXML2 REQUIRED)

#boost
set (Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.57.0 REQUIRED filesystem system)

#angle script, assume that GASS_DEPENDENCIES_DIR hold this lib
set(ANGELSCRIPT_DIR  "${GASS_DEPENDENCIES_DIR}/angelscript" CACHE PATH "Angel Script folder")
find_package(AngelScript REQUIRED)

#TBB
find_package(TBB REQUIRED)
#post-process results 
set(TBB_LIBRARIES optimized ${TBB_LIBRARY}
	 optimized ${TBB_MALLOC_LIBRARY}
	 debug ${TBB_LIBRARY_DEBUG}
	 debug ${TBB_MALLOC_LIBRARY_DEBUG})


set(GASS_CORE_INCLUDE_DIRS ${Boost_INCLUDE_DIR} 
	${TINYXML2_INCLUDE_DIRS}
	${TBB_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/source)

#TODO: remove this and move libraries to dedicated foldres	
set(GASS_CORE_LIB_DIRS ${GASS_DEPENDENCIES_DIR}/lib)
set(GASS_CORE_LINK_LIBRARIES ${Boost_LIBRARIES} ${TINYXML2_LIBRARIES} ${TBB_LIBRARIES})

set(GASS_COMMON_INCLUDE_DIRS ${GASS_CORE_INCLUDE_DIRS} ${ANGELSCRIPT_INCLUDE_DIRS})
set(GASS_COMMON_LIB_DIRS ${GASS_CORE_LIB_DIRS})
set(GASS_COMMON_LINK_LIBRARIES ${GASS_CORE_LINK_LIBRARIES} ${ANGELSCRIPT_LIBRARIES})
	 
	 
#message( STATUS "CMAKE_CL_64: ${CMAKE_CL_64}")



set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) #static libs
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) #dynamic libs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) #dlls and exe

set(CMAKE_DEBUG_POSTFIX  "_d" CACHE STRING "add a postfix, usually d on windows")

set(GASS_INSTALL_LIB_DIR_DEBUG "lib/debug")
set(GASS_INSTALL_BIN_DIR_DEBUG "bin/debug")

set(GASS_INSTALL_LIB_DIR_RELEASE "lib/release")
set(GASS_INSTALL_BIN_DIR_RELEASE "bin/release")

set(GASS_PLUGIN_INSTALL_LIB_DIR_DEBUG "${GASS_INSTALL_LIB_DIR_DEBUG}")
set(GASS_PLUGIN_INSTALL_BIN_DIR_DEBUG "${GASS_INSTALL_BIN_DIR_DEBUG}")

set(GASS_PLUGIN_INSTALL_LIB_DIR_RELEASE "${GASS_INSTALL_LIB_DIR_RELEASE}")
set(GASS_PLUGIN_INSTALL_BIN_DIR_RELEASE "${GASS_INSTALL_BIN_DIR_RELEASE}")

set(GASS_INSTALL_PLUGIN_LIB_DIR  "lib")
set(GASS_INSTALL_INCLUDE_DIR  "include")
set(GASS_INSTALL_SAMPLES_DIR  "bin")

option(GASS_BUILD_SIM "Build GASS Sim" ON )
CMAKE_DEPENDENT_OPTION(GASSPLUGINS_BUILD_OGRE "Build Plugin." ON "GASS_BUILD_SIM" OFF)
CMAKE_DEPENDENT_OPTION(GASSPLUGINS_BUILD_PAGED_GEOMETRY "Build Plugin." ON "GASS_BUILD_SIM;GASSPLUGINS_BUILD_OGRE" OFF)
CMAKE_DEPENDENT_OPTION(GASSPLUGINS_BUILD_ENVIRONMENT "Build Plugin." ON "GASS_BUILD_SIM;GASSPLUGINS_BUILD_OGRE" OFF)
CMAKE_DEPENDENT_OPTION(GASSPLUGINS_BUILD_OSG "Build Plugin." ON "GASS_BUILD_SIM" OFF)
CMAKE_DEPENDENT_OPTION(GASSPLUGINS_BUILD_ODE "Build Plugin." ON "GASS_BUILD_SIM" OFF)
CMAKE_DEPENDENT_OPTION(GASSPLUGINS_BUILD_OIS "Build Plugin." ON "GASS_BUILD_SIM" OFF)
CMAKE_DEPENDENT_OPTION(GASSPLUGINS_BUILD_OPENAL "Build Plugin." ON "GASS_BUILD_SIM" OFF)
CMAKE_DEPENDENT_OPTION(GASSPLUGINS_BUILD_RAKNET "Build Plugin." ON "GASS_BUILD_SIM" OFF)
CMAKE_DEPENDENT_OPTION(GASSPLUGINS_BUILD_PHYSX "Build Plugin." ON "GASS_BUILD_SIM" OFF)
CMAKE_DEPENDENT_OPTION(GASSPLUGINS_BUILD_MYGUI "Build Plugin." OFF "GASS_BUILD_SIM" OFF)
CMAKE_DEPENDENT_OPTION(GASS_BUILD_SAMPLES "Build Samples" OFF "GASS_BUILD_SIM" OFF)
option(GASS_BUILD_DOCUMENTATION "Use Doxygen to create the HTML based API documentation" OFF)

add_subdirectory( source )
if(GASS_BUILD_SAMPLES)
	add_subdirectory( Samples )
endif()

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
install( DIRECTORY CMake/ DESTINATION ${DEPENDENCIES_DIR}/CMake/ PATTERN *.cmake)
# Install xsd files in order to re-use them in other projects
file( GLOB_RECURSE files ${PROJECT_SOURCE_DIR}/source/ "*.xsd")
install( FILES ${files} DESTINATION schema)