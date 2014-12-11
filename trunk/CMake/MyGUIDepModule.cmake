
#Set OIS include directories
set (MYGUI_INC_DIR "${GASS_DEPENDENCIES_DIR}/MyGUI_3.2.0/include/MYGUI"
	${OGRE_INC_DIR}
	${OSG_INC_DIR}
	)

set (MYGUI_LIB_DIR "${GASS_DEPENDENCIES_DIR}/MyGUI_3.2.0/lib"
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
	${GASS_DEPENDENCIES_DIR}/MyGUI_3.2.0/bin/release/MyGUIEngine.dll
)

set(MYGUI_BIN_FILES_DEBUG
	${GASS_DEPENDENCIES_DIR}/MyGUI_3.2.0/bin/debug/MyGUIEngine_d.dll
)