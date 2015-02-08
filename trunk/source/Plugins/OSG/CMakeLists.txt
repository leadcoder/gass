#set lib name here!
set(LIB_NAME "GASSPluginOSG")

#find_package(OpenSceneGraph 3.2.1 REQUIRED osgUtil osgDB osgGA osgText osgShadow osgViewer osgSim osgTerrain)
find_package(OSGExt 3.2.1 REQUIRED osgUtil osgDB osgGA osgText osgShadow osgViewer osgSim osgTerrain)

include_directories(${GASS_SIM_INCLUDE_DIRS} ${OPENSCENEGRAPH_INCLUDE_DIRS})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#Add some folders where to search for dependency libraries
link_directories(${GASS_SIM_LIB_DIRS})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})
	
target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_SIM_LIBRARIES})
target_link_libraries(${LIB_NAME} ${OPENSCENEGRAPH_LIBRARIES})

gass_install_plugin_target()

