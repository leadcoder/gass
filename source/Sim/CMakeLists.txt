#set lib name here!
set(LIB_NAME "GASSSim")

find_package(AngelScript REQUIRED)

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
#add_definitions( -DGASS_EXPORTS )

#only support shared lib for now
add_library (${LIB_NAME} ${GASS_BUILDTYPE}  ${CPP_FILES} ${H_FILES})

target_include_directories(${LIB_NAME}
	PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
	PUBLIC  $<BUILD_INTERFACE:${ANGELSCRIPT_INCLUDE_DIRS}>
    #$<INSTALL_INTERFACE:${lib_INSTALL_HEADER_DIRECTORY}>  
)

#Add project dependency libs
target_link_libraries(${LIB_NAME} PUBLIC GASSCore  PRIVATE ${ANGELSCRIPT_LIBRARIES} uuid)

if(NOT WIN32)
   #Add dependency guid
   target_link_libraries(${LIB_NAME} PRIVATE uuid)
endif()

target_compile_definitions(${LIB_NAME} PRIVATE GASS_EXPORTS ${GASS_COMMON_DEFINITIONS} )
		
if(WIN32) #If we are going to use this globally we need to move this into macro
	set_target_properties (${LIB_NAME} PROPERTIES LINK_FLAGS_RELEASE "/LTCG" )
endif()

#install section
gass_install_target()
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${GASS_INSTALL_INCLUDE_DIR} FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

