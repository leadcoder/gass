#set lib name here!
set(LIB_NAME "GASSEditorModule")

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 

add_library (${LIB_NAME} ${GASS_BUILDTYPE}  ${CPP_FILES} ${H_FILES})
#Add project dependency libs
target_link_libraries(${LIB_NAME} GASSSim)
target_compile_definitions(${LIB_NAME} PRIVATE ${GASS_COMMON_DEFINITIONS} EDITOR_MODULE_EXPORTS)

gass_install_target()
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include/Modules FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

