cmake_minimum_required(VERSION 2.8)
set(APP_NAME "GASSEd")
project(${APP_NAME})

set(QT_DIR $ENV{QT_DIR} CACHE PATH "QT4 folder")

set(GASS_DIR $ENV{GASS_HOME} CACHE PATH "GASS folder")
set(GASS_DEPENDENCIES_DIR $ENV{GASS_DEPENDENCIES} CACHE PATH "GASS 3rd-party dependency folder")

if(EXISTS "${GASS_DEPENDENCIES_DIR}/boost-v1.54")
	set(BOOST_DIR  "${GASS_DEPENDENCIES_DIR}/boost-v1.54" CACHE PATH "BOOST folder")
else()
	set(BOOST_DIR $ENV{BOOST_HOME} CACHE PATH "BOOST folder")
endif()

if(EXISTS "${GASS_DEPENDENCIES_DIR}/OpenSceneGraph-v3.0.1")
	set(OSG_DIR  "${GASS_DEPENDENCIES_DIR}/OpenSceneGraph-v3.0.1" CACHE PATH "OSG folder")
else()
	set(OSG_DIR $ENV{OGRE_HOME} CACHE PATH "OSG folder")
endif()

if(EXISTS "${GASS_DEPENDENCIES_DIR}/ogre-v1-9")
	set(OGRE_DIR  "${GASS_DEPENDENCIES_DIR}/ogre-v1-9" CACHE PATH "Ogre 3D folder")
else()
	set(OGRE_DIR $ENV{OGRE_HOME} CACHE PATH "Ogre 3D folder")
endif()

set(PHYSX_DIR $ENV{PHYSX_HOME}  CACHE PATH "PhysX folder")
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

set(GASS_COMMON_INCLUDE_DIRS "${BOOST_DIR}"  "${GASS_DIR}/include" 	"${GASS_DEPENDENCIES_DIR}/tinyxml"	"${GASS_DEPENDENCIES_DIR}/tbb/include" "${GASS_DEPENDENCIES_DIR}/angelscript/include")
set(GASS_COMMON_LIB_DIRS "${BOOST_DIR}/lib/" "${GASS_DEPENDENCIES_DIR}/lib/" "${GASS_DEPENDENCIES_DIR}/angelscript/lib" "${GASS_DIR}/lib/")
set(GASS_COMMON_LINK_LIBRARIES
	optimized GASSCore
	optimized GASSSim
	optimized GASSEditorModule
	optimized tinyxml
	debug GASSCore_d
	debug GASSSim_d
	debug GASSEditorModule_d
	debug tinyxml_d 
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
			)
			
			
			set(DEP_DLL_FILES_RELEASE 
			${GASS_DEPENDENCIES_DIR}/qtpropertybrowser-2.5_1/lib/QtSolutions_PropertyBrowser-2.5.dll
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
	${OSG_DIR}/bin/zlib1.dll
	${OSG_DIR}/bin/libpng13.dll
	${OSG_DIR}/bin/glut32.dll
	${OSG_DIR}/bin/ot12-OpenThreads.dll
	${OSG_DIR}/bin/osg80-osg.dll
	${OSG_DIR}/bin/osg80-osgUtil.dll
	${OSG_DIR}/bin/osg80-osgDB.dll
	${OSG_DIR}/bin/osg80-osgGA.dll
	${OSG_DIR}/bin/osg80-osgText.dll
	${OSG_DIR}/bin/osg80-osgViewer.dll
	${OSG_DIR}/bin/osg80-osgTerrain.dll
	${OSG_DIR}/bin/osg80-osgFX.dll
	${OSG_DIR}/bin/osg80-osgManipulator.dll
	${OSG_DIR}/bin/osg80-osgVolume.dll
	${OSG_DIR}/bin/osg80-osgPresentation.dll
	${OSG_DIR}/bin/osg80-osgSim.dll
	${OSG_DIR}/bin/osg80-osgAnimation.dll
	${OSG_DIR}/bin/osg80-osgWidget.dll
	${OSG_DIR}/bin/osg80-osgParticle.dll
	${OSG_DIR}/bin/osg80-osgShadow.dll
	
	${PHYSX_BIN_DIR}/PhysX3Common${DLL_SUFFIX}.dll
	${PHYSX_BIN_DIR}/PhysX3${DLL_SUFFIX}.dll
	${PHYSX_BIN_DIR}/PhysX3Cooking${DLL_SUFFIX}.dll
	${PHYSX_BIN_DIR}/PhysX3CharacterKinematic${DLL_SUFFIX}.dll
	${PHYSX_BIN_DIR}/PhysX3Profile${DLL_SUFFIX}.dll
	
	
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
	${GASS_DIR}/bin/release/GASSPluginAI.dll
	${GASS_DIR}/bin/release/GASSEditorModule.dll
	${QT_DIR}/bin/QtCore4.dll
	${QT_DIR}/bin/QtGui4.dll
	)
	set(DEP_DLL_FILES_DEBUG 
	${GASS_DEPENDENCIES_DIR}/qtpropertybrowser-2.5_1/lib/QtSolutions_PropertyBrowser-2.5d.dll
	${GASS_DEPENDENCIES_DIR}/lib/tbb_debug.dll
	${GASS_DEPENDENCIES_DIR}/lib/OpenAL32.dll	
	${GASS_DEPENDENCIES_DIR}/lib/OIS_d.dll
	#${GASS_DEPENDENCIES_DIR}/MyGUI_3.2.0/bin/debug/MyGUIEngine_d.dll
	${OGRE_DIR}/bin/debug/OgreMain_d.dll
	${OGRE_DIR}/bin/debug/OgreTerrain_d.dll
	${OGRE_DIR}/bin/debug/OgrePaging_d.dll
	${OGRE_DIR}/bin/debug/OgreOverlay_d.dll
	${OGRE_DIR}/bin/debug/Plugin_CgProgramManager_d.dll
	${OGRE_DIR}/bin/debug/OgreOverlay_d.dll
	${OGRE_DIR}/bin/debug/Plugin_OctreeSceneManager_d.dll
	${OGRE_DIR}/bin/debug/Plugin_ParticleFX_d.dll
	${OGRE_DIR}/bin/debug/RenderSystem_Direct3D9_d.dll
	${OGRE_DIR}/bin/debug/RenderSystem_GL_d.dll
	${OGRE_DIR}/bin/release/cg.dll
	${OSG_DIR}/bin/glut32D.dll
	${OSG_DIR}/bin/libpng13d.dll
	${OSG_DIR}/bin/osg80-osgAnimationd.dll
	${OSG_DIR}/bin/osg80-osgd.dll
	${OSG_DIR}/bin/osg80-osgDBd.dll
	${OSG_DIR}/bin/osg80-osgFXd.dll
	${OSG_DIR}/bin/osg80-osgGAd.dll
	${OSG_DIR}/bin/osg80-osgManipulatord.dll
	${OSG_DIR}/bin/osg80-osgParticled.dll
	${OSG_DIR}/bin/osg80-osgPresentationd.dll
	${OSG_DIR}/bin/osg80-osgShadowd.dll
	${OSG_DIR}/bin/osg80-osgSimd.dll
	${OSG_DIR}/bin/osg80-osgTerraind.dll
	${OSG_DIR}/bin/osg80-osgTextd.dll
	${OSG_DIR}/bin/osg80-osgUtild.dll
	${OSG_DIR}/bin/osg80-osgViewerd.dll
	${OSG_DIR}/bin/osg80-osgVolumed.dll
	${OSG_DIR}/bin/osg80-osgWidgetd.dll
	${OSG_DIR}/bin/ot12-OpenThreadsd.dll
	${OSG_DIR}/bin/zlib1d.dll
	${PHYSX_BIN_DIR}/PhysX3CommonCHECKED${DLL_SUFFIX}.dll
	${PHYSX_BIN_DIR}/PhysX3CHECKED${DLL_SUFFIX}.dll
	${PHYSX_BIN_DIR}/PhysX3CookingCHECKED${DLL_SUFFIX}.dll
	${PHYSX_BIN_DIR}/PhysX3CharacterKinematicCHECKED${DLL_SUFFIX}.dll
	${PHYSX_BIN_DIR}/PhysX3Profile${DLL_SUFFIX}.dll
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
	${GASS_DIR}/bin/debug/GASSPluginAI_d.dll
	${GASS_DIR}/bin/debug/GASSEditorModule_d.dll
	${QT_DIR}/bin/QtCored4.dll
	${QT_DIR}/bin/QtGuid4.dll
	)

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

			
install(TARGETS ${APP_NAME} DESTINATION /bin)