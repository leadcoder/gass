
MACRO(LIST_CONTAINS var value)
  SET(${var})
  FOREACH (value2 ${ARGN})
    IF (${value} STREQUAL ${value2})
      SET(${var} TRUE)
    ENDIF (${value} STREQUAL ${value2})
  ENDFOREACH (value2)
ENDMACRO(LIST_CONTAINS)

get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(${SELF_DIR}/GASSCoreTargets.cmake)


set(GASS_COMPONENT_LIST ${GASS_FIND_COMPONENTS})

#handle lowercase version ie find_package(gass ...) 
if(NOT GASS_COMPONENT_LIST AND gass_FIND_COMPONENTS)
	set(GASS_COMPONENT_LIST  ${gass_FIND_COMPONENTS})
endif()

LIST_CONTAINS(USE_SIM Sim ${GASS_COMPONENT_LIST})
if(USE_SIM)
	include(${SELF_DIR}/AngelscriptTargets.cmake)
	include(${SELF_DIR}/GASSSimTargets.cmake)
	LIST_CONTAINS(USE_EDITOR_MODULE EditorModule ${GASS_COMPONENT_LIST})
	if (USE_EDITOR_MODULE)
		include(${SELF_DIR}/GASSEditorModuleTargets.cmake)
	endif()
endif()
