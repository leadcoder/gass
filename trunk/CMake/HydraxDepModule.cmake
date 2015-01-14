set(HYDRAX_DIR "${DEPENDENCIES_DIR}/Hydrax-v0.5.1" CACHE PATH "Hydrax folder")

set(HYDRAX_INC_DIR ${HYDRAX_DIR}/include)
set(HYDRAX_LIB_DIR ${HYDRAX_DIR}/lib)

set(HYDRAX_LINK_LIBRARIES debug Hydrax_d
			optimized Hydrax)
