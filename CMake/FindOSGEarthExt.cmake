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

find_package(OsgEarth)

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
set(OSGEARTH_BINARY_DIR ${OSGEARTH_DIR}/bin)
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

set(OSGEARTH_BINARY_LIST_REL 
	osgEarth
	osgEarthAnnotation
	osgEarthFeatures
	osgEarthSilverLining
	osgEarthSplat
	osgEarthSymbology
	osgEarthTriton
	osgEarthUtil
)


set(OSGEARTH_BINARY_LIST_DBG
	osgEarthd
	osgEarthAnnotationd
	osgEarthFeaturesd
	osgEarthSilverLiningd
	osgEarthSplatd
	osgEarthSymbologyd
	osgEarthTritond
	osgEarthUtild
)

if(${MSVC_VERSION} GREATER 1900) #MSVC 2017
	set(OSGEARTH_BINARY_LIST_REL ${OSGEARTH_BINARY_LIST_REL} geos libprotobuf)
	set(OSGEARTH_BINARY_LIST_DBG ${OSGEARTH_BINARY_LIST_DBG} geos_d libprotobuf)
endif()

set(OSGEARTH_PLUGINS_BINARY_LIST
	osgdb_earth
	osgdb_fastdxt
	osgdb_kml
	osgdb_osgearth_agglite
	osgdb_osgearth_arcgis
	osgdb_osgearth_bing
	osgdb_osgearth_bumpmap
	osgdb_osgearth_cache_filesystem
	osgdb_osgearth_cesiumion
	osgdb_osgearth_colorramp
	osgdb_osgearth_debug
	osgdb_osgearth_detail
	osgdb_osgearth_engine_mp
	osgdb_osgearth_engine_rex
	osgdb_osgearth_featurefilter_intersect
	osgdb_osgearth_featurefilter_join
	osgdb_osgearth_feature_elevation
	osgdb_osgearth_feature_mapnikvectortiles
	osgdb_osgearth_feature_ogr
	osgdb_osgearth_feature_tfs
	osgdb_osgearth_feature_wfs
	osgdb_osgearth_feature_xyz
	osgdb_osgearth_gdal
	osgdb_osgearth_label_annotation
	osgdb_osgearth_mapinspector
	osgdb_osgearth_mask_feature
	osgdb_osgearth_mbtiles
	osgdb_osgearth_model_feature_geom
	osgdb_osgearth_model_simple
	osgdb_osgearth_monitor
	osgdb_osgearth_osg
	osgdb_osgearth_scriptengine_javascript
	osgdb_osgearth_skyview
	osgdb_osgearth_sky_gl
	osgdb_osgearth_sky_silverlining
	osgdb_osgearth_sky_simple
	osgdb_osgearth_terrainshader
	osgdb_osgearth_tilecache
	osgdb_osgearth_tileindex
	osgdb_osgearth_tilepackage
	osgdb_osgearth_tms
	osgdb_osgearth_vdatum_egm2008
	osgdb_osgearth_vdatum_egm84
	osgdb_osgearth_vdatum_egm96
	osgdb_osgearth_viewpoints
	osgdb_osgearth_vpb
	osgdb_osgearth_wcs
	osgdb_osgearth_wms
	osgdb_osgearth_xyz
	osgdb_template)
	

if (WIN32)
	set(_SHARED_LIB_EXT .dll)
else() #assume linux
	set(_SHARED_LIB_EXT .so)
endif()

foreach(_OSGEARTH_BIN ${OSGEARTH_BINARY_LIST_DBG})
	    STRING(TOUPPER ${_OSGEARTH_BIN} _UPPER_NAME)
		set(_BIN_NAME_DBG OSGEARTH_${_UPPER_NAME}_BINARY_DBG)
		find_file(${_BIN_NAME_DBG} NAMES ${_OSGEARTH_BIN}${_SHARED_LIB_EXT} HINTS ${OSGEARTH_BINARY_DIR})
	    set(OSGEARTH_BINARIES_DBG ${OSGEARTH_BINARIES_DBG} ${${_BIN_NAME_DBG}})
endforeach()

foreach(_OSGEARTH_BIN ${OSGEARTH_BINARY_LIST_REL})
	    STRING(TOUPPER ${_OSGEARTH_BIN} _UPPER_NAME)
		set(_BIN_NAME_REL OSGEARTH_${_UPPER_NAME}_BINARY_REL)
		find_file(${_BIN_NAME_REL} NAMES ${_OSGEARTH_BIN}${_SHARED_LIB_EXT} HINTS ${OSGEARTH_BINARY_DIR})
	    set(OSGEARTH_BINARIES_REL ${OSGEARTH_BINARIES_REL} ${${_BIN_NAME_REL}})
endforeach()

set(OSGEARTHPLUGINS_BINARY_DIR ${OSGEARTH_DIR}/bin/osgPlugins-${OSG_VERSION})
foreach(_OSGEARTH_PLUGIN ${OSGEARTH_PLUGINS_BINARY_LIST})
	    STRING(TOUPPER ${_OSGEARTH_PLUGIN} _UPPER_NAME)
		set(_PLUGIN_NAME_REL OSGEARTHPLUGIN_${_UPPER_NAME}_BINARY_REL)
		set(_PLUGIN_NAME_DBG OSGEARTHPLUGIN_${_UPPER_NAME}_BINARY_DBG)
		find_file(${_PLUGIN_NAME_REL} NAMES ${_OSGEARTH_PLUGIN}${_SHARED_LIB_EXT} HINTS ${OSGEARTHPLUGINS_BINARY_DIR})
		find_file(${_PLUGIN_NAME_DBG} NAMES ${_OSGEARTH_PLUGIN}d${_SHARED_LIB_EXT} HINTS ${OSGEARTHPLUGINS_BINARY_DIR})
	    
		if(EXISTS ${${_PLUGIN_NAME_REL}})
			set(OSGEARTHPLUGIN_BINARIES_REL ${OSGEARTHPLUGIN_BINARIES_REL} ${${_PLUGIN_NAME_REL}})
		endif()
		
		if(EXISTS ${${_PLUGIN_NAME_DBG}})
			set(OSGEARTHPLUGIN_BINARIES_DBG ${OSGEARTHPLUGIN_BINARIES_DBG} ${${_PLUGIN_NAME_DBG}})
		endif()
endforeach()