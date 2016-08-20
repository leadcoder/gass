#FindOgre.cmake use environment var OGRE_HOME
find_package(OGRE)
gass_create_dep_target(Ogre "${OGRE_INCLUDE_DIR}" "${OGRE_LIBRARIES}")

set(SKYX_DIR   $ENV{SKYX_HOME} CACHE PATH "SkyX folder")
find_package(SkyX)
gass_create_dep_target(SkyX "${SKYX_INCLUDE_DIRS}" "${SKYX_LIBRARIES}")

set(HYDRAX_DIR $ENV{HYDRAX_HOME} CACHE PATH "Hydrax folder")
find_package(Hydrax)
gass_create_dep_target(Hydrax "${HYDRAX_INCLUDE_DIRS}" "${HYDRAX_LIBRARIES}")

set(ENV_DEP_TARGETS Ogre SkyX Hydrax)
gass_setup_plugin(GASSPluginEnvironment "${ENV_DEP_TARGETS}")


