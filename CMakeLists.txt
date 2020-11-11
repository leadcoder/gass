cmake_minimum_required(VERSION 3.5)

# The version number.
set(GASS_VERSION_MAJOR 1)
set(GASS_VERSION_MINOR 0)
set(GASS_VERSION_PATCH 0)
set(GASS_VERSION ${GASS_VERSION_MAJOR}.${GASS_VERSION_MINOR}.${GASS_VERSION_PATCH})

project(GASS VERSION ${GASS_VERSION})

set(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/CMake
  ${CMAKE_MODULE_PATH}
  )

include(CMakeDependentOption)
include(Common)
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GASS_GIT_SHA1)

#all options
option(GASS_BUILD_CORE "Build Core" ON)
cmake_dependent_option(GASS_BUILD_CORE_SAMPLES "Build Core Samples" ON "GASS_BUILD_CORE" OFF)
option(GASS_BUILD_SIM "Build GASS Sim" ON )
option(GASS_BUILD_TESTS "Enable tests (using Catch test framework)" OFF)
option(GASS_BUILDTYPE_STATIC "Static Build" OFF)

cmake_dependent_option(GASS_BUILD_PLUGIN_BASE "Build Plugin." ON "GASS_BUILD_SIM" OFF)
cmake_dependent_option(GASS_BUILD_PLUGIN_RECASTNAVIGATION "Build Plugin." ON "GASS_BUILD_SIM" OFF)
cmake_dependent_option(GASS_BUILD_PLUGIN_VEHICLE "Build Plugin." ON "GASS_BUILD_SIM" OFF)
cmake_dependent_option(GASS_BUILD_PLUGIN_INPUT "Build Plugin." ON "GASS_BUILD_SIM" OFF)
cmake_dependent_option(GASS_BUILD_PLUGIN_ROADNETWORK "Build Plugin." ON "GASS_BUILD_SIM" OFF)
cmake_dependent_option(GASS_BUILD_PLUGIN_OSG "Build Plugin." ON "GASS_BUILD_SIM" OFF)
cmake_dependent_option(GASS_BUILD_PLUGIN_ODE "Build Plugin." ON "GASS_BUILD_SIM" OFF)
cmake_dependent_option(GASS_BUILD_PLUGIN_OIS "Build Plugin." ON "GASS_BUILD_SIM" OFF)
cmake_dependent_option(GASS_BUILD_PLUGIN_OPENAL "Build Plugin." ON "GASS_BUILD_SIM" OFF)
cmake_dependent_option(GASS_BUILD_PLUGIN_RAKNET "Build Plugin." ON "GASS_BUILD_SIM" OFF)
cmake_dependent_option(GASS_BUILD_PLUGIN_PHYSX "Build Plugin." ON "GASS_BUILD_SIM" OFF)
cmake_dependent_option(GASS_BUILD_PLUGIN_OSGEARTH "Build Plugin" OFF "GASS_BUILD_SIM;GASS_BUILD_PLUGIN_OSG" OFF)
cmake_dependent_option(GASS_BUILD_SIM_SAMPLES "Build Sim Samples" OFF "GASS_BUILD_SIM" OFF)
cmake_dependent_option(GASS_BUILD_GASSED "Build GASSEd" OFF "GASS_BUILD_SIM" OFF)

if(MSVC)
  option(GASS_FORCE_W4 "Build GASS with warning level 4" ON )
  option(GASS_COMPILE_WITH_MP "Build with Multiple Processes" ON)
  option(GASS_INSTALL_DEP_BINARIES "Install dependency binaries (and copy to build folder) " OFF)
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Enable INSTALL project to be built by default. Inactivated for every configuration otherwise.
set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) #static libs
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) #dynamic libs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) #dlls and exe

set(CMAKE_DEBUG_POSTFIX  "_d" CACHE STRING "add a postfix, usually d on windows")

include(GNUInstallDirs)
set(GASS_INSTALL_LIB_DIR_DEBUG ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Debug lib install path")
set(GASS_INSTALL_BIN_DIR_DEBUG ${CMAKE_INSTALL_BINDIR} CACHE PATH "Debug bin install path")

set(GASS_INSTALL_LIB_DIR_RELEASE ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Release lib install path")
set(GASS_INSTALL_BIN_DIR_RELEASE ${CMAKE_INSTALL_BINDIR} CACHE PATH "Release bin install path")

set(GASS_INSTALL_INCLUDE_DIR  ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "include install path")
set(GASS_INSTALL_PLUGIN_LIB_DIR  "lib")
set(GASS_INSTALL_SAMPLES_DIR  "bin")

if(MSVC AND GASS_FORCE_W4)
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	  # turn on -Wall and -Wextra, but skip  long-long and unused-parameter warnings
	  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-long-long -Wno-unused-parameter")
endif()

if(MSVC AND GASS_COMPILE_WITH_MP)
	add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)
endif()

if(GASS_BUILDTYPE_STATIC)
	set(GASS_BUILDTYPE STATIC)
else()
	set(GASS_BUILDTYPE SHARED)
endif()

include(DepTargets)


enable_testing()
add_subdirectory(source)
add_subdirectory(samples)
add_subdirectory(applications)

if(GASS_INSTALL_DEP_BINARIES)
	include(GASSDependentDLLs)
	gass_find_and_install_dep_binaries(GASS_DEP_BINARIES_REL GASS_DEP_BINARIES_DBG)
endif()