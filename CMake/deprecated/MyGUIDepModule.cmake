set(MYGUI_DIR "${DEPENDENCIES_DIR}/MyGUI_3.2.0" CACHE PATH "MyGUI folder")

set (MYGUI_INC_DIR "${MYGUI_DIR}/include/MYGUI"
	${OGRE_INC_DIR}
	${OSG_INC_DIR}
	)

set (MYGUI_LIB_DIR "${MYGUI_DIR}/lib"
					${OGRE_LIB_DIR}
					${OSG_LIB_DIR})

#Add project dependency libs
set(MYGUI_LINK_LIBRARIES
	optimized MyGUIEngine
	optimized MyGUI.OgrePlatform
	optimized MyGUI.OpenGLPlatform
	debug MyGUIEngine_d
	debug MyGUI.OgrePlatform_d
	debug MyGUI.OpenGLPlatform_d
	${OSG_LINK_LIBRARIES}
	${OGRE_LINK_LIBRARIES}
)

set(MYGUI_BIN_FILES_RELEASE
	$ENV{MYGUI_HOME}/bin/release/MyGUIEngine.dll
)

set(MYGUI_BIN_FILES_DEBUG
	$ENV{MYGUI_HOME}/bin/debug/MyGUIEngine_d.dll
)