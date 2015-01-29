# if(EXISTS "${DEPENDENCIES_DIR}/tbb")
	# set(TBB_DIR "${DEPENDENCIES_DIR}/tbb" CACHE PATH "TBB folder")
# else()
	# set(BOOST_DIR $ENV{TBB_HOME} CACHE PATH "TBB folder")
# endif()

# set(TBB_INC_DIR "${TBB_DIR}/include")
# if(${CMAKE_CL_64})
	# if(${MSVC10})
		# set(TBB_LIB_DIR "${TBB_DIR}/lib/intel64/vc10")
		# set(TBB_BIN_DIR "${TBB_DIR}/bin/intel64/vc10")
	# elseif(${MSVC11})
		# set(TBB_LIB_DIR "${TBB_DIR}/lib/intel64/vc11")
		# set(TBB_BIN_DIR "${TBB_DIR}/bin/intel64/vc11")
	# endif()
# else()
	# if(${MSVC10})
		# set(TBB_LIB_DIR "${TBB_DIR}/lib/ia32/vc10")
		# set(TBB_BIN_DIR "${TBB_DIR}/bin/ia32/vc10")
	# elseif(${MSVC11})
		# set(TBB_LIB_DIR "${TBB_DIR}/lib/ia32/vc11")
		# set(TBB_BIN_DIR "${TBB_DIR}/bin/ia32/vc11")
	# endif()
# endif()

# set(TBB_BIN_FILES_DEBUG ${TBB_BIN_DIR}/tbb_debug.dll)
# set(TBB_BIN_FILES_RELEASE ${TBB_BIN_DIR}/tbb.dll)


##############################################################################
# Configuration
##############################################################################

# If your TBB install directory is not found automatically, enter it here or use TBB_INSTALL_DIR env variable. (w/o trailing slash)
# If your compiler is not detected automatically, enter it here. (e.g. vc9 or cc3.2.3_libc2.3.2_kernel2.4.21 or cc4.0.1_os10.4.9)
#set(TBB_COMPILER "...")

##############################################################################
# Find TBB
##############################################################################
find_package(TBB REQUIRED)

if(NOT TBB_FOUND)
        MESSAGE (STATUS "TBB not found. Example project will not be built.")
endif()

