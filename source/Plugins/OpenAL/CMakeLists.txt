find_package(OpenALExt REQUIRED)
set(PLUGIN_DEPS openal::openal)

if(NOT WIN32)
		find_package(ALUT REQUIRED)
		list(APPEND PLUGIN_DEPS alut::alut)
endif()

gass_setup_plugin(GASSPluginOpenAL PRIVATE_DEPS ${PLUGIN_DEPS})