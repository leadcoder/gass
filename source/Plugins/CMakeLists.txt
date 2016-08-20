#always build depdency free plugins 
ADD_SUBDIRECTORY(Base)
ADD_SUBDIRECTORY(RecastNavigation)
ADD_SUBDIRECTORY(Vehicle)
ADD_SUBDIRECTORY(Input)
ADD_SUBDIRECTORY(RoadNetwork)

#All plugins that have depdencies should be optional
if(GASS_BUILD_PLUGIN_OGRE)
	ADD_SUBDIRECTORY(Ogre)
	#no need to build ogre dependent plugins if we dont have the ogre rendersystem plugin active
	if(GASS_BUILD_PLUGIN_PAGED_GEOMETRY)
		ADD_SUBDIRECTORY(PagedGeometry)
	endif()
	if(GASS_BUILD_PLUGIN_ENVIRONMENT)
		ADD_SUBDIRECTORY(Environment)
	endif()
endif()

if(GASS_BUILD_PLUGIN_OSG)
	ADD_SUBDIRECTORY(OSG)
endif()
if(GASS_BUILD_PLUGIN_ODE)
	ADD_SUBDIRECTORY(ODE)
endif()

if(GASS_BUILD_PLUGIN_OIS)
	ADD_SUBDIRECTORY(OIS)
endif()

if(GASS_BUILD_PLUGIN_OPENAL)
	ADD_SUBDIRECTORY(OpenAL)
endif()

if(GASS_BUILD_PLUGIN_RAKNET)
	ADD_SUBDIRECTORY(RakNet)
	#ADD_SUBDIRECTORY(RakNet4)
endif()

if(GASS_BUILD_PLUGIN_PHYSX)
	ADD_SUBDIRECTORY(PhysX3)
endif()

if(GASS_BUILD_PLUGIN_MYGUI)
	ADD_SUBDIRECTORY(MyGUI)
endif()

if(GASS_BUILD_PLUGIN_OSGEARTH)
	ADD_SUBDIRECTORY(OSGEarth)
endif()

