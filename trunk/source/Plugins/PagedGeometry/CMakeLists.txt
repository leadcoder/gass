#set lib name here!
set(LIB_NAME "GASSPluginPagedGeometry")

if(EXISTS "${GASS_DEPENDENCIES_DIR}/ogre")
	set(OGRE_DIR  "${GASS_DEPENDENCIES_DIR}/ogre" CACHE PATH "Ogre 3D folder")
else()
	set(OGRE_DIR $ENV{OGRE_HOME} CACHE PATH "Ogre 3D folder")
endif()

#Set include directiores
set (DEP_INCLUDE_DIRS "${OGRE_DIR}/include/OGRE"
		"${GASS_DEPENDENCIES_DIR}/PagedGeometry-1.1.1/include")
		
include_directories(${GASS_COMMON_INCLUDE_DIRS} ${DEP_INCLUDE_DIRS})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#Add some foldets where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS} "${OGRE_DIR}/lib")
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

#Add project dependency libs
set(OGRE_LINK_LIBRARIES optimized OgreMain 
	optimized OgrePaging 
	optimized PagedGeometry
	debug OgreMain_d 
	debug PagedGeometry_d
	)

target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_COMMON_LINK_LIBRARIES})
target_link_libraries(${LIB_NAME} ${OGRE_LINK_LIBRARIES})

install (TARGETS ${LIB_NAME} DESTINATION ${GASS_INSTALL_PLUGIN_LIB_DIR})		
#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include/plugins FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

