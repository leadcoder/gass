
# set(OPENAL_DIR  "${DEPENDENCIES_DIR}/OpenAL1.1" CACHE PATH "OpenAL folder")

# set(OPENAL_INC_DIR ${OPENAL_DIR}/include)
# set(OPENAL_BIN_DIR "${OPENAL_DIR}/bin")

# if(${CMAKE_CL_64})
	# set(OPENAL_LIB_DIR "${OPENAL_DIR}/libs/Win64")
# else()
	# set(OPENAL_LIB_DIR "${OPENAL_DIR}/libs/Win32")
# endif()


# set(OPENAL_LINK_LIBRARIES 
	# debug OpenAL32 
	# optimized OpenAL32)
	
find_package(OpenAL)

set(OPENAL_BIN_FILES_DEBUG ${OPENAL_BIN_DIR}/OpenAL32.dll)
set(OPENAL_BIN_FILES_RELEASE ${OPENAL_BIN_DIR}/OpenAL32.dll)
