set(SKYX_BIN_DIR ${GASS_DEPENDENCIES_DIR}/SkyX-v0.4/bin)
set(SKYX_INC_DIR ${GASS_DEPENDENCIES_DIR}/SkyX-v0.4/include/SKYX)
set(SKYX_LIB_DIR ${GASS_DEPENDENCIES_DIR}/SkyX-v0.4/lib)

set(SKYX_LINK_LIBRARIES debug SkyX_d
			optimized SkyX)

set(SKYX_BIN_FILES_DEBUG ${SKYX_BIN_DIR}/debug/skyx_d.dll)
set(SKYX_BIN_FILES_RELEASE ${SKYX_BIN_DIR}/release/skyx.dll)

