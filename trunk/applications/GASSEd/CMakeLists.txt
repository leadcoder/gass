cmake_minimum_required(VERSION 2.8)
set(APP_NAME "GASSEd")
project(${APP_NAME})

set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/../../CMake"
  )

set(GASS_DIR $ENV{GASS_HOME} CACHE PATH "GASS folder")
set(GASS_DEPENDENCIES_DIR $ENV{GASS_DEPENDENCIES} CACHE PATH "GASS 3rd-party dependency folder")
set(QT_DIR $ENV{QT_DIR} CACHE PATH "QT4 folder")

set(QTPROPERTYBROWSER_DIR ${GASS_DEPENDENCIES_DIR}/qtpropertybrowser-2.5_1)
 
find_package(GASSSim COMPONENTS GASSEditorModule 
								GASSPluginOgre 
								GASSPluginOIS 
								GASSPluginPhysX3 
								GASSPluginODE 
								GASSPluginPagedGeometry
								GASSPluginOpenAL
								GASSPluginGame
								GASSPluginBase
								GASSPluginEnvironment
								GASSPluginRecastNavigation
								GASSPluginRoadNetwork)
find_package(Qt4 REQUIRED)
find_package(QTPropertyBrowser)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) #build dlls and exe here
set(CMAKE_DEBUG_POSTFIX  "_d" CACHE STRING "add a postfix, usually d on windows")

set(GASSED_INSTALL_BIN_DIR  "bin")
set(GASSED_INSTALL_BIN_DIR_DEBUG "bin/debug")
set(GASSED_INSTALL_BIN_DIR_RELEASE "bin/release")

#set(GASS_COMMON_INCLUDE_DIRS ${BOOST_INC_DIR}  "${GASS_DIR}/include" ${TINYXML2_INC_DIR} "${GASS_DEPENDENCIES_DIR}/tbb/include" ${ANGELSCRIPT_INC_DIR})
#set(GASS_COMMON_LIB_DIRS ${BOOST_LIB_DIR} "${GASS_DEPENDENCIES_DIR}/lib/" ${TINYXML2_LIB_DIR} ${ANGELSCRIPT_LIB_DIR} "${GASS_DIR}/lib/")
#set(GASS_COMMON_LINK_LIBRARIES
#	optimized GASSCore
#	optimized GASSSim
#	optimized GASSEditorModule
#	debug GASSCore_d
#	debug GASSSim_d
#	debug GASSEditorModule_d
#	${TINYXML2_LINK_LIBRARIES}
#	)
	

set(GASSED_H 
	GASSEd.h
	GASSBrushSettingsWidget.h
	GASSPropertyWidget.h
	GASSRenderWidget.h
	GASSResourceTreeWidget.h
	GASSSceneSelectionWidget.h
	GASSSceneTreeWidget.h
	GASSSSaveSceneWidget.h
	GASSTools.h)
set(GASSED_RESOURCES GASSEd.qrc)
qt4_wrap_cpp(GASSED_MOC_H ${GASSED_H})
QT4_ADD_RESOURCES(GASSED_RESOURCES_RCC ${GASSED_RESOURCES})

set(GASSED_CPP main.cpp
	GASSEd.cpp
	GASSBrushSettingsWidget.cpp
	GASSPropertyWidget.cpp
	GASSRenderWidget.cpp
	GASSResourceTreeWidget.cpp
	GASSSceneSelectionWidget.cpp
	GASSSceneTreeWidget.cpp
	GASSSSaveSceneWidget.cpp
	GASSTools.cpp)


add_definitions( -D_SCL_SECURE_NO_WARNINGS -DQT_QTPROPERTYBROWSER_IMPORT)

link_directories(${GASS_DEPENDENCIES_DIR}/qtpropertybrowser-2.5_1/lib)
set_target_properties(${LIB_NAME} PROPERTIES DEBUG_POSTFIX _d)

include(${QT_USE_FILE})

include_directories(${GASS_SIM_INCLUDE_DIRS} 
					${QTPROPERTYBROWSER_DIR_INCLUDE_DIRS})

add_executable(${APP_NAME} ${GASSED_CPP} ${GASSED_MOC_H} ${GASSED_RESOURCES_RCC})
target_link_libraries(${APP_NAME} ${QT_LIBRARIES} 
					${GASS_SIM_LIBRARIES}
					${QTPROPERTYBROWSER_LIBRARIES}					
					)

FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Configuration DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#FILE(GLOB GASSED_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/Configuration/*.xml")					
#FILE(COPY ${GASSED_CONFIG}  DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
#FILE(COPY ${GASSED_CONFIG}  DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)

if(WIN32)
	#HACK to get qt-binaries, findqt dont return binaries nor root/install path, 
	#we use the QT_DOC_DIR for now as to get root level..

	set(QT_BINARIES_REL ${QT_DOC_DIR}/../bin/QtCore4.dll
	${QT_DOC_DIR}/../bin/QtGui4.dll
	${QTPROPERTYBROWSER_BINARY_REL})
	
	set(QT_BINARIES_DBG ${QT_DOC_DIR}/../bin/QtCored4.dll
	${QT_DOC_DIR}/../bin/QtGuid4.dll
	${QTPROPERTYBROWSER_BINARY_DBG})
		
	set(GASSED_BINARIES_REL ${GASS_SIM_BINARIES_REL} ${GASS_PLUGIN_BINARIES_REL} ${QT_BINARIES_REL})
	set(GASSED_BINARIES_DBG ${GASS_SIM_BINARIES_DBG} ${GASS_PLUGIN_BINARIES_REL} ${QT_BINARIES_DBG})
	FILE(COPY ${GASSED_BINARIES_REL}  DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	FILE(COPY ${GASSED_BINARIES_DBG}  DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
endif()

# ADD_CUSTOM_COMMAND(TARGET ${APP_NAME}
			# POST_BUILD
			# COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Configuration ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Configuration
			# COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../../samples/common/data ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data
			# COMMAND ${CMAKE_COMMAND} -E copy_directory ${OSG_PLUGINS_DIR} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug
			# COMMAND ${CMAKE_COMMAND} -E copy_directory ${OSG_PLUGINS_DIR} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release
			# )
			

#gather all dlls needed for GASSED to run
if(WIN32)
	# find_package(TBB)
	
	# set(OPENAL_BIN_DIR  "${GASS_DEPENDENCIES_DIR}/OpenAL1.1/bin" CACHE PATH "OpenAL bin folder")
	# find_package(OpenALExt)
	
	# set (OIS_DIR "${GASS_DEPENDENCIES_DIR}/ois")
	# find_package(OIS)
	
	# set(SKYX_DIR  "${GASS_DEPENDENCIES_DIR}/SkyX-v0.4" CACHE PATH "SkyX folder")
	# find_package(SkyX)
	
	# find_package(Ogre)
	# set(OGRE_BIN_FILES_RELEASE 
		# ${OGRE_BINARY_REL}
		# ${OGRE_Overlay_BINARY_REL}
		# ${OGRE_Paging_BINARY_REL}
		# ${OGRE_Terrain_BINARY_REL}
		# ${OGRE_Plugin_CgProgramManager_REL}
		# ${OGRE_Plugin_OctreeSceneManager_REL}
		# ${OGRE_Plugin_ParticleFX_REL}
		# ${OGRE_RenderSystem_Direct3D9_REL}
		# ${OGRE_RenderSystem_GL_REL}
		# ${OGRE_PLUGIN_DIR_REL}/cg.dll)
		
		# set(OGRE_BIN_FILES_DEBUG 
		# ${OGRE_BINARY_DBG}
		# ${OGRE_Overlay_BINARY_DBG}
		# ${OGRE_Paging_BINARY_DBG}
		# ${OGRE_Terrain_BINARY_DBG}
		# ${OGRE_Plugin_CgProgramManager_DBG}
		# ${OGRE_Plugin_OctreeSceneManager_DBG}
		# ${OGRE_Plugin_ParticleFX_DBG}
		# ${OGRE_RenderSystem_Direct3D9_DBG}
		# ${OGRE_RenderSystem_GL_DBG}
		# ${OGRE_PLUGIN_DIR_DBG}/cg.dll)
		
	# set(ALL_BIN_FILES_RELEASE 
		# ${OPENAL_BIN_FILES_RELEASE}
		# ${SKYX_BINARY_REL}
		# ${OGRE_BIN_FILES_RELEASE}
		# ${TBB_BINARIES_REL}
		# ${OIS_BINARY_REL}
		# ${GASS_DEPENDENCIES_DIR}/qtpropertybrowser-2.5_1/lib/QtSolutions_PropertyBrowser-2.5.dll
		# ${GASS_DIR}/bin/release/GASSCore.dll
	# ${GASS_DIR}/bin/release/GASSSim.dll 
	# ${GASS_DIR}/bin/release/GASSPluginOgre.dll
	# ${GASS_DIR}/bin/release/GASSPluginOSG.dll
	# ${GASS_DIR}/bin/release/GASSPluginPhysX3.dll
	# ${GASS_DIR}/bin/release/GASSPluginOIS.dll
	# ${GASS_DIR}/bin/release/GASSPluginODE.dll
	# ${GASS_DIR}/bin/release/GASSPluginPagedGeometry.dll
	# ${GASS_DIR}/bin/release/GASSPluginEnvironment.dll
	# ${GASS_DIR}/bin/release/GASSPluginOpenAL.dll
	# ${GASS_DIR}/bin/release/GASSPluginGame.dll
	# ${GASS_DIR}/bin/release/GASSPluginRoadNetwork.dll
	# ${GASS_DIR}/bin/release/GASSPluginBase.dll
	# ${GASS_DIR}/bin/release/GASSPluginRakNet.dll
	# ${GASS_DIR}/bin/release/GASSPluginRecastNavigation.dll
	# ${GASS_DIR}/bin/release/GASSEditorModule.dll
	# ${QT_DOC_DIR}/../bin/QtCore4.dll
	# ${QT_DOC_DIR}/../bin/QtGui4.dll
	
	
		# )
	# set(ALL_BIN_FILES_DEBUG	
		# ${OPENAL_BIN_FILES_DEBUG}
		# ${SKYX_BINARY_DBG}
		# ${OGRE_BIN_FILES_DEBUG}
		# ${TBB_BINARY_DBG}
		# ${OIS_BINARY_DBG}
		# ${GASS_DEPENDENCIES_DIR}/qtpropertybrowser-2.5_1/lib/QtSolutions_PropertyBrowser-2.5d.dll
		# ${GASS_DIR}/bin/debug/GASSCore_d.dll
	# ${GASS_DIR}/bin/debug/GASSSim_d.dll 
	# ${GASS_DIR}/bin/debug/GASSPluginOgre_d.dll
	# ${GASS_DIR}/bin/debug/GASSPluginOSG_d.dll
	# ${GASS_DIR}/bin/debug/GASSPluginOIS_d.dll
	# ${GASS_DIR}/bin/debug/GASSPluginODE_d.dll
	# ${GASS_DIR}/bin/debug/GASSPluginPhysx3_d.dll
	# ${GASS_DIR}/bin/debug/GASSPluginPagedGeometry_d.dll
	# ${GASS_DIR}/bin/debug/GASSPluginEnvironment_d.dll
	# ${GASS_DIR}/bin/debug/GASSPluginOpenAL_d.dll
	# ${GASS_DIR}/bin/debug/GASSPluginGame_d.dll
	# ${GASS_DIR}/bin/debug/GASSPluginRoadNetwork_d.dll
	# ${GASS_DIR}/bin/debug/GASSPluginBase_d.dll
	# ${GASS_DIR}/bin/debug/GASSPluginRakNet_d.dll
	# ${GASS_DIR}/bin/debug/GASSPluginRecastNavigation_d.dll
	# ${GASS_DIR}/bin/debug/GASSEditorModule_d.dll
	# ${QT_DOC_DIR}/../bin/QtCored4.dll
	# ${QT_DOC_DIR}/../bin/QtGuid4.dll
		# )
	# FILE(COPY ${ALL_BIN_FILES_RELEASE}  DESTINATION  ${CMAKE_BINARY_DIR}/out/release)
	# FILE(COPY ${ALL_BIN_FILES_DEBUG}  DESTINATION  ${CMAKE_BINARY_DIR}/out/debug)
endif()

	
	# FILE(COPY ${OSG_PLUGINS}  DESTINATION  ${CMAKE_BINARY_DIR}/out/debug)

#add dll files to out dir to enable debugging from IDE without install step
# foreach(DLL_FILE ${DEP_DLL_FILES_DEBUG})
	# ADD_CUSTOM_COMMAND(TARGET ${APP_NAME}
			# POST_BUILD
			# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLL_FILE} $<TARGET_FILE_DIR:${APP_NAME}>)
# endforeach()

# foreach(DLL_FILE ${DEP_DLL_FILES_RELEASE})
	# ADD_CUSTOM_COMMAND(TARGET ${APP_NAME}
			# POST_BUILD
			# COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLL_FILE} $<TARGET_FILE_DIR:${APP_NAME}>)
# endforeach()

install(TARGETS ${APP_NAME} RUNTIME DESTINATION ${GASSED_INSTALL_BIN_DIR} CONFIGURATIONS Release)
#install(FILES  ${DEP_DLL_FILES_RELEASE} DESTINATION ${GASSED_INSTALL_BIN_DIR} CONFIGURATIONS Release)

install(FILES ${GASSED_CONFIG} DESTINATION ${GASSED_INSTALL_BIN_DIR}/../Configuration)
#install(FILES ${OSG_PLUGINS} DESTINATION ${GASSED_INSTALL_BIN_DIR}/${OSG_PLUGINS_DIR_NAME})
