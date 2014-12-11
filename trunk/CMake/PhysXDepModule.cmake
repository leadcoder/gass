if(EXISTS "${GASS_DEPENDENCIES_DIR}/PhysX-v3.2.4")
	set(PHYSX_DIR  "${GASS_DEPENDENCIES_DIR}/PhysX-v3.2.4" CACHE PATH "PhysX folder")
else()
	set(PHYSX_DIR $ENV{PHYSX_HOME} CACHE PATH "PhysX folder")
endif()

set(PHYSX_INC_DIR ${GASS_DEPENDENCIES_DIR}/PhysX-v3.2.4/Include)
if(${CMAKE_CL_64})
	set(PHYSX_LIB_DIR "${PHYSX_DIR}/Lib/win64")
	set(PHYSX_BIN_DIR "${PHYSX_DIR}/Bin/win64")
	set(LIB_SUFFIX "_x64")
	set(DLL_SUFFIX "_x64")
else()
	set(PHYSX_LIB_DIR "${PHYSX_DIR}/Lib/win32")
	set(PHYSX_BIN_DIR "${PHYSX_DIR}/Bin/win32")
	set(LIB_SUFFIX "_x86")
	set(DLL_SUFFIX "_x86")
endif()

set(PHYSX_LINK_LIBRARIES 
			optimized PhysX3Common${LIB_SUFFIX}
			optimized PhysX3${LIB_SUFFIX}
			optimized PhysX3Cooking${LIB_SUFFIX}
			optimized PhysX3CharacterKinematic${LIB_SUFFIX}
			optimized PhysX3Extensions
			optimized PhysX3Vehicle
			optimized RepX3
			optimized RepXUpgrader3
			optimized PhysXProfileSDK
			optimized PhysXVisualDebuggerSDK
			optimized PxTask
			
			debug PhysX3CommonDEBUG${LIB_SUFFIX}
			debug PhysX3DEBUG${LIB_SUFFIX}
			debug PhysX3CookingDEBUG${LIB_SUFFIX}
			debug PhysX3CharacterKinematicDEBUG${LIB_SUFFIX}
			debug PhysX3ExtensionsDEBUG
			debug PhysX3VehicleDEBUG
			debug RepX3DEBUG
			debug RepXUpgrader3DEBUG
			debug PhysXProfileSDKDEBUG
			debug PhysXVisualDebuggerSDKDEBUG
			debug PxTaskDEBUG
)

set(PHYSX_BIN_FILES_DEBUG
	${PHYSX_BIN_DIR}/PhysX3CommonCHECKED${DLL_SUFFIX}.dll
	${PHYSX_BIN_DIR}/PhysX3CHECKED${DLL_SUFFIX}.dll
	${PHYSX_BIN_DIR}/PhysX3CookingCHECKED${DLL_SUFFIX}.dll
	${PHYSX_BIN_DIR}/PhysX3CharacterKinematicCHECKED${DLL_SUFFIX}.dll
	${PHYSX_BIN_DIR}/PhysX3Profile${DLL_SUFFIX}.dll
)
set(PHYSX_BIN_FILES_RELEASE
	${PHYSX_BIN_DIR}/PhysX3CommonCHECKED${LIB_SUFFIX}.dll
	${PHYSX_BIN_DIR}/PhysX3CHECKED${LIB_SUFFIX}.dll
	${PHYSX_BIN_DIR}/PhysX3CookingCHECKED${LIB_SUFFIX}.dll
	${PHYSX_BIN_DIR}/PhysX3CharacterKinematicCHECKED${LIB_SUFFIX}.dll
	${PHYSX_BIN_DIR}/PhysX3Profile${LIB_SUFFIX}.dll
)