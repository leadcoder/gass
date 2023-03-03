set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)
#set(VCPKG_PLATFORM_TOOLSET v142) 
set(VCPKG_DISABLE_COMPILER_TRACKING TRUE)

if(PORT MATCHES "osg")
	set(osg_OPENGL_PROFILE "GL2")
	
	#Force that fontconfig is disabled in osg, 
	#fontconfig is a default-feature that can be disabled by either
	#install osg like this: vcpkg install osg[core,openexr,nvtt,plugins,freetype]
	#or modify/override osg port
	#The first option require that all dependent port need to update there manifests
	#The second option introduce problem with upstream sync (in relation to this small fix)
	#Instead we override OSG_TEXT_USE_FONTCONFIG here, ie it will always be "off" even if fontconfig
	#feature in osg port is requested (wich is default-feature)	
	set(VCPKG_CMAKE_CONFIGURE_OPTIONS "-DOSG_TEXT_USE_FONTCONFIG=OFF")
endif()

