gass_setup_plugin(GASSPluginRecastNavigation PRIVATE_DEPS)


if(NOT MSVC)
target_compile_options(GASSPluginRecastNavigation PRIVATE -Wno-ignored-qualifiers
-Wno-maybe-uninitialized
-Wno-unused-result
-Wno-implicit-fallthrough
-Wno-unused-but-set-variable)
endif()
