# create_source_group(relativeSourcePath sourceGroupName files)
    #
    # Creates a source group with the specified name relative to the relative path
    # specified.
    #
    # Parameters:
    #    - sourceGroupName: Name of the source group to create.
    #    - relativeSourcePath: Relative path to the files.
    #    - sourceFiles: Files to add to the source group.
    #
    # For example if you have the following directory structure:
    #
    #    - ExampleApplication
    #        - include
    #            - Main.h
    #                - Window
    #                    Window.h
    #        - source
    #            - Main.cpp
    #                - Window
    #                    Window.cpp
    #
    # You can get your list of files and call create_source_group the following way
    #
    #    file(GLOB_RECURSE my_source_files ${CMAKE_CURRENT_SOURCE_DIR}/source/*)
    #    create_source_group("Source Files"  "${CMAKE_CURRENT_SOURCE_DIR}/source" ${my_source_files})
    #    file(GLOB_RECURSE my_header_files ${CMAKE_CURRENT_SOURCE_DIR}/include/*)
    #    create_source_group("Header Files" "${CMAKE_CURRENT_SOURCE_DIR}/include" ${my_header_files})
    #    add_executable(ExampleApplication ${my_source_files} ${my_header_files})
    #
    # Then the generated solution would look like this
    #
    #    - ExampleApplication (project)
    #        - Header Files
    #            - Main.h
    #                - Window
    #                    Window.h
    #        - Source Files
    #            - Main.cpp
    #                - Window
    #                    Window.cpp
    #
function(create_source_group sourceGroupName relativeSourcePath sourceFiles)
		#dont know why but foreach skip first element if we use ARGN
		set(my_files ${ARGN})
		
		FOREACH(currentSourceFile ${my_files})
		   
			FILE(RELATIVE_PATH folder ${relativeSourcePath} ${currentSourceFile})
            get_filename_component(filename ${folder} NAME)
			
            string(REPLACE ${filename} "" folder ${folder})
			
            if(NOT folder STREQUAL "")
                string(REGEX REPLACE "/+$" "" folderlast ${folder})
                string(REPLACE "/" "\\" folderlast ${folderlast})
                SOURCE_GROUP("${sourceGroupName}\\${folderlast}" FILES ${currentSourceFile})
            endif(NOT folder STREQUAL "")
        ENDFOREACH(currentSourceFile ${ARGN})
endfunction(create_source_group)

macro(add_source_from_current_dir)
	#Grab cpp and h  files from path recursively
	file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
	file(GLOB_RECURSE H_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

	#add dummy element 
	set(TEMP_CPP_FILES "dummy" ${CPP_FILES})
	set(TEMP_H_FILES "dummy" ${H_FILES})
	
	#Create ide source groups that repilicates folder structure 
	create_source_group("Source Files"  "${CMAKE_CURRENT_SOURCE_DIR}" ${TEMP_CPP_FILES})
	create_source_group("Header Files"  "${CMAKE_CURRENT_SOURCE_DIR}" ${TEMP_H_FILES})	
	
endmacro(add_source_from_current_dir)


macro(gass_install_target)
	install(TARGETS ${LIB_NAME}
  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug
  LIBRARY DESTINATION ${GASS_INSTALL_LIB_DIR_DEBUG} CONFIGURATIONS Debug
  ARCHIVE DESTINATION ${GASS_INSTALL_LIB_DIR_DEBUG} CONFIGURATIONS Debug
  )

install(TARGETS ${LIB_NAME}
  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release
  LIBRARY DESTINATION ${GASS_INSTALL_LIB_DIR_RELEASE} CONFIGURATIONS Release
  ARCHIVE DESTINATION ${GASS_INSTALL_LIB_DIR_RELEASE} CONFIGURATIONS Release)	
	
endmacro(gass_install_target) 

macro(gass_install_plugin_target)
	install(TARGETS ${LIB_NAME}
  RUNTIME DESTINATION ${GASS_PLUGIN_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug
  LIBRARY DESTINATION ${GASS_PLUGIN_INSTALL_LIB_DIR_DEBUG} CONFIGURATIONS Debug
  ARCHIVE DESTINATION ${GASS_PLUGIN_INSTALL_LIB_DIR_DEBUG} CONFIGURATIONS Debug
  )

install(TARGETS ${LIB_NAME}
  RUNTIME DESTINATION ${GASS_PLUGIN_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release
  LIBRARY DESTINATION ${GASS_PLUGIN_INSTALL_LIB_DIR_RELEASE} CONFIGURATIONS Release
  ARCHIVE DESTINATION ${GASS_PLUGIN_INSTALL_LIB_DIR_RELEASE} CONFIGURATIONS Release
  )	
	
endmacro(gass_install_plugin_target) 


macro(FILTER_LIST INPUT OUTPUT GOOD BAD)
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
endmacro(FILTER_LIST)


MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE new_list ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

	
macro(gass_setup_plugin PLUGIN_NAME)
	set (extra_macro_args ${ARGN})
	# Did we get any optional args?
    list(LENGTH extra_macro_args num_extra_args)
    if (${num_extra_args} GREATER 0)
        list(GET extra_macro_args 0 optional_arg)
        message ("Got an optional arg: ${optional_arg}")
		set(_DEPS ${ARGN})
		message(${_DEPS})
    endif ()

	
	add_source_from_current_dir()
	add_library (${PLUGIN_NAME} ${GASS_BUILDTYPE}  ${CPP_FILES} ${H_FILES})
	HEADER_DIRECTORIES(SUBDIRS)
	foreach(INC_DIR ${SUBDIRS})
		target_include_directories(${PLUGIN_NAME} PRIVATE  $<BUILD_INTERFACE:${INC_DIR}>)
	endforeach()
	target_link_libraries(${PLUGIN_NAME} GASSSim ${_DEPS})
	target_compile_definitions(${PLUGIN_NAME} PRIVATE ${GASS_COMMON_DEFINITIONS} GASS_PLUGIN_EXPORTS)
	install(TARGETS ${PLUGIN_NAME}
		RUNTIME DESTINATION ${GASS_PLUGIN_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug	
		LIBRARY DESTINATION ${GASS_PLUGIN_INSTALL_LIB_DIR_DEBUG} CONFIGURATIONS Debug
		ARCHIVE DESTINATION ${GASS_PLUGIN_INSTALL_LIB_DIR_DEBUG} CONFIGURATIONS Debug)

	install(TARGETS ${PLUGIN_NAME}
	  RUNTIME DESTINATION ${GASS_PLUGIN_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release
	  LIBRARY DESTINATION ${GASS_PLUGIN_INSTALL_LIB_DIR_RELEASE} CONFIGURATIONS Release
	  ARCHIVE DESTINATION ${GASS_PLUGIN_INSTALL_LIB_DIR_RELEASE} CONFIGURATIONS Release)
endmacro()

macro(gass_create_dep_target DEP_NAME DEP_INCLUDE_DIRS DEP_LIBRARIES)
	set(_DEP_INCLUDE_DIRS ${DEP_INCLUDE_DIRS})
	set(_DEP_LIBRARIES ${DEP_LIBRARIES})
	add_library(${DEP_NAME} INTERFACE IMPORTED)
	#add_library(foo UNKNOWN IMPORTED)
	set_property(TARGET ${DEP_NAME} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${_DEP_INCLUDE_DIRS})
	#set_property(TARGET ODE SHARED IMPORTED) PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${ODE_INCLUDE_DIRS})
	#set_property(TARGET ODE PROPERTY IMPORTED_LOCATION ${ODE_LIBRARIES})
	FILTER_LIST("${_DEP_LIBRARIES}" _RELEASE_LIBS optimized debug)
	FILTER_LIST("${_DEP_LIBRARIES}" _DEBUG_LIBS debug optimized)
	message( ${_RELEASE_LIBS} )
	set_property(TARGET ${DEP_NAME} PROPERTY INTERFACE_LINK_LIBRARIES $<$<CONFIG:Debug>:${_DEBUG_LIBS}> $<$<NOT:$<CONFIG:Debug>>:${_RELEASE_LIBS}>)
endmacro()



function(mkcmakeconfig packagename packageversion)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${packagename}ConfigVersion.cmake"
  VERSION ${packageversion}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT LibraryTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${packagename}Targets.cmake"
  NAMESPACE ${packagename}::
)
configure_file(cmake/${packagename}Config.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/${packagename}Config.cmake"
  COPYONLY
)

if(WIN32)
  set(ConfigPackageLocation cmake)
else()
  set(ConfigPackageLocation lib/cmake/${packagename})
endif()
install(EXPORT LibraryTargets
  FILE
    ${packagename}Targets.cmake
  NAMESPACE
    ${packagename}::
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    cmake/${packagename}Config.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/${packagename}ConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)
endfunction()

