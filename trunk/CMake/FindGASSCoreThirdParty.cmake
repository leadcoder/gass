
#folder holding precompiled dependencies
set(GASS_DEPENDENCIES_DIR $ENV{GASS_DEPENDENCIES} CACHE PATH "3rd-party dependency folder")

#tinyxml2, specific version used in gass, ie avoid system wide search 
set(TINYXML2_DIR "${GASS_DEPENDENCIES_DIR}/tinyxml2" CACHE PATH "tinyxml2 folder")
find_package(TinyXML2 REQUIRED)

#boost
set (Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.57.0 REQUIRED filesystem system)

#TBB
find_package(TBB REQUIRED)
#post-process results 
set(TBB_LIBRARIES optimized ${TBB_LIBRARY}
	 optimized ${TBB_MALLOC_LIBRARY}
	 debug ${TBB_LIBRARY_DEBUG}
	 debug ${TBB_MALLOC_LIBRARY_DEBUG})
