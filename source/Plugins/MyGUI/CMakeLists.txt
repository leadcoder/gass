set(LIB_NAME GASSPluginMyGUI)

set(CPP_FILES LoadSceneDialog.cpp
		MainMenu.cpp
		MyGUILayoutComponent.cpp
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
	find_package(OpenGL)
	set(CPP_FILES ${CPP_FILES}
		MyGUIOSG.cpp
		MyGUIOSGSystem.cpp)
	set(H_FILES ${H_FILES}
		MyGUIOSG.h
		MyGUIOSGSystem.h)
endif()

if(GASS_BUILD_PLUGIN_OGRE)
	
	set(CPP_FILES ${CPP_FILES}
		MyGUIOgreSystem.cpp)
	set(H_FILES ${H_FILES}
		MyGUIOgreSystem.h)
endif()

gass_setup_lib(${LIB_NAME} 
				BUILDTYPE ${GASS_BUILDTYPE}
				SOURCE_FILES ${CPP_FILES} 
				HEADER_FILES ${h_FILES} 
				PRIVATE_DEPS GASSSim MyGUI
				SKIP_HEADER_INSTALL)
				
if(GASS_BUILD_PLUGIN_OSG)
	find_package(OpenGL)
	target_link_libraries(${LIB_NAME} PRIVATE OSG ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
endif()

if(GASS_BUILD_PLUGIN_OGRE)
	target_link_libraries(${LIB_NAME} PRIVATE Ogre)
endif()

gass_get_header_directories(HEADER_SUBDIRS)
foreach(INC_DIR ${HEADER_SUBDIRS})
	target_include_directories(${LIB_NAME} PRIVATE  $<BUILD_INTERFACE:${INC_DIR}>)
endforeach()

target_compile_definitions(${LIB_NAME} PRIVATE ${GASS_COMMON_DEFINITIONS} GASS_PLUGIN_EXPORTS)

set_target_properties(${LIB_NAME} PROPERTIES FOLDER "Plugins")

install(TARGETS ${LIB_NAME}
	RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug	
	LIBRARY DESTINATION ${GASS_INSTALL_LIB_DIR_DEBUG} CONFIGURATIONS Debug
	ARCHIVE DESTINATION ${GASS_INSTALL_LIB_DIR_DEBUG} CONFIGURATIONS Debug)

install(TARGETS ${LIB_NAME}
  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release
  LIBRARY DESTINATION ${GASS_INSTALL_LIB_DIR_RELEASE} CONFIGURATIONS Release
  ARCHIVE DESTINATION ${GASS_INSTALL_LIB_DIR_RELEASE} CONFIGURATIONS Release)
	  
 
# add_library (${LIB_NAME} ${GASS_BUILDTYPE}  ${CPP_FILES} ${H_FILES})

# target_include_directories(${LIB_NAME}
	# PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	# PUBLIC  $<BUILD_INTERFACE:${MYGUI_INCLUDE_DIRS}>
    # PUBLIC  $<BUILD_INTERFACE:${OPENSCENEGRAPH_INCLUDE_DIRS}>
	# PUBLIC  $<BUILD_INTERFACE:${OGRE_INCLUDE_DIR}>	
# )

# target_link_libraries(${LIB_NAME} GASSSim ${MYGUI_LIBRARIES})
# target_compile_definitions(${LIB_NAME} PRIVATE ${GASS_COMMON_DEFINITIONS})
# gass_install_plugin_target()

# if(GASS_INSTALL_DEP_BINARIES)
	# FILE(COPY ${MYGUI_BINARIES_REL} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	# FILE(COPY ${MYGUI_BINARIES_DBG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	
	# install(FILES ${MYGUI_BINARIES_REL} DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
	# install(FILES ${MYGUI_BINARIES_DBG} DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)
# endif()