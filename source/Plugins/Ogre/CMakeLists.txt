#set lib name here!
set(LIB_NAME "GASSPluginOgre")

#FindOgre.cmake use environment var OGRE_HOME
find_package(OGRE)

#add all source files from current directory (recursively)
add_source_from_current_dir()

add_library (${LIB_NAME} ${GASS_BUILDTYPE}  ${CPP_FILES} ${H_FILES})

target_include_directories(${LIB_NAME}
	PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	PUBLIC  $<BUILD_INTERFACE:${OGRE_INCLUDE_DIR}>	
	PUBLIC  $<BUILD_INTERFACE:${OGRE_Paging_INCLUDE_DIR}>
    PUBLIC  $<BUILD_INTERFACE:${OGRE_Terrain_INCLUDE_DIR}>
	PUBLIC  $<BUILD_INTERFACE:${OGRE_Overlay_INCLUDE_DIR}>	
)

target_link_libraries(${LIB_NAME} PRIVATE GASSSim
	${OGRE_LIBRARIES}
	${OGRE_Terrain_LIBRARIES}
	${OGRE_Paging_LIBRARIES} 
	${OGRE_Overlay_LIBRARIES})
target_compile_definitions(${LIB_NAME} PRIVATE ${GASS_COMMON_DEFINITIONS} )

gass_install_plugin_target()		


