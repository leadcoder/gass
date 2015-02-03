#set lib name here!
set(LIB_NAME "GASSPluginPagedGeometry")

#FindOgre.cmake use environment var OGRE_HOME
find_package(Ogre)

set(PAGEDGEOMETRY_DIR "${GASS_DEPENDENCIES_DIR}/PagedGeometry-1.1.1" CACHE PATH "PagedGeometry folder")
find_package(PagedGeometry)

include_directories(${GASS_COMMON_INCLUDE_DIRS} 
					${OGRE_INCLUDE_DIR} 
					${PAGEDGEOMETRY_INCLUDE_DIRS})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#Add some folders where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

target_link_libraries(${LIB_NAME} 
				GASSSim 
				GASSCore 
				${GASS_COMMON_LINK_LIBRARIES} 
				${OGRE_LIBRARIES} 
				${PAGEDGEOMETRY_LIBRARIES}) 

gass_install_plugin_target()
		

