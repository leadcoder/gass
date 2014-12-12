cmake_minimum_required(VERSION 2.8)
set(APP_NAME "GASSEd")
project(${APP_NAME})

set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/../../CMake"
)

include(OSGDepModule)
include(TinyXML2DepModule)
include(BoostDepModule)
include(AngelScriptDepModule)
include(TBBDepModule)
include(SkyXDepModule)
include(PhysXDepModule)

set(QT_DIR $ENV{QT_DIR} CACHE PATH "QT4 folder")

set(GASS_DIR $ENV{GASS_HOME} CACHE PATH "GASS folder")
set(GASS_DEPENDENCIES_DIR $ENV{GASS_DEPENDENCIES} CACHE PATH "GASS 3rd-party dependency folder")


#msvc only, need other check if gcc
if(${CMAKE_CL_64})
	set(PHYSX_BIN_DIR "${PHYSX_DIR}/bin/win64")
	set(DLL_SUFFIX "_x64")
else()
	set(PHYSX_BIN_DIR "${PHYSX_DIR}/bin/win32")
	set(DLL_SUFFIX "_x86")
endif()

message( STATUS "CMAKE_CL_64: ${CMAKE_CL_64}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) #dlls and exe
set(CMAKE_DEBUG_POSTFIX  "_d" CACHE STRING "add a postfix, usually d on windows")

set(GASSED_INSTALL_BIN_DIR  "bin")
set(GASSED_INSTALL_BIN_DIR_DEBUG "bin/debug")
set(GASSED_INSTALL_BIN_DIR_RELEASE "bin/release")

set(GASS_COMMON_INCLUDE_DIRS ${BOOST_INC_DIR}  "${GASS_DIR}/include" ${TINYXML2_INC_DIR} ${TBB_INC_DIR} ${ANGELSCRIPT_INC_DIR})
set(GASS_COMMON_LIB_DIRS ${BOOST_LIB_DIR} "${GASS_DEPENDENCIES_DIR}/lib/" ${TINYXML2_LIB_DIR} ${ANGELSCRIPT_LIB_DIR} "${GASS_DIR}/lib/")

set(GASS_COMMON_LINK_LIBRARIES
	optimized GASSCore
	optimized GASSSim
	optimized GASSEditorModule
	debug GASSCore_d
	debug GASSSim_d
	debug GASSEditorModule_d
	${TINYXML2_LINK_LIBRARIES}
	)
	
find_package(Qt4 REQUIRED)
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

link_directories(${GASS_COMMON_LIB_DIRS} ${GASS_DEPENDENCIES_DIR}/qtpropertybrowser-2.5_1/lib)
set_target_properties(${LIB_NAME} PROPERTIES DEBUG_POSTFIX _d)
include(${QT_USE_FILE})
include_directories(${GASS_COMMON_INCLUDE_DIRS} ${GASS_DEPENDENCIES_DIR}/qtpropertybrowser-2.5_1/src)
add_executable(${APP_NAME} ${GASSED_CPP} ${GASSED_MOC_H} ${GASSED_RESOURCES_RCC})
target_link_libraries(${APP_NAME} ${QT_LIBRARIES} ${GASS_COMMON_LINK_LIBRARIES} debug QtSolutions_PropertyBrowser-2.5d optimized QtSolutions_PropertyBrowser-2.5)

ADD_CUSTOM_COMMAND(TARGET ${APP_NAME}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Configuration ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Configuration
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../../samples/common/data ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${OSG_PLUGINS_DIR} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${OSG_PLUGINS_DIR} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release
			)
			
			
set(DEP_DLL_FILES_RELEASE 
	${GASS_DEPENDENCIES_DIR}/qtpropertybrowser-2.5_1/lib/QtSolutions_PropertyBrowser-2.5.dll
	${TBB_BIN_FILES_RELEASE}
	${OPENAL_BIN_FILES_RELEASE}
	${OIS_BIN_FILES_RELEASE}
	${MYGUI_BIN_FILES_RELEASE}
	${OGRE_BIN_FILES_RELEASE}
	${OSG_BIN_FILES_RELEASE}
	${SKYX_BIN_FILES_RELEASE}
	${PHYSX_BIN_FILES_RELEASE}
	${OSG_BIN_FILES_RELEASE}
	${GASS_DIR}/bin/release/GASSCore.dll
	${GASS_DIR}/bin/release/GASSSim.dll 
	${GASS_DIR}/bin/release/GASSPluginOgre.dll
	${GASS_DIR}/bin/release/GASSPluginOSG.dll
	${GASS_DIR}/bin/release/GASSPluginPhysX3.dll
	${GASS_DIR}/bin/release/GASSPluginOIS.dll
	${GASS_DIR}/bin/release/GASSPluginODE.dll
	${GASS_DIR}/bin/release/GASSPluginPagedGeometry.dll
	${GASS_DIR}/bin/release/GASSPluginEnvironment.dll
	${GASS_DIR}/bin/release/GASSPluginOpenAL.dll
	${GASS_DIR}/bin/release/GASSPluginGame.dll
	${GASS_DIR}/bin/release/GASSPluginRoadNetwork.dll
	${GASS_DIR}/bin/release/GASSPluginBase.dll
	${GASS_DIR}/bin/release/GASSPluginRakNet.dll
	${GASS_DIR}/bin/release/GASSPluginRecastNavigation.dll
	${GASS_DIR}/bin/release/GASSEditorModule.dll
	${QT_DIR}/bin/QtCore4.dll
	${QT_DIR}/bin/QtGui4.dll
	
	)
	set(DEP_DLL_FILES_DEBUG 
	${GASS_DEPENDENCIES_DIR}/qtpropertybrowser-2.5_1/lib/QtSolutions_PropertyBrowser-2.5d.dll
	${TBB_BIN_FILES_DEBUG}
	${OPENAL_BIN_FILES_DEBUG}
	${OIS_BIN_FILES_DEBUG}
	${MYGUI_BIN_FILES_DEBUG}
	${OGRE_BIN_FILES_DEBUG}
	${SKYX_BIN_FILES_DEBUG}
	${PHYSX_BIN_FILES_DEBUG}
	
	${GASS_DIR}/bin/debug/GASSCore_d.dll
	${GASS_DIR}/bin/debug/GASSSim_d.dll 
	${GASS_DIR}/bin/debug/GASSPluginOgre_d.dll
	${GASS_DIR}/bin/debug/GASSPluginOSG_d.dll
	${GASS_DIR}/bin/debug/GASSPluginOIS_d.dll
	${GASS_DIR}/bin/debug/GASSPluginODE_d.dll
	${GASS_DIR}/bin/debug/GASSPluginPhysx3_d.dll
	${GASS_DIR}/bin/debug/GASSPluginPagedGeometry_d.dll
	${GASS_DIR}/bin/debug/GASSPluginEnvironment_d.dll
	${GASS_DIR}/bin/debug/GASSPluginOpenAL_d.dll
	${GASS_DIR}/bin/debug/GASSPluginGame_d.dll
	${GASS_DIR}/bin/debug/GASSPluginRoadNetwork_d.dll
	${GASS_DIR}/bin/debug/GASSPluginBase_d.dll
	${GASS_DIR}/bin/debug/GASSPluginRakNet_d.dll
	${GASS_DIR}/bin/debug/GASSPluginRecastNavigation_d.dll
	${GASS_DIR}/bin/debug/GASSEditorModule_d.dll
	${QT_DIR}/bin/QtCored4.dll
	${QT_DIR}/bin/QtGuid4.dll
	${OSG_BIN_FILES_DEBUG}
	)
	
	FILE(COPY ${OSG_PLUGINS}  DESTINATION  ${CMAKE_BINARY_DIR}/out/debug)

#add dll files to out dir to enable debugging from IDE without install step
foreach(DLL_FILE ${DEP_DLL_FILES_DEBUG})
	ADD_CUSTOM_COMMAND(TARGET ${APP_NAME}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLL_FILE} $<TARGET_FILE_DIR:${APP_NAME}>)
endforeach()

foreach(DLL_FILE ${DEP_DLL_FILES_RELEASE})
	ADD_CUSTOM_COMMAND(TARGET ${APP_NAME}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLL_FILE} $<TARGET_FILE_DIR:${APP_NAME}>)
endforeach()

install(TARGETS ${APP_NAME} RUNTIME DESTINATION ${GASSED_INSTALL_BIN_DIR} CONFIGURATIONS Release)
install(FILES  ${DEP_DLL_FILES_RELEASE} DESTINATION ${GASSED_INSTALL_BIN_DIR} CONFIGURATIONS Release)
FILE(GLOB GASSED_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/Configuration/*.xml")
install(FILES ${GASSED_CONFIG} DESTINATION ${GASSED_INSTALL_BIN_DIR}/../Configuration)
install(FILES ${OSG_PLUGINS} DESTINATION ${GASSED_INSTALL_BIN_DIR}/${OSG_PLUGINS_DIR_NAME})
