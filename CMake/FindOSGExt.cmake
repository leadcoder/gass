#This is an extension of the standard FindOpenSceneGraph find_package
#This extension first call the native version (with all it's commands) 
#and then added the following variables:
#OSG_VERSION
#OSG_BINARIES_REL
#OSG_BINARIES_DBG
#OSGPLUGIN_BINARIES_REL
#OSGPLUGIN_BINARIES_REL

set(OSG_MODULES ${OSGExt_FIND_COMPONENTS})
#set(OSG_VERSION ${OSGExt_FIND_VERSION})

macro(extract_version var str)
STRING(REGEX REPLACE ".*VERSION\ *([0-9]+)" "\\1" ${var} ${str})
endmacro(extract_version)

include(CMakeParseArguments)


macro(add_bin_rel _BIN_NAME)
	cmake_parse_arguments(
        PARSED_ARGS # prefix of output variables
        ""
        "" # list of names of mono-valued arguments
        "NAMES" # list of names of multi-valued arguments (output variables are lists)
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )
	set(_VAR_NAME OSG_${_BIN_NAME}_BIN_REL)
	find_file(${_VAR_NAME} NAMES ${PARSED_ARGS_NAMES} HINTS ${OSG_BINARY_DIR})
	set(OSG_BINARIES_REL ${OSG_BINARIES_REL} ${${_VAR_NAME}})
endmacro()

macro(add_bin_dbg _BIN_NAME)
	cmake_parse_arguments(
        PARSED_ARGS # prefix of output variables
        ""
        "" # list of names of mono-valued arguments
        "NAMES" # list of names of multi-valued arguments (output variables are lists)
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )
	set(_VAR_NAME OSG_${_BIN_NAME}_BIN_DBG)
	find_file(${_VAR_NAME} NAMES ${PARSED_ARGS_NAMES} HINTS ${OSG_BINARY_DIR})
	set(OSG_BINARIES_DBG ${OSG_BINARIES_DBG} ${${_VAR_NAME}})
endmacro()


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

if (WIN32)
	set(_SHARED_LIB_EXT .dll)
else() #assume linux
	set(_SHARED_LIB_EXT .so)
endif()

if (WIN32)
	find_path(OSG_BINARY_DIR osgviewer.exe HINTS ${OSG_DIR}/bin)
	find_file(OSG_BINARY_REL NAMES ${OSG_SHARED_PREFIX}osg${_SHARED_LIB_EXT} HINTS ${OSG_BINARY_DIR})
	find_file(OSG_BINARY_DBG NAMES ${OSG_SHARED_PREFIX}osgd${_SHARED_LIB_EXT} HINTS ${OSG_BINARY_DIR})
	
	find_file(OSG_OT_BINARY_REL NAMES ot21-OpenThreads${_SHARED_LIB_EXT} ot20-OpenThreads${_SHARED_LIB_EXT} HINTS ${OSG_BINARY_DIR})
	find_file(OSG_OT_BINARY_DBG NAMES ot21-OpenThreadsd${_SHARED_LIB_EXT} ot20-OpenThreadsd${_SHARED_LIB_EXT} HINTS ${OSG_BINARY_DIR})
	
	set(OSG_BINARIES_REL ${OSG_BINARIES_REL} 
		${OSG_BINARY_REL} 
		${OSG_OT_BINARY_REL})
	set(OSG_BINARIES_DBG ${OSG_BINARIES_DBG} 
			${OSG_BINARY_DBG} 
			${OSG_OT_BINARY_DBG})
			
	foreach(_OSG_MODULE ${OSG_MODULES})
	    STRING(TOUPPER ${_OSG_MODULE} _UPPER_NAME)
		set(_COMP_NAME_REL ${_UPPER_NAME}_BINARY_REL)
		set(_COMP_NAME_DBG ${_UPPER_NAME}_BINARY_DBG)
	
		find_file(${_COMP_NAME_REL} NAMES ${OSG_SHARED_PREFIX}${_OSG_MODULE}${_SHARED_LIB_EXT} HINTS ${OSG_BINARY_DIR})
		find_file(${_COMP_NAME_DBG} NAMES ${OSG_SHARED_PREFIX}${_OSG_MODULE}d${_SHARED_LIB_EXT} HINTS ${OSG_BINARY_DIR})
	    set(OSG_BINARIES_REL ${OSG_BINARIES_REL} ${${_COMP_NAME_REL}})
	    set(OSG_BINARIES_DBG ${OSG_BINARIES_DBG} ${${_COMP_NAME_DBG}})
	endforeach()
			
	#thirdparty shared libs			

	add_bin_rel(ZLIB NAMES zlib.dll)
	add_bin_dbg(ZLIB NAMES zlibd.dll)
	
	add_bin_rel(PNG NAMES libpng16.dll libpng.dll)
	add_bin_dbg(PNG NAMES libpng16d.dll libpngd.dll)
	
	add_bin_rel(GDAL NAMES gdal202.dll gdal201.dll gdal110.dll)
	add_bin_dbg(GDAL NAMES gdal202.dll gdal201.dll gdal110.dll)
	
	add_bin_rel(PROJ NAMES proj.dll)
	add_bin_dbg(PROJ NAMES proj.dll)
	
	add_bin_rel(CURL NAMES libcurl.dll)
	add_bin_dbg(CURL NAMES libcurld.dll)
	
	add_bin_rel(EAY NAMES libeay32.dll)
	add_bin_dbg(EAY NAMES libeay32.dll)
	
	add_bin_rel(SSLEAY NAMES ssleay32.dll)
	add_bin_dbg(SSLEAY NAMES ssleay32.dll)
	
	add_bin_rel(TIFF NAMES libtiff.dll tiff.dll)
	add_bin_dbg(TIFF NAMES libtiff.dll tiff.dll)
	
	# Different names depending on VS version
	if(${MSVC_VERSION} EQUAL 1800) #MSVC 2013
   		set(OSG_PLUGIN_DAE "dae")
   	else()
   		set(OSG_PLUGIN_DAE "")
   	endif()
	

	#this plug-in list works with 3.2.1
	set(OSG_PLUGINS 3dc
					3ds
					ac
					bmp
					bsp
					bvh
					cfg
					curl
					${OSG_PLUGIN_DAE}
					dds
					dot
					#dw
					dxf
					#exr
					freetype
					gdal
					#gif
					glsl
					gz
					hdr
					ive
					jpeg
					ktx
					logo
					lwo
					lws
					md2
					mdl
					normals
					#nvtt
					obj
					ogr
					openflight
					osc
					osg
					osga
					osgshadow
					osgterrain
					osgtgz
					osgviewer
					p3d
					pic
					ply
					png
					pnm
					pov
					pvr
					#qfont
					revisions
					rgb
					rot
					scale
					serializers_osg
					serializers_osganimation
					serializers_osgfx
					serializers_osgga
					serializers_osgmanipulator
					serializers_osgparticle
					serializers_osgshadow
					serializers_osgsim
					serializers_osgterrain
					serializers_osgtext
					serializers_osgviewer
					serializers_osgvolume
					deprecated_osg
					deprecated_osganimation
					deprecated_osgfx
					deprecated_osgparticle
					deprecated_osgshadow
					deprecated_osgsim
					deprecated_osgterrain
					deprecated_osgtext
					deprecated_osgviewer
					deprecated_osgvolume
					deprecated_osgwidget
					shp
					stl
					tga
					tgz
					tiff
					trans
					trk
					txf
					txp
					vtf
					x
					zip)
	
	set(OSGPLUGINS_BINARY_DIR ${OSG_BINARY_DIR}/osgPlugins-${OSG_VERSION})
	
	foreach(_OSG_PLUGIN ${OSG_PLUGINS})
	    STRING(TOUPPER ${_OSG_PLUGIN} _UPPER_NAME)
		set(_PLUGIN_NAME_REL OSGPLUGIN_${_UPPER_NAME}_BINARY_REL)
		set(_PLUGIN_NAME_DBG OSGPLUGIN_${_UPPER_NAME}_BINARY_DBG)
		#message("osgdb_${_OSG_PLUGIN}${_SHARED_LIB_EXT}")
		find_file(${_PLUGIN_NAME_REL} NAMES osgdb_${_OSG_PLUGIN}${_SHARED_LIB_EXT} HINTS ${OSGPLUGINS_BINARY_DIR})
		find_file(${_PLUGIN_NAME_DBG} NAMES osgdb_${_OSG_PLUGIN}d${_SHARED_LIB_EXT} HINTS ${OSGPLUGINS_BINARY_DIR})
	    set(OSGPLUGIN_BINARIES_REL ${OSGPLUGIN_BINARIES_REL} ${${_PLUGIN_NAME_REL}})
	    set(OSGPLUGIN_BINARIES_DBG ${OSGPLUGIN_BINARIES_DBG} ${${_PLUGIN_NAME_DBG}})
	endforeach()
endif()