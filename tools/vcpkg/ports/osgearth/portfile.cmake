
set(SOURCE_PATH ${CURRENT_BUILDTREES_DIR}/src)

#remove old check out
file(REMOVE_RECURSE ${SOURCE_PATH})

#do git clone to include submodules
find_program(GIT NAMES git git.cmd)
vcpkg_execute_required_process(
      COMMAND ${GIT} clone https://github.com/leadcoder/osgearth.git ${SOURCE_PATH}
      ALLOW_IN_DOWNLOAD_MODE
      WORKING_DIRECTORY ${CURRENT_BUILDTREES_DIR}
      LOGNAME git-clone-${TARGET_TRIPLET}
    )
	
#checkout commit
vcpkg_execute_required_process(
      ALLOW_IN_DOWNLOAD_MODE
      COMMAND ${GIT} checkout 67f0ca6a40fff5dcac90bf6d05b7aec5b5c3573a
      WORKING_DIRECTORY ${SOURCE_PATH}
      LOGNAME git-checkout-${TARGET_TRIPLET}
    )

vcpkg_execute_required_process(
      ALLOW_IN_DOWNLOAD_MODE
      COMMAND ${GIT} submodule update --init --recursive
      WORKING_DIRECTORY ${SOURCE_PATH}
      LOGNAME git-submodule-${TARGET_TRIPLET}
	)


vcpkg_apply_patches(
    SOURCE_PATH "${SOURCE_PATH}"
    PATCHES 
		export-plugins.patch
)


if("tools" IN_LIST FEATURES)
	message(STATUS "Downloading submodules")
	# Download submodules from github manually since vpckg doesn't support submodules natively.
	# IMGUI
	#osgEarth is currently using imgui docking branch for osgearth_imgui example
	vcpkg_from_github(
		OUT_SOURCE_PATH IMGUI_SOURCE_PATH
		REPO ocornut/imgui
		REF 9e8e5ac36310607012e551bb04633039c2125c87 #docking branch
		SHA512 1f1f743833c9a67b648922f56a638a11683b02765d86f14a36bc6c242cc524c4c5c5c0b7356b8053eb923fafefc53f4c116b21fb3fade7664554a1ad3b25e5ff
		HEAD_REF master
	)

	# Remove exisiting folder in case it was not cleaned
	file(REMOVE_RECURSE "${SOURCE_PATH}/src/third_party/imgui")
	# Copy the submodules to the right place
	file(COPY "${IMGUI_SOURCE_PATH}/" DESTINATION "${SOURCE_PATH}/src/third_party/imgui")
endif()

file(REMOVE
    "${SOURCE_PATH}/CMakeModule/FindGEOS.cmake"
    "${SOURCE_PATH}/CMakeModule/FindLibZip.cmake"
    "${SOURCE_PATH}/CMakeModule/FindOSG.cmake"
    "${SOURCE_PATH}/CMakeModule/FindSqlite3.cmake"
    "${SOURCE_PATH}/CMakeModule/FindWEBP.cmake"
    "${SOURCE_PATH}/src/osgEarth/tinyxml.h" # https://github.com/gwaldron/osgearth/issues/1002
)

string(COMPARE EQUAL "${VCPKG_LIBRARY_LINKAGE}" "dynamic" BUILD_SHARED)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        tools       OSGEARTH_BUILD_TOOLS
        blend2d     CMAKE_REQUIRE_FIND_PACKAGE_BLEND2D
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        ${FEATURE_OPTIONS}
        -DLIB_POSTFIX=
        -DOSGEARTH_BUILD_SHARED_LIBS=${BUILD_SHARED}
        -DOSGEARTH_BUILD_EXAMPLES=OFF
        -DOSGEARTH_BUILD_TESTS=OFF
        -DOSGEARTH_BUILD_DOCS=OFF
        -DOSGEARTH_BUILD_PROCEDURAL_NODEKIT=ON
		-DOSGEARTH_BUILD_LEGACY_SPLAT_NODEKIT=ON
        -DOSGEARTH_BUILD_TRITON_NODEKIT=OFF
        -DOSGEARTH_BUILD_SILVERLINING_NODEKIT=OFF
        -DOSGEARTH_BUILD_ZIP_PLUGIN=OFF
		-DWITH_EXTERNAL_TINYXML=ON
	    -DCMAKE_JOB_POOL_LINK=console # Serialize linking to avoid OOM
    OPTIONS_DEBUG
        -DOSGEARTH_BUILD_TOOLS=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup()

if(VCPKG_LIBRARY_LINKAGE STREQUAL "static")
    vcpkg_replace_string("${CURRENT_PACKAGES_DIR}/include/osgEarth/Export" "defined( OSGEARTH_LIBRARY_STATIC )" "1")
endif()

set(osg_plugin_pattern "${VCPKG_TARGET_SHARED_LIBRARY_PREFIX}osgdb*${VCPKG_TARGET_SHARED_LIBRARY_SUFFIX}")
if("tools" IN_LIST FEATURES)
    if(VCPKG_LIBRARY_LINKAGE STREQUAL "dynamic")
        file(GLOB osg_plugins "${CURRENT_PACKAGES_DIR}/plugins/${osg_plugins_subdir}/${osg_plugin_pattern}")
        file(INSTALL ${osg_plugins} DESTINATION "${CURRENT_PACKAGES_DIR}/tools/${PORT}/${osg_plugins_subdir}")
        if(NOT VCPKG_BUILD_TYPE)
            file(GLOB osg_plugins "${CURRENT_PACKAGES_DIR}/debug/plugins/${osg_plugins_subdir}/${osg_plugin_pattern}")
            file(INSTALL ${osg_plugins} DESTINATION "${CURRENT_PACKAGES_DIR}/tools/${PORT}/debug/${osg_plugins_subdir}")
        endif()
    endif()
    vcpkg_copy_tools(TOOL_NAMES osgearth_3pv osgearth_atlas osgearth_bakefeaturetiles osgearth_boundarygen
        osgearth_clamp osgearth_conv osgearth_imgui osgearth_tfs osgearth_toc osgearth_version osgearth_viewer
		osgearth_createtile osgearth_mvtindex
        AUTO_CLEAN
    )
	file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/tools/${PORT}/debug")
endif()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

# Handle copyright
file(INSTALL "${SOURCE_PATH}/LICENSE.txt" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
