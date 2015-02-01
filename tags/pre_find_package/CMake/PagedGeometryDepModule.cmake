set(PG_DIR "${DEPENDENCIES_DIR}/PagedGeometry-1.1.1" CACHE PATH "PagedGeometry folder")

set(PG_INC_DIR ${PG_DIR}/include/)
set(PG_LIB_DIR ${PG_DIR}/lib)

set(PG_LINK_LIBRARIES debug PagedGeometry_d
			optimized PagedGeometry)
			
