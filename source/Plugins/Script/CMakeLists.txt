#set lib name here!
set(LIB_NAME "GASSPluginScript")


#Set ogre sdk include directories
set (LUA_INCLUDE_DIRS "${GASS_DEPENDENCIES_DIR}/lua/include")
		
		
include_directories(${GASS_SIM_INCLUDE_DIRS} ${LUA_INCLUDE_DIRS})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS)

#Add some folders where to search for dependency libraries
link_directories(${GASS_SIM_LIB_DIRS})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

#Add project dependency libs
set(PROJ_LINK_LIBRARIES optimized lua debug lua_d)

target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_SIM_LIBRARIES})
target_link_libraries(${LIB_NAME} ${PROJ_LINK_LIBRARIES})

gass_install_plugin_target()

