#always build depdency free plugins 
ADD_SUBDIRECTORY( Base)
ADD_SUBDIRECTORY( RecastNavigation)
ADD_SUBDIRECTORY( Game)
ADD_SUBDIRECTORY( RoadNetwork)

#All plugins that have depdencies should be optional
if(GASSPLUGINS_BUILD_OGRE)
	ADD_SUBDIRECTORY( Ogre)
	#no need to build ogre dependent plugins if we dont have the ogre rendersystem plugin active
	if(GASSPLUGINS_BUILD_PAGED_GEOMETRY)
		ADD_SUBDIRECTORY( PagedGeometry)
	endif()
	if(GASSPLUGINS_BUILD_ENVIRONMENT)
		ADD_SUBDIRECTORY(Environment)
	endif()
endif()
if(GASSPLUGINS_BUILD_OSG)
	ADD_SUBDIRECTORY( OSG)
endif()
if(GASSPLUGINS_BUILD_ODE)
	ADD_SUBDIRECTORY(ODE)
endif()

if(GASSPLUGINS_BUILD_OIS)
	ADD_SUBDIRECTORY( OIS)
endif()

if(GASSPLUGINS_BUILD_OPENAL)
	ADD_SUBDIRECTORY( OpenAL)
endif()

if(GASSPLUGINS_BUILD_RAKNET)
	ADD_SUBDIRECTORY( RakNet)
endif()

if(GASSPLUGINS_BUILD_PHYSX)
	ADD_SUBDIRECTORY( PhysX3)
endif()

if(GASSPLUGINS_BUILD_MYGUI)
	ADD_SUBDIRECTORY( MyGUI)
endif()
