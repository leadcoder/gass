vcpkg_fail_port_install(ON_TARGET "UWP")

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO leadcoder/gass
    REF 67adb9f79b56c24fb2c4f988b01872049488ac5d
    SHA512 9df1e26a63a607438cd00e8fe6db33b68f7cf81b9f62da40482f7550ef88f7a7ba55194e53960a565844e63ab91d75ce6ab632203932fa4422b3d11b26f184cf
    HEAD_REF devel
)

if (VCPKG_LIBRARY_LINKAGE STREQUAL "static")
    set(GASS_BUILDTYPE_STATIC TRUE)
else()
    set(GASS_BUILDTYPE_STATIC FALSE)
endif()

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
    OPTIONS
        -DGASS_BUILDTYPE_STATIC=${GASS_BUILDTYPE_STATIC}
        -DGASS_BUILD_CORE=ON
		-DGASS_BUILD_SIM=OFF
		-DGASS_INSTALL_DEP_BINARIES=OFF
		-DGASS_INSTALL_INCLUDE_DIR:STRING=include/gasscore
)

vcpkg_install_cmake()
vcpkg_copy_pdbs()

vcpkg_fixup_cmake_targets(CONFIG_PATH "CMake")

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include ${CURRENT_PACKAGES_DIR}/debug/share)

set(GASS_TOOL_PATH_REL ${CURRENT_PACKAGES_DIR}/tools/${PORT})
set(GASS_TOOL_PATH_DBG ${CURRENT_PACKAGES_DIR}/debug/tools/${PORT})

file(GLOB GASS_TOOLS ${CURRENT_PACKAGES_DIR}/bin/*.exe)
if(GASS_TOOLS)
	file(COPY ${GASS_TOOLS} DESTINATION ${GASS_TOOL_PATH_REL})
	file(REMOVE_RECURSE ${GASS_TOOLS})
endif()

file(GLOB GASS_TOOLS_DBG ${CURRENT_PACKAGES_DIR}/debug/bin/*.exe)

if(GASS_TOOLS_DBG)
	file(REMOVE_RECURSE ${GASS_TOOLS_DBG})
endif()

#move cmake-files to share
file(GLOB GASS_CMAKE_FILES ${CURRENT_PACKAGES_DIR}/CMake/*.cmake)
file(COPY ${GASS_CMAKE_FILES} DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT}/CMake)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/CMake ${CURRENT_PACKAGES_DIR}/debug/CMake)

#install copyright
file(INSTALL ${SOURCE_PATH}/LGPL DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)