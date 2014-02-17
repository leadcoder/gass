

#pragma once


class dtNavMesh;

class TiledNavigationMeshSerializer 
{
public:
	TiledNavigationMeshSerializer();
	virtual ~TiledNavigationMeshSerializer();
	dtNavMesh* Load(const char* path);
	void Save(const char* path, const dtNavMesh* mesh);
private:
};


