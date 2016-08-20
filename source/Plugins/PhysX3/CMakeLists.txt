#set lib name here!
set(LIB_NAME "GASSPluginPhysX3")

set(PHYSX3_INSTALL_DIR $ENV{PHYSX_HOME} CACHE PATH "PhysX folder")
find_package(PhysX3 REQUIRED) 

gass_create_dep_target(PhysX "${PHYSX3_INCLUDE_DIR}" "${PHYSX3_LIBRARIES}")
gass_setup_plugin(GASSPluginPhysX PhysX)

