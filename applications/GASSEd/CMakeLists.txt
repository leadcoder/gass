cmake_minimum_required(VERSION 2.8)
set(APP_NAME "GASSEd")
project(${APP_NAME})

set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/../../CMake"
  )

#prepapre search dirs for find_package
set(GASS_DIR $ENV{GASS_HOME} CACHE PATH "GASS folder")

set(QT_DIR $ENV{QT_DIR} CACHE PATH "QT folder")
set(QTPROPERTYBROWSER_DIR $ENV{QTPROPERTYBROWSER_HOME} CACHE PATH "qt property browser dependency folder")

option(GASSED_USE_OSG "Add OSG render system" OFF ) 
#add osg plugin if requested
 
set(GASS_COMPONENTS  GASSEditorModule 
								GASSPluginOgre 
								GASSPluginOIS 
								GASSPluginPhysX3 
								GASSPluginODE 
								GASSPluginPagedGeometry
								GASSPluginOpenAL
								GASSPluginSensor
								GASSPluginWeapon
								GASSPluginBase
								GASSPluginEnvironment
								GASSPluginRecastNavigation
								GASSPluginRoadNetwork)

if(GASSED_USE_OSG)
	set(GASS_COMPONENTS  ${GASS_COMPONENTS} GASSPluginOSG)
endif()


find_package(GASSSim COMPONENTS ${GASS_COMPONENTS})

FIND_PACKAGE(Qt5Core QUIET)
FIND_PACKAGE(Qt5Widgets QUIET)
FIND_PACKAGE(Qt5Gui QUIET)
FIND_PACKAGE(Qt5OpenGL QUIET)
SET(QT_INCLUDES ${Qt5Widgets_INCLUDE_DIRS} ${Qt5OpenGL_INCLUDE_DIRS})

find_package(QTPropertyBrowser)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) #build dlls and exe here
set(CMAKE_DEBUG_POSTFIX  "_d" CACHE STRING "add a postfix, usually d on windows")

set(GASSED_INSTALL_BIN_DIR  "bin")
set(GASSED_INSTALL_BIN_DIR_DEBUG "bin/debug")
set(GASSED_INSTALL_BIN_DIR_RELEASE "bin/release")

set(GASSED_H 
	GASSEd.h
	GASSBrushSettingsWidget.h
	GASSPropertyWidget.h
	GASSRenderWidget.h
	GASSResourceTreeWidget.h
	GASSSceneSelectionWidget.h
	GASSSceneTreeWidget.h
	GASSSSaveSceneWidget.h
	GASSTools.h
	CustomTypes.h
	fileedit.h
	variantfactory.h
	variantmanager.h
	)

	set(GASSED_RESOURCES GASSEd.qrc)
	QT5_ADD_RESOURCES(GASSED_RESOURCES_RCC ${GASSED_RESOURCES})
    SET(GASSED_MOC_H)
    FOREACH( LIB_MOC_HDR ${GASSED_H} )
        GET_FILENAME_COMPONENT( LIB_MOC_HDR_ABS ${LIB_MOC_HDR} ABSOLUTE )
        QT5_WRAP_CPP( LIB_MOC_SRC ${LIB_MOC_HDR} OPTIONS "-f${LIB_MOC_HDR_ABS}" )
        LIST( APPEND GASSED_MOC_H ${LIB_MOC_SRC} )
    ENDFOREACH()

set(GASSED_CPP main.cpp
	GASSEd.cpp
	GASSBrushSettingsWidget.cpp
	GASSPropertyWidget.cpp
	GASSRenderWidget.cpp
	GASSResourceTreeWidget.cpp
	GASSSceneSelectionWidget.cpp
	GASSSceneTreeWidget.cpp
	GASSSSaveSceneWidget.cpp
	GASSTools.cpp
	CustomTypes.cpp
	fileedit.cpp
	variantfactory.cpp
	variantmanager.cpp)
	
add_definitions( -D_SCL_SECURE_NO_WARNINGS -DQT_QTPROPERTYBROWSER_IMPORT)

#link_directories(${QTPROPERTYBROWSER_DIR}/lib)
set_target_properties(${LIB_NAME} PROPERTIES DEBUG_POSTFIX _d)

include_directories(${GASS_SIM_INCLUDE_DIRS}
					${QT_INCLUDES}
					${QTPROPERTYBROWSER_DIR_INCLUDE_DIRS}
					)

add_executable(${APP_NAME} ${GASSED_CPP} ${GASSED_MOC_H} ${GASSED_RESOURCES_RCC})
target_link_libraries(${APP_NAME} ${QT_LIBRARIES} 
					${GASS_SIM_LIBRARIES}
					${QT_QTCORE_LIBRARY}
					${QT_QTGUI_LIBRARY}
					${QTPROPERTYBROWSER_LIBRARIES}					
					)
# Use the required module from Qt5.
qt5_use_modules(${APP_NAME} Core Widgets Gui OpenGL)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Configuration DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
install(TARGETS ${APP_NAME} RUNTIME DESTINATION ${GASSED_INSTALL_BIN_DIR} CONFIGURATIONS Release)
#install(FILES ${GASSED_CONFIG} DESTINATION ${GASSED_INSTALL_BIN_DIR}/../Configuration)
install(DIRECTORY Configuration DESTINATION ${GASSED_INSTALL_BIN_DIR}/../)
if(WIN32)
	#HACK to get qt-binaries, findqt don't return binaries nor root/install path, 
	#we use the QT_DOC_DIR for now as to get root level..

IF(Qt5Widgets_FOUND)
	
else()
	set(QT_BINARIES_REL ${QT_DOC_DIR}/../bin/QtCore4.dll
	${QT_DOC_DIR}/../bin/QtGui4.dll
	${QTPROPERTYBROWSER_BINARY_REL})
	
	set(QT_BINARIES_DBG ${QT_DOC_DIR}/../bin/QtCored4.dll
	${QT_DOC_DIR}/../bin/QtGuid4.dll
	${QTPROPERTYBROWSER_BINARY_DBG})
endif()
	
	set(GASSED_BINARIES_REL ${GASS_SIM_BINARIES_REL} ${GASS_PLUGIN_BINARIES_REL} ${QT_BINARIES_REL})
	set(GASSED_BINARIES_DBG ${GASS_SIM_BINARIES_DBG} ${GASS_PLUGIN_BINARIES_DBG} ${QT_BINARIES_DBG})
	
	file(COPY ${GASSED_BINARIES_REL}  DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${GASSED_BINARIES_DBG}  DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	
	if(GASSED_USE_OSG) 
		#put osg plugins in own dir (must be named osgPlugins-<version> to) avoid cluttering
		file(COPY ${OSGPLUGIN_BINARIES_REL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/osgPlugins-${OSG_VERSION})
		file(COPY ${OSGPLUGIN_BINARIES_DBG} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/osgPlugins-${OSG_VERSION})
	endif()

	install(FILES ${GASSED_BINARIES_REL} DESTINATION ${GASSED_INSTALL_BIN_DIR})
endif()

