if (NOT "$ENV{ANGELSCRIPT_DIR}" STREQUAL "")
	set (ANGELSCRIPT_DIR $ENV{ANGELSCRIPT_DIR})
endif()
	
set(ANGELSCRIPT_INCLUDE_DIRS ${ANGELSCRIPT_DIR}/include CACHE PATH "AngleScript include directory" FORCE)
set(ANGELSCRIPT_LIBRARY_DIRS ${ANGELSCRIPT_DIR}/lib)

find_library(ANGELSCRIPT_LIBRARY angelscript HINTS ${ANGELSCRIPT_LIBRARY_DIRS} ${ANGELSCRIPT_LIBRARY_DIRS}/release
        PATHS ENV LIBRARY_PATH ENV LD_LIBRARY_PATH)

find_library(ANGELSCRIPT_LIBRARY_DEBUG angelscriptd HINTS ${ANGELSCRIPT_LIBRARY_DIRS} ${ANGELSCRIPT_LIBRARY_DIRS}/debug
        PATHS ENV LIBRARY_PATH ENV LD_LIBRARY_PATH)
		
mark_as_advanced(ANGELSCRIPT_LIBRARY ANGELSCRIPT_LIBRARY_DEBUG)

set(ANGELSCRIPT_LIBRARIES optimized ${ANGELSCRIPT_LIBRARY}
	 debug ${ANGELSCRIPT_LIBRARY_DEBUG})
