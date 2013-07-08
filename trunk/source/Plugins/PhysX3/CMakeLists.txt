#set lib name here!
set(LIB_NAME "GASSPluginPhysX3")

if(EXISTS "${GASS_DEPENDENCIES_DIR}/physx")
	set(PHYSX_DIR  "${GASS_DEPENDENCIES_DIR}/physx" CACHE PATH "PhysX folder")
else()
	set(PHYSX_DIR $ENV{PHYSX_HOME} CACHE PATH "PhysX folder")
endif()
		
include_directories(${GASS_COMMON_INCLUDE_DIRS} "${PHYSX_DIR}/include")

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

#msvc only, need other check if gcc
if(${CMAKE_CL_64})
	set(PHYSX_LINK_DIR "${PHYSX_DIR}/lib/win64")
	set(LIB_SUFFIX "_x64")
else()
	set(PHYSX_LINK_DIR "${PHYSX_DIR}/lib/win32")
	set(LIB_SUFFIX "_x86")
endif()


#Add some foldets where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS} ${PHYSX_LINK_DIR})
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})


#Add project dependency libs
set(PROJ_LINK_LIBRARIES 
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

target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_COMMON_LINK_LIBRARIES})
target_link_libraries(${LIB_NAME} ${PROJ_LINK_LIBRARIES})

install (TARGETS ${LIB_NAME} DESTINATION ${GASS_INSTALL_PLUGIN_LIB_DIR})		
#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include/plugins FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

