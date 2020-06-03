set(APP_NAME GASSEd)
#prepapre search dirs for find_package
#set (CMAKE_PREFIX_PATH "C:/Qt/Qt5.6.0/5.6/msvc2013_64")
set(Qt5_DIR $ENV{QT5_DIR} CACHE PATH "QT5 folder where Qt5Config.cmake is located")
set(QTPROPERTYBROWSER_DIR $ENV{QTPROPERTYBROWSER_HOME} CACHE PATH "qt property browser dependency folder")

set(GASS_COMPONENTS  GASSEditorModule 
								GASSPluginOSG
								GASSPluginOIS 
								GASSPluginPhysX3 
								GASSPluginODE 
								GASSPluginPagedGeometry
								GASSPluginOpenAL
								GASSPluginVehicle
								GASSPluginBase
								GASSPluginEnvironment
								GASSPluginRecastNavigation
								GASSPluginRoadNetwork)
								
set(GASSED_QT_COMPONENTS Core Widgets Gui OpenGL)
find_package(Qt5 COMPONENTS ${GASSED_QT_COMPONENTS} REQUIRED)

#find_package(Qt5Core)
#find_package(Qt5Widgets QUIET)
#find_package(Qt5Gui QUIET)
#find_package(Qt5OpenGL QUIET)

set(QT_INCLUDES ${Qt5Widgets_INCLUDE_DIRS} ${Qt5OpenGL_INCLUDE_DIRS})
find_package(QTPropertyBrowser)

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
	

add_executable(${APP_NAME} ${GASSED_CPP} ${GASSED_H} ${GASSED_MOC_H} ${GASSED_RESOURCES_RCC})
target_include_directories(${APP_NAME} PRIVATE $<BUILD_INTERFACE:${GASS_SIM_INCLUDE_DIRS}>)
target_include_directories(${APP_NAME} PRIVATE $<BUILD_INTERFACE:${QT_INCLUDES}>)
target_include_directories(${APP_NAME} PRIVATE $<BUILD_INTERFACE:${QTPROPERTYBROWSER_DIR_INCLUDE_DIRS}>)

target_link_libraries(${APP_NAME} 
					GASSSim
					GASSEditorModule
					${QT_LIBRARIES} 
					${QT_QTCORE_LIBRARY}
					${QT_QTGUI_LIBRARY}
					${QTPROPERTYBROWSER_LIBRARIES}					
					)
# Use the required module from Qt5.
qt5_use_modules(${APP_NAME} ${GASSED_QT_COMPONENTS})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Configuration DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
install(TARGETS ${APP_NAME} RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
install(DIRECTORY Configuration DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE}/../)

install(TARGETS ${APP_NAME} RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS DEBUG)
install(DIRECTORY Configuration DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG}/../)

if(WIN32)
	if(Qt5Widgets_FOUND)
		#HACK to get qt-binaries, findqt don't return binaries nor root/install path, 
		#we use the Qt5_DIR to get path dir
		set(QT5_BIN_DIR ${Qt5_DIR}/../../../bin)
		#message(${QT5_BIN_DIR})
		foreach(_QT5_BIN ${GASSED_QT_COMPONENTS})
			STRING(TOUPPER ${_QT5_BIN} _UPPER_NAME)
			set(_BIN_NAME_DBG QT5_${_UPPER_NAME}_BINARY_DBG)
			set(_BIN_NAME_REL QT5_${_UPPER_NAME}_BINARY_REL)
			find_file(${_BIN_NAME_REL} NAMES Qt5${_QT5_BIN}.dll HINTS ${QT5_BIN_DIR})
			find_file(${_BIN_NAME_DBG} NAMES Qt5${_QT5_BIN}d.dll HINTS ${QT5_BIN_DIR})
			set(QT5_BINARIES_DBG ${QT5_BINARIES_DBG} ${${_BIN_NAME_DBG}})
			set(QT5_BINARIES_REL ${QT5_BINARIES_REL} ${${_BIN_NAME_REL}})
		endforeach()
		file(COPY ${QT5_BINARIES_REL}  DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
		file(COPY ${QT5_BINARIES_DBG}  DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
		install(FILES ${QT5_BINARIES_REL} DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
		install(FILES ${QT5_BINARIES_DBG} DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)
	endif()
endif()

