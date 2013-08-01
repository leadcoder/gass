#set lib name here!
set(LIB_NAME "GASSPluginOSG")

if(EXISTS "${GASS_DEPENDENCIES_DIR}/OpenSceneGraph-v3.0.1")
	set(OSG_DIR  "${GASS_DEPENDENCIES_DIR}/OpenSceneGraph-v3.0.1" CACHE PATH "OSG folder")
else()
	set(OSG_DIR $ENV{OSG_HOME} CACHE PATH "OSG folder")
endif()
		
include_directories(${GASS_COMMON_INCLUDE_DIRS} "${OSG_DIR}/include")

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#Add some foldets where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS} "${OSG_DIR}/lib")
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

#Add project dependency libs
set(PROJ_LINK_LIBRARIES debug OpenThreadsd
			debug osgd
			debug osgDBd
			debug osgUtild
			debug osgGAd
			debug osgViewerd
			debug osgTextd
			debug osgShadowd
			optimized OpenThreads
			optimized osg
			optimized osgDB
			optimized osgUtil
			optimized osgGA
			optimized osgViewer
			optimized osgText
			optimized osgShadow
			opengl32
			glu32)
	
target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_COMMON_LINK_LIBRARIES})
target_link_libraries(${LIB_NAME} ${PROJ_LINK_LIBRARIES})

install (TARGETS ${LIB_NAME} DESTINATION ${GASS_INSTALL_PLUGIN_LIB_DIR})		
#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include/plugins FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

