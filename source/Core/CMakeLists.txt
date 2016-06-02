#set lib name here!
set(LIB_NAME "GASSCore")

find_package(GASSCoreThirdParty)

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export
add_definitions( -DGASS_CORE_EXPORTS )
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

target_include_directories(${LIB_NAME}
	PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
	PUBLIC  $<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>
	PUBLIC  $<BUILD_INTERFACE:${TINYXML2_INCLUDE_DIRS}>
	PUBLIC  $<BUILD_INTERFACE:${TBB_INCLUDE_DIRS}>
  #$<INSTALL_INTERFACE:${lib_INSTALL_HEADER_DIRECTORY}>  
)

target_link_libraries(${LIB_NAME} 
		PUBLIC ${Boost_LIBRARIES}
		PUBLIC ${TINYXML2_LIBRARIES}
		PUBLIC ${TBB_LIBRARIES})
if(WIN32)
   #Add dependency for timer
   target_link_libraries(${LIB_NAME} PUBLIC Winmm)
endif()

#Install section
gass_install_target()
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${GASS_INSTALL_INCLUDE_DIR} FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)


