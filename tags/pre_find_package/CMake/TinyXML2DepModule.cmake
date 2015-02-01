set(TINYXML2_DIR "${DEPENDENCIES_DIR}/tinyxml2" CACHE PATH "Tinyxml2 folder")

set(TINYXML2_INC_DIR ${TINYXML2_DIR}/include)
set(TINYXML2_LIB_DIR ${TINYXML2_DIR}/lib)
set(TINYXML2_LINK_LIBRARIES debug tinyxml2_d
	optimized tinyxml2)
