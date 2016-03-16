CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(recast_utils_SRCS
	Source/ChunkyTriMesh.cpp
	Source/InputGeom.cpp
	Source/MeshLoaderObj.cpp
)

SET(recast_utils_HDRS
	Include/ChunkyTriMesh.h
	Include/InputGeom.h
	Include/MeshLoaderObj.h
)

INCLUDE_DIRECTORIES(Include 
	../Detour/Include
	../DetourTileCache
	../Recast/Include)

ADD_LIBRARY(RecastUtils ${recast_utils_SRCS} ${recast_utils_HDRS})
