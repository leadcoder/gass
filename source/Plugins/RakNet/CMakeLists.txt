set(RAKNET_DIR  $ENV{RAKNET_HOME} CACHE PATH "RakNet folder")
find_package(RakNet)
if(WIN32)
	set(RAKNET_LIBRARIES ${RAKNET_LIBRARIES} debug ws2_32 optimized ws2_32)
endif()
gass_create_dep_target(RakNet INCLUDE_DIRS ${RAKNET_INCLUDE_DIRS}  LIBRARIES ${RAKNET_LIBRARIES})

gass_setup_plugin(GASSPluginRakNet PRIVATE_DEPS RakNet)
