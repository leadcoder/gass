if (NOT "$ENV{PHYSX4_HOME}" STREQUAL "")
	set (PHYSX4_INSTALL_DIR $ENV{PHYSX4_HOME})
endif()


IF( CMAKE_CL_64 )
  SET( LIB "64" )
ELSE( CMAKE_CL_64 )
  SET( LIB "32" )
ENDIF()


# Look for the header file.
FIND_PATH( PHYSX4_INCLUDE_DIR NAMES PxPhysics.h
           PATHS 
		   PATH_SUFFIXES physx)

MARK_AS_ADVANCED(PHYSX4_INCLUDE_DIR)

SET( PHYSX4_LIBS_FOUND 1 )
SET( PHYSX4_LIBS_DEBUG_FOUND 1 )


# Decide which libraires to add
IF ( NOT DEFINED PHYSX4_LIBS )

  SET( PHYSX4_LIBS 
      "PhysXTask_static"
      "PhysXCommon"
      "PhysXExtensions_static"
      "PhysX"
	  "PhysXFoundation"
      "PhysXVehicle_static"
      "PhysXCooking"
      "PhysXCharacterKinematic_static"
      "PhysXPvdSDK_static"
      )

ENDIF()

#vcpkg hack
set(PHYSX4_DEBUG_LIB_DIR ${PHYSX4_INCLUDE_DIR}/../../debug/lib)
set(PHYSX4_RELEASE_LIB_DIR ${PHYSX4_INCLUDE_DIR}/../../lib)
message(${PHYSX4_RELEASE_LIB_DIR})
# Look for the libraries.
FOREACH( PHYSX4_LIB ${PHYSX4_LIBS})
  STRING(TOUPPER ${PHYSX4_LIB} _upper_lib_name)
  SET( LIB_RELASE_NAME PHYSX4_${_upper_lib_name}_LIBRARY )
  SET( LIB_DEBUG_NAME PHYSX4_${_upper_lib_name}_DEBUG_LIBRARY )
  # unset libraries so that they are always looked for. This is because we want it to automatically
  # update if the PHYSX4_LIB_TYPE is changed.
  UNSET( ${LIB_RELASE_NAME} CACHE)
  #message(${LIB_RELASE_NAME}  ${PHYSX4_INSTALL_DIR}/Lib/${PHYSX4_MSVC_DIR}${LIB}/${PHYSX4_LIB}${PHYSX4_LIB_TYPE_SUFFIX}_${ARCH})
  
  # FIND RELEASE LIBS
  FIND_LIBRARY( ${LIB_RELASE_NAME}
                NAMES ${PHYSX4_LIB}_${LIB}
        		PATHS ${PHYSX4_RELEASE_LIB_DIR}
				NO_DEFAULT_PATH)
  MARK_AS_ADVANCED(${LIB_RELASE_NAME})
                      
  IF( ${LIB_RELASE_NAME} )
    SET( PHYSX4_LIBS_PATHS ${PHYSX4_LIBS_PATHS} optimized ${${LIB_RELASE_NAME}} )
  ELSE()
    SET( PHYSX4_LIBS_FOUND 0 )
    SET( PHYSX4_LIBS_NOTFOUND ${PHYSX4_LIBS_NOTFOUND} ${PHYSX4_LIB} ) 
  ENDIF()
  
  #FIND DEBUG LIBS
  FIND_LIBRARY( ${LIB_DEBUG_NAME}
                NAMES ${PHYSX4_LIB}_${LIB}
                PATHS ${PHYSX4_DEBUG_LIB_DIR}
				NO_DEFAULT_PATH)
  MARK_AS_ADVANCED(${LIB_DEBUG_NAME})
                      
  IF( ${LIB_DEBUG_NAME} )
    IF ( UNIX )
      # To avoid undefined symbols at runtime we need to include the entire static library in our shared library
      SET ( PHYSX4_${_upper_lib_name}_LIBRARY -Wl,-whole-archive ${PHYSX4_${_upper_lib_name}_LIBRARY} -Wl,-no-whole-archive )
    ENDIF()
    SET( PHYSX4_LIBS_DEBUG_PATHS ${PHYSX4_LIBS_DEBUG_PATHS} debug ${${LIB_DEBUG_NAME}} )
  ELSE()
    SET( PHYSX4_DEBUG_LIBS_FOUND 0 )
    SET( PHYSX4_DEBUG_LIBS_NOTFOUND ${PHYSX4_DEBUG_LIBS_NOTFOUND} ${PHYSX4_LIB} ) 
  ENDIF()
ENDFOREACH()

                      
MARK_AS_ADVANCED(PHYSX4_LIBS)

# Copy the results to the output variables.
IF ( PHYSX4_INCLUDE_DIR AND 
     PHYSX4_LIBS_FOUND AND
     PHYSX4_LIBS_DEBUG_FOUND )
  SET(PHYSX4_FOUND 1)
  SET(PHYSX4_LIBRARIES ${PHYSX4_LIBS_PATHS} ${PHYSX4_LIBS_DEBUG_PATHS})
  SET(PHYSX4_INCLUDE_DIR ${PHYSX4_INCLUDE_DIR} )
ELSE()
  SET(PHYSX4_FOUND 0)
  SET(PHYSX4_LIBRARIES)
  SET(PHYSX4_INCLUDE_DIR)
ENDIF()

# Report the results.
IF(NOT PHYSX4_FOUND)
  SET(PHYSX4_DIR_MESSAGE
    "PHYSX4 was not found. Set PHYSX4_INSTALL_DIR to the root directory of the 
installation containing the 'include' and 'lib' folders.")
  IF(PHYSX4_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "${PHYSX4_DIR_MESSAGE}")
  ELSEIF(NOT PHYSX4_FIND_QUIETLY)
    MESSAGE(STATUS "${PHYSX4_DIR_MESSAGE}")
  ENDIF(PHYSX4_FIND_REQUIRED)
ENDIF(NOT PHYSX4_FOUND)