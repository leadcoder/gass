#set lib name here!
set(LIB_NAME "GASSPluginOIS")

#Set OIS include directiores
set (OIS_INCLUDE_DIRS "${GASS_DEPENDENCIES_DIR}/ois/includes")
		
include_directories(${GASS_COMMON_INCLUDE_DIRS} ${OIS_INCLUDE_DIRS})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#Add some foldets where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

#Add project dependency libs
set(PROJ_LINK_LIBRARIES optimized OIS debug OIS_d)

target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_COMMON_LINK_LIBRARIES})
target_link_libraries(${LIB_NAME} ${PROJ_LINK_LIBRARIES})

install (TARGETS ${LIB_NAME} DESTINATION ${GASS_INSTALL_PLUGIN_LIB_DIR})		
#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include/plugins FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

