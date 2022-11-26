find_package(OSGExt REQUIRED osgUtil osgDB osgGA  osgFX osgVolume osgText osgShadow osgViewer osgSim osgTerrain osgManipulator)
gass_setup_plugin(GASSPluginOSG PRIVATE_DEPS ${OSG_ALL_TARGETS} ImGui)


target_sources(GASSPluginOSG PRIVATE 
	Shaders/Base.frag.glsl
	Shaders/Base.vert.glsl)


if(NOT MSVC)
	target_compile_options(GASSPluginOSG PRIVATE -Wno-cast-function-type
	-Wno-format-security)
endif()

	

