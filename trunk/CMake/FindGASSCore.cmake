
if (NOT "$ENV{GASS_HOME}" STREQUAL "")
	set (GASS_DIR $ENV{GASS_HOME} CACHE PATH "GASS home")
endif()

#folder holding pre-compiled dependencies
set(GASS_DEPENDENCIES_DIR $ENV{GASS_DEPENDENCIES} CACHE PATH "3rd-party dependency folder")

find_package(GASSCoreThirdParty)

set(GASS_CORE_INCLUDE_DIRS ${GASS_DIR}/include ${GASS_DIR}/source ${Boost_INCLUDE_DIRS}  ${TINYXML2_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS})
set(GASS_LIBRARY_DIRS ${GASS_DIR}/lib ${GASS_DIR}/lib/debug ${GASS_DIR}/lib/release)
set(GASS_BINARY_DIRS ${GASS_DIR}/bin ${GASS_DIR}/bin/debug ${GASS_DIR}/bin/release)

find_library(GASS_CORE_LIBRARY_RELEASE GASSCore HINTS ${GASS_LIBRARY_DIRS})
find_library(GASS_CORE_LIBRARY_DEBUG GASSCore_d HINTS ${GASS_LIBRARY_DIRS})

set(GASS_CORE_LIBRARIES optimized ${GASS_CORE_LIBRARY_RELEASE}
	 debug ${GASS_CORE_LIBRARY_DEBUG}
	 ${Boost_LIBRARIES}
	 ${TINYXML2_LIBRARIES}
	 ${TBB_LIBRARIES})

if (WIN32) #find dlls 
	  find_file(GASS_CORE_BINARY_RELEASE NAMES GASSCore.dll HINTS ${GASS_BINARY_DIRS})
	  find_file(GASS_CORE_BINARY_DEBUG NAMES GASSCore_d.dll HINTS ${GASS_BINARY_DIRS})
	  
	   #TBB
	   find_package(TBB)
	   
	   #Set GASSSim out binaries
	   set(GASS_CORE_BINARIES_REL ${GASS_CORE_BINARY_RELEASE}
		${TBB_BINARY_REL})
	
	   set(GASS_CORE_BINARIES_DBG ${GASS_CORE_BINARY_DEBUG}
		${TBB_BINARY_DBG})
endif()
