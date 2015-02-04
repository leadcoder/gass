#set lib name here!
set(LIB_NAME "GASSPluginEnvironment")

#FindOgre.cmake use environment var OGRE_HOME
find_package(Ogre)

set(SKYX_DIR  "${GASS_DEPENDENCIES_DIR}/SkyX-v0.4" CACHE PATH "SkyX folder")
find_package(SkyX)

set(HYDRAX_DIR "${GASS_DEPENDENCIES_DIR}/Hydrax-v0.5.1" CACHE PATH "Hydrax folder")
find_package(Hydrax)

include_directories(${GASS_SIM_INCLUDE_DIRS} ${OGRE_INCLUDE_DIR}
		${SKYX_INCLUDE_DIRS}
		${HYDRAX_INCLUDE_DIRS})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#Add some folders where to search for dependency libraries
link_directories(${GASS_SIM_LIB_DIRS})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

target_link_libraries(${LIB_NAME} GASSSim 
						GASSCore 
						${GASS_SIM_LIBRARIES}
						${OGRE_LIBRARIES}
						${SKYX_LIBRARIES}
						${HYDRAX_LIBRARIES})

gass_install_plugin_target()

