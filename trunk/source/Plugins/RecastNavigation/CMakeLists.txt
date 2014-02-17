#set lib name here!
set(LIB_NAME "GASSPluginRecastNavigation")

#Set include directories
include_directories(${GASS_COMMON_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/source/Plugins/RecastNavigation/API/Detour/include"
		"${PROJECT_SOURCE_DIR}/source/Plugins/RecastNavigation/API/DetourCrowd/include"
		"${PROJECT_SOURCE_DIR}/source/Plugins/RecastNavigation/API/DetourTileCache/include"
		"${PROJECT_SOURCE_DIR}/source/Plugins/RecastNavigation/API/Recast/include"
		"${PROJECT_SOURCE_DIR}/source/Plugins/RecastNavigation/API/Utils/include"
		)

#add all source files from current directory (recursively)
add_source_from_current_dir()


#Add some folders where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

#Add project dependency libs
target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_COMMON_LINK_LIBRARIES})

gass_install_plugin_target()	

#install(FILES  ${CMAKE_CURRENT_SOURCE_DIR}/RecastNavigationMessages.h DESTINATION ${GASS_INSTALL_INCLUDE_DIR}/plugins/RecastNavigation)

