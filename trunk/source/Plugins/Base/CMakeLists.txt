#set lib name here!
set(LIB_NAME "GASSPluginBase")

#Set include directories
include_directories(${GASS_SIM_INCLUDE_DIRS})

#add all source files from current directory (recursively)
add_source_from_current_dir()


#Add some folders where to search for dependency libraries
link_directories(${GASS_SIM_LIB_DIRS})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

#Add project dependency libs
target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_SIM_LIBRARIES})

gass_install_plugin_target()
install(FILES  ${CMAKE_CURRENT_SOURCE_DIR}/CoreMessages.h DESTINATION ${GASS_INSTALL_INCLUDE_DIR}/plugins/Base)
