# Update with proper paths to ${GASS_DEPENDENCIES_DIR}/ois when setup there instead of in ${GASS_DEPENDENCIES_DIR}/lib
set(OIS_DIR ${GASS_DEPENDENCIES_DIR}/lib)
set(OIS_INC_DIR ${OIS_DIR})
set(OIS_BIN_DIR ${OIS_DIR})
set(OIS_LIB_DIR ${OIS_DIR})

#set(OIS_DIR ${GASS_DEPENDENCIES_DIR}/OIS)
#set(OIS_INC_DIR ${OIS_DIR}/include)
#set(OIS_BIN_DIR ${OIS_DIR}/bin)
#set(OIS_LIB_DIR ${OIS_DIR}/lib)

set(OIS_LINK_LIBRARIES 
	debug OIS_d
	optimized OIS)

set(OIS_BIN_FILES_DEBUG ${OIS_BIN_DIR}/OIS_d.dll)
set(OIS_BIN_FILES_RELEASE ${OIS_BIN_DIR}/OIS.dll)

