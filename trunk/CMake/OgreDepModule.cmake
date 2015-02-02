
find_package(Ogre)

# if(EXISTS "${DEPENDENCIES_DIR}/ogre-v1-9")
	# set(OGRE_DIR  "${DEPENDENCIES_DIR}/ogre-v1-9" CACHE PATH "Ogre 3D folder")
# else()
	# set(OGRE_DIR $ENV{OGRE_HOME} CACHE PATH "Ogre 3D folder")
# endif()
# Set ogre sdk include directories
# set (OGRE_INC_DIR "${OGRE_DIR}/include/OGRE"
		# "${OGRE_DIR}/include/OGRE/Plugins/OctreeSceneManager"
		# "${OGRE_DIR}/include/OGRE/Terrain"
		# "${OGRE_DIR}/include/OGRE/Paging"
# )
		
# set(OGRE_BIN_DIR ${OGRE_DIR}/bin)
# set(OGRE_LIB_DIR ${OGRE_DIR}/lib)

# set(OGRE_LINK_LIBRARIES optimized OgreMain 
	# optimized OgrePaging 
	# optimized OgreTerrain 
	# optimized OgreOverlay 
	# debug OgreMain_d
	# debug OgrePaging_d 
	# debug OgreTerrain_d 
	# debug OgreOverlay_d
	# )

# set(OGRE_BIN_FILES_DEBUG ${OGRE_BIN_DIR}/debug/OgreMain_d.dll
	# ${OGRE_BIN_DIR}/debug/OgreTerrain_d.dll
	# ${OGRE_BIN_DIR}/debug/OgrePaging_d.dll
	# ${OGRE_BIN_DIR}/debug/OgreOverlay_d.dll
	# ${OGRE_BIN_DIR}/debug/Plugin_CgProgramManager_d.dll
	# ${OGRE_BIN_DIR}/debug/OgreOverlay_d.dll
	# ${OGRE_BIN_DIR}/debug/Plugin_OctreeSceneManager_d.dll
	# ${OGRE_BIN_DIR}/debug/Plugin_ParticleFX_d.dll
	# ${OGRE_BIN_DIR}/debug/RenderSystem_Direct3D9_d.dll
	# ${OGRE_BIN_DIR}/debug/RenderSystem_GL_d.dll
	# ${OGRE_BIN_DIR}/release/cg.dll)

# set(OGRE_BIN_FILES_RELEASE ${OGRE_BIN_DIR}/release/OgreMain.dll
	# ${OGRE_BIN_DIR}/release/OgreTerrain.dll
	# ${OGRE_BIN_DIR}/release/OgrePaging.dll
	# ${OGRE_BIN_DIR}/release/OgreOverlay.dll
	# ${OGRE_BIN_DIR}/release/Plugin_CgProgramManager.dll
	# ${OGRE_BIN_DIR}/release/OgreOverlay.dll
	# ${OGRE_BIN_DIR}/release/Plugin_OctreeSceneManager.dll
	# ${OGRE_BIN_DIR}/release/Plugin_ParticleFX.dll
	# ${OGRE_BIN_DIR}/release/RenderSystem_Direct3D9.dll
	# ${OGRE_BIN_DIR}/release/RenderSystem_GL.dll
	# ${OGRE_BIN_DIR}/release/cg.dll
# )
