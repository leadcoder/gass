# - Find PHYSX3
# Find the native PHYSX3 headers and libraries.
#
#  PHYSX3_INCLUDE_DIR -  where to find the include files of PHYSX3
#  PHYSX3_LIBRARIES    - List of libraries when using PHYSX3.
#  PHYSX3_FOUND        - True if PHYSX3 found.
#  PHYSX3_FLAGS        - Flags needed for physx to build 


if (NOT "$ENV{PHYSX3_HOME}" STREQUAL "")
	set (PHYSX3_INSTALL_DIR $ENV{PHYSX3_HOME})
endif()

if(MSVC_VERSION EQUAL 1600)
	set(PHYSX3_MSVC_DIR vc10win)
endif()
if(MSVC_VERSION EQUAL 1700)
	set(PHYSX3_MSVC_DIR vc11win)
endif()
if(MSVC_VERSION EQUAL 1800)
	set(PHYSX3_MSVC_DIR vc12win)
endif()

if(MSVC_VERSION EQUAL 1900)
	set(PHYSX3_MSVC_DIR vc14win)
endif()

if(MSVC_VERSION GREATER 1900)
	set(PHYSX3_MSVC_DIR vc15win)
endif()



GET_FILENAME_COMPONENT(module_file_path ${CMAKE_CURRENT_LIST_FILE} PATH )

IF( CMAKE_CL_64 )
  SET( LIB "64" )
  SET( ARCH "x64" )
ELSE( CMAKE_CL_64 )
  SET( LIB "32" )
  SET( ARCH "x86" )
ENDIF( CMAKE_CL_64 )

IF( NOT DEFINED PHYSX3_INSTALL_DIR )
  SET(PHYSX3_INSTALL_DIR "" CACHE PATH "Path to external PhysX 3 installation" )
ENDIF()
MARK_AS_ADVANCED(PHYSX3_INSTALL_DIR)


# Look for the header file.
FIND_PATH( PHYSX3_INCLUDE_DIR NAMES PxPhysics.h
           PATHS /usr/local/include
                 ${PHYSX3_INSTALL_DIR}/Include
		 ${PHYSX3_INSTALL_DIR}/include
                 ${PHYSX3_INSTALL_DIR}/include/PhysX3
                 ../External/include/physx3
                 ../../External/include
                 ${module_file_path}/../../External/include
                 ${module_file_path}/../../../External/include )

MARK_AS_ADVANCED(PHYSX3_INCLUDE_DIR)

SET( PHYSX3_LIBS_FOUND 1 )
SET( PHYSX3_LIBS_DEBUG_FOUND 1 )


# Decide which libraires to add
IF ( NOT DEFINED PHYSX3_LIBS )

  SET( PHYSX3_LIBS 
      "PxTask"
      "PhysX3Common"
      "PhysX3Extensions"
      "PhysX3"
      "PhysX3Vehicle"
      "PhysX3Cooking"
      "PhysX3CharacterKinematic"
      "PhysXProfileSDK"
      "PhysXVisualDebuggerSDK"
      )

  IF( UNIX )
    SET( PHYSX3_LIBS ${PHYSX3_LIBS}
      "PvdRuntime"
      "LowLevel"
      "LowLevelCloth"
      "SceneQuery"
      "SimulationController"
      )
  ENDIF()
  
ENDIF()


SET( PHYSX3_LIB_TYPE "CHECKED" CACHE STRING "PhysX library type")
SET_PROPERTY( CACHE PHYSX3_LIB_TYPE PROPERTY STRINGS RELEASE CHECKED PROFILE )
IF( ${PHYSX3_LIB_TYPE} STREQUAL RELEASE )
  SET( PHYSX3_LIB_TYPE_SUFFIX "" )
ELSE()
  SET( PHYSX3_LIB_TYPE_SUFFIX ${PHYSX3_LIB_TYPE} )
ENDIF()


# Look for the libraries.
FOREACH( PHYSX3_LIB ${PHYSX3_LIBS})
  STRING(TOUPPER ${PHYSX3_LIB} _upper_lib_name)
  SET( PX_LIB_NAME PHYSX3_${_upper_lib_name}_LIBRARY )
  SET( LIB_DEBUG_NAME PHYSX3_${_upper_lib_name}_DEBUG_LIBRARY )
  # unset libraries so that they are always looked for. This is because we want it to automatically
  # update if the PHYSX3_LIB_TYPE is changed.
  UNSET( ${PX_LIB_NAME} CACHE)
  #message(${PX_LIB_NAME}  ${PHYSX3_INSTALL_DIR}/Lib/${PHYSX3_MSVC_DIR}${LIB}/${PHYSX3_LIB}${PHYSX3_LIB_TYPE_SUFFIX}_${ARCH})
  # FIND RELEASE LIBS
  FIND_LIBRARY( ${PX_LIB_NAME}
                NAMES ${PHYSX3_LIB}${PHYSX3_LIB_TYPE_SUFFIX}_${ARCH} ${PHYSX3_LIB}${PHYSX3_LIB_TYPE_SUFFIX}
                PATHS ${PHYSX3_INSTALL_DIR}/Lib/win${LIB}
                      ${PHYSX3_INSTALL_DIR}/Lib/${PHYSX3_MSVC_DIR}${LIB}
                      ${PHYSX3_INSTALL_DIR}/Lib/linux${LIB}
                      ${PHYSX3_INSTALL_DIR}/lib${LIB}
                      ../External/lib${LIB}
                      ../../External/lib${LIB}
                      ${module_file_path}/../../External/lib${LIB}
                      ${module_file_path}/../../../External/lib${LIB} )
  MARK_AS_ADVANCED(${PX_LIB_NAME})
                      
  IF( ${PX_LIB_NAME} )
    IF ( UNIX )
      # To avoid undefined symbols at runtime we need to include the entire static library in our shared library
      SET ( PHYSX3_${_upper_lib_name}_LIBRARY -Wl,-whole-archive ${PHYSX3_${_upper_lib_name}_LIBRARY} -Wl,-no-whole-archive )
    ENDIF()
    SET( PHYSX3_LIBS_PATHS ${PHYSX3_LIBS_PATHS} optimized ${${PX_LIB_NAME}} )
  ELSE()
    SET( PHYSX3_LIBS_FOUND 0 )
    SET( PHYSX3_LIBS_NOTFOUND ${PHYSX3_LIBS_NOTFOUND} ${PHYSX3_LIB} ) 
  ENDIF()
  
  

  #FIND DEBUG LIBS
  FIND_LIBRARY( ${LIB_DEBUG_NAME}
                NAMES ${PHYSX3_LIB}DEBUG_${ARCH} ${PHYSX3_LIB}DEBUG
                PATHS ${PHYSX3_INSTALL_DIR}/Lib/win${LIB}
                      ${PHYSX3_INSTALL_DIR}/Lib/${PHYSX3_MSVC_DIR}${LIB}
                      ${PHYSX3_INSTALL_DIR}/Lib/linux${LIB}
                      ${PHYSX3_INSTALL_DIR}/lib${LIB}
                      ../External/lib${LIB}
                      ../../External/lib${LIB}
                      ${module_file_path}/../../External/lib${LIB}
                      ${module_file_path}/../../../External/lib${LIB} )
  MARK_AS_ADVANCED(${LIB_DEBUG_NAME})
                      
  IF( ${LIB_DEBUG_NAME} )
    IF ( UNIX )
      # To avoid undefined symbols at runtime we need to include the entire static library in our shared library
      SET ( PHYSX3_${_upper_lib_name}_LIBRARY -Wl,-whole-archive ${PHYSX3_${_upper_lib_name}_LIBRARY} -Wl,-no-whole-archive )
    ENDIF()
    SET( PHYSX3_LIBS_DEBUG_PATHS ${PHYSX3_LIBS_DEBUG_PATHS} debug ${${LIB_DEBUG_NAME}} )
  ELSE()
    SET( PHYSX3_DEBUG_LIBS_FOUND 0 )
    SET( PHYSX3_DEBUG_LIBS_NOTFOUND ${PHYSX3_DEBUG_LIBS_NOTFOUND} ${PHYSX3_LIB} ) 
  ENDIF()
ENDFOREACH()

#ADDED BY JH
IF(WIN32)
SET( PHYSX3_BINS 
      "PhysX3Common"
      "PhysX3"
      "PhysX3Cooking"
      "PhysX3CharacterKinematic"
      "PhysX3Gpu"
      )
FOREACH( PHYSX3_BIN ${PHYSX3_BINS})
  #STRING(TOUPPER ${PHYSX3_BIN} _upper_lib_name)
  #SET( PX_BIN_NAME_REL PHYSX3_${_upper_bin_name}_BINARY )
  #SET( PX_BIN_NAME_DBG PHYSX3_${_upper_bin_name}_DEBUG_BINARY )
  SET( PX_BIN_NAME_REL PHYSX3_${PHYSX3_BIN}_BINARY )
  SET( PX_BIN_NAME_DBG PHYSX3_${PHYSX3_BIN}_DEBUG_BINARY )
  
  
  UNSET( ${PX_BIN_NAME_REL} CACHE)
  
  FIND_FILE( ${PX_BIN_NAME_REL}
                NAMES ${PHYSX3_BIN}${PHYSX3_LIB_TYPE_SUFFIX}_${ARCH}.dll ${PHYSX3_LIB}${PHYSX3_LIB_TYPE_SUFFIX}.dll
                PATHS ${PHYSX3_INSTALL_DIR}/bin/win${LIB}
                      ${PHYSX3_INSTALL_DIR}/bin/${PHYSX3_MSVC_DIR}${LIB}
                      ${PHYSX3_INSTALL_DIR}/bin${LIB}
                      ../External/bin${LIB}
                      ../../External/bin${LIB}
                      ${module_file_path}/../../External/bin${LIB}
                      ${module_file_path}/../../../External/bin${LIB} )
  SET(PX_BINARIES_REL ${PX_BINARIES_REL} ${${PX_BIN_NAME_REL}})
  
  FIND_FILE( ${PX_BIN_NAME_DBG}
                NAMES ${PHYSX3_BIN}DEBUG_${ARCH}.dll ${PHYSX3_LIB}DEBUG
                PATHS ${PHYSX3_INSTALL_DIR}/bin/win${LIB}
                      ${PHYSX3_INSTALL_DIR}/bin/${PHYSX3_MSVC_DIR}${LIB}
                      ${PHYSX3_INSTALL_DIR}/bin/linux${LIB}
                      ${PHYSX3_INSTALL_DIR}/bin${LIB}
                      ../External/bin${LIB}
                      ../../External/bin${LIB}
                      ${module_file_path}/../../External/bin${LIB}
                      ${module_file_path}/../../../External/bin${LIB} )
  
  SET(PX_BINARIES_DBG ${PX_BINARIES_DBG} ${${PX_BIN_NAME_DBG}})
  ENDFOREACH()
  FIND_FILE(PX_NV_BIN_NAME
                NAMES nvToolsExt64_1.dll nvToolsExt_1.dll
                PATHS ${PHYSX3_INSTALL_DIR}/bin/win${LIB}
                      ${PHYSX3_INSTALL_DIR}/bin/${PHYSX3_MSVC_DIR}${LIB}
                      ${PHYSX3_INSTALL_DIR}/bin${LIB})
  SET(PX_BINARIES_REL ${PX_BINARIES_REL} ${PX_NV_BIN_NAME})
  SET(PX_BINARIES_DBG ${PX_BINARIES_DBG} ${PX_NV_BIN_NAME})
  
ENDIF()
                      
MARK_AS_ADVANCED(PHYSX3_LIBS)

# Copy the results to the output variables.
IF ( PHYSX3_INCLUDE_DIR AND 
     PHYSX3_LIBS_FOUND AND
     PHYSX3_LIBS_DEBUG_FOUND )
  SET(PHYSX3_FOUND 1)
  SET(PHYSX3_LIBRARIES ${PHYSX3_LIBS_PATHS} ${PHYSX3_LIBS_DEBUG_PATHS})
  SET(PHYSX3_INCLUDE_DIR ${PHYSX3_INCLUDE_DIR} )
ELSE()
  SET(PHYSX3_FOUND 0)
  SET(PHYSX3_LIBRARIES)
  SET(PHYSX3_INCLUDE_DIR)
ENDIF()

# Report the results.
IF(NOT PHYSX3_FOUND)
  SET(PHYSX3_DIR_MESSAGE
    "PHYSX3 was not found. Set PHYSX3_INSTALL_DIR to the root directory of the 
installation containing the 'include' and 'lib' folders.")
  IF(PHYSX3_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "${PHYSX3_DIR_MESSAGE}")
  ELSEIF(NOT PHYSX3_FIND_QUIETLY)
    MESSAGE(STATUS "${PHYSX3_DIR_MESSAGE}")
  ENDIF(PHYSX3_FIND_REQUIRED)
ENDIF(NOT PHYSX3_FOUND)
