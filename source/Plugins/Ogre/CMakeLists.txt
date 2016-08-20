
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

