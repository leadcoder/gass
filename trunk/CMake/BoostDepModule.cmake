if(EXISTS "${DEPENDENCIES_DIR}/boost_1_57_0")
	set(BOOST_DIR  "${DEPENDENCIES_DIR}/boost_1_57_0" CACHE PATH "BOOST folder")
else()
	set(BOOST_DIR $ENV{BOOST_HOME} CACHE PATH "BOOST folder")
endif()

set(BOOST_INC_DIR ${BOOST_DIR})
if(${CMAKE_CL_64})
	if(${MSVC10})
		set(BOOST_LIB_DIR "${BOOST_DIR}/lib64-msvc-10.0")
	elseif(${MSVC11})
		set(BOOST_LIB_DIR "${BOOST_DIR}/lib64-msvc-11.0")
	endif()
else()
	if(${MSVC10})
		set(BOOST_LIB_DIR "${BOOST_DIR}/lib32-msvc-10.0")
	elseif(${MSVC11})
		set(BOOST_LIB_DIR "${BOOST_DIR}/lib32-msvc-11.0")
	endif()
endif()

