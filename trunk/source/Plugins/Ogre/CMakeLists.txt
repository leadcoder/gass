#set lib name here!
set(LIB_NAME "GASSPluginOgre")

if(EXISTS "${GASS_DEPENDENCIES_DIR}/ogre")
	set(OGRE_DIR  "${GASS_DEPENDENCIES_DIR}/ogre" CACHE PATH "Ogre 3D folder")
else()
	set(OGRE_DIR $ENV{OGRE_HOME} CACHE PATH "Ogre 3D folder")
endif()

#Set ogre sdk include directiores
set (OGRE_SDK_INCLUDE_DIRS "${OGRE_DIR}/include/OGRE"
		"${OGRE_DIR}/include/OGRE/Plugins/OctreeSceneManager"
		"${OGRE_DIR}/include/OGRE/Terrain"
		"${OGRE_DIR}/include/OGRE/Paging")
		
		
include_directories(${GASS_COMMON_INCLUDE_DIRS} ${OGRE_SDK_INCLUDE_DIRS})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#Add some foldets where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS} "${OGRE_DIR}/lib")
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

#Add project dependency libs
set(PROJ_LINK_LIBRARIES optimized OgreMain 
	optimized OgrePaging 
	optimized OgreTerrain 
	optimized OgreOverlay 
	debug OgreMain_d 
	debug OgrePaging_d 
	debug OgreTerrain_d 
	debug OgreOverlay_d
	)

target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_COMMON_LINK_LIBRARIES})
target_link_libraries(${LIB_NAME} ${PROJ_LINK_LIBRARIES})

install (TARGETS ${LIB_NAME} DESTINATION ${GASS_INSTALL_PLUGIN_LIB_DIR})		
#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include/plugins FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

