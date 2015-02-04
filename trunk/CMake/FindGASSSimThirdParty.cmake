
#angle script, assume that GASS_DEPENDENCIES_DIR hold this lib
set(ANGELSCRIPT_DIR  "${GASS_DEPENDENCIES_DIR}/angelscript" CACHE PATH "Angel Script folder")
find_package(AngelScript REQUIRED)