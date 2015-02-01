#set lib name here!
set(LIB_NAME "GASSPluginOgre")

include(OgreDepModule)		
include_directories(${GASS_COMMON_INCLUDE_DIRS} ${OGRE_INC_DIR})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#Add some folders where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS} ${OGRE_LIB_DIR})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_COMMON_LINK_LIBRARIES})
target_link_libraries(${LIB_NAME} ${OGRE_LINK_LIBRARIES})

gass_install_plugin_target()		


