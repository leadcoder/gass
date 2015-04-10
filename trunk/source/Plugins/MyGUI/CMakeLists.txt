#set lib name here!
set(LIB_NAME "GASSPluginMyGUI")

find_package(MyGUI)
		
set(MY_INC_DIRS ${GASS_SIM_INCLUDE_DIRS} ${MYGUI_INCLUDE_DIRS})




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

include_directories(${GASS_SIM_INCLUDE_DIRS} ${MYGUI_INCLUDE_DIRS} ${OPENSCENEGRAPH_INCLUDE_DIRS} ${OGRE_INCLUDE_DIR})

if(GASS_BUILD_PLUGIN_OGRE)
	find_package(Ogre)
	set(CPP_FILES ${CPP_FILES}
		MyGUIOgreSystem.cpp)
	set(H_FILES ${H_FILES}
		MyGUIOgreSystem.h)
	set(MYGUI_LIBRARIES	${MYGUI_LIBRARIES} ${OGRE_LIBRARIES})
	set(MY_INC_DIRS ${MY_INC_DIRS} ${OGRE_INCLUDE_DIR})
 endif()
 
#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#Add some folders where to search for dependency libraries
link_directories(${MY_INC_DIRS})

add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})
target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_SIM_LIBRARIES} ${MYGUI_LIBRARIES})

gass_install_plugin_target()