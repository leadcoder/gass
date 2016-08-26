#set lib name here!
set(LIB_NAME "GASSPluginRoadNetwork")

#add all source files from current directory (recursively)
add_source_from_current_dir()

add_library (${LIB_NAME} ${GASS_BUILDTYPE}  ${CPP_FILES} ${H_FILES})

target_include_directories(${LIB_NAME}
	PRIVATE  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	PRIVATE  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/RoadLib>)

target_link_libraries(${LIB_NAME} GASSSim)
target_compile_definitions(${LIB_NAME} PRIVATE ${GASS_COMMON_DEFINITIONS} )

gass_install_plugin_target()	

#TODO: copy interface files in cmake generate 
install(FILES  ${CMAKE_CURRENT_SOURCE_DIR}/../AI/AIMessages.h DESTINATION ${GASS_INSTALL_INCLUDE_DIR}/plugins/AI)

