find_package(OSGExt REQUIRED osgUtil osgDB osgGA  osgFX osgVolume osgText osgShadow osgViewer osgSim osgTerrain osgManipulator)
gass_setup_plugin(GASSPluginOSG PRIVATE_DEPS ${OSG_ALL_TARGETS} ImGui)

