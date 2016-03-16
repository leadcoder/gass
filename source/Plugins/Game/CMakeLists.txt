#set lib name here!
set(LIB_NAME "GASSPluginGame")

#Set include directories
include_directories(${GASS_SIM_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/source/Plugins/Game")

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Add some folders where to search for dependency libraries
link_directories(${GASS_SIM_LIB_DIRS})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

#Add project dependency libs
target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_SIM_LIBRARIES})

gass_install_plugin_target()		
#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include/plugins FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)
install(FILES  ${CMAKE_CURRENT_SOURCE_DIR}/GameMessages.h DESTINATION ${GASS_INSTALL_INCLUDE_DIR}/plugins/Game)
install(FILES  ${CMAKE_CURRENT_SOURCE_DIR}/DetectionData.h DESTINATION ${GASS_INSTALL_INCLUDE_DIR}/plugins/Game)
install(FILES  ${CMAKE_CURRENT_SOURCE_DIR}/PlatformType.h DESTINATION ${GASS_INSTALL_INCLUDE_DIR}/plugins/Game)

