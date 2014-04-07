#always build depdency free plugins 
ADD_SUBDIRECTORY( Base)
ADD_SUBDIRECTORY( RecastNavigation)
ADD_SUBDIRECTORY( Game)
ADD_SUBDIRECTORY( RoadNetwork)

#All plugins that have depdencies should be optional
if(GASSPLUGIN_BUILD_OGRE)
	ADD_SUBDIRECTORY( Ogre)
	#no need to build ogre dependent plugins if we dont have the ogre rendersystem plugin active
	if(GASSPLUGIN_BUILD_PAGED_GEOMETRY)
		ADD_SUBDIRECTORY( PagedGeometry)
	endif()
	if(GASSPLUGIN_BUILD_ENVIRONMENT)
		ADD_SUBDIRECTORY(Environment)
	endif()
endif()
if(GASSPLUGIN_BUILD_OSG)
	ADD_SUBDIRECTORY( OSG)
endif()
if(GASSPLUGIN_BUILD_ODE)
	ADD_SUBDIRECTORY(ODE)
endif()

if(GASSPLUGIN_BUILD_OIS)
	ADD_SUBDIRECTORY( OIS)
endif()

if(GASSPLUGIN_BUILD_OPENAL)
	ADD_SUBDIRECTORY( OpenAL)
endif()

if(GASSPLUGIN_BUILD_RAKNET)
	ADD_SUBDIRECTORY( RakNet)
endif()

if(GASSPLUGIN_BUILD_PHYSX)
	ADD_SUBDIRECTORY( PhysX3)
endif()

if(GASSPLUGIN_BUILD_MYGUI)
	ADD_SUBDIRECTORY( MyGUI)
endif()
