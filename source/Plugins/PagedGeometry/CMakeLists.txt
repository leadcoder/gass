#set lib name here!
set(LIB_NAME "GASSPluginPagedGeometry")

#FindOgre.cmake use environment var OGRE_HOME
find_package(OGRE)

set(PAGEDGEOMETRY_DIR $ENV{PAGEDGEOMETRY_HOME} CACHE PATH "PagedGeometry folder")
find_package(PagedGeometry)

#add all source files from current directory (recursively)
add_source_from_current_dir()

add_library (${LIB_NAME} ${GASS_BUILDTYPE}  ${CPP_FILES} ${H_FILES})

target_include_directories(${LIB_NAME}
	PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	PUBLIC  $<BUILD_INTERFACE:${OGRE_INCLUDE_DIR}>	
	PUBLIC  $<BUILD_INTERFACE:${PAGEDGEOMETRY_INCLUDE_DIRS}>	
)

target_link_libraries(${LIB_NAME} PRIVATE GASSSim
	${OGRE_LIBRARIES} 
	${PAGEDGEOMETRY_LIBRARIES})

target_compile_definitions(${LIB_NAME} PRIVATE ${GASS_COMMON_DEFINITIONS} )

gass_install_plugin_target()
		

