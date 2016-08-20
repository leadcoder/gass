#find_package(OpenSceneGraph 3.2.1 REQUIRED osgUtil osgDB osgGA osgText osgShadow osgViewer osgSim osgTerrain)
find_package(OSGExt 3.2.1 REQUIRED osgUtil osgDB osgGA osgText osgShadow osgViewer osgSim osgTerrain)
gass_create_dep_target(OSG "${OPENSCENEGRAPH_INCLUDE_DIRS}" "${OPENSCENEGRAPH_LIBRARIES}")

gass_setup_plugin(GASSPluginOSG OSG)