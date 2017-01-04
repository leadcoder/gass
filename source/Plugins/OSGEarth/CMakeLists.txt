gass_setup_plugin(GASSPluginOSGEarth PRIVATE_DEPS OSGEarth OSG)

# set(LIB_NAME "GASSPluginOSGEarth")

# find_package(OpenSceneGraph 3.2.1 REQUIRED osgUtil osgDB osgGA osgText osgShadow osgViewer osgSim osgTerrain)

#add find module
# set(CMAKE_MODULE_PATH
  # ${CMAKE_MODULE_PATH}
  # $ENV{OSGEARTHDIR}/CMakeModules
# )
# find_package(OSGEarth)


# include_directories(${GASS_SIM_INCLUDE_DIRS} ${OSGEARTH_INCLUDE_DIRS} ${OPENSCENEGRAPH_INCLUDE_DIRS})

# SET(CPP_FILES 
	# OSGEarthCameraManipulatorComponent.cpp
	# OSGEarthLocationComponent.cpp
	# OSGEarthMapComponent.cpp
	# OSGEarthSceneManager.cpp
	# OSGEarthSkyComponent.cpp)

# SET(H_FILES  
	# OSGEarthCameraManipulatorComponent.h
	# OSGEarthLocationComponent.h
	# OSGEarthMapComponent.h
	# OSGEarthMessages.h
	# OSGEarthSceneManager.h
	# OSGEarthSkyComponent.h)

# add_definitions(-DGASS_PLUGIN_EXPORTS )
# add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

# target_link_libraries(${LIB_NAME} GASSSim GASSCore
	# ${OSGEARTH_LIBRARY}
	# ${OSGEARTHFEATURES_LIBRARY}
	# ${OSGEARTHUTIL_LIBRARY}
	# ${OSGEARTHSYMBOLOGY_LIBRARY}
	# ${OSGEARTHANNOTATION_LIBRARY} 
	# ${OPENSCENEGRAPH_LIBRARIES})
# gass_install_plugin_target()



