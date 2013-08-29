#set lib name here!
set(LIB_NAME "GASSPluginOpenAL")

#Set OIS include directories
set (OIS_INCLUDE_DIRS "${GASS_DEPENDENCIES_DIR}/OpenAL1.1/include")
		
include_directories(${GASS_COMMON_INCLUDE_DIRS} ${OIS_INCLUDE_DIRS})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#Add some folders where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

#Add project dependency libs
set(PROJ_LINK_LIBRARIES OpenAL32)  
	
target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_COMMON_LINK_LIBRARIES})
target_link_libraries(${LIB_NAME} ${PROJ_LINK_LIBRARIES})

gass_install_plugin_target()

