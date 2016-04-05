#set lib name here!
set(LIB_NAME "GASSCore")

#Set include directories
include_directories(${GASS_CORE_INCLUDE_DIRS})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export
add_definitions( -DGASS_CORE_EXPORTS )
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

if(WIN32)
   #Add dependency for timer
   set(GASS_CORE_LIBRARIES Winmm ${GASS_CORE_LIBRARIES})
endif()
target_link_libraries(${LIB_NAME} ${GASS_CORE_LIBRARIES})

#Install section
gass_install_target()
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${GASS_INSTALL_INCLUDE_DIR} FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

