set (ODE_HOME $ENV{ODE_HOME})
find_package(ODE)
set(GASS_COMMON_DEFINITIONS ${GASS_COMMON_DEFINITIONS} dDOUBLE)
gass_create_dep_target(ODE "${ODE_INCLUDE_DIRS}" "${ODE_LIBRARIES}")
gass_setup_plugin(GASSPluginODE ODE)


