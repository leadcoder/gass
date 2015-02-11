#set lib name here!
set(LIB_NAME "GASSPluginMyGUI")

#include(OSGDepModule)
#include(OgreDepModule)
set(MYGUI_DIR ${GASS_DEPENDENCIES_DIR}/MyGUI_3.2.0)
find_package(MyGUI)

find_package(OpenGL)

#needed for if we want osg support...TODO: add this as option
find_package(OSGExt 3.2.1 REQUIRED osgUtil osgDB osgGA osgText osgShadow osgViewer osgSim osgTerrain)

#needed for if we want Ogre support...TODO: add this as option
find_package(Ogre)
		
include_directories(${GASS_SIM_INCLUDE_DIRS} ${MYGUI_INCLUDE_DIRS} ${OPENSCENEGRAPH_INCLUDE_DIRS} ${OGRE_INCLUDE_DIR})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#Add some folders where to search for dependency libraries
link_directories(${GASS_SIM_LIB_DIRS})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})
target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_SIM_LIBRARIES} ${MYGUI_LIBRARIES} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${OPENSCENEGRAPH_LIBRARIES} ${OGRE_LIBRARIES})

gass_install_plugin_target()

