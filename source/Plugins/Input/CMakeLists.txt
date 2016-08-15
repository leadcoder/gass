#set lib name here!
set(LIB_NAME "GASSPluginInput")

#add all source files from current directory (recursively)
add_source_from_current_dir()

add_library (${LIB_NAME} ${GASS_BUILDTYPE}  ${CPP_FILES} ${H_FILES})
target_link_libraries(${LIB_NAME} GASSSim)
target_compile_definitions(${LIB_NAME} PRIVATE ${GASS_COMMON_DEFINITIONS} )

gass_install_plugin_target()		
