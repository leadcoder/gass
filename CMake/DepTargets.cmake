#This file hold "proxy" build targets for dependencies that can be 
#used by any GASS lib by just refering to dependencies like 
#internal targets

include(Common)

if(GASS_BUILD_PLUGIN_OGRE)
	#FindOgre.cmake use environment var OGRE_HOME
	find_package(OGRE REQUIRED)
	
	set(OGRE_INCLUDE_DIRS
		${OGRE_INCLUDE_DIR}
		${OGRE_Paging_INCLUDE_DIR}
		${OGRE_Terrain_INCLUDE_DIR}
		${OGRE_Overlay_INCLUDE_DIR})
	set(OGRE_LIBRARY_LIST ${OGRE_LIBRARIES}
		 ${OGRE_Terrain_LIBRARIES}
		 ${OGRE_Paging_LIBRARIES}
		 ${OGRE_Overlay_LIBRARIES})
		 
	set(OGRE_BIN_FILES_RELEASE 
		${OGRE_BINARY_REL}
		${OGRE_Overlay_BINARY_REL}
		${OGRE_Paging_BINARY_REL}
		${OGRE_Terrain_BINARY_REL}
		${OGRE_Plugin_CgProgramManager_REL}
		${OGRE_Plugin_OctreeSceneManager_REL}
		${OGRE_Plugin_ParticleFX_REL}
		${OGRE_RenderSystem_Direct3D9_REL}
		${OGRE_RenderSystem_GL_REL}
		${OGRE_PLUGIN_DIR_REL}/cg.dll)
		
	set(OGRE_BIN_FILES_DEBUG 
		${OGRE_BINARY_DBG}
		${OGRE_Overlay_BINARY_DBG}
		${OGRE_Paging_BINARY_DBG}
		${OGRE_Terrain_BINARY_DBG}
		${OGRE_Plugin_CgProgramManager_DBG}
		${OGRE_Plugin_OctreeSceneManager_DBG}
		${OGRE_Plugin_ParticleFX_DBG}
		${OGRE_RenderSystem_Direct3D9_DBG}
		${OGRE_RenderSystem_GL_DBG}
		${OGRE_PLUGIN_DIR_DBG}/cg.dll)
	
	if (OGRE_CONFIG_THREAD_PROVIDER EQUAL 1) #need boost headers
   		find_package(Boost)
		set(OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIRS} ${Boost_INCLUDE_DIR})
   	endif()
	gass_create_dep_target(Ogre 
		INCLUDE_DIRS ${OGRE_INCLUDE_DIRS}
		LIBRARIES ${OGRE_LIBRARY_LIST}
		BINARIES_REL ${OGRE_BIN_FILES_RELEASE}
		BINARIES_DBG ${OGRE_BIN_FILES_DEBUG})	
endif()

if(GASS_BUILD_PLUGIN_ENVIRONMENT)
	find_package(SkyX REQUIRED)
	gass_create_dep_target(SkyX 
					INCLUDE_DIRS ${SKYX_INCLUDE_DIRS} 
					LIBRARIES ${SKYX_LIBRARIES}
					BINARIES_REL ${SKYX_BINARY_REL}
					BINARIES_DBG ${SKYX_BINARY_DBG})
	
	find_package(Hydrax REQUIRED)
	gass_create_dep_target(Hydrax INCLUDE_DIRS ${HYDRAX_INCLUDE_DIRS} LIBRARIES ${HYDRAX_LIBRARIES})
endif()

if(GASS_BUILD_PLUGIN_MYGUI)
	find_package(MyGUI REQUIRED)
	gass_create_dep_target(MyGUI INCLUDE_DIRS ${MYGUI_INCLUDE_DIRS}  
							LIBRARIES ${MYGUI_LIBRARIES}
							BINARIES_REL ${MYGUI_BINARIES_REL}
							BINARIES_DBG ${MYGUI_BINARIES_DBG})
endif()

if(GASS_BUILD_PLUGIN_PAGED_GEOMETRY)
	find_package(PagedGeometry)
	gass_create_dep_target(PagedGeometry 
		INCLUDE_DIRS ${PAGEDGEOMETRY_INCLUDE_DIRS}  
		LIBRARIES ${PAGEDGEOMETRY_LIBRARIES})
endif()

#if(GASS_BUILD_PLUGIN_ODE)
#	find_package(ODE REQUIRED)
#	gass_create_dep_target(ODE INCLUDE_DIRS ${ODE_INCLUDE_DIRS} LIBRARIES ${ODE_LIBRARIES} DEFINITIONS dDOUBLE)
#endif()



#RakNet
# if(GASS_BUILD_PLUGIN_RAKNET)
	# find_package(RakNet)
	# if(WIN32)
		# set(RAKNET_LIBRARIES ${RAKNET_LIBRARIES} debug ws2_32 optimized ws2_32)
	# endif()
	# gass_create_dep_target(RakNet INCLUDE_DIRS ${RAKNET_INCLUDE_DIRS}  LIBRARIES ${RAKNET_LIBRARIES})
# endif()

#OSG
# if(GASS_BUILD_PLUGIN_OSG)
	# find_package(OSGExt 3.2.1 REQUIRED osgUtil osgDB osgGA  osgFX osgVolume osgText osgShadow osgViewer osgSim osgTerrain osgManipulator)
	# gass_create_dep_target(OSG INCLUDE_DIRS ${OPENSCENEGRAPH_INCLUDE_DIRS} LIBRARIES ${OPENSCENEGRAPH_LIBRARIES} BINARIES_REL ${OSG_BINARIES_REL} BINARIES_DBG ${OSG_BINARIES_DBG})
	
	# #store all plugin filenames in list, used when configuring consumer find scripts
	# gass_filename_only(OSGPLUGIN_BINARIES_REL GASS_OSGPLUGIN_BINARIES_REL)
	# gass_filename_only(OSGPLUGIN_BINARIES_DBG GASS_OSGPLUGIN_BINARIES_DBG)
	
	# if(GASS_INSTALL_DEP_BINARIES AND WIN32) #install osg plugins to subfolder
		# install(FILES ${OSGPLUGIN_BINARIES_REL} DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE}/osgPlugins-${OSG_VERSION} CONFIGURATIONS Release)
		# install(FILES ${OSGPLUGIN_BINARIES_DBG} DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG}/osgPlugins-${OSG_VERSION} CONFIGURATIONS Debug)
		
		# file(COPY ${OSGPLUGIN_BINARIES_REL} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
		# file(COPY ${OSGPLUGIN_BINARIES_DBG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	# endif()
# endif()

#PhysX
# if(GASS_BUILD_PLUGIN_PHYSX)
	# set(PHYSX_INSTALL_DIR $ENV{PHYSX_HOME} CACHE PATH "PhysX folder")
	# find_package(PhysX4 REQUIRED)
	# gass_create_dep_target(PhysX4 INCLUDE_DIRS ${PHYSX4_INCLUDE_DIR} 
								# LIBRARIES ${PHYSX4_LIBRARIES}
								# BINARIES_REL ${PX_BINARIES_REL}
								# BINARIES_DBG ${PX_BINARIES_DBG})
# endif()

if(GASS_BUILD_PLUGIN_OSGEARTH)
	find_package(OSGEarthExt)
	set(OSGEARTH_LIBRARIES optimized ${OSGEARTH_LIBRARY}
						optimized ${OSGEARTHFEATURES_LIBRARY}
						optimized ${OSGEARTHUTIL_LIBRARY}
						optimized ${OSGEARTHSYMBOLOGY_LIBRARY}
						optimized ${OSGEARTHANNOTATION_LIBRARY}
						debug ${OSGEARTH_LIBRARY_DEBUG}
						debug ${OSGEARTHFEATURES_LIBRARY_DEBUG}
						debug ${OSGEARTHUTIL_LIBRARY_DEBUG}
						debug ${OSGEARTHSYMBOLOGY_LIBRARY_DEBUG}
						debug ${OSGEARTHANNOTATION_LIBRARY_DEBUG})
	
	gass_create_dep_target(OSGEarth 
					INCLUDE_DIRS ${OSGEARTH_INCLUDE_DIRS} 
					LIBRARIES ${OSGEARTH_LIBRARIES}
					BINARIES_REL ${OSGEARTH_BINARIES_REL} 
					BINARIES_DBG ${OSGEARTH_BINARIES_DBG})
	#store all plugin filenames in list, used when configuring consumer find scripts
	gass_filename_only(OSGEARTHPLUGIN_BINARIES_REL GASS_OSGEARTHPLUGIN_BINARIES_REL)
	gass_filename_only(OSGEARTHPLUGIN_BINARIES_DBG GASS_OSGEARTHPLUGIN_BINARIES_DBG)
					
	if(GASS_INSTALL_DEP_BINARIES AND WIN32) #install osg plugins to subfolder
		install(FILES ${OSGEARTHPLUGIN_BINARIES_REL} DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE}/osgPlugins-${OSG_VERSION} CONFIGURATIONS Release)
		install(FILES ${OSGEARTHPLUGIN_BINARIES_DBG} DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG}/osgPlugins-${OSG_VERSION} CONFIGURATIONS Debug)
		file(COPY ${OSGEARTHPLUGIN_BINARIES_REL} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
		file(COPY ${OSGEARTHPLUGIN_BINARIES_DBG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)		
	endif()
endif()
