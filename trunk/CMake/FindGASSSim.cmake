#This file will export:
#GASS_SIM_INCLUDE_DIRS = holding all include dirs are needed for compiling a project using gass sim
#GASS_SIM_LIBRARIES = holding all libraraies that are needed for linking gass sim

#GASS_SIM_BINARIES_REL = holding all shared libraries needed to run debug application using gass sim
#GASS_SIM_BINARIES_DBG = holding all shared libraries needed to run release application using gass sim
#GASS_PLUGIN_BINARIES_REL
#GASS_PLUGIN_BINARIES_DBG 

MACRO(LIST_CONTAINS var value)
  SET(${var})
  FOREACH (value2 ${ARGN})
    IF (${value} STREQUAL ${value2})
      SET(${var} TRUE)
    ENDIF (${value} STREQUAL ${value2})
  ENDFOREACH (value2)
ENDMACRO(LIST_CONTAINS)

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

LIST_CONTAINS(USE_EDITOR_MODULE GASSEditorModule ${GASSSim_FIND_COMPONENTS})

if (USE_EDITOR_MODULE)
	find_library(GASS_EDITOR_LIBRARY_RELEASE GASSEditorModule HINTS ${GASS_LIBRARY_DIRS} ${GASS_LIBRARY_DIRS}/release)
	find_library(GASS_EDITOR_LIBRARY_DEBUG GASSEditorModule_d HINTS ${GASS_LIBRARY_DIRS} ${GASS_LIBRARY_DIRS}/debug)
	set(GASS_SIM_LIBRARIES ${GASS_SIM_LIBRARIES} 
				 optimized ${GASS_EDITOR_LIBRARY_RELEASE}
				 debug ${GASS_EDITOR_LIBRARY_DEBUG})
endif()

find_file(GASS_SIM_BINARY_RELEASE NAMES GASSSim${_SHARED_LIB_EXT} HINTS ${GASS_BINARY_DIRS})
find_file(GASS_SIM_BINARY_DEBUG NAMES GASSSim_d${_SHARED_LIB_EXT} HINTS ${GASS_BINARY_DIRS})

#Set GASSSim out binaries
set(GASS_SIM_BINARIES_REL ${GASS_SIM_BINARY_RELEASE} ${GASS_CORE_BINARIES_REL})
set(GASS_SIM_BINARIES_DBG ${GASS_SIM_BINARY_DEBUG} ${GASS_CORE_BINARIES_DBG})
	
#get shared libraries for components

foreach (_COMP_NAME ${GASSSim_FIND_COMPONENTS})
    set(_COMP_NAME_REL GASS_${_COMP_NAME}_REL)
	set(_COMP_NAME_DBG GASS_${_COMP_NAME}_DBG)
	
	find_file(${_COMP_NAME_REL} NAMES ${_COMP_NAME}${_SHARED_LIB_EXT} HINTS ${GASS_BINARY_DIRS})
	find_file(${_COMP_NAME_DBG} NAMES ${_COMP_NAME}_d${_SHARED_LIB_EXT} HINTS ${GASS_BINARY_DIRS})
	if(_COMP_NAME MATCHES "(GASSPlugin)")
	  set(GASS_PLUGIN_BINARIES_REL ${GASS_PLUGIN_BINARIES_REL} ${${_COMP_NAME_REL}})
	  set(GASS_PLUGIN_BINARIES_DBG ${GASS_PLUGIN_BINARIES_DBG} ${${_COMP_NAME_DBG}})
	else() #must be component
	 set(GASS_SIM_BINARIES_REL ${GASS_SIM_BINARIES_REL} ${${_COMP_NAME_REL}})
	 set(GASS_SIM_BINARIES_DBG ${GASS_SIM_BINARIES_DBG} ${${_COMP_NAME_DBG}})
	endif()
endforeach()

if (WIN32) #find third party shared libraries, TODO: support *nix?
	#check requested components
	LIST_CONTAINS(USE_OGRE_PLUGIN GASSPluginOgre ${GASSSim_FIND_COMPONENTS})
	LIST_CONTAINS(USE_OSG_PLUGIN GASSPluginOSG ${GASSSim_FIND_COMPONENTS})
	LIST_CONTAINS(USE_OIS_PLUGIN GASSPluginOIS ${GASSSim_FIND_COMPONENTS})
	LIST_CONTAINS(USE_PX_PLUGIN GASSPluginPhysX3 ${GASSSim_FIND_COMPONENTS})
	LIST_CONTAINS(USE_ENV_PLUGIN GASSPluginEnvironment ${GASSSim_FIND_COMPONENTS})
	LIST_CONTAINS(USE_OPENAL_PLUGIN GASSPluginOpenAL ${GASSSim_FIND_COMPONENTS})
	
	if(USE_OGRE_PLUGIN)
	  find_package(Ogre)
	  set(OGRE_BINARIES_REL 
		${OGRE_BINARY_REL}
		${OGRE_Overlay_BINARY_REL}
		${OGRE_Paging_BINARY_REL}
		${OGRE_Terrain_BINARY_REL}
		${OGRE_Plugin_CgProgramManager_REL}
		${OGRE_Plugin_OctreeSceneManager_REL}
		${OGRE_Plugin_ParticleFX_REL}
		${OGRE_RenderSystem_Direct3D9_REL}
		${OGRE_RenderSystem_GL_REL}
		${OGRE_PLUGIN_DIR_REL}/cg${_SHARED_LIB_EXT})
		
	set(OGRE_BINARIES_DBG
		${OGRE_BINARY_DBG}
		${OGRE_Overlay_BINARY_DBG}
		${OGRE_Paging_BINARY_DBG}
		${OGRE_Terrain_BINARY_DBG}
		${OGRE_Plugin_CgProgramManager_DBG}
		${OGRE_Plugin_OctreeSceneManager_DBG}
		${OGRE_Plugin_ParticleFX_DBG}
		${OGRE_RenderSystem_Direct3D9_DBG}
		${OGRE_RenderSystem_GL_DBG}
		${OGRE_PLUGIN_DIR_DBG}/cg${_SHARED_LIB_EXT})
		set(GASS_SIM_BINARIES_REL ${GASS_SIM_BINARIES_REL} ${OGRE_BINARIES_REL})
		set(GASS_SIM_BINARIES_DBG ${GASS_SIM_BINARIES_DBG} ${OGRE_BINARIES_DBG})
	endif()
	 
	if(USE_PX_PLUGIN)
	  set(PHYSX3_INSTALL_DIR $ENV{PHYSX_HOME} CACHE PATH "PhysX folder")
	  find_package(PhysX3)
	  
	  set(GASS_SIM_BINARIES_REL ${GASS_SIM_BINARIES_REL} ${PX_BINARIES_REL})
	  set(GASS_SIM_BINARIES_DBG ${GASS_SIM_BINARIES_DBG} ${PX_BINARIES_DBG})
	endif()

	if(USE_OIS_PLUGIN)
	  #Set OIS directory
	  set (OIS_DIR ${GASS_DEPENDENCIES_DIR}/ois)
	  #get binaries
	  find_package(OIS)
	  set(GASS_SIM_BINARIES_REL ${GASS_SIM_BINARIES_REL} ${OIS_BINARY_REL})
	  set(GASS_SIM_BINARIES_DBG ${GASS_SIM_BINARIES_DBG} ${OIS_BINARY_DBG})
	 endif()
	 
	 if(USE_OPENAL_PLUGIN)
	  set(OPENAL_BIN_DIR  "${GASS_DEPENDENCIES_DIR}/OpenAL1.1/bin" CACHE PATH "OpenAL bin folder")
	  find_package(OpenALExt)
	  set(GASS_SIM_BINARIES_REL ${GASS_SIM_BINARIES_REL} ${OPENAL_BIN_FILES_RELEASE})
	  set(GASS_SIM_BINARIES_DBG ${GASS_SIM_BINARIES_DBG} ${OPENAL_BIN_FILES_DEBUG})
	 endif()
	 if(USE_ENV_PLUGIN)
	  set(SKYX_DIR  ${GASS_DEPENDENCIES_DIR}/SkyX-v0.4 CACHE PATH "SkyX home")
	  find_package(SkyX)
	  set(GASS_SIM_BINARIES_REL ${GASS_SIM_BINARIES_REL} ${SKYX_BINARY_REL})
	  set(GASS_SIM_BINARIES_DBG ${GASS_SIM_BINARIES_DBG} ${SKYX_BINARY_DBG})
	 endif()
	 
	 if(USE_OSG_PLUGIN)
	  find_package(OSGExt 3.2.1 REQUIRED osgUtil
										 osgAnimation
										 osgDB
										 osgGA
										 osgVolume
										 osgFX
										 osgText
										 osgShadow
										 osgViewer
										 osgSim
										 osgTerrain
										 osgManipulator
										 osgParticle
										 osgWidget)
	  set(GASS_SIM_BINARIES_REL ${GASS_SIM_BINARIES_REL} ${OSG_BINARIES_REL})
	  set(GASS_SIM_BINARIES_DBG ${GASS_SIM_BINARIES_DBG} ${OSG_BINARIES_DBG})
	 endif()
	 
	 if(USE_MYGUI_PLUGIN)
	  find_package(MyGUI)
	  set(GASS_SIM_BINARIES_REL ${GASS_SIM_BINARIES_REL} ${MYGUI_BINARIES_REL})
	  set(GASS_SIM_BINARIES_DBG ${GASS_SIM_BINARIES_DBG} ${MYGUI_BINARIES_DBG})
	 endif()
endif()
