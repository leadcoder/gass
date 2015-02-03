
if (NOT "$ENV{TINYXML2_DIR}" STREQUAL "")
	set(TINYXML2_DIR $ENV{TINYXML2_DIR})
endif()
	
set(TINYXML2_INCLUDE_DIRS ${TINYXML2_DIR}/include CACHE PATH "TinyXML2 include directory" FORCE)
set(TINYXML2_LIBRARY_DIRS ${TINYXML2_DIR}/lib)

find_library(TINYXML2_LIBRARY tinyxml2 HINTS ${TINYXML2_LIBRARY_DIRS} ${TINYXML2_LIBRARY_DIRS}/release)
find_library(TINYXML2_LIBRARY_DEBUG tinyxml2_d HINTS ${TINYXML2_LIBRARY_DIRS} ${TINYXML2_LIBRARY_DIRS}/debug)
		
mark_as_advanced(TINYXML2_LIBRARY TINYXML2_LIBRARY_DEBUG)

set(TINYXML2_LIBRARIES optimized ${TINYXML2_LIBRARY}
	 debug ${TINYXML2_LIBRARY_DEBUG})
