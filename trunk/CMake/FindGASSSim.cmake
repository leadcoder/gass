#This file will export:
#GASS_SIM_INCLUDE_DIRS = holding all include dirs are needed for compiling a project using gass sim
#GASS_SIM_LIBRARIES = holding all libraraies that are needed for linking gass sim
#GASS_SIM_BINARIES_REL = holding all shared libraries needed to run debug application using gass sim
#GASS_SIM_BINARIES_DBG = holding all shared libraries needed to run release application using gass sim

find_package(GASSCore)
find_package(GASSSimThirdParty)

#just append angelscript to core includes 
set(GASS_SIM_INCLUDE_DIRS ${GASS_CORE_INCLUDE_DIRS} ${ANGELSCRIPT_INCLUDE_DIRS})

find_library(GASS_SIM_LIBRARY_RELEASE GASSSim HINTS ${GASS_LIBRARY_DIRS})
find_library(GASS_SIM_LIBRARY_DEBUG GASSSim_d HINTS ${GASS_LIBRARY_DIRS})

#generate out varible for libraries by appending sim and angelscript to core libs
set(GASS_SIM_LIBRARIES optimized ${GASS_SIM_LIBRARY_RELEASE}
	 debug ${GASS_SIM_LIBRARY_DEBUG}
	 ${GASS_CORE_LIBRARIES})

if (";${GASSSim_FIND_COMPONENTS};" MATCHES ";EditorModule;")
	find_library(GASS_EDITOR_LIBRARY_RELEASE GASSEditorModule HINTS ${GASS_LIBRARY_DIRS} ${GASS_LIBRARY_DIRS}/release)
	find_library(GASS_EDITOR_LIBRARY_DEBUG GASSEditorModule_d HINTS ${GASS_LIBRARY_DIRS} ${GASS_LIBRARY_DIRS}/debug)
	set(GASS_SIM_LIBRARIES ${GASS_SIM_LIBRARIES} 
				 optimized ${GASS_EDITOR_LIBRARY_RELEASE}
				 debug ${GASS_EDITOR_LIBRARY_DEBUG})
endif()

if (WIN32) #find dlls
	  find_file(GASS_SIM_BINARY_RELEASE NAMES GASSSim.dll HINTS ${GASS_BINARY_DIRS})
	  find_file(GASS_SIM_BINARY_DEBUG NAMES GASSSim_d.dll HINTS ${GASS_BINARY_DIRS})

	   #Set GASSSim out binaries
	   set(GASS_SIM_BINARIES_REL ${GASS_SIM_BINARY_RELEASE} ${GASS_CORE_BINARIES_REL})
	   set(GASS_SIM_BINARIES_DBG ${GASS_SIM_BINARY_DEBUG} ${GASS_CORE_BINARIES_DBG})
		
	   #Add GASS modules
	   if (";${GASSSim_FIND_COMPONENTS};" MATCHES ";EditorModule;") 
		find_file(GASS_EDITOR_BINARY_RELEASE NAMES GASSEditorModule.dll HINTS ${GASS_BINARY_DIRS})
		find_file(GASS_EDITOR_BINARY_DEBUG NAMES GASSEditorModule_d.dll HINTS ${GASS_BINARY_DIRS})
		set(GASS_SIM_BINARIES_REL ${GASS_SIM_BINARIES_REL} ${GASS_EDITOR_BINARY_RELEASE})
		set(GASS_SIM_BINARIES_DBG ${GASS_SIM_BINARIES_DBG} ${GASS_EDITOR_BINARY_DEBUG})
	   endif()
	
	#Add GASS plugins
	if (";${GASSSim_FIND_COMPONENTS};" MATCHES ";OgrePlugin;")
	  #plugins
	  find_file(GASS_PLUGIN_OGRE_BINARY_RELEASE NAMES GASSPluginOgre.dll HINTS ${GASS_BINARY_DIRS})
	  find_file(GASS_PLUGIN_OGRE_BINARY_DEBUG NAMES GASSPluginOgre_d.dll HINTS ${GASS_BINARY_DIRS})
	  find_package(Ogre)
	  set(GASS_PLUGIN_OGRE_BINARIES_REL 
		${GASS_PLUGIN_OGRE_BINARY_RELEASE}
		${OGRE_BINARY_REL}
		${OGRE_Overlay_BINARY_REL}
		${OGRE_Paging_BINARY_REL}
		${OGRE_Terrain_BINARY_REL}
		${OGRE_Plugin_CgProgramManager_REL}
		${OGRE_Plugin_OctreeSceneManager_REL}
		${OGRE_Plugin_ParticleFX_REL}
		${OGRE_RenderSystem_Direct3D9_REL}
		${OGRE_RenderSystem_GL_REL}
		${OGRE_PLUGIN_DIR_REL}/cg.dll)
		
		set(GASS_PLUGIN_OGRE_BINARIES_DBG
		${GASS_PLUGIN_OGRE_BINARY_DEBUG}
		${OGRE_BINARY_DBG}
		${OGRE_Overlay_BINARY_DBG}
		${OGRE_Paging_BINARY_DBG}
		${OGRE_Terrain_BINARY_DBG}
		${OGRE_Plugin_CgProgramManager_DBG}
		${OGRE_Plugin_OctreeSceneManager_DBG}
		${OGRE_Plugin_ParticleFX_DBG}
		${OGRE_RenderSystem_Direct3D9_DBG}
		${OGRE_RenderSystem_GL_DBG}
		${OGRE_PLUGIN_DIR_DBG}/cg.dll)
		set(GASS_SIM_BINARIES_REL ${GASS_SIM_BINARIES_REL} ${GASS_PLUGIN_OGRE_BINARIES_REL})
		set(GASS_SIM_BINARIES_DBG ${GASS_SIM_BINARIES_DBG} ${GASS_PLUGIN_OGRE_BINARIES_DBG})
	 endif()  
endif()
