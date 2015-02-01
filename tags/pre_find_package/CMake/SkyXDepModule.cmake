set(SKYX_DIR  "${DEPENDENCIES_DIR}/SkyX-v0.4" CACHE PATH "SkyX folder")

set(SKYX_BIN_DIR ${SKYX_DIR}/bin)
set(SKYX_INC_DIR ${SKYX_DIR}/include/SKYX)
set(SKYX_LIB_DIR ${SKYX_DIR}/lib)

set(SKYX_LINK_LIBRARIES debug SkyX_d
			optimized SkyX)

set(SKYX_BIN_FILES_DEBUG ${SKYX_BIN_DIR}/debug/skyx_d.dll)
set(SKYX_BIN_FILES_RELEASE ${SKYX_BIN_DIR}/release/skyx.dll)

