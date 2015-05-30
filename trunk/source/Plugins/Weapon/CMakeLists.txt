#set lib name here!
set(LIB_NAME "GASSPluginWeapon")

#Set include directories
include_directories(${GASS_SIM_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/source/Plugins/Weapon")

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Add some folders where to search for dependency libraries
link_directories(${GASS_SIM_LIB_DIRS})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

#Add project dependency libs
target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_SIM_LIBRARIES})

gass_install_plugin_target()		
