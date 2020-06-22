find_package(OSGExt REQUIRED osgUtil 
							osgDB 
							osgGA  
							osgText
							osgViewer 
							osgSim 
							osgTerrain 
							osgManipulator)
find_package(OSGEarthExt)
		
gass_setup_plugin(GASSPluginOSGEarth PRIVATE_DEPS ${OSG_ALL_TARGETS} 
													osgEarth::osgEarth 
													osgEarth::osgEarthFeatures 
													osgEarth::osgEarthUtil
													osgEarth::osgEarthSymbology
													osgEarth::osgEarthAnnotation)