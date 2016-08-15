#set lib name here!
set(LIB_NAME "GASSPluginBase")

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Add some folders where to search for dependency libraries
add_library (${LIB_NAME} ${GASS_BUILDTYPE}  ${CPP_FILES} ${H_FILES})

#Add project dependency libs
target_link_libraries(${LIB_NAME} GASSSim)

target_compile_definitions(${LIB_NAME} PRIVATE ${GASS_COMMON_DEFINITIONS} )

gass_install_plugin_target()
install(FILES  ${CMAKE_CURRENT_SOURCE_DIR}/CoreMessages.h DESTINATION ${GASS_INSTALL_INCLUDE_DIR}/plugins/Base)
