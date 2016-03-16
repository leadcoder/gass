
#angle script, assume that ANGELSCRIPT_HOME hold this lib
set(ANGELSCRIPT_DIR   $ENV{ANGELSCRIPT_HOME} CACHE PATH "Angel Script folder")
find_package(AngelScript REQUIRED)