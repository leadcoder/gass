#Set OIS directory
set (OIS_DIR $ENV{OIS_HOME})
find_package(OIS)
		
gass_create_dep_target(OIS "${OIS_INCLUDE_DIRS}" "${OIS_LIBRARIES}")
gass_setup_plugin(GASSPluginOIS OIS)

