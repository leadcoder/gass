#set lib name here!
set(LIB_NAME "PhysicsDemo")

include(TBBDepModule)
include(OpenALDepModule)
include(OISDepModule)
include(OSGDepModule)
include(PhysXDepModule)
include(OgreDepModule)
include(MyGUIDepModule)


#Set include directories
include_directories(${GASS_COMMON_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/Samples/sim_demo/source")
include(OgreDepModule)
include(SkyXDepModule)
include(OpenALDepModule)

set(CPP_FILES "source/PhysicsDemo.cpp")
set(H_FILES "")

#Add some folders where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS})

add_executable (${LIB_NAME} ${CPP_FILES} ${H_FILES})
set_target_properties(${LIB_NAME} PROPERTIES DEBUG_POSTFIX _d)

#Add project dependency libs
target_link_libraries(${LIB_NAME} GASSCore GASSSim  ${GASS_COMMON_LINK_LIBRARIES})

add_dependencies(${LIB_NAME} GASSCore 
		GASSSim 
		GASSPluginODE 
		GASSPluginOgre
		GASSPluginOSG
		GASSPluginOIS
		GASSPluginODE
		GASSPluginPagedGeometry
		GASSPluginEnvironment
		GASSPluginOpenAL
		GASSPluginGame
		GASSPluginBase
		GASSPluginRakNet)


set(DEP_DLL_FILES_RELEASE 
	${TBB_BIN_FILES_RELEASE}
	${OPENAL_BIN_FILES_RELEASE}
	${OIS_BIN_FILES_RELEASE}
	${MYGUI_BIN_FILES_RELEASE}
	${OGRE_BIN_FILES_RELEASE}
	${PHYSX_BIN_FILES_RELEASE}
	${OSG_BIN_FILES_RELEASE})

set(DEP_DLL_FILES_DEBUG 
	${TBB_BIN_FILES_DEBUG}
	${OPENAL_BIN_FILES_DEBUG}
	${OIS_BIN_FILES_DEBUG}
	${MYGUI_BIN_FILES_DEBUG}
	${OGRE_BIN_FILES_DEBUG}
	${PHYSX_BIN_FILES_DEBUG}
	${OSG_BIN_FILES_DEBUG})
	

#add dll files to out dir to enable debugging from IDE without install step
foreach(DLL_FILE ${DEP_DLL_FILES_DEBUG})
	ADD_CUSTOM_COMMAND(TARGET ${LIB_NAME}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLL_FILE} $<TARGET_FILE_DIR:${LIB_NAME}>)
endforeach()


foreach(DLL_FILE ${DEP_DLL_FILES_RELEASE})
	ADD_CUSTOM_COMMAND(TARGET ${LIB_NAME}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLL_FILE} $<TARGET_FILE_DIR:${LIB_NAME}>)
endforeach()
			
ADD_CUSTOM_COMMAND(TARGET ${LIB_NAME}
			POST_BUILD
			#COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/configuration
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/bin/configuration $<TARGET_FILE_DIR:${LIB_NAME}>
			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../data
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../common/data ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../data)

#install exe			

install(TARGETS ${LIB_NAME}  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)
install(TARGETS ${LIB_NAME}  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)

#install config
FILE(GLOB GASS_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/bin/Configuration/*.xml")
install(FILES ${GASS_CONFIG} DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG})
install(FILES ${GASS_CONFIG} DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE})

#install data
install(DIRECTORY ../common/data DESTINATION ${GASS_INSTALL_SAMPLES_DIR}/../ FILES_MATCHING PATTERN "*.*" PATTERN ".svn" EXCLUDE)

#install dlls
install (FILES  ${DEP_DLL_FILES_DEBUG} DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)
install (FILES  ${DEP_DLL_FILES_RELEASE} DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)

