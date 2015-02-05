#Used to gather all dlls needed for samples to run
if(WIN32)
	#TBB
	find_package(TBB)
	
	#MY GUI
	set(MYGUI_BIN_DIR "${GASS_DEPENDENCIES_DIR}/MyGUI_3.2.0/bin/" CACHE PATH "MyGUI bin folder")
	set(MYGUI_BIN_FILES_RELEASE ${MYGUI_BIN_DIR}/release/MyGUIEngine.dll)
	set(MYGUI_BIN_FILES_DEBUG  	${MYGUI_BIN_DIR}/debug/MyGUIEngine_d.dll)	
	
	#OpenAL
	set(OPENAL_BIN_DIR  "${GASS_DEPENDENCIES_DIR}/OpenAL1.1/bin" CACHE PATH "OpenAL bin folder")
	find_package(OpenALExt)
	
	#OIS
	find_package(OIS)
	
	#SKYX
	find_package(SkyX)
	
	#Ogre
	find_package(Ogre)
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
		
	set(ALL_BIN_FILES_RELEASE 
		${OPENAL_BIN_FILES_RELEASE}
		${SKYX_BINARY_REL}
		${OGRE_BIN_FILES_RELEASE}
		${TBB_BINARIES_REL}
		${OIS_BINARY_REL}
		#${MYGUI_BIN_FILES_RELEASE}
		)
	set(ALL_BIN_FILES_DEBUG	
		${OPENAL_BIN_FILES_DEBUG}
		${SKYX_BINARY_DBG}
		${OGRE_BIN_FILES_DEBUG}
		${TBB_BINARY_DBG}
		${OIS_BINARY_DBG}
		#${MYGUI_BIN_FILES_DEBUG}
		)
endif()




