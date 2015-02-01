
#pragma once

class dtNavMesh;
class dtTileCache;
class dtNavMeshQuery;
class InputGeom;
class rcContext;


class TiledNavigationMeshBuilder
{
public:
	TiledNavigationMeshBuilder(rcContext* ctx);
	virtual ~TiledNavigationMeshBuilder();
	dtNavMesh* Build(InputGeom* geom);
private:
	unsigned char* BuildTileMesh(InputGeom* geom, const int tx, const int ty, const float* bmin, const float* bmax, int& dataSize);
private:
	rcContext* m_Ctx;
public:
	float m_CellSize;
	float m_CellHeight;
	float m_AgentHeight;
	float m_AgentRadius;
	float m_AgentMaxClimb;
	float m_AgentMaxSlope;
	float m_RegionMinSize;
	float m_RegionMergeSize;
	float m_EdgeMaxLen;
	float m_EdgeMaxError;
	float m_VertsPerPoly;
	float m_DetailSampleDist;
	float m_DetailSampleMaxError;
	int m_TileSize;
	bool m_MonotonePartitioning;
};


