

#include "TiledNavigationMeshBuilder.h"
#include "Recast.h"
#include "RecastAlloc.h"
#include "RecastAssert.h"
#include "DetourAssert.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "DetourCommon.h"
#include "DetourTileCache.h"
#include "InputGeom.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


TiledNavigationMeshBuilder::TiledNavigationMeshBuilder(rcContext* ctx) : m_Ctx(ctx),
	m_CellSize (0.3f),
	m_CellHeight (0.2f),
	m_AgentHeight ( 2.0f),
	m_AgentRadius ( 0.6f),
	m_AgentMaxClimb ( 2.2f),
	m_AgentMaxSlope ( 45.0f),
	m_RegionMinSize ( 50),
	m_RegionMergeSize ( 20),
	m_EdgeMaxLen ( 12.0f),
	m_EdgeMaxError ( 1.3f),
	m_VertsPerPoly ( 6.0f),
	m_DetailSampleDist ( 6.0f),
	m_DetailSampleMaxError ( 1.0f),
	m_TileSize ( 64),
	m_MonotonePartitioning(false)
{

}

TiledNavigationMeshBuilder::~TiledNavigationMeshBuilder() 
{

}

//helpers
inline unsigned int nextPow2(unsigned int v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

inline unsigned int ilog2(unsigned int v)
{
	unsigned int r;
	unsigned int shift;
	r = (v > 0xffff) << 4; v >>= r;
	shift = (v > 0xff) << 3; v >>= shift; r |= shift;
	shift = (v > 0xf) << 2; v >>= shift; r |= shift;
	shift = (v > 0x3) << 1; v >>= shift; r |= shift;
	r |= (v >> 1);
	return r;
}


dtNavMesh* TiledNavigationMeshBuilder::Build(InputGeom* geom)
{
	const float* bmin = geom->getMeshBoundsMin();
	const float* bmax = geom->getMeshBoundsMax();

	dtStatus status;
	//recreate nav mesh
	//dtFreeNavMesh(m_NavMesh);
	dtNavMesh* nav_mesh = dtAllocNavMesh();

	if (!nav_mesh)
	{
		m_Ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate navmesh.");
		return NULL;
	}

	int gw = 0, gh = 0;
	rcCalcGridSize(bmin, bmax, m_CellSize, &gw, &gh);
	const int ts = (int)m_TileSize;
	const int tw = (gw + ts-1) / ts;
	const int th = (gh + ts-1) / ts;

	// Max tiles and max polys affect how the tile IDs are caculated.
	// There are 22 bits available for identifying a tile and a polygon.
	int tileBits = rcMin((int)ilog2(nextPow2(tw*th)), 14);
	if (tileBits > 14) tileBits = 14;
	int polyBits = 22 - tileBits;
	int maxTiles = 1 << tileBits;
	int maxPolysPerTile = 1 << polyBits;

	dtNavMeshParams nm_params;
	rcVcopy(nm_params.orig, bmin);
	nm_params.tileWidth = m_TileSize*m_CellSize;
	nm_params.tileHeight = m_TileSize*m_CellSize;
	nm_params.maxTiles = maxTiles;
	nm_params.maxPolys = maxPolysPerTile;

	status = nav_mesh->init(&nm_params);
	if (dtStatusFailed(status))
	{
		m_Ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init navmesh.");
		return NULL;
	}

	rcCalcGridSize(bmin, bmax, m_CellSize, &gw, &gh);

	const float tcs = m_TileSize*m_CellSize;

	// Start the build process.
	m_Ctx->startTimer(RC_TIMER_TEMP);

	for (int y = 0; y < th; ++y)
	{
		for (int x = 0; x < tw; ++x)
		{
			float tileBmin[3];
			float tileBmax[3];

			tileBmin[0] = bmin[0] + x*tcs;
			tileBmin[1] = bmin[1];
			tileBmin[2] = bmin[2] + y*tcs;

			tileBmax[0] = bmin[0] + (x+1)*tcs;
			tileBmax[1] = bmax[1];
			tileBmax[2] = bmin[2] + (y+1)*tcs;

			int dataSize = 0;
			unsigned char* data = BuildTileMesh(geom, x, y, tileBmin, tileBmax, dataSize);
			if (data)
			{
				// Remove any previous data (navmesh owns and deletes the data).
				nav_mesh->removeTile(nav_mesh->getTileRefAt(x,y,0),0,0);
				// Let the navmesh own the data.
				status = nav_mesh->addTile(data,dataSize,DT_TILE_FREE_DATA,0,0);
				if (dtStatusFailed(status))
					dtFree(data);
			}
		}
	}
	// Start the build process.	
	m_Ctx->stopTimer(RC_TIMER_TEMP);
	float totalBuildTimeMs = m_Ctx->getAccumulatedTime(RC_TIMER_TEMP)/1000.0f;
	return nav_mesh;
}

unsigned char* TiledNavigationMeshBuilder::BuildTileMesh(InputGeom* geom, const int tx, const int ty, const float* bmin, const float* bmax, int& dataSize)
{
	/*if (!m_Geom || !m_Geom->getMesh() || !m_Geom->getChunkyMesh())
	{
	m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Input mesh is not specified.");
	return 0;
	}*/

	const float* verts = geom->getMesh()->getVerts();
	const int nverts = geom->getMesh()->getVertCount();
	const int ntris = geom->getMesh()->getTriCount();
	const rcChunkyTriMesh* chunkyMesh = geom->getChunkyMesh();



	rcConfig cfg;
	memset(&cfg, 0, sizeof(cfg));
	cfg.cs = m_CellSize;
	cfg.ch = m_CellHeight;
	cfg.walkableSlopeAngle = m_AgentMaxSlope;
	cfg.walkableHeight = (int)ceilf(m_AgentHeight / cfg.ch);
	cfg.walkableClimb = (int)floorf(m_AgentMaxClimb / cfg.ch);
	cfg.walkableRadius = (int)ceilf(m_AgentRadius / cfg.cs);
	cfg.maxEdgeLen = (int)(m_EdgeMaxLen / m_CellSize);
	cfg.maxSimplificationError = m_EdgeMaxError;
	cfg.minRegionArea = (int)rcSqr(m_RegionMinSize);		// Note: area = size*size
	cfg.mergeRegionArea = (int)rcSqr(m_RegionMergeSize);	// Note: area = size*size
	cfg.maxVertsPerPoly = (int)m_VertsPerPoly;
	cfg.tileSize = (int)m_TileSize;
	cfg.borderSize = cfg.walkableRadius + 3; // Reserve enough padding.
	cfg.width = cfg.tileSize + cfg.borderSize*2;
	cfg.height = cfg.tileSize + cfg.borderSize*2;
	cfg.detailSampleDist = m_DetailSampleDist < 0.9f ? 0 : m_CellSize * m_DetailSampleDist;
	cfg.detailSampleMaxError = m_CellHeight * m_DetailSampleMaxError;

	rcVcopy(cfg.bmin, bmin);
	rcVcopy(cfg.bmax, bmax);
	cfg.bmin[0] -= cfg.borderSize*cfg.cs;
	cfg.bmin[2] -= cfg.borderSize*cfg.cs;
	cfg.bmax[0] += cfg.borderSize*cfg.cs;
	cfg.bmax[2] += cfg.borderSize*cfg.cs;

	// Reset build times gathering.
	m_Ctx->resetTimers();

	// Start the build process.
	m_Ctx->startTimer(RC_TIMER_TOTAL);

	m_Ctx->log(RC_LOG_PROGRESS, "Building navigation:");
	m_Ctx->log(RC_LOG_PROGRESS, " - %d x %d cells", cfg.width, cfg.height);
	m_Ctx->log(RC_LOG_PROGRESS, " - %.1fK verts, %.1fK tris", nverts/1000.0f, ntris/1000.0f);

	// Allocate voxel heightfield where we rasterize our input data to.
	rcHeightfield* solid = rcAllocHeightfield();
	if (!solid)
	{
		m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
		return 0;
	}
	if (!rcCreateHeightfield(m_Ctx, *solid, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs, cfg.ch))
	{
		//m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
		return 0;
	}

	// Allocate array that can hold triangle flags.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	unsigned char*  triareas = new unsigned char[chunkyMesh->maxTrisPerChunk];
	if (!triareas)
	{
		m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", chunkyMesh->maxTrisPerChunk);
		return 0;
	}

	float tbmin[2], tbmax[2];
	tbmin[0] = cfg.bmin[0];
	tbmin[1] = cfg.bmin[2];
	tbmax[0] = cfg.bmax[0];
	tbmax[1] = cfg.bmax[2];
	int cid[512];// TODO: Make grow when returning too many items.
	const int ncid = rcGetChunksOverlappingRect(chunkyMesh, tbmin, tbmax, cid, 512);
	if (!ncid)
		return 0;

	int tileTriCount = 0;

	for (int i = 0; i < ncid; ++i)
	{
		const rcChunkyTriMeshNode& node = chunkyMesh->nodes[cid[i]];
		const int* ctris = &chunkyMesh->tris[node.i*3];
		const int nctris = node.n;

		tileTriCount += nctris;

		memset(triareas, 0, nctris*sizeof(unsigned char));
		rcMarkWalkableTriangles(m_Ctx, cfg.walkableSlopeAngle,
			verts, nverts, ctris, nctris, triareas);

		//Hack to get areas from orginal mesh to chunky mesh
		const int *mesh_areas =  geom->getMesh()->getAreas();
		for (int j = 0; j < nctris; ++j)
		{
			if(triareas[j])
				triareas[j] = mesh_areas[chunkyMesh->area_indices[node.i+j]];
		}

		rcRasterizeTriangles(m_Ctx, verts, nverts, ctris, triareas, nctris, *solid, cfg.walkableClimb);
	}

	//if (!m_keepInterResults)
	{
		delete [] triareas;
		triareas = 0;
	}

	// Once all geometry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	rcFilterLowHangingWalkableObstacles(m_Ctx, cfg.walkableClimb, *solid);
	rcFilterLedgeSpans(m_Ctx, cfg.walkableHeight, cfg.walkableClimb, *solid);
	rcFilterWalkableLowHeightSpans(m_Ctx, cfg.walkableHeight, *solid);

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	rcCompactHeightfield* chf = rcAllocCompactHeightfield();
	if (!chf)
	{
		m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
		return 0;
	}
	if (!rcBuildCompactHeightfield(m_Ctx, cfg.walkableHeight, cfg.walkableClimb, *solid, *chf))
	{
		m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
		return 0;
	}

	//	if (!m_keepInterResults)
	{
		rcFreeHeightField(solid);
		solid = 0;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(m_Ctx, cfg.walkableRadius, *chf))
	{
		m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
		return 0;
	}

	// (Optional) Mark areas.
	const ConvexVolume* vols = geom->getConvexVolumes();
	for (int i  = 0; i < geom->getConvexVolumeCount(); ++i)
		rcMarkConvexPolyArea(m_Ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *chf);

	if (m_MonotonePartitioning)
	{
		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegionsMonotone(m_Ctx, *chf, cfg.borderSize, cfg.minRegionArea, cfg.mergeRegionArea))
		{
			m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build regions.");
			return 0;
		}
	}
	else
	{
		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(m_Ctx, *chf))
		{
			m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
			return 0;
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(m_Ctx, *chf, cfg.borderSize, cfg.minRegionArea, cfg.mergeRegionArea))
		{
			m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build regions.");
			return 0;
		}
	}
	// Create contours.
	rcContourSet* cset = rcAllocContourSet();
	if (!cset)
	{
		m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
		return 0;
	}
	if (!rcBuildContours(m_Ctx, *chf, cfg.maxSimplificationError, cfg.maxEdgeLen, *cset))
	{
		m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
		return 0;
	}

	if (cset->nconts == 0)
	{
		return 0;
	}

	// Build polygon navmesh from the contours.


	rcPolyMesh* pmesh = rcAllocPolyMesh();
	if (!pmesh)
	{
		m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
		return 0;
	}
	if (!rcBuildPolyMesh(m_Ctx, *cset, cfg.maxVertsPerPoly, *pmesh))
	{
		m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
		return 0;
	}

	// Build detail mesh.
	rcPolyMeshDetail *dmesh = rcAllocPolyMeshDetail();
	if (!dmesh)
	{
		m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'dmesh'.");
		return 0;
	}

	if (!rcBuildPolyMeshDetail(m_Ctx, *pmesh, *chf,
		cfg.detailSampleDist, cfg.detailSampleMaxError,
		*dmesh))
	{
		m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Could build polymesh detail.");
		return 0;
	}

	//	if (!m_keepInterResults)
	{
		rcFreeCompactHeightfield(chf);
		chf = 0;
		rcFreeContourSet(cset);
		cset = 0;
	}

	unsigned char* navData = 0;
	int navDataSize = 0;
	if (cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	{
		if (pmesh->nverts >= 0xffff)
		{
			// The vertex indices are ushorts, and cannot point to more than 0xffff vertices.
			m_Ctx->log(RC_LOG_ERROR, "Too many vertices per tile %d (max: %d).", pmesh->nverts, 0xffff);
			return 0;
		}

		// Update poly flags from areas.
		for (int i = 0; i < pmesh->npolys; ++i)
		{
			//if (pmesh->areas[i] == RC_WALKABLE_AREA)
			//	pmesh->areas[i] = LAND_COVER_DEFAULT;

			//pmesh->areas[i] = geom->getMesh()->getAreas()[i];
			pmesh->flags[i] = 0x01;

			//std::cout << geom->getMesh()->getAreas()[i] << "\n";

			/*	if (pmesh->areas[i] == LAND_COVER_DEFAULT ||
			pmesh->areas[i] == LAND_COVER_GRASS_C1 ||
			pmesh->areas[i] == LAND_COVER_ROAD_C1)
			{
			pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
			}
			else if (pmesh->areas[i] == LAND_COVER_WATER_C1)
			{
			pmesh->flags[i] = SAMPLE_POLYFLAGS_SWIM;
			}
			else if (pmesh->areas[i] == LAND_COVER_DOOR)
			{
			pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
			}
			}*/

			dtNavMeshCreateParams params;
			memset(&params, 0, sizeof(params));
			params.verts = pmesh->verts;
			params.vertCount = pmesh->nverts;
			params.polys = pmesh->polys;
			params.polyAreas = pmesh->areas;
			params.polyFlags = pmesh->flags;
			params.polyCount = pmesh->npolys;
			params.nvp = pmesh->nvp;
			params.detailMeshes = dmesh->meshes;
			params.detailVerts = dmesh->verts;
			params.detailVertsCount = dmesh->nverts;
			params.detailTris = dmesh->tris;
			params.detailTriCount = dmesh->ntris;
			params.offMeshConVerts = geom->getOffMeshConnectionVerts();
			params.offMeshConRad = geom->getOffMeshConnectionRads();
			params.offMeshConDir = geom->getOffMeshConnectionDirs();
			params.offMeshConAreas = geom->getOffMeshConnectionAreas();
			params.offMeshConFlags = geom->getOffMeshConnectionFlags();
			params.offMeshConUserID = geom->getOffMeshConnectionId();
			params.offMeshConCount = geom->getOffMeshConnectionCount();
			params.walkableHeight = m_AgentHeight;
			params.walkableRadius = m_AgentRadius;
			params.walkableClimb = m_AgentMaxClimb;
			params.tileX = tx;
			params.tileY = ty;
			params.tileLayer = 0;
			rcVcopy(params.bmin, pmesh->bmin);
			rcVcopy(params.bmax, pmesh->bmax);
			params.cs = cfg.cs;
			params.ch = cfg.ch;
			params.buildBvTree = true;

			if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
			{
				m_Ctx->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
				return 0;
			}		
		}
		int tileMemUsage = navDataSize/1024.0f;

		m_Ctx->stopTimer(RC_TIMER_TOTAL);
		// Show performance stats.
		//duLogBuildTimes(*m_Ctx, m_Ctx->getAccumulatedTime(RC_TIMER_TOTAL));
		m_Ctx->log(RC_LOG_PROGRESS, ">> Polymesh: %d vertices  %d polygons", pmesh->nverts, pmesh->npolys);

		//Free mesh
		rcFreePolyMesh(pmesh);
		rcFreePolyMeshDetail(dmesh);

		float tileBuildTime = m_Ctx->getAccumulatedTime(RC_TIMER_TOTAL)/1000.0f;
		dataSize = navDataSize;
		return navData;
	}
	return NULL;
}

