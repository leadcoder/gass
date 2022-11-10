
function(gass_get_dep_binary_dirs RELASE_DIRS DEBUG_DIRS )

	if(EXISTS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
		set(_VCPKG_DIR ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET})
		set(_BIN_DIR_REL 
						 "${_VCPKG_DIR}/bin"
						 "${_VCPKG_DIR}/plugins")
		set(_BIN_DIR_DBG 
						 "${_VCPKG_DIR}/debug/bin"
						 "${_VCPKG_DIR}/debug/plugins")
		set(${RELASE_DIRS} ${_BIN_DIR_REL} PARENT_SCOPE)
		set(${DEBUG_DIRS} ${_BIN_DIR_DBG} PARENT_SCOPE)
	endif()
endfunction()

function(gass_find_files BINARY_NAMES SEARCH_DIRS VAR_PREFIX FOUND_BINARIES)
	foreach(_FILE_NAME ${${BINARY_NAMES}})
		 string(TOUPPER ${_FILE_NAME} _FILE_NAME_UPPER)
		 set(VAR_NAME ${VAR_PREFIX}_${_FILE_NAME_UPPER})
		 find_file(${VAR_NAME} NAMES ${_FILE_NAME} HINTS ${${SEARCH_DIRS}})
		 if(${VAR_NAME})
			list(APPEND _FOUND_BINARIES ${${VAR_NAME}})
		 endif()
	endforeach()
	set(${FOUND_BINARIES} ${_FOUND_BINARIES} PARENT_SCOPE)
endfunction()

function(gass_find_and_install_dep_binaries RELASE_BINS DEBUG_BINS)

	gass_get_dep_binary_dirs(RELASE_DIRS DEBUG_DIRS)
	gass_find_files(${RELASE_BINS} RELASE_DIRS "GASSDEP_REL" FOUND_REL_BINS)
	gass_find_files(${DEBUG_BINS} DEBUG_DIRS "GASSDEP_DBG" FOUND_DBG_BINS)
	if(FOUND_REL_BINS)
		install(FILES ${FOUND_REL_BINS} DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
		file(COPY ${FOUND_REL_BINS}  DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(FOUND_DBG_BINS)
		install(FILES ${FOUND_DBG_BINS} DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)
		file(COPY ${FOUND_DBG_BINS}  DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
endfunction()

macro(gass_filename_only in_list out_list)
	foreach(_FILENAME ${${in_list}})
		get_filename_component(_BARENAME ${_FILENAME} NAME)
		set(${out_list} ${${out_list}} ${_BARENAME})
	endforeach()
endmacro()

macro(gass_get_source_from_current_dir SOURCE_FILES HEADER_FILES)
	#Grab cpp and h  files from path recursively
	file(GLOB_RECURSE ${SOURCE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
	file(GLOB_RECURSE ${HEADER_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
endmacro()

macro(gass_filter_list INPUT OUTPUT GOOD BAD)
  set(LST ${INPUT})   # can we avoid this?
  set(PICKME YES)
  foreach(ELEMENT IN LISTS LST)
    if(${ELEMENT} STREQUAL general OR ${ELEMENT} STREQUAL ${GOOD})
      set(PICKME YES)
    elseif(${ELEMENT} STREQUAL ${BAD})
      set(PICKME NO)
    elseif(PICKME)
      list(APPEND ${OUTPUT} ${ELEMENT})
    endif()
  endforeach()
endmacro()

macro(gass_get_header_directories _RETURN_DIRS)
    file(GLOB_RECURSE NEW_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
    set(DIR_LIST "")
    foreach(C_FILE_PATH ${NEW_LIST})
        get_filename_component(DIR_PATH ${C_FILE_PATH} PATH)
        set(DIR_LIST ${DIR_LIST} ${DIR_PATH})
    endforeach()
    list(REMOVE_DUPLICATES DIR_LIST)
    set(${_RETURN_DIRS} ${DIR_LIST})
endmacro()

macro(gass_setup_plugin _PLUGIN_NAME)
	gass_get_source_from_current_dir(SOURCE_FILES HEADER_FILES)
	gass_setup_lib(${_PLUGIN_NAME} 
					${ARGN} 
					BUILDTYPE ${GASS_BUILDTYPE}
					SOURCE_FILES ${SOURCE_FILES} 
					HEADER_FILES ${HEADER_FILES} 
					SKIP_HEADER_INSTALL )
	gass_get_header_directories(HEADER_SUBDIRS)
	foreach(INC_DIR ${HEADER_SUBDIRS})
		target_include_directories(${_PLUGIN_NAME} PRIVATE  $<BUILD_INTERFACE:${INC_DIR}>)
	endforeach()
	
	target_link_libraries(${_PLUGIN_NAME} PRIVATE GASSSim)
	
	target_compile_definitions(${_PLUGIN_NAME} PRIVATE GASS_PLUGIN_EXPORTS)
	#set_target_properties(${_PLUGIN_NAME} PROPERTIES SUFFIX .gassp)
	set_target_properties(${_PLUGIN_NAME} PROPERTIES PREFIX "")
	set_target_properties(${_PLUGIN_NAME} PROPERTIES FOLDER "Plugins")
endmacro()

macro(gass_setup_module _MODULE_NAME)
	gass_get_source_from_current_dir(SOURCE_FILES HEADER_FILES)
	gass_setup_lib(${_MODULE_NAME} 
					${ARGN} 
					BUILDTYPE ${GASS_BUILDTYPE}
					SOURCE_FILES ${SOURCE_FILES} 
					HEADER_FILES ${HEADER_FILES} 
					SKIP_HEADER_INSTALL )
	target_link_libraries(${_MODULE_NAME} PRIVATE GASSSim)
	set_target_properties(${_MODULE_NAME} PROPERTIES FOLDER "Modules")
	set_target_properties(${_MODULE_NAME} PROPERTIES PREFIX "")
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${GASS_INSTALL_INCLUDE_DIR}/modules FILES_MATCHING PATTERN "*.h")
endmacro()

include(CMakeParseArguments)

macro(gass_setup_lib _LIB_NAME)
	cmake_parse_arguments(
        PARSED_ARGS # prefix of output variables
        "SKIP_HEADER_INSTALL" # list of names of the boolean arguments (only defined ones will be true)
        "BUILDTYPE" # list of names of mono-valued arguments
        "HEADER_FILES;SOURCE_FILES;PUBLIC_INCLUDE_DIRS;PRIVATE_INCLUDE_DIRS;PUBLIC_DEPS;PRIVATE_DEPS;PUBLIC_DEFINITIONS;PRIVATE_DEFINITIONS" # list of names of multi-valued arguments (output variables are lists)
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )
	
	set(ALL_FILES ${PARSED_ARGS_SOURCE_FILES} ${PARSED_ARGS_HEADER_FILES})
	
	add_library (${_LIB_NAME} ${PARSED_ARGS_BUILDTYPE} ${PARSED_ARGS_SOURCE_FILES} ${PARSED_ARGS_HEADER_FILES})
	foreach(INC_DIR ${PARSED_ARGS_PUBLIC_INCLUDE_DIRS})
		target_include_directories(${_LIB_NAME} PUBLIC $<BUILD_INTERFACE:${INC_DIR}>)
		
	endforeach()
	
	target_include_directories(${_LIB_NAME} PUBLIC $<INSTALL_INTERFACE:${GASS_INSTALL_INCLUDE_DIR}>)
	
	foreach(INC_DIR ${PARSED_ARGS_PRIVATE_INCLUDE_DIRS})
		target_include_directories(${_LIB_NAME} PRIVATE $<BUILD_INTERFACE:${INC_DIR}>)
	endforeach()

	foreach(CUR_DEP ${PARSED_ARGS_PUBLIC_DEPS})
		target_link_libraries(${_LIB_NAME} PUBLIC ${CUR_DEP})
	endforeach()
	
	foreach(CUR_DEP ${PARSED_ARGS_PRIVATE_DEPS})
		target_link_libraries(${_LIB_NAME} PRIVATE ${CUR_DEP})
	endforeach()

	target_compile_definitions(${_LIB_NAME} PUBLIC ${PARSED_ARGS_PUBLIC_DEFINITIONS})
	target_compile_definitions(${_LIB_NAME} PRIVATE ${PARSED_ARGS_PRIVATE_DEFINITIONS})
	set_target_properties(${_LIB_NAME} PROPERTIES DEBUG_POSTFIX _d)
	
	if(NOT PARSED_ARGS_SKIP_HEADER_INSTALL)
		#INSTALL(FILES ${PARSED_ARGS_HEADER_FILES} DESTINATION include)
		install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${GASS_INSTALL_INCLUDE_DIR} FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
	endif()
	
	install(TARGETS ${_LIB_NAME} EXPORT ${_LIB_NAME}Targets
		RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug	
		LIBRARY DESTINATION ${GASS_INSTALL_LIB_DIR_DEBUG} CONFIGURATIONS Debug
		ARCHIVE DESTINATION ${GASS_INSTALL_LIB_DIR_DEBUG} CONFIGURATIONS Debug
		RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release
		LIBRARY DESTINATION ${GASS_INSTALL_LIB_DIR_RELEASE} CONFIGURATIONS Release
		ARCHIVE DESTINATION ${GASS_INSTALL_LIB_DIR_RELEASE} CONFIGURATIONS Release)
  
	  
	 install(EXPORT ${_LIB_NAME}Targets
        FILE ${_LIB_NAME}Targets.cmake
        NAMESPACE GASS::
        DESTINATION CMake)

endmacro()

macro(gass_create_dep_target DEP_NAME)
	cmake_parse_arguments(
        PARSED_ARGS # prefix of output variables
        "" # list of names of the boolean arguments (only defined ones will be true)
        "" # list of names of mono-valued arguments
        "LIBRARIES;INCLUDE_DIRS;DEFINITIONS;BINARIES_REL;BINARIES_DBG" # list of names of multi-valued arguments (output variables are lists)
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )
	add_library(${DEP_NAME} INTERFACE IMPORTED)
	set_property(TARGET ${DEP_NAME} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${PARSED_ARGS_INCLUDE_DIRS})
	set(_RELEASE_LIBS "")
	set(_DEBUG_LIBS "")
	gass_filter_list("${PARSED_ARGS_LIBRARIES}" _RELEASE_LIBS optimized debug)
	gass_filter_list("${PARSED_ARGS_LIBRARIES}" _DEBUG_LIBS debug optimized)
	set_property(TARGET ${DEP_NAME} PROPERTY INTERFACE_LINK_LIBRARIES $<$<CONFIG:Debug>:${_DEBUG_LIBS}> $<$<NOT:$<CONFIG:Debug>>:${_RELEASE_LIBS}>)
	set_property(TARGET ${DEP_NAME} PROPERTY INTERFACE_COMPILE_DEFINITIONS ${PARSED_ARGS_DEFINITIONS})
	#message("${DEP_NAME} INTERFACE_COMPILE_DEFINITIONS ${PARSED_ARGS_DEFINITIONS}")
	
	#store all binary filenames in list, used when configuring consumer find scripts
	set(_BINARY_LIST_NAME_REL GASS_${DEP_NAME}_BINARIES_REL)
	gass_filename_only(PARSED_ARGS_BINARIES_REL ${_BINARY_LIST_NAME_REL})
	set(_BINARY_LIST_NAME_DBG GASS_${DEP_NAME}_BINARIES_DBG)
	gass_filename_only(PARSED_ARGS_BINARIES_DBG ${_BINARY_LIST_NAME_DBG})
	
	if(GASS_INSTALL_DEP_BINARIES)
		if(PARSED_ARGS_BINARIES_REL)
			install(FILES ${PARSED_ARGS_BINARIES_REL} DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
			if(WIN32)
				file(COPY ${PARSED_ARGS_BINARIES_REL}  DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release) 
			endif()
		endif()
		if(PARSED_ARGS_BINARIES_DBG)
			install(FILES ${PARSED_ARGS_BINARIES_DBG} DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)
			if(WIN32)
				file(COPY ${PARSED_ARGS_BINARIES_DBG}  DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug) 
			endif()
		endif()
	endif()
endmacro()

macro(gass_setup_sim_sample SAMPLE_NAME)
	cmake_parse_arguments(
        PARSED_ARGS # prefix of output variables
        "" # list of names of the boolean arguments (only defined ones will be true)
        "" # list of names of mono-valued arguments
        "DEPS" # list of names of multi-valued arguments (output variables are lists)
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )
	
	
	gass_get_source_from_current_dir(CPP_FILES H_FILES)
	add_executable (${SAMPLE_NAME} ${CPP_FILES} ${H_FILES})
	
	if(NOT WIN32 AND NOT APPLE)
		#Fix dlopen linkage issue on Ubuntu:
		#https://stackoverflow.com/questions/19926466/undefined-reference-to-dlopen-since-ubuntu-upgrade
		#set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed" )
		#SET_TARGET_PROPERTIES(${SAMPLE_NAME} PROPERTIES LINK_FLAGS "-Wl,--no-as-needed")
    endif()
	
	gass_get_header_directories(HEADER_SUBDIRS)
	
	foreach(INC_DIR ${HEADER_SUBDIRS})
		target_include_directories(${SAMPLE_NAME} PRIVATE  $<BUILD_INTERFACE:${INC_DIR}>)
	endforeach()
	
	target_link_libraries(${SAMPLE_NAME} GASSSim)
	
	foreach(CUR_DEP ${PARSED_ARGS_DEPS})
		target_link_libraries(${SAMPLE_NAME} ${CUR_DEP})
		#message("${_LIB_NAME} PUBLIC ${CUR_DEP}")
	endforeach()
	
	
	
	set_target_properties(${SAMPLE_NAME} PROPERTIES DEBUG_POSTFIX _d)
	set_target_properties(${SAMPLE_NAME} PROPERTIES FOLDER "SimSamples")
	
	
	set(SAMPLE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/${SAMPLE_NAME}.xml)

	if(MSVC)
		set_target_properties(${SAMPLE_NAME} PROPERTIES 
			VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${SAMPLE_NAME}>")
		
		gass_get_dep_binary_dirs(BIN_RELASE_DIRS BIN_DEBUG_DIRS)
		set(_BIN_DIRS $<IF:$<CONFIG:Debug>,${BIN_DEBUG_DIRS},${BIN_RELASE_DIRS}>)
		set(_BIN_DIRS "${_BIN_DIRS}")
		set_target_properties(${SAMPLE_NAME} PROPERTIES 
			VS_DEBUGGER_ENVIRONMENT "PATH=${_BIN_DIRS}\nGASS_DATA_HOME=${PROJECT_SOURCE_DIR}/samples/common/data")
			#;%PATH%")
	endif()
	
	#copy configurations to enable execution from build folder
	if(EXISTS ${SAMPLE_CONFIG})
		if(MSVC)
			file(COPY ${SAMPLE_CONFIG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
			file(COPY ${SAMPLE_CONFIG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo)
			file(COPY ${SAMPLE_CONFIG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
		else()
			file(COPY ${SAMPLE_CONFIG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
		endif()
	endif()

	#install executable
	install(TARGETS ${SAMPLE_NAME}  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
	install(TARGETS ${SAMPLE_NAME}  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)

	#install configuration files
	if(EXISTS ${SAMPLE_CONFIG})
		install(FILES ${SAMPLE_CONFIG} DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
		install(FILES ${SAMPLE_CONFIG} DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)
	endif()
endmacro()

macro(gass_setup_core_sample SAMPLE_NAME)

	if(NOT WIN32 AND NOT APPLE) 
		#Fix dlopen linkage issue on Ubuntu:
		#https://stackoverflow.com/questions/19926466/undefined-reference-to-dlopen-since-ubuntu-upgrade
		set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed" )
	endif()
	gass_get_source_from_current_dir(CPP_FILES H_FILES)
	add_executable (${SAMPLE_NAME} ${CPP_FILES} ${H_FILES})
	target_link_libraries(${SAMPLE_NAME} GASSCore)
	set_target_properties(${SAMPLE_NAME} PROPERTIES DEBUG_POSTFIX _d)
	set_target_properties(${SAMPLE_NAME} PROPERTIES FOLDER "CoreSamples")
	
	#install executable
	install(TARGETS ${SAMPLE_NAME}  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
	install(TARGETS ${SAMPLE_NAME}  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)

	#install configuration files
	install(FILES ${SAMPLE_CONFIG} DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
	install(FILES ${SAMPLE_CONFIG} DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)
endmacro()
