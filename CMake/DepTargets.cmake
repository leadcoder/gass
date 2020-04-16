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



if(GASS_BUILD_PLUGIN_PAGED_GEOMETRY)
	find_package(PagedGeometry)
	gass_create_dep_target(PagedGeometry 
		INCLUDE_DIRS ${PAGEDGEOMETRY_INCLUDE_DIRS}  
		LIBRARIES ${PAGEDGEOMETRY_LIBRARIES})
endif()

if(GASS_BUILD_PLUGIN_MYGUI)
	find_package(MyGUI REQUIRED)
	gass_create_dep_target(MyGUI INCLUDE_DIRS ${MYGUI_INCLUDE_DIRS}  
							LIBRARIES ${MYGUI_LIBRARIES}
							BINARIES_REL ${MYGUI_BINARIES_REL}
							BINARIES_DBG ${MYGUI_BINARIES_DBG})
endif()
