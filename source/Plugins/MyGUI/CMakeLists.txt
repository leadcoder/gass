#set lib name here!
set(LIB_NAME "GASSPluginMyGUI")

find_package(MyGUI)
		

set(CPP_FILES LoadSceneDialog.cpp
		MainMenu.cpp
		MyGUILayoutComponent.cpp
		MyGUIOgreSystem.cpp
		MyGUISystem.cpp
		MyGUIVehicleInstrumentsComponent.cpp
		MyGUI_RTTLayer.cpp)

set(H_FILES LoadSceneDialog.h
		 MainMenu.h
		 MyGUILayoutComponent.h
		 MyGUIMessages.h
		 MyGUISystem.h
		 MyGUIVehicleInstrumentsComponent.h
		 MyGUI_RTTLayer.h
		 StatisticInfo.h)
		 
#add osg and ogre stuff
if(GASS_BUILD_PLUGIN_OSG)
	find_package(OSGExt 3.2.1 REQUIRED osgUtil osgDB osgGA osgText osgShadow osgViewer osgSim osgTerrain)
	find_package(OpenGL)
	set(CPP_FILES ${CPP_FILES}
		MyGUIOSG.cpp
		MyGUIOSGSystem.cpp)
	set(H_FILES ${H_FILES}
		MyGUIOSG.h
		MyGUIOSGSystem.h)
	set(MYGUI_LIBRARIES	${MYGUI_LIBRARIES} 
		${OPENGL_gl_LIBRARY} 
		${OPENGL_glu_LIBRARY} 
		${OPENSCENEGRAPH_LIBRARIES})
	set(MY_INC_DIRS ${MY_INC_DIRS} ${OPENSCENEGRAPH_INCLUDE_DIRS})	
endif()

if(GASS_BUILD_PLUGIN_OGRE)
	find_package(Ogre)
	set(CPP_FILES ${CPP_FILES}
		MyGUIOgreSystem.cpp)
	set(H_FILES ${H_FILES}
		MyGUIOgreSystem.h)
	set(MYGUI_LIBRARIES	${MYGUI_LIBRARIES} ${OGRE_LIBRARIES})
	set(MY_INC_DIRS ${MY_INC_DIRS} ${OGRE_INCLUDE_DIR})
endif()
 
add_library (${LIB_NAME} ${GASS_BUILDTYPE}  ${CPP_FILES} ${H_FILES})

target_include_directories(${LIB_NAME}
	PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	PUBLIC  $<BUILD_INTERFACE:${MYGUI_INCLUDE_DIRS}>
    PUBLIC  $<BUILD_INTERFACE:${OPENSCENEGRAPH_INCLUDE_DIRS}>
	PUBLIC  $<BUILD_INTERFACE:${OGRE_INCLUDE_DIR}>	
)

target_link_libraries(${LIB_NAME} GASSSim ${MYGUI_LIBRARIES})
target_compile_definitions(${LIB_NAME} PRIVATE ${GASS_COMMON_DEFINITIONS})
gass_install_plugin_target()