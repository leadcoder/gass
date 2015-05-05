#This is an extension of the standard FindOpenSceneGraph find_package
#This extension first call the native version (with all it's commands) 
#and then added the following variables:
#OSG_VERSION
#OSG_BINARIES_REL
#OSG_BINARIES_DBG
#OSGPLUGIN_BINARIES_REL
#OSGPLUGIN_BINARIES_REL

set(OSG_MODULES ${OSGExt_FIND_COMPONENTS})
set(OSG_VERSION ${OSGExt_FIND_VERSION})
find_package(OpenSceneGraph ${OSG_VERSION} REQUIRED ${OSG_MODULES})

#TODO: get this version somehow
set(OSG_SHARED_PREFIX osg100-)

if (WIN32)
	set(_SHARED_LIB_EXT .dll)
else() #assume linux
	set(_SHARED_LIB_EXT .so)
endif()

if (WIN32)

	
	find_path(OSG_BINARY_DIR osgviewer.exe	
		$ENV{OSG_ROOT}/bin
		$ENV{OSG_DIR}/bin
		$ENV{OSGDIR}/bin
		${OSG_DIR}/bin		
		NO_DEFAULT_PATH	
		)

	find_file(OSG_BINARY_REL NAMES ${OSG_SHARED_PREFIX}osg${_SHARED_LIB_EXT} HINTS ${OSG_BINARY_DIR})
	find_file(OSG_BINARY_DBG NAMES ${OSG_SHARED_PREFIX}osgd${_SHARED_LIB_EXT} HINTS ${OSG_BINARY_DIR})
	
	find_file(OSG_OT_BINARY_REL NAMES ot20-OpenThreads${_SHARED_LIB_EXT} HINTS ${OSG_BINARY_DIR})
	find_file(OSG_OT_BINARY_DBG NAMES ot20-OpenThreadsd${_SHARED_LIB_EXT} HINTS ${OSG_BINARY_DIR})
	
	find_file(OSG_ZLIB_BINARY_REL NAMES zlib${_SHARED_LIB_EXT} HINTS ${OSG_BINARY_DIR})
	find_file(OSG_ZLIB_BINARY_DBG NAMES zlibd${_SHARED_LIB_EXT} HINTS ${OSG_BINARY_DIR})
	
	find_file(OSG_LIBPNG_BINARY_REL NAMES libpng16${_SHARED_LIB_EXT} HINTS ${OSG_BINARY_DIR})
	find_file(OSG_LIBPNG_BINARY_DBG NAMES libpng16d${_SHARED_LIB_EXT} HINTS ${OSG_BINARY_DIR})
	
	set(OSG_BINARIES_REL ${OSG_BINARIES_REL} 
		${OSG_BINARY_REL} 
		${OSG_OT_BINARY_REL}
		${OSG_ZLIB_BINARY_REL}
		${OSG_LIBPNG_BINARY_REL})
	set(OSG_BINARIES_DBG ${OSG_BINARIES_DBG} 
			${OSG_BINARY_DBG} 
			${OSG_OT_BINARY_DBG}
			${OSG_ZLIB_BINARY_DBG}
			${OSG_ZLIB_BINARY_REL}
			${OSG_LIBPNG_BINARY_DBG})
	
	foreach(_OSG_MODULE ${OSG_MODULES})
	    STRING(TOUPPER ${_OSG_MODULE} _UPPER_NAME)
		set(_COMP_NAME_REL ${_UPPER_NAME}_BINARY_REL)
		set(_COMP_NAME_DBG ${_UPPER_NAME}_BINARY_DBG)
	
		find_file(${_COMP_NAME_REL} NAMES ${OSG_SHARED_PREFIX}${_OSG_MODULE}${_SHARED_LIB_EXT} HINTS ${OSG_BINARY_DIR})
		find_file(${_COMP_NAME_DBG} NAMES ${OSG_SHARED_PREFIX}${_OSG_MODULE}d${_SHARED_LIB_EXT} HINTS ${OSG_BINARY_DIR})
	    set(OSG_BINARIES_REL ${OSG_BINARIES_REL} ${${_COMP_NAME_REL}})
	    set(OSG_BINARIES_DBG ${OSG_BINARIES_DBG} ${${_COMP_NAME_DBG}})
	endforeach()
	
	#this plug-in list works with 3.2.1
	set(OSG_PLUGINS 3dc
					3ds
					ac
					bmp
					bsp
					bvh
					cfg
					curl
					dae
					dds
					dot
					dw
					dxf
					#exr
					freetype
					gdal
					gif
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
					nvtt
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

#extend with DLL's if windows