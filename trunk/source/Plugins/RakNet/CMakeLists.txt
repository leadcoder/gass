#set lib name here!
set(LIB_NAME "GASSPluginRaknet")
		
include_directories(${GASS_COMMON_INCLUDE_DIRS} "${GASS_DEPENDENCIES_DIR}/RakNet/Source")

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#Add some foldets where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

#Add project dependency libs
set(PROJ_LINK_LIBRARIES ws2_32 optimized RakNet debug RakNet_d)

target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_COMMON_LINK_LIBRARIES})
target_link_libraries(${LIB_NAME} ${PROJ_LINK_LIBRARIES})

gass_install_plugin_target()

