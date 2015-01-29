
set(DEPENDENCIES_DIR $ENV{GASS_DEPENDENCIES} CACHE PATH "3rd-party dependency folder")

#TODO: use find package here instead
set(TINYXML2_DIR "${DEPENDENCIES_DIR}/tinyxml2" CACHE PATH "Tinyxml2 folder")
set(ANGELSCRIPT_DIR "${DEPENDENCIES_DIR}/angelscript" CACHE PATH "Angelscript folder")

set (Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.57.0 REQUIRED filesystem system)

find_package(TBB REQUIRED)

if(NOT TBB_FOUND)
        MESSAGE (STATUS "TBB not found. Example project will not be built.")
endif()

set(TBB_LIBRARIES optimized ${TBB_LIBRARY}
	 optimized ${TBB_MALLOC_LIBRARY}
	 debug ${TBB_LIBRARY_DEBUG}
	 debug ${TBB_MALLOC_LIBRARY_DEBUG})

find_package(TinyXML2 REQUIRED)
find_package(AngelScript REQUIRED)





