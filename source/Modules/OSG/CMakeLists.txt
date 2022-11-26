find_package(OSGExt REQUIRED osgUtil osgDB osgGA  osgFX osgVolume osgText osgShadow osgViewer osgSim osgTerrain osgManipulator)
gass_setup_module(GASSModuleOSG PUBLIC_DEPS ${OSG_ALL_TARGETS} ImGui)


target_sources(GASSModuleOSG PRIVATE 
	Shaders/Base.frag.glsl
	Shaders/Base.vert.glsl)


if(NOT MSVC)
	target_compile_options(GASSModuleOSG PRIVATE -Wno-cast-function-type
	-Wno-format-security)
endif()

	

