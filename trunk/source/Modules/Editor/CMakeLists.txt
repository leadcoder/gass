#set lib name here!
set(LIB_NAME "GASSEditorModule")

#Set include directiores
include_directories(${GASS_COMMON_INCLUDE_DIRS})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DEDITOR_MODULE_EXPORTS )

#Add some foldets where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

#Add project dependency libs
target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_COMMON_LINK_LIBRARIES})

gass_install_target()		
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include/Modules FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

