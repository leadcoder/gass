#Used to gather all dlls needed for samples to run
if(WIN32)
	#TBB
	set(TBB_DIR "${GASS_DEPENDENCIES_DIR}/tbb")
	if(${CMAKE_CL_64})
	if(${MSVC10})
		set(TBB_BIN_DIR "${TBB_DIR}/bin/intel64/vc10" CACHE PATH "TBB bin folder")
	elseif(${MSVC11})
		set(TBB_BIN_DIR "${TBB_DIR}/bin/intel64/vc11" CACHE PATH "TBB bin folder")
	endif()
	else()
	if(${MSVC10})
		set(TBB_BIN_DIR "${TBB_DIR}/bin/ia32/vc10" CACHE PATH "TBB bin folder")
	elseif(${MSVC11})
		set(TBB_BIN_DIR "${TBB_DIR}/bin/ia32/vc11" CACHE PATH "TBB bin folder")
	endif()
	endif()
	
	set(TBB_BIN_FILES_DEBUG ${TBB_BIN_DIR}/tbb_debug.dll)
	set(TBB_BIN_FILES_RELEASE ${TBB_BIN_DIR}/tbb.dll)
	
	#MY GUI
	set(MYGUI_BIN_DIR "${GASS_DEPENDENCIES_DIR}/MyGUI_3.2.0/bin/" CACHE PATH "MyGUI bin folder")
	set(MYGUI_BIN_FILES_RELEASE ${MYGUI_BIN_DIR}/release/MyGUIEngine.dll)
	set(MYGUI_BIN_FILES_DEBUG  	${MYGUI_BIN_DIR}/debug/MyGUIEngine_d.dll)	
	
	#OpenAL
	set(OPENAL_BIN_DIR  "${GASS_DEPENDENCIES_DIR}/OpenAL1.1/bin" CACHE PATH "OpenAL bin folder")
	set(OPENAL_BIN_FILES_DEBUG ${OPENAL_BIN_DIR}/OpenAL32.dll)
	set(OPENAL_BIN_FILES_RELEASE ${OPENAL_BIN_DIR}/OpenAL32.dll)
	
	#OIS
	set(OIS_BIN_DIR  "${GASS_DEPENDENCIES_DIR}/OIS/bin" CACHE PATH "OIS bin folder")
	set(OIS_BIN_FILES_DEBUG ${OIS_BIN_DIR}/OIS_d.dll)
	set(OIS_BIN_FILES_RELEASE ${OIS_BIN_DIR}/OIS.dll)
	
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
		
		set(OGRE_BIN_FILES_RELEASE 
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
		${SKYX_BIN_FILES_RELEASE}
		${OGRE_BIN_FILES_RELEASE}
		${TBB_BIN_FILES_RELEASE}
		${OIS_BIN_FILES_RELEASE}
		#${MYGUI_BIN_FILES_RELEASE}
		)
	
	set(ALL_BIN_FILES_DEBUG	
		${OPENAL_BIN_FILES_DEBUG}
		${SKYX_BIN_FILES_DEBUG}
		${OGRE_BIN_FILES_DEBUG}
		${TBB_BIN_FILES_DEBUG}
		${OIS_BIN_FILES_DEBUG}
		#${MYGUI_BIN_FILES_DEBUG}
		)
			
endif()




