
set(PLATFORM_DEPS "")
if(NOT WIN32)
   #Add dependency guid
   set(PLATFORM_DEPS uuid)
endif()

gass_get_source_from_current_dir(CPP_FILES H_FILES)
gass_setup_lib(GASSSim
					BUILDTYPE ${GASS_BUILDTYPE}
					SOURCE_FILES ${CPP_FILES} 
					HEADER_FILES ${H_FILES}
					PUBLIC_DEPS GASSCore AngelScript TBB
					PRIVATE_DEPS ${PLATFORM_DEPS}
					PRIVATE_DEFINITIONS GASS_EXPORTS ${GASS_COMMON_DEFINITIONS} )
					
# add_library (${LIB_NAME} ${GASS_BUILDTYPE}  ${CPP_FILES} ${H_FILES})

# target_include_directories(${LIB_NAME}
	# PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
	# PUBLIC  $<BUILD_INTERFACE:${ANGELSCRIPT_INCLUDE_DIRS}> 
# )

#Add project dependency libs
#target_link_libraries(${LIB_NAME} PUBLIC GASSCore  PRIVATE ${ANGELSCRIPT_LIBRARIES} uuid)

# if(NOT WIN32)
   #Add dependency guid
   # target_link_libraries(${LIB_NAME} PRIVATE uuid)
# endif()

# target_compile_definitions(${LIB_NAME} PRIVATE GASS_EXPORTS ${GASS_COMMON_DEFINITIONS} )
		
# if(WIN32) #If we are going to use this globally we need to move this into macro
	# set_target_properties (${LIB_NAME} PROPERTIES LINK_FLAGS_RELEASE "/LTCG" )
# endif()

# gass_install_target()
# install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${GASS_INSTALL_INCLUDE_DIR} FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

