#set lib name here!
set(LIB_NAME "GASSPluginMyGUI")
set(OSG_DIR  "${GASS_DEPENDENCIES_DIR}/OpenSceneGraph-v3.0.1" CACHE PATH "OSG folder")


#Set OIS include directories
set (MYGUI_INCLUDE_DIRS "${GASS_DEPENDENCIES_DIR}/MyGUI_3.2.0/include/MYGUI"
	"${GASS_DEPENDENCIES_DIR}/ogre-v1-9/include/OGRE"
	"${OSG_DIR}/include"
	)



set (MYGUI_LIB_DIRS "${GASS_DEPENDENCIES_DIR}/MyGUI_3.2.0/lib"
					"${GASS_DEPENDENCIES_DIR}/ogre-v1-9/lib"
					"${OSG_DIR}/lib")
		
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
 optimized MyGUI.OpenGLPlatform
 optimized OgreMain
 optimized OpenThreads
 optimized osg
 optimized osgDB
 optimized osgUtil
 optimized osgGA
 optimized osgViewer
 optimized osgText
 optimized glu32
 optimized opengl32
 debug MyGUIEngine_d
 debug MyGUI.OgrePlatform_d
 debug MyGUI.OpenGLPlatform_d
 debug OgreMain_d
 debug OpenThreadsd
 debug osgd
 debug osgDBd
 debug osgUtild
 debug osgGAd
 debug osgViewerd
 debug osgTextd
 debug glu32
 debug opengl32
  )

target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_COMMON_LINK_LIBRARIES})
target_link_libraries(${LIB_NAME} ${PROJ_LINK_LIBRARIES})

gass_install_plugin_target()

