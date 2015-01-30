find_package(TBB REQUIRED)

if(NOT TBB_FOUND)
        MESSAGE (STATUS "TBB not found. Example project will not be built.")
endif()

set(TBB_LIBRARIES optimized ${TBB_LIBRARY}
	 optimized ${TBB_MALLOC_LIBRARY}
	 debug ${TBB_LIBRARY_DEBUG}
	 debug ${TBB_MALLOC_LIBRARY_DEBUG})


