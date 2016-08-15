#set lib name here!
set(LIB_NAME "GASSPluginODE")

set (ODE_HOME $ENV{ODE_HOME})
find_package(ODE)

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Add some folders where to search for dependency libraries
add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})
target_link_libraries(${LIB_NAME} GASSSim ${ODE_LIBRARIES})

target_include_directories(${LIB_NAME}
	PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	PUBLIC  $<BUILD_INTERFACE:${ODE_INCLUDE_DIRS}>
)

target_compile_definitions(${LIB_NAME} PRIVATE ${GASS_COMMON_DEFINITIONS} dDOUBLE)

gass_install_plugin_target()

