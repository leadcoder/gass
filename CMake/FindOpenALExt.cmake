
find_package(OpenAL)
if (WIN32)
	  find_file(OPENAL_BINARY_REL NAMES "OpenAL32.dll" HINTS ${OPENAL_BIN_DIR} ${OPENAL_BIN_DIR}/release)
	  find_file(OPENAL_BINARY_DBG NAMES "OpenAL32.dll" HINTS ${OPENAL_BIN_DIR} ${OPENAL_BIN_DIR}/debug)
endif()