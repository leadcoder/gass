#set lib name here!
set(LIB_NAME "GASSPluginRaknet4")

find_package(RakNet4)
		
include_directories(${GASS_SIM_INCLUDE_DIRS} ${RAKNET4_INCLUDE_DIRS})

#add all source files from current directory (recursively)
add_source_from_current_dir()

#Enable class export 
add_definitions( -DGASS_PLUGIN_EXPORTS )

link_directories(${GASS_SIM_LIB_DIRS})

add_library (${LIB_NAME} SHARED  ${CPP_FILES} ${H_FILES})

#Add project dependency libs
set(SOCKET_LIB "")
if(WIN32)
	set(SOCKET_LIB ws2_32)
endif()

target_link_libraries(${LIB_NAME} GASSSim 
			GASSCore 
			${GASS_SIM_LIBRARIES}
			${RAKNET4_LIBRARIES}
			${SOCKET_LIB}
			)

gass_install_plugin_target()

