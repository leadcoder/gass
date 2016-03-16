find_package(TinyXML2 REQUIRED)
#boost
if(WIN32)
  set (Boost_USE_STATIC_LIBS ON)
else()
  set (Boost_USE_STATIC_LIBS OFF CACHE BOOL "Use static boost libraries")
endif()

find_package(Boost 1.46.1 REQUIRED filesystem system)

#TBB
find_package(TBB REQUIRED)
#post-process results 
set(TBB_LIBRARIES optimized ${TBB_LIBRARY}
	 optimized ${TBB_MALLOC_LIBRARY}
	 debug ${TBB_LIBRARY_DEBUG}
	 debug ${TBB_MALLOC_LIBRARY_DEBUG})
