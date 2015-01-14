set(ANGELSCRIPT_DIR "${DEPENDENCIES_DIR}/angelscript" CACHE PATH "Angelscript folder")

set(ANGELSCRIPT_INC_DIR ${ANGELSCRIPT_DIR}/include/)
set(ANGELSCRIPT_LIB_DIR ${ANGELSCRIPT_DIR}/lib)

set(ANGELSCRIPT_LINK_LIBRARIES debug angelscriptd
			optimized angelscript)
