#FindOSGEarth extention that add binary lists
#This file will call FindOSGEarth that will get you libraries and include folders
#Output from this file is debug and release binaries (including thirdparty binaries)

#Output vars
#OSGEARTH_BINARIES_REL : release binaries 
#OSGEARTH_PLUGINS_BINARIES_REL : release plugin-binaries: 
#OSGEARTH_BINARIES_DBG : debug binaries 
#OSGEARTH_PLUGINS_BINARIES_DBG : debug plugin-binaries: 

#Input vars
#OSGEARTH_ROOT,OSGEARTHDIR,OSGEARTH_DIR or OSGEARTHHOME: Environment var holding path to osgEarth
#OSG_DIR, OSGDIR. OSG_HOME or OSG_ROOT : Environment var holding path to osg

find_package(OSGEarth)

macro(extract_version var str)
STRING(REGEX REPLACE ".*VERSION\ *([0-9]+)" "\\1" ${var} ${str})
endmacro(extract_version)

#find osg version
find_path(OSG_DIR "include/osg/Version" HINTS $ENV{OSGDIR} $ENV{OSG_DIR} $ENV{OSG_ROOT} $ENV{OSG_HOME} DOC "OpenSceneGraph install path")
if(${OSG_DIR} STREQUAL "OSG_DIR-NOTFOUND")
	message("Please set OSG_DIR")
else() 
	file(STRINGS "${OSG_DIR}/include/osg/Version" OSG_MAJOR REGEX "#define OPENSCENEGRAPH_MAJOR_VERSION")
	file(STRINGS "${OSG_DIR}/include/osg/Version" OSG_MINOR REGEX "#define OPENSCENEGRAPH_MINOR_VERSION")
	file(STRINGS "${OSG_DIR}/include/osg/Version" OSG_PATCH REGEX "#define OPENSCENEGRAPH_PATCH_VERSION")
	file(STRINGS "${OSG_DIR}/include/osg/Version" OSG_SOVERSION REGEX "#define OPENSCENEGRAPH_SOVERSION")
	extract_version(OSG_MAJOR ${OSG_MAJOR})
	extract_version(OSG_MINOR ${OSG_MINOR})
	extract_version(OSG_PATCH ${OSG_PATCH})
	extract_version(OSG_SOVERSION ${OSG_SOVERSION})
	set(OSG_VERSION "${OSG_MAJOR}.${OSG_MINOR}.${OSG_PATCH}" CACHE STRING "OpenSceneGraph Version")
	set(OSG_SHARED_PREFIX "osg${OSG_SOVERSION}-" CACHE STRING "OpenSceneGraph so-version")
endif()

#find osgEarth version
find_path(OSGEARTH_DIR "include/osgEarth/Version" HINTS $ENV{OSGEARTH_DIR} $ENV{OSGEARTHDIR} $ENV{OSGEARTH_ROOT} $ENV{OSGEARTH_HOME} DOC "OSGEarth install path")
set(OSGEARTH_BINARY_DIR ${OSGEARTH_DIR}/bin ${OSGEARTH_DIR}/debug/bin)
if(${OSGEARTH_DIR} STREQUAL "OSGEARTH_DIR-NOTFOUND")
	message("OSGEarth dir not found")
else() 
	file(STRINGS "${OSGEARTH_DIR}/include/osgEarth/Version" OSGEARTH_MAJOR REGEX "#define OSGEARTH_MAJOR_VERSION")
	file(STRINGS "${OSGEARTH_DIR}/include/osgEarth/Version" OSGEARTH_MINOR REGEX "#define OSGEARTH_MINOR_VERSION")
	file(STRINGS "${OSGEARTH_DIR}/include/osgEarth/Version" OSGEARTH_PATCH REGEX "#define OSGEARTH_PATCH_VERSION")
	extract_version(OSGEARTH_MAJOR ${OSGEARTH_MAJOR})
	extract_version(OSGEARTH_MINOR ${OSGEARTH_MINOR})
	extract_version(OSGEARTH_PATCH ${OSGEARTH_PATCH})
	set(OSGEARTH_VERSION "${OSGEARTH_MAJOR}.${OSGEARTH_MINOR}.${OSGEARTH_PATCH}" CACHE STRING "OSGEarth Version")
endif()

if(OSGEARTH_FOUND)
	set(OE_TARGETS 
		osgEarth
		osgEarthSilverLining
		osgEarthSplat
		osgEarthTriton
	)

	set(OSGEARTH_ALL_TARGETS)
	foreach(_OE_TARGET ${OE_TARGETS})	
		STRING(TOUPPER ${_OE_TARGET} _OE_TARGET_UPPER)
		set(_TARGET_LIB_REL ${${_OE_TARGET_UPPER}_LIBRARY})
		set(_TARGET_LIB_DBG ${${_OE_TARGET_UPPER}_LIBRARY_DEBUG})
		if(_TARGET_LIB_REL AND _TARGET_LIB_DBG)
			set(_TARGET_NAME osgEarth::${_OE_TARGET})
			list(APPEND OSGEARTH_ALL_TARGETS ${_TARGET_NAME})
			if(NOT TARGET ${_TARGET_NAME})
				add_library(${_TARGET_NAME} UNKNOWN IMPORTED)
				set_target_properties(${_TARGET_NAME} PROPERTIES
					INTERFACE_INCLUDE_DIRECTORIES ${OSGEARTH_INCLUDE_DIR}
					IMPORTED_LOCATION ${_TARGET_LIB_REL}
					IMPORTED_LOCATION_DEBUG ${_TARGET_LIB_DBG})
			endif()
		endif()
	endforeach()
endif()
