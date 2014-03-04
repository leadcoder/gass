#set lib name here!
set(LIB_NAME "GASSPluginEnvironment")

if(EXISTS "${GASS_DEPENDENCIES_DIR}/ogre-v1-9")
	set(OGRE_DIR  "${GASS_DEPENDENCIES_DIR}/ogre-v1-9" CACHE PATH "Ogre 3D folder")
else()
	set(OGRE_DIR $ENV{OGRE_HOME} CACHE PATH "Ogre 3D folder")
endif()

#Set include directories
set (DEP_INCLUDE_DIRS "${OGRE_DIR}/include/OGRE"
		"${GASS_DEPENDENCIES_DIR}/Hydrax-v0.5.1/src"
		"${GASS_DEPENDENCIES_DIR}/SkyX-v0.4/include/SKYX")
		
include_directories(${GASS_COMMON_INCLUDE_DIRS} ${DEP_INCLUDE_DIRS})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#Add some folders where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS} "${OGRE_DIR}/lib" "${GASS_DEPENDENCIES_DIR}/SkyX-v0.4/lib")
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

#Add project dependency libs
set(PROJ_LINK_LIBRARIES optimized OgreMain 
	optimized OgrePaging 
	optimized Hydrax
	optimized SkyX
	debug OgreMain_d 
	debug Hydrax_d
	debug SkyX_d
	)

target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_COMMON_LINK_LIBRARIES})
target_link_libraries(${LIB_NAME} ${PROJ_LINK_LIBRARIES})

gass_install_plugin_target()		

