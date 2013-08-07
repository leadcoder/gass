#set lib name here!
set(LIB_NAME "SimDemo")

#Set include directiores
include_directories(${GASS_COMMON_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/Samples/sim_demo/source")

set(CPP_FILES "source/SimDemo.cpp")
set(H_FILES "source/Client.h" "source/Server.h" "source/SimApplication.h")


#change output folder to local project folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/samples_output/${LIB_NAME}) #dlls and exe

#Add some foldets where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS})



add_executable (${LIB_NAME} ${CPP_FILES} ${H_FILES})
set_target_properties(${LIB_NAME} PROPERTIES DEBUG_POSTFIX _d)


#Add project dependency libs
target_link_libraries(${LIB_NAME} GASSCore GASSSim  ${GASS_COMMON_LINK_LIBRARIES})

add_dependencies(${LIB_NAME} GASSCore 
		GASSSim 
		GASSPluginODE 
		GASSPluginOgre
		GASSPluginOIS
		GASSPluginODE
		GASSPluginPagedGeometry
		GASSPluginEnvironment
		GASSPluginOpenAL
		GASSPluginGame
		GASSPluginBase
		GASSPluginRakNet)

if(EXISTS "${GASS_DEPENDENCIES_DIR}/ogre-v1-9")
	set(OGRE_DIR  "${GASS_DEPENDENCIES_DIR}/ogre-v1-9" CACHE PATH "Ogre 3D folder")
else()
	set(OGRE_DIR $ENV{OGRE_HOME} CACHE PATH "Ogre 3D folder")
endif()

	set(DEP_DLL_FILES 
	${GASS_DEPENDENCIES_DIR}/lib/tbb.dll 
	${GASS_DEPENDENCIES_DIR}/lib/OpenAL32.dll
	${GASS_DEPENDENCIES_DIR}/lib/OIS.dll
	${OGRE_DIR}/bin/release/OgreMain.dll
	${OGRE_DIR}/bin/release/OgreTerrain.dll
	${OGRE_DIR}/bin/release/OgrePaging.dll
	${OGRE_DIR}/bin/release/OgreOverlay.dll
	${OGRE_DIR}/bin/release/Plugin_CgProgramManager.dll
	${OGRE_DIR}/bin/release/OgreOverlay.dll
	${OGRE_DIR}/bin/release/Plugin_OctreeSceneManager.dll
	${OGRE_DIR}/bin/release/Plugin_ParticleFX.dll
	${OGRE_DIR}/bin/release/RenderSystem_Direct3D9.dll
	${OGRE_DIR}/bin/release/RenderSystem_GL.dll
	${OGRE_DIR}/bin/release/cg.dll
	${GASS_DEPENDENCIES_DIR}/lib/tbb_debug.dll 
	${GASS_DEPENDENCIES_DIR}/lib/OIS_d.dll
	${OGRE_DIR}/bin/debug/OgreMain_d.dll
	${OGRE_DIR}/bin/debug/OgreTerrain_d.dll
	${OGRE_DIR}/bin/debug/OgrePaging_d.dll
	${OGRE_DIR}/bin/debug/OgreOverlay_d.dll
	${OGRE_DIR}/bin/debug/Plugin_CgProgramManager_d.dll
	${OGRE_DIR}/bin/debug/OgreOverlay_d.dll
	${OGRE_DIR}/bin/debug/Plugin_OctreeSceneManager_d.dll
	${OGRE_DIR}/bin/debug/Plugin_ParticleFX_d.dll
	${OGRE_DIR}/bin/debug/RenderSystem_Direct3D9_d.dll
	${OGRE_DIR}/bin/debug/RenderSystem_GL_d.dll)
	
	
	set(GASS_RELEASE_DLL_FILES 
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/GASSCore.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/GASSSim.dll 
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/GASSPluginOgre.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/GASSPluginOIS.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/GASSPluginODE.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/GASSPluginPagedGeometry.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/GASSPluginEnvironment.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/GASSPluginOpenAL.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/GASSPluginGame.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/GASSPluginBase.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/GASSPluginRakNet.dll)
	set(GASS_DEBUG_DLL_FILES 
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/GASSCore_d.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/GASSSim_d.dll 
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/GASSPluginOgre_d.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/GASSPluginOIS_d.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/GASSPluginODE_d.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/GASSPluginPagedGeometry_d.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/GASSPluginEnvironment_d.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/GASSPluginOpenAL_d.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/GASSPluginGame_d.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/GASSPluginBase_d.dll
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/GASSPluginRakNet_d.dll)


#set(ALL_DLL_FILES 
#	${DEP_DLL_FILES}
#	${GASS_DLL_FILES}
#	)

#add dll files to out dir to enable debugging from IDE without install step
#foreach(DLL_FILE ${DEP_DLL_FILES})
#	ADD_CUSTOM_COMMAND(TARGET ${LIB_NAME}
#			POST_BUILD
#			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLL_FILE} $<TARGET_FILE_DIR:${LIB_NAME}>)
#endforeach()

add_custom_command(TARGET ${LIB_NAME}
            POST_BUILD
            COMMAND cmd /C ${CMAKE_CURRENT_SOURCE_DIR}/scripts/copy_gass_dlls.bat $<CONFIGURATION> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} $<TARGET_FILE_DIR:${LIB_NAME}>)
			
add_custom_command(TARGET ${LIB_NAME}
            POST_BUILD
            COMMAND cmd /C ${CMAKE_CURRENT_SOURCE_DIR}/scripts/copy_ogre_dlls.bat $<CONFIGURATION> ${OGRE_DIR}/bin $<TARGET_FILE_DIR:${LIB_NAME}>)

add_custom_command(TARGET ${LIB_NAME}
            POST_BUILD
            COMMAND cmd /C ${CMAKE_CURRENT_SOURCE_DIR}/scripts/copy_dep_dlls.bat $<CONFIGURATION> ${GASS_DEPENDENCIES_DIR}/lib/ $<TARGET_FILE_DIR:${LIB_NAME}>)
			
	ADD_CUSTOM_COMMAND(TARGET ${LIB_NAME}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/configuration
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/bin/configuration ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/configuration
			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../data
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../common/data ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../data)

#install exe			
install (TARGETS ${LIB_NAME} DESTINATION ${GASS_INSTALL_SAMPLES_DIR}/${LIB_NAME}/bin)

#install config
FILE(GLOB GASS_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/bin/Configuration/*.xml")
install(FILES ${GASS_CONFIG} DESTINATION ${GASS_INSTALL_SAMPLES_DIR}/${LIB_NAME}/Configuration)

#install data
install(DIRECTORY ../common/data DESTINATION ${GASS_INSTALL_SAMPLES_DIR} FILES_MATCHING PATTERN "*.*" PATTERN ".svn" EXCLUDE)

#install dlls
install (FILES  ${DEP_DLL_FILES} DESTINATION ${GASS_INSTALL_SAMPLES_DIR}/${LIB_NAME}/bin)

install (FILES  ${GASS_DEBUG_DLL_FILES} DESTINATION ${GASS_INSTALL_SAMPLES_DIR}/${LIB_NAME}/bin CONFIGURATIONS Debug)
install (FILES  ${GASS_RELEASE_DLL_FILES} DESTINATION ${GASS_INSTALL_SAMPLES_DIR}/${LIB_NAME}/bin CONFIGURATIONS Release)

#install (CODE  "cmd /C ${CMAKE_CURRENT_SOURCE_DIR}/scripts/copy_gass_dlls.bat $<CONFIGURATION> ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}  ${GASS_INSTALL_SAMPLES_DIR}/${LIB_NAME}/bin")


