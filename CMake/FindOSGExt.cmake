#This is an extension of the standard FindOpenSceneGraph find_package
#This extension first call the native version (with all it's commands) 
#and then add cmake targets for all libraries

#The following targets will be created:
#osg::osg
#osg::openthreads
#osg::<lowercase_module_name>

#The following variables will be added:
#OSG_VERSION
#OSG_ALL_TARGETS

set(OSG_MODULES ${OSGExt_FIND_COMPONENTS})

macro(extract_version var str)
STRING(REGEX REPLACE ".*VERSION\ *([0-9]+)" "\\1" ${var} ${str})
endmacro(extract_version)

find_path(OSG_DIR "include/osg/Version" HINTS $ENV{OSG_DIR} $ENV{OSG_ROOT} $ENV{OSG_HOME} DOC "OpenSceneGraph install path")

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

find_package(OpenSceneGraph ${OSG_VERSION} REQUIRED ${OSG_MODULES})

if(OPENSCENEGRAPH_FOUND)
	#create osg target
	if(NOT TARGET osg::osg)
			add_library(osg::osg UNKNOWN IMPORTED)
			set_target_properties(osg::osg PROPERTIES
				INTERFACE_INCLUDE_DIRECTORIES ${OSG_INCLUDE_DIR}
				IMPORTED_LOCATION ${OSG_LIBRARY_RELEASE}
				IMPORTED_LOCATION_DEBUG ${OSG_LIBRARY_DEBUG})
	endif()

	#create openthreads target 
	if(NOT TARGET osg::openthreads)
			add_library(osg::openthreads UNKNOWN IMPORTED)
			set_target_properties(osg::openthreads PROPERTIES
				INTERFACE_INCLUDE_DIRECTORIES ${OPENTHREADS_INCLUDE_DIR}
				IMPORTED_LOCATION ${OPENTHREADS_LIBRARY_RELEASE}
				IMPORTED_LOCATION_DEBUG ${OPENTHREADS_LIBRARY_DEBUG})
	endif()


	#create target for each module/lib
	set(_OSG_ALL_MODULE_TARGETS)
	foreach(_OSG_MODULE ${OSG_MODULES})
		STRING(TOLOWER ${_OSG_MODULE} _OSG_MODULE_LOWER)
		STRING(TOUPPER ${_OSG_MODULE} _OSG_MODULE_UPPER)
		set(_TARGET_NAME osg::${_OSG_MODULE_LOWER})
		list(APPEND _OSG_ALL_MODULE_TARGETS ${_TARGET_NAME})
		if(NOT TARGET ${_TARGET_NAME})
			add_library(${_TARGET_NAME} UNKNOWN IMPORTED)
			set_target_properties(${_TARGET_NAME} PROPERTIES
				INTERFACE_INCLUDE_DIRECTORIES ${${_OSG_MODULE_UPPER}_INCLUDE_DIR}
				IMPORTED_LOCATION ${${_OSG_MODULE_UPPER}_LIBRARY_RELEASE}
				IMPORTED_LOCATION_DEBUG ${${_OSG_MODULE_UPPER}_LIBRARY_DEBUG})
		endif()
	endforeach()
	
	#set all targets to variable
	set(OSG_ALL_TARGETS osg::osg osg::openthreads)
	list(APPEND OSG_ALL_TARGETS ${_OSG_ALL_MODULE_TARGETS})
endif()	
