#set lib name here!
set(LIB_NAME "GASSPluginMyGUI")

#Set OIS include directories
set (MYGUI_INCLUDE_DIRS "${GASS_DEPENDENCIES_DIR}/MyGUI_3.2.0/include/MYGUI"
	"${GASS_DEPENDENCIES_DIR}/ogre-v1-9/include/OGRE"
	)
set (MYGUI_LIB_DIRS "${GASS_DEPENDENCIES_DIR}/MyGUI_3.2.0/lib"
					"${GASS_DEPENDENCIES_DIR}/ogre-v1-9/lib")
		
include_directories(${GASS_COMMON_INCLUDE_DIRS} ${MYGUI_INCLUDE_DIRS})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#Add some folders where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS} ${MYGUI_LIB_DIRS})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

#Add project dependency libs
set(PROJ_LINK_LIBRARIES 
 optimized MyGUIEngine
 optimized MyGUI.OgrePlatform
 optimized OgreMain
 debug MyGUIEngine_d
 debug MyGUI.OgrePlatform_d
 debug OgreMain_d
  )

target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_COMMON_LINK_LIBRARIES})
target_link_libraries(${LIB_NAME} ${PROJ_LINK_LIBRARIES})

gass_install_plugin_target()

