find_package(OpenALExt)
gass_create_dep_target(OpenAL "${OPENAL_INCLUDE_DIR}" "${OPENAL_LIBRARY}")
gass_setup_plugin(GASSPluginOpenAL OpenAL)
