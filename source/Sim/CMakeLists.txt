#set lib name here!
set(LIB_NAME "GASSSim")

find_package(GASSSimThirdParty)

#set(GASS_SIM_INCLUDE_DIRS ${GASS_CORE_INCLUDE_DIRS} ${ANGELSCRIPT_INCLUDE_DIRS})
#set(GASS_SIM_LIB_DIRS ${GASS_CORE_LIB_DIRS})
#set(GASS_SIM_LIBRARIES ${GASS_CORE_LIBRARIES} ${ANGELSCRIPT_LIBRARIES})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_EXPORTS )

#Add some folders where to search for dependency libraries
link_directories(${GASS_SIM_LIB_DIRS})

#only support shared lib for now
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

target_include_directories(${LIB_NAME}
	PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
	PUBLIC  $<BUILD_INTERFACE:${ANGELSCRIPT_INCLUDE_DIRS}>
    #$<INSTALL_INTERFACE:${lib_INSTALL_HEADER_DIRECTORY}>  
)

#Add project dependency libs
target_link_libraries(${LIB_NAME} PUBLIC GASSCore  PRIVATE ${ANGELSCRIPT_LIBRARIES})

if(WIN32) #If we are going to use this globally we need to move this into macro
	set_target_properties (${LIB_NAME} PROPERTIES LINK_FLAGS_RELEASE "/LTCG" )
endif()

#install section
gass_install_target()
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${GASS_INSTALL_INCLUDE_DIR} FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

