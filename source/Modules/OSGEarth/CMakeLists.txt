find_package(osgEarth CONFIGS osgEarth-config.cmake CONFIG REQUIRED)
if(UNIX)
   	find_package(Threads REQUIRED) #osgEarth config  depend on Threads::Threads target
endif()
find_package(Fontconfig REQUIRED)
gass_setup_module(GASSModuleOSGEarth PRIVATE_DEPS GASSModuleOSG osgEarth::osgEarth)