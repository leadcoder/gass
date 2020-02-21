
set(PLATFORM_DEPS "")
if(NOT WIN32)
	#Add dependency guid
	find_package(UUID REQUIRED)
	gass_create_dep_target(UUID INCLUDE_DIRS ${UUID_INCLUDE_DIRS} LIBRARIES ${UUID_LIBRARIES})
   	set(PLATFORM_DEPS UUID)
endif()

#gass_get_source_from_current_dir(CPP_FILES H_FILES)
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${CPP_FILES})
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Header Files" FILES ${H_FILES})

set(CPP_FILES
	GASSASScriptComponent.cpp
	GASSBaseSceneComponent.cpp
	GASSBaseSceneManager.cpp
	GASSGeometryFlags.cpp
	GASSGraphicsMesh.cpp
	GASSPhysicsMesh.cpp
	GASSPlatformTeam.cpp
	GASSPlatformType.cpp
	GASSResource.cpp
	GASSResourceGroup.cpp
	GASSResourceHandle.cpp
	GASSResourceLocation.cpp
	GASSResourceManager.cpp
	GASSScene.cpp
	GASSSceneManagerFactory.cpp
	GASSSceneObject.cpp
	GASSSceneObjectLink.cpp
	GASSSceneObjectRef.cpp
	GASSSceneObjectTemplate.cpp
	GASSScriptManager.cpp
	GASSSimEngine.cpp
	GASSSimEngineConfig.cpp
	GASSSimSystem.cpp
	GASSSimSystemManager.cpp
	GASSSystemFactory.cpp
	GASSSystemStepper.cpp
	Utils/GASSCollisionHelper.cpp
	Utils/GASSProfiler.cpp
	Utils/GASSProfileRuntimeHandler.cpp
	Utils/guid.cpp
	Utils/Script/scriptarray.cpp
	Utils/Script/scriptbuilder.cpp
	Utils/Script/scriptstdstring.cpp
	Utils/Script/scriptstdstring_utils.cpp)

set(H_FILES
	GASS.h
	GASSASScriptComponent.h
	GASSBaseSceneComponent.h
	GASSBaseSceneManager.h
	GASSCommon.h
	GASSEulerRotation.h
	GASSFileResourceCallback.h
	GASSGeoLocation.h
	GASSGeometryFlags.h
	GASSGraphicsMaterial.h
	GASSGraphicsMesh.h
	GASSPhysicsMaterial.h
	GASSPhysicsMesh.h
	GASSPlatformDetectionData.h
	GASSPlatformTeam.h
	GASSPlatformType.h
	GASSResource.h
	GASSResourceGroup.h
	GASSResourceHandle.h
	GASSResourceLocation.h
	GASSResourceManager.h
	GASSScene.h
	GASSSceneManagerFactory.h
	GASSSceneObject.h
	GASSSceneObjectLink.h
	GASSSceneObjectRef.h
	GASSSceneObjectTemplate.h
	GASSSceneObjectVisitors.h
	GASSScriptManager.h
	GASSSimEngine.h
	GASSSimEngineConfig.h
	GASSSimSystem.h
	GASSSimSystemManager.h
	GASSSimSystemManagerConfig.h
	GASSSystemFactory.h
	GASSSystemStepper.h
	GASSThreading.h
	Interface/GASSICameraComponent.h
	Interface/GASSICollisionComponent.h
	Interface/GASSICollisionSceneManager.h
	Interface/GASSICollisionSystem.h
	Interface/GASSIControlSettingsSystem.h
	Interface/GASSIGeometryComponent.h
	Interface/GASSIGraphComponent.h
	Interface/GASSIGraphEdgeComponent.h
	Interface/GASSIGraphicsSceneManager.h
	Interface/GASSIGraphicsSystem.h
	Interface/GASSIGraphNodeComponent.h
	Interface/GASSIGroupComponent.h
	Interface/GASSIInputComponent.h
	Interface/GASSIInputSystem.h
	Interface/GASSILightComponent.h
	Interface/GASSILocationComponent.h
	Interface/GASSIManualMeshComponent.h
	Interface/GASSIMapCameraComponent.h
	Interface/GASSIMapComponent.h
	Interface/GASSIMaterialSystem.h
	Interface/GASSIMeshComponent.h
	Interface/GASSIMissionSceneManager.h
	Interface/GASSIMissionSystem.h
	Interface/GASSINavigationComponent.h
	Interface/GASSINavigationMeshComponent.h
	Interface/GASSINetworkComponent.h
	Interface/GASSIPhysicsBodyComponent.h
	Interface/GASSIPhysicsGeometryComponent.h
	Interface/GASSIPhysicsHingeJointComponent.h
	Interface/GASSIPhysicsPrismaticJointComponent.h
	Interface/GASSIPhysicsSceneManager.h
	Interface/GASSIPhysicsSuspensionComponent.h
	Interface/GASSIPhysicsSystem.h
	Interface/GASSIPlatformComponent.h
	Interface/GASSIProjectionSceneManager.h
	Interface/GASSIRenderWindow.h
	Interface/GASSIResourceComponent.h
	Interface/GASSIRoadNavigationComponent.h
	Interface/GASSISceneManager.h
	Interface/GASSIShape.h
	Interface/GASSITemplateSourceComponent.h
	Interface/GASSITerrainComponent.h
	Interface/GASSITerrainSceneManager.h
	Interface/GASSIViewport.h
	Interface/GASSIWaypointListComponent.h
	Interface/GASSIWGS84SceneManager.h
	Interface/GASSIWorldLocationComponent.h
	Messages/GASSCoreSceneMessages.h
	Messages/GASSCoreSceneObjectMessages.h
	Messages/GASSCoreSystemMessages.h
	Messages/GASSGeoSceneObjectMessages.h
	Messages/GASSGraphicsSceneMessages.h
	Messages/GASSGraphicsSceneObjectMessages.h
	Messages/GASSGraphicsSystemMessages.h
	Messages/GASSInputMessages.h
	Messages/GASSNetworkSceneObjectMessages.h
	Messages/GASSNetworkSystemMessages.h
	Messages/GASSPhysicsSceneMessages.h
	Messages/GASSPhysicsSceneObjectMessages.h
	Messages/GASSPlatformMessages.h
	Messages/GASSScriptSystemMessages.h
	Messages/GASSSensorMessages.h
	Messages/GASSSoundSceneObjectMessages.h
	Messages/GASSWeaponMessages.h
	Utils/GASSCollisionHelper.h
	Utils/GASSGUID.h
	Utils/GASSProfiler.h
	Utils/GASSProfileRuntimeHandler.h
	Utils/GASSSimpleProfile.h
	Utils/guid.h
	Utils/Script/scriptarray.h
	Utils/Script/scriptbuilder.h
	Utils/Script/scriptstdstring.h)

if(GASS_BUILD_CORE)
	set(CORE_TARGET GASSCore)
else()
	find_package(gass)
	set(CORE_TARGET GASS::GASSCore)
endif()
gass_setup_lib(GASSSim
				BUILDTYPE ${GASS_BUILDTYPE}
				SOURCE_FILES ${CPP_FILES} 
				HEADER_FILES ${H_FILES}
				PUBLIC_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../"
				PUBLIC_DEPS ${CORE_TARGET} Angelscript
				PRIVATE_DEPS ${PLATFORM_DEPS}
				PRIVATE_DEFINITIONS GASS_EXPORTS ${GASS_COMMON_DEFINITIONS} )
					
