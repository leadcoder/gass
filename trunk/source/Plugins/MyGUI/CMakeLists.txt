#set lib name here!
set(LIB_NAME "GASSPluginMyGUI")

include(OSGDepModule)
include(OgreDepModule)
include(MyGUIDepModule)
		
include_directories(${GASS_SIM_INCLUDE_DIRS} ${MYGUI_INC_DIR})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#Add some folders where to search for dependency libraries
link_directories(${GASS_SIM_LIB_DIRS} ${MYGUI_LIB_DIR})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})


target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_SIM_LIBRARIES})
target_link_libraries(${LIB_NAME} ${MYGUI_LINK_LIBRARIES})

gass_install_plugin_target()

