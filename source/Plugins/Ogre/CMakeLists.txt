
#FindOgre.cmake use environment var OGRE_HOME
find_package(OGRE)

set(OGRE_INCLUDE_DIRS
	${OGRE_INCLUDE_DIR}
	${OGRE_Paging_INCLUDE_DIR}
    ${OGRE_Terrain_INCLUDE_DIR}
	${OGRE_Overlay_INCLUDE_DIR})
set(OGRE_LIBRARY_LIST ${OGRE_LIBRARIES}
	 ${OGRE_Terrain_LIBRARIES}
	 ${OGRE_Paging_LIBRARIES}
	 ${OGRE_Overlay_LIBRARIES})

gass_create_dep_target(Ogre "${OGRE_INCLUDE_DIRS}" "${OGRE_LIBRARY_LIST}")
gass_setup_plugin(GASSPluginOgre Ogre)

if(GASS_INSTALL_DEP_BINARIES)
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
		
		install(FILES ${OGRE_BIN_FILES_RELEASE} DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
		install(FILES ${OGRE_BIN_FILES_DEBUG} DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)

		FILE(COPY ${OGRE_BIN_FILES_RELEASE} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
		FILE(COPY ${OGRE_BIN_FILES_DEBUG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
endif()

