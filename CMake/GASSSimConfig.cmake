
include(CMakeFindDependencyMacro)
find_dependency(GASSCore)

set(GASSSIM_COMPONENT_LIST ${GASSSim_FIND_COMPONENTS})

#handle lowercase version ie find_package(gasssim ...) 
if(NOT GASSSIM_COMPONENT_LIST AND gasssim_FIND_COMPONENTS)
	set(GASSSIM_COMPONENT_LIST  ${gasssim_FIND_COMPONENTS})
endif()

get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

include(${SELF_DIR}/AngelscriptTargets.cmake)
include(${SELF_DIR}/GASSSimTargets.cmake)

if ("EditorModule" IN_LIST GASSSIM_COMPONENT_LIST)
	include(${SELF_DIR}/GASSEditorModuleTargets.cmake)
endif()