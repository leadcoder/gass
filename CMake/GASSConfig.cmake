
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

LIST_CONTAINS(USE_SIM Sim ${GASS_FIND_COMPONENTS})

if(USE_SIM)
	include(${SELF_DIR}/AngelscriptTargets.cmake)
	include(${SELF_DIR}/GASSSimTargets.cmake)
	LIST_CONTAINS(USE_EDITOR_MODULE EditorModule ${GASS_FIND_COMPONENTS})
	if (USE_EDITOR_MODULE)
		include(${SELF_DIR}/GASSEditorModuleTargets.cmake)
	endif()
endif()