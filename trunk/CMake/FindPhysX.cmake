
# From osgPhysX (LGPL) see https://github.com/xarray/osgPhysX

FIND_PATH(PHYSX_SDK_ROOT Include/PxPhysicsAPI.h
    PATHS
    $ENV{PATH}
    /usr/include/
    /usr/local/include/
)

FIND_PATH(PHYSX_INCLUDE_DIR PxPhysicsAPI.h PATHS ${PHYSX_SDK_ROOT}/Include)

SET(PHYSX_LIBPATH_PREFIX win32)
SET(PHYSX_LIBNAME_POSTFIX _x86)
IF(CMAKE_CL_64)
    SET(PHYSX_LIBPATH_PREFIX win64)
    SET(PHYSX_LIBNAME_POSTFIX _x64)
ENDIF(CMAKE_CL_64)

MACRO(FIND_PHYSX_LIBRARY VARNAME LIBNAME WITHPOSTFIX)

    SET(LIBPOSTFIX "")
    IF(${WITHPOSTFIX})
        SET(LIBPOSTFIX ${PHYSX_LIBNAME_POSTFIX})
    ENDIF(${WITHPOSTFIX})
    FIND_LIBRARY(PHYSX_${VARNAME}_LIBRARY NAMES ${LIBNAME}${LIBPOSTFIX}
                 PATHS ${PHYSX_SDK_ROOT}/Lib/${PHYSX_LIBPATH_PREFIX})
    FIND_LIBRARY(PHYSX_${VARNAME}_LIBRARY_DEBUG NAMES ${LIBNAME}DEBUG${LIBPOSTFIX}
                 PATHS ${PHYSX_SDK_ROOT}/Lib/${PHYSX_LIBPATH_PREFIX})

ENDMACRO(FIND_PHYSX_LIBRARY VARNAME LIBNAME)

FIND_PHYSX_LIBRARY(CORE PhysX3 1)
FIND_PHYSX_LIBRARY(COMMON PhysX3Common 1)
FIND_PHYSX_LIBRARY(COOKING PhysX3Cooking 1)
FIND_PHYSX_LIBRARY(CHARACTER PhysX3CharacterKinematic 1)
FIND_PHYSX_LIBRARY(EXTENSIONS PhysX3Extensions 0)
FIND_PHYSX_LIBRARY(VEHICLE PhysX3Vehicle 0)
FIND_PHYSX_LIBRARY(TASK PxTask 0)
FIND_PHYSX_LIBRARY(DEBUGGER PhysXVisualDebuggerSDK 0)

SET(PHYSX_FOUND "NO")
IF(PHYSX_INCLUDE_DIR AND PHYSX_CORE_LIBRARY)
    SET(PHYSX_FOUND "YES")
    SET(PHYSX_LIBRARIES
        debug ${PHYSX_CORE_LIBRARY_DEBUG} optimized ${PHYSX_CORE_LIBRARY}
        debug ${PHYSX_COMMON_LIBRARY_DEBUG} optimized ${PHYSX_COMMON_LIBRARY}
        debug ${PHYSX_COOKING_LIBRARY_DEBUG} optimized ${PHYSX_COOKING_LIBRARY}
        debug ${PHYSX_CHARACTER_LIBRARY_DEBUG} optimized ${PHYSX_CHARACTER_LIBRARY}
        debug ${PHYSX_EXTENSIONS_LIBRARY_DEBUG} optimized ${PHYSX_EXTENSIONS_LIBRARY}
        debug ${PHYSX_VEHICLE_LIBRARY_DEBUG} optimized ${PHYSX_VEHICLE_LIBRARY}
        debug ${PHYSX_TASK_LIBRARY_DEBUG} optimized ${PHYSX_TASK_LIBRARY}
        debug ${PHYSX_DEBUGGER_LIBRARY_DEBUG} optimized ${PHYSX_DEBUGGER_LIBRARY}
    )
ENDIF(PHYSX_INCLUDE_DIR AND PHYSX_CORE_LIBRARY)