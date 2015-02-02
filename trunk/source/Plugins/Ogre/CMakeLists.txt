#set lib name here!
set(LIB_NAME "GASSPluginOgre")

include(OgreDepModule)		
include_directories(${GASS_COMMON_INCLUDE_DIRS} ${OGRE_INCLUDE_DIR} ${OGRE_Paging_INCLUDE_DIR} ${OGRE_Terrain_INCLUDE_DIR} ${OGRE_Overlay_INCLUDE_DIR})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#Add some folders where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})


target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_COMMON_LINK_LIBRARIES} 
${OGRE_LIBRARIES}
${OGRE_Terrain_LIBRARIES}
${OGRE_Paging_LIBRARIES} 
${OGRE_Overlay_LIBRARIES})

gass_install_plugin_target()		


