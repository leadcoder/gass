/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/

#include "RecastNavigationMeshComponent.h"
#include "Recast/Recast.h"
#include "Recast/RecastAlloc.h"
#include "Recast/RecastAssert.h"
#include "Detour/DetourAssert.h"
#include "Detour/DetourNavMesh.h"
#include "Detour/DetourNavMeshBuilder.h"
#include "Detour/DetourNavMeshQuery.h"
#include "Detour/DetourCommon.h"
#include "Detour/DetourTileCache.h"
#include "RecastOffmeshMeshConnectionComponent.h" 
#include "RecastConvexVolumeComponent.h"
#include "InputGeom.h"
#include "tinyxml.h"
#include "Core/Utils/GASSFileUtils.h"

namespace GASS
{
	std::vector<SceneObjectPtr> NavMeshEnumeration(BaseReflectionObjectPtr obj)
	{
		RecastNavigationMeshComponentPtr navmesh_comp = DYNAMIC_PTR_CAST<RecastNavigationMeshComponent>(obj);
		return navmesh_comp->GetMeshSelectionEnum();
	}


	typedef SPTR<SceneObjectEnumerationProxyPropertyMetaData > SceneObjectEnumerationProxyPropertyMetaDataPtr;
	RecastNavigationMeshComponent::RecastNavigationMeshComponent() :m_NavVisTriMesh(new ManualMeshData()),
		m_NavVisLineMesh(new ManualMeshData()),
		m_NavMesh(NULL),
		m_ShowMeshLines(false),
		m_ShowMeshSolid(false),
		m_Transparency(30),
		m_Visible(true),
		m_Initialized(false),
		m_CellSize (0.3f),
		m_CellHeight (0.2f),
		m_AgentHeight ( 2.0f),
		m_AgentRadius ( 0.6f),
		m_AgentMaxClimb ( 0.9f),
		m_AgentMaxSlope ( 45.0f),
		m_RegionMinSize ( 50),
		m_RegionMergeSize ( 20),
		m_EdgeMaxLen ( 12.0f),
		m_EdgeMaxError ( 1.3f),
		m_VertsPerPoly ( 6.0f),
		m_DetailSampleDist ( 6.0f),
		m_DetailSampleMaxError ( 1.0f),
		m_TileSize ( 64),
		m_NavQuery ( dtAllocNavMeshQuery()),
		m_Geom ( new InputGeom()),
		m_Ctx( new rcContext(true)),
		m_MonotonePartitioning ( false)
	{
	}

	RecastNavigationMeshComponent::~RecastNavigationMeshComponent()
	{
		delete m_Geom;
		dtFreeNavMeshQuery(m_NavQuery);
		dtFreeNavMesh(m_NavMesh);
	}

	void RecastNavigationMeshComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("RecastNavigationMeshComponent",new Creator<RecastNavigationMeshComponent, IComponent>);

		GetClassRTTI()->SetMetaData(ObjectMetaDataPtr(new ObjectMetaData("RecastNavigationMeshComponent", OF_VISIBLE)));
		
		RegisterProperty<float>("CellSize", &GetCellSize, &SetCellSize,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("CellHeight", &GetCellHeight, &SetCellHeight,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("AgentHeight", &GetAgentHeight, &SetAgentHeight,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("AgentRadius", &GetAgentRadius, &SetAgentRadius,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("AgentMaxClimb", &GetAgentMaxClimb, &SetAgentMaxClimb,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("AgentMaxSlope", &GetAgentMaxSlope, &SetAgentMaxSlope,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("RegionMinSize", &GetRegionMinSize, &SetRegionMinSize,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("RegionMergeSize", &GetRegionMergeSize, &SetRegionMergeSize,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("EdgeMaxLen", &GetEdgeMaxLen, &SetEdgeMaxLen,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("EdgeMaxError", &GetEdgeMaxError, &SetEdgeMaxError,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("VertsPerPoly", &GetVertsPerPoly, &SetVertsPerPoly,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("DetailSampleDist", &GetDetailSampleDist, &SetDetailSampleDist,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("DetailSampleMaxError", &GetDetailSampleMaxError, &SetDetailSampleMaxError,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("Build", &GetBuild, &SetBuild,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("Visible", &GetVisible, &SetVisible,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("ShowMeshLines", &GetShowMeshLines, &SetShowMeshLines,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("ShowMeshSolid", &GetShowMeshSolid, &SetShowMeshSolid,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec3>("MeshBoundingMin", &GetMeshBoundingMin, &SetMeshBoundingMin,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec3>("MeshBoundingMax", &GetMeshBoundingMax, &SetMeshBoundingMax,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<int>("TileSize", &GetTileSize, &SetTileSize,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<int>("Transparency", &GetTransparency, &SetTransparency,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterVectorProperty<SceneObjectRef>("MeshSelection", &GetSelectedMeshes, &SetSelectedMeshes,
			SceneObjectEnumerationProxyPropertyMetaDataPtr(new SceneObjectEnumerationProxyPropertyMetaData("Transmitter connection",PF_VISIBLE,NavMeshEnumeration)));
		RegisterProperty<std::string>("BoundingBoxFromShape", &GetBoundingBoxFromShape, &SetBoundingBoxFromShape,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<FilePath>("ImportMesh", &GetImportMesh, &SetImportMesh,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<FilePath>("ExportMesh", &GetExportMesh, &SetExportMesh,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
	}

	void RecastNavigationMeshComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RecastNavigationMeshComponent::OnEditPosition,EditPositionMessage,1));

		const std::string filename = m_NavMeshFilePath + GetSceneObject()->GetName() + ".bin";
		if(FILE*fp = fopen(filename.c_str(),"rt"))
		{
			fclose(fp);
			m_NavMesh = LoadAll(filename.c_str());
			m_NavQuery->init(m_NavMesh, 2048);
			if(m_NavMesh)
				UpdateNavMeshVis();
		}
		//initlize visibility
		SetVisible(m_Visible);
		m_Initialized = true;
	}

	void RecastNavigationMeshComponent::OnDelete()
	{
		delete m_Geom;
		m_Geom = NULL;
		dtFreeNavMeshQuery(m_NavQuery);
		m_NavQuery = NULL;
		dtFreeNavMesh(m_NavMesh);
		m_NavMesh = NULL;
	}

	void RecastNavigationMeshComponent::OnEditPosition(EditPositionMessagePtr message)
	{
		if (m_NavMesh && m_NavQuery)
		{
			dtQueryFilter filter;
			float ext[3];
			float point[3];
			Vec3 pos = message->GetPosition();
			point[0] =pos.x; point[1] =pos.y; point[2] =pos.z;
			ext[0] = 0.1; ext[1] = 0.1; ext[2] = 0.1;
			float tgt[3];
			dtPolyRef ref;
			m_NavQuery->findNearestPoly(point, ext, &filter, &ref, tgt);
			if (ref)
			{
				unsigned short flags = 0;
				if (dtStatusSucceed(m_NavMesh->getPolyFlags(ref, &flags)))
				{
					flags ^= SAMPLE_POLYFLAGS_DISABLED;
					m_NavMesh->setPolyFlags(ref, flags);
					UpdateNavMeshVis();
				}
			}
		}
	}


	// Quick and dirty convex hull.

// Returns true if 'c' is left of line 'a'-'b'.
inline bool left(const float* a, const float* b, const float* c)
{ 
	const float u1 = b[0] - a[0];
	const float v1 = b[2] - a[2];
	const float u2 = c[0] - a[0];
	const float v2 = c[2] - a[2];
	return u1 * v2 - v1 * u2 < 0;
}

// Returns true if 'a' is more lower-left than 'b'.
inline bool cmppt(const float* a, const float* b)
{
	if (a[0] < b[0]) return true;
	if (a[0] > b[0]) return false;
	if (a[2] < b[2]) return true;
	if (a[2] > b[2]) return false;
	return false;
}
// Calculates convex hull on xz-plane of points on 'pts',
// stores the indices of the resulting hull in 'out' and
// returns number of points on hull.
static int convexhull(const float* pts, int npts, int* out)
{
	// Find lower-leftmost point.
	int hull = 0;
	for (int i = 1; i < npts; ++i)
		if (cmppt(&pts[i*3], &pts[hull*3]))
			hull = i;
	// Gift wrap hull.
	int endpt = 0;
	int i = 0;
	do
	{
		out[i++] = hull;
		endpt = 0;
		for (int j = 1; j < npts; ++j)
			if (hull == endpt || left(&pts[hull*3], &pts[endpt*3], &pts[j*3]))
				endpt = j;
		hull = endpt;
	}
	while (endpt != out[0]);

	return i;
}



	void RecastNavigationMeshComponent::UpdateConvexVolumes()
	{
		for(size_t i = 0; i < m_Geom->getConvexVolumeCount();i++)
		{
			m_Geom->deleteConvexVolume(static_cast<int>(i));
		}
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<RecastConvexVolumeComponent>(components,true);
		
		for(size_t i = 0; i < components.size(); i++)
		{
			RecastConvexVolumeComponentPtr comp = DYNAMIC_PTR_CAST<RecastConvexVolumeComponent>(components[i]);
			GeometryComponentPtr geom = comp->GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
			if(geom)
			{
				LandCoverType area = comp->GetLandCoverType().GetValue();

				AABox box = geom->GetBoundingBox();
				std::vector<Vec3> corners = box.GetCorners();

				LocationComponentPtr loc_comp = comp->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
				assert(loc_comp);
				Mat4 trans;
				trans.SetTransformation(loc_comp->GetWorldPosition(),loc_comp->GetWorldRotation(),Vec3(1,1,1));

				std::vector<float> points;

				float minh,maxh;

				minh = FLT_MAX;
				maxh = -FLT_MAX;

				for(size_t j = 0; j < 8; j++)
				{
					Vec3 vertex = trans*corners[j];
					points.push_back(vertex.x);
					points.push_back(vertex.y);
					points.push_back(vertex.z);
					if(vertex.y < minh)
						minh = vertex.y;
					if(vertex.y > maxh)
						maxh = vertex.y;
				}

				m_Geom->addConvexVolume(&points[0], 4, minh, maxh, area);

				/*int hull[12];
				std::vector<float> hull_points;
				
				int nhull = convexhull(&points[0], 4, hull);
				for(size_t j = 0; j < nhull; j++)
				{
					float x = points[hull[j]*3];
					float y = points[hull[j]*3+1];
					float z = points[hull[j]*3+2];
					hull_points.push_back(x);
					hull_points.push_back(y);
					hull_points.push_back(z);

					if(y < minh)
						minh = y;
					if(y > maxh)
						maxh = y;
				}
				m_Geom->addConvexVolume(&hull_points[0], nhull, minh-1, maxh+1, area);*/
			}
		}
	}


	void RecastNavigationMeshComponent::UpdateOffmeshConnections()
	{
		for(size_t i = 0; i < m_Geom->getOffMeshConnectionCount();i++)
		{
			m_Geom->deleteOffMeshConnection(static_cast<int>(i));
		}

		const unsigned char area = LAND_COVER_JUMP;
		const unsigned short flags = SAMPLE_POLYFLAGS_JUMP; 
		float p1[3];
		float p2[3];
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<RecastOffmeshMeshConnectionComponent>(components,true);
		for(int i = 0; i < components.size(); i++)
		{
			RecastOffmeshMeshConnectionComponentPtr comp = DYNAMIC_PTR_CAST<RecastOffmeshMeshConnectionComponent>(components[i]);
			Vec3 pos1 = comp->GetStartPos();
			Vec3 pos2 = comp->GetEndPos();

			unsigned char bidir = 0;
			const std::string connection_mode = comp->GetMode();
			if(connection_mode == "BIDIRECTIONAL")
			{
				bidir = 1;
			}
			else if(connection_mode == "UP_STREAM")
			{
				pos2 = comp->GetStartPos();
				pos1 = comp->GetEndPos();
			}

			p1[0] =pos1.x;p1[1] =pos1.y;p1[2] =pos1.z;
			p2[0] =pos2.x;p2[1] =pos2.y;p2[2] =pos2.z;
			
			//m_Geom->addOffMeshConnection(p1, p2, m_AgentRadius, bidir, area, flags);
			m_Geom->addOffMeshConnection(p1, p2, comp->GetRadius(), bidir, area, flags);
			
			
		}
	}

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

	bool RecastNavigationMeshComponent::GenerateTiles()
	{

		

		RawNavMeshData rnm_data;
		rnm_data.BMax = NULL;
		rnm_data.BMin = NULL;
		rnm_data.TriNorm = NULL;
		rnm_data.Tris = NULL;
		rnm_data.Tris = NULL;
		rnm_data.NumTris = 0;
		rnm_data.NumVerts = 0;
		GetRawMeshData(rnm_data);

		if(rnm_data.NumVerts == 0)
			return false;
		m_Geom->createMesh(rnm_data.Verts, rnm_data.TriNorm, rnm_data.Tris, rnm_data.NumVerts,rnm_data.NumTris);
		m_Geom->setMeshBoundsMax(rnm_data.BMax);
		m_Geom->setMeshBoundsMin(rnm_data.BMin);
		
		

		UpdateConvexVolumes();
		UpdateOffmeshConnections();
		

		dtStatus status;
		dtFreeNavMesh(m_NavMesh);
		m_NavMesh = dtAllocNavMesh();

		if (!m_NavMesh)
		{
			m_Ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate navmesh.");
			return false;
		}

		int gw = 0, gh = 0;
		rcCalcGridSize(rnm_data.BMin, rnm_data.BMax, m_CellSize, &gw, &gh);
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

		dtNavMeshParams params;
		rcVcopy(params.orig, rnm_data.BMin);
		params.tileWidth = m_TileSize*m_CellSize;
		params.tileHeight = m_TileSize*m_CellSize;
		params.maxTiles = maxTiles;
		params.maxPolys = maxPolysPerTile;

		status = m_NavMesh->init(&params);
		if (dtStatusFailed(status))
		{
			m_Ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init navmesh.");
			return false;
		}

		status = m_NavQuery->init(m_NavMesh, 2048);
		if (dtStatusFailed(status))
		{
			m_Ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init Detour navmesh query");
			return false;
		}
		BuildAllTiles();
		return true;
	}

	void RecastNavigationMeshComponent::BuildAllTiles()
	{
		if (!m_NavMesh) 
			return;

		const float* bmin = m_Geom->getMeshBoundsMin();
		const float* bmax = m_Geom->getMeshBoundsMax();
		int gw = 0, gh = 0;
		rcCalcGridSize(bmin, bmax, m_CellSize, &gw, &gh);
		const int ts = (int)m_TileSize;
		const int tw = (gw + ts-1) / ts;
		const int th = (gh + ts-1) / ts;
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
				unsigned char* data = BuildTileMesh(x, y, tileBmin, tileBmax, dataSize);
				if (data)
				{
					// Remove any previous data (navmesh owns and deletes the data).
					m_NavMesh->removeTile(m_NavMesh->getTileRefAt(x,y,0),0,0);
					// Let the navmesh own the data.
					dtStatus status = m_NavMesh->addTile(data,dataSize,DT_TILE_FREE_DATA,0,0);
					if (dtStatusFailed(status))
						dtFree(data);
				}
			}
		}
		// Start the build process.	
		m_Ctx->stopTimer(RC_TIMER_TEMP);
		float totalBuildTimeMs = m_Ctx->getAccumulatedTime(RC_TIMER_TEMP)/1000.0f;
	}

	unsigned char* RecastNavigationMeshComponent::BuildTileMesh(const int tx, const int ty, const float* bmin, const float* bmax, int& dataSize)
	{
		if (!m_Geom || !m_Geom->getMesh() || !m_Geom->getChunkyMesh())
		{
			m_Ctx->log(RC_LOG_ERROR, "buildNavigation: Input mesh is not specified.");
			return 0;
		}

		const float* verts = m_Geom->getMesh()->getVerts();
		const int nverts = m_Geom->getMesh()->getVertCount();
		const int ntris = m_Geom->getMesh()->getTriCount();
		const rcChunkyTriMesh* chunkyMesh = m_Geom->getChunkyMesh();


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
		const ConvexVolume* vols = m_Geom->getConvexVolumes();
		for (int i  = 0; i < m_Geom->getConvexVolumeCount(); ++i)
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
				if (pmesh->areas[i] == RC_WALKABLE_AREA)
					pmesh->areas[i] = LAND_COVER_DEFAULT;

				if (pmesh->areas[i] == LAND_COVER_DEFAULT ||
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
			}

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
			params.offMeshConVerts = m_Geom->getOffMeshConnectionVerts();
			params.offMeshConRad = m_Geom->getOffMeshConnectionRads();
			params.offMeshConDir = m_Geom->getOffMeshConnectionDirs();
			params.offMeshConAreas = m_Geom->getOffMeshConnectionAreas();
			params.offMeshConFlags = m_Geom->getOffMeshConnectionFlags();
			params.offMeshConUserID = m_Geom->getOffMeshConnectionId();
			params.offMeshConCount = m_Geom->getOffMeshConnectionCount();
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


	int RecastNavigationMeshComponent::GetTransparency() const
	{
		return m_Transparency;
	}

	bool RecastNavigationMeshComponent::GetVisible()  const
	{
		return m_Visible;
	}

	void RecastNavigationMeshComponent::SetVisible(bool value) 
	{
		m_Visible = value;
		if(GetSceneObject())
		{
			GetSceneObject()->PostMessage(MessagePtr(new VisibilityMessage(value)));
		}
	}

	void  RecastNavigationMeshComponent::SetTransparency(int value)
	{
		m_Transparency = value;
		if(m_Initialized)
		{
			UpdateNavMeshVis();
		}
	}

	Vec3 RecastNavigationMeshComponent::GetMeshBoundingMin() const
	{
		return m_MeshBounding.m_Min;
	}

	void RecastNavigationMeshComponent::SetMeshBoundingMin(const Vec3 &min)
	{
		m_MeshBounding.m_Min = min;
	}

	Vec3 RecastNavigationMeshComponent::GetMeshBoundingMax() const
	{
		return m_MeshBounding.m_Max;
	}

	void RecastNavigationMeshComponent::SetMeshBoundingMax(const Vec3 &max)
	{
		m_MeshBounding.m_Max = max;
	}

	float RecastNavigationMeshComponent::GetCellSize() const 
	{
		return m_CellSize;

	}

	float RecastNavigationMeshComponent::GetCellHeight() const 
	{
		return m_CellHeight;
	}

	float RecastNavigationMeshComponent::GetAgentHeight() const 
	{
		return m_AgentHeight;
	}

	float RecastNavigationMeshComponent::GetAgentRadius() const 
	{
		return m_AgentRadius;
	}

	float RecastNavigationMeshComponent::GetAgentMaxClimb() const 
	{
		return m_AgentMaxClimb;
	}

	float RecastNavigationMeshComponent::GetAgentMaxSlope() const 
	{
		return m_AgentMaxSlope;
	}

	float RecastNavigationMeshComponent::GetRegionMinSize() const 
	{
		return m_RegionMinSize;
	}

	float RecastNavigationMeshComponent::GetRegionMergeSize() const 
	{
		return m_RegionMergeSize;
	}

	float RecastNavigationMeshComponent::GetEdgeMaxLen() const 
	{
		return m_EdgeMaxLen;
	}

	float RecastNavigationMeshComponent::GetEdgeMaxError() const 
	{
		return m_EdgeMaxError;
	}

	float RecastNavigationMeshComponent::GetVertsPerPoly() const 
	{
		return m_VertsPerPoly;
	}

	float RecastNavigationMeshComponent::GetDetailSampleDist() const 
	{
		return m_DetailSampleDist;
	}

	float RecastNavigationMeshComponent::GetDetailSampleMaxError() const 
	{
		return m_DetailSampleMaxError;
	}

	bool RecastNavigationMeshComponent::GetBuild() const 
	{
		
		return false;
	}

	int RecastNavigationMeshComponent::GetTileSize() const 
	{
		return m_TileSize;
	}

	

	void RecastNavigationMeshComponent::SetCellSize(const float value) {m_CellSize = value;}
	void RecastNavigationMeshComponent::SetCellHeight(const float value) {m_CellHeight = value;}
	void RecastNavigationMeshComponent::SetAgentHeight(const float value) {m_AgentHeight= value;}
	void RecastNavigationMeshComponent::SetAgentRadius (const float value) {m_AgentRadius =value;}
	void RecastNavigationMeshComponent::SetAgentMaxClimb(const float value) {m_AgentMaxClimb = value;}
	void RecastNavigationMeshComponent::SetAgentMaxSlope(const float value) {m_AgentMaxSlope=value;}
	void RecastNavigationMeshComponent::SetRegionMinSize(const float value) {m_RegionMinSize=value;}
	void RecastNavigationMeshComponent::SetRegionMergeSize(const float value) {m_RegionMergeSize=value;}
	void RecastNavigationMeshComponent::SetEdgeMaxLen(const float value)  {m_EdgeMaxLen=value;}
	void RecastNavigationMeshComponent::SetEdgeMaxError(const float value) {m_EdgeMaxError=value;}
	void RecastNavigationMeshComponent::SetVertsPerPoly(const float value) {m_VertsPerPoly=value;}
	void RecastNavigationMeshComponent::SetDetailSampleDist(const float value) {m_DetailSampleDist=value;}
	void RecastNavigationMeshComponent::SetDetailSampleMaxError(const float value) {m_DetailSampleMaxError=value;}
	void RecastNavigationMeshComponent::SetTileSize(const int size) {m_TileSize=size;}

	void RecastNavigationMeshComponent::SetBuild(const bool value)
	{
		if(m_Initialized)
		{
			GenerateTiles();
			UpdateNavMeshVis();
		}
	}

	void RecastNavigationMeshComponent::SaveXML(TiXmlElement *obj_elem)
	{
		m_NavMeshFilePath = obj_elem->GetDocument()->Value();
		m_NavMeshFilePath = FileUtils::RemoveFilename(m_NavMeshFilePath);
		if(m_NavMesh)
		{
			std::string filename = m_NavMeshFilePath + GetSceneObject()->GetName() + ".bin";
			SaveAllTiles(filename.c_str(),m_NavMesh);
		}
		BaseComponent::SaveXML(obj_elem);
	}

	void RecastNavigationMeshComponent::LoadXML(TiXmlElement *obj_elem)
	{
		m_NavMeshFilePath = obj_elem->GetDocument()->Value();
		m_NavMeshFilePath = FileUtils::RemoveFilename(m_NavMeshFilePath);
		BaseComponent::LoadXML(obj_elem);
	}



	void RecastNavigationMeshComponent::SetBoundingBoxFromShape(const std::string &value)
	{
		m_BBShape = value;

		if(GetSceneObject()) //initlized?
		{
			std::vector<SceneObjectPtr> objs;
			GetSceneObject()->GetScene()->GetRootSceneObject()->GetChildrenByName(objs,value);
			if(objs.size() > 0)
			{
				SceneObjectPtr obj = objs.front();
				if(obj)
				{
					GeometryComponentPtr geom = obj->GetFirstComponentByClass<IGeometryComponent>();
					LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
					AABox box = geom->GetBoundingBox();
					if(lc)
					{
						Vec3 world_pos = lc->GetWorldPosition();
						Quaternion world_rot = lc->GetWorldRotation();
						Mat4 trans_mat;
						trans_mat.Identity();
						world_rot.ToRotationMatrix(trans_mat);
						trans_mat.SetTranslation(world_pos.x,world_pos.y,world_pos.z);
						box.Transform(trans_mat);
					}
					m_MeshBounding = box;
				}
			}
		}
	}


	std::string RecastNavigationMeshComponent::GetBoundingBoxFromShape() const
	{
		return m_BBShape;
	}

	void RecastNavigationMeshComponent::SetExportMesh(const FilePath &value)
	{
		if(m_Initialized && m_NavMesh)
		{
			
			SaveAllTiles(value.GetFullPath().c_str(), m_NavMesh);
		}
	}

	FilePath RecastNavigationMeshComponent::GetExportMesh() const
	{
		return FilePath("");
	}

	void RecastNavigationMeshComponent::SetImportMesh(const FilePath &value)
	{
		if(m_Initialized)
		{
			dtFreeNavMesh(m_NavMesh);
			m_NavMesh = LoadAll(value.GetFullPath().c_str());
			m_NavQuery->init(m_NavMesh, 2048);

			UpdateNavMeshVis();
		}
	}

	FilePath RecastNavigationMeshComponent::GetImportMesh() const
	{
		return FilePath("");
	}

	std::vector<SceneObjectPtr> RecastNavigationMeshComponent::GetMeshSelectionEnum()
	{
		std::vector<SceneObjectPtr> objs;
		if(GetSceneObject())
		{
			IComponentContainer::ComponentVector components;
			GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<IMeshComponent>(components, true);
			for(size_t i = 0; i < components.size() ; i++)
			{
				BaseSceneComponentPtr comp = DYNAMIC_PTR_CAST<BaseSceneComponent>(components[i]);
				objs.push_back(comp->GetSceneObject());
			}
		}
		return objs;
	}


	void RecastNavigationMeshComponent::SetSelectedMeshes(const std::vector<SceneObjectRef> &value)
	{
		m_SelectedMeshes = value;
		//update bounds from mesh selection
		if(GetSceneObject())
		{
			m_MeshBounding = AABox();
			for(int i = 0;  i <  m_SelectedMeshes.size(); i++)
			{
				SceneObjectPtr obj = m_SelectedMeshes[i].GetRefObject();
				if(obj)
				{
					GeometryComponentPtr geom = obj->GetFirstComponentByClass<IGeometryComponent>();
					LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
					AABox box = geom->GetBoundingBox();

					if(lc)
					{
						Vec3 world_pos = lc->GetWorldPosition();
						Quaternion world_rot = lc->GetWorldRotation();
						Mat4 trans_mat;
						trans_mat.Identity();
						world_rot.ToRotationMatrix(trans_mat);
						trans_mat.SetTranslation(world_pos.x,world_pos.y,world_pos.z);
						box.Transform(trans_mat);
					}
					m_MeshBounding.Union(box);
				}

			}
		}
	}

	bool RecastNavigationMeshComponent::GetRawMeshData(RawNavMeshData &rnm_data)
	{
		if(m_SelectedMeshes.size()>0)
		{
			std::vector<MeshDataPtr> mesh_data_vec;
			for(int i = 0;  i <  m_SelectedMeshes.size(); i++)
			{
				{
					SceneObjectPtr obj = m_SelectedMeshes[i].GetRefObject();
					if(obj)
					{
						MeshDataPtr mesh_data = new MeshData;
						MeshComponentPtr mesh = obj->GetFirstComponentByClass<IMeshComponent>();

						mesh->GetMeshData(mesh_data);
						GeometryComponentPtr geom = obj->GetFirstComponentByClass<IGeometryComponent>();
						LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
						if(lc)
						{
							Vec3 world_pos = lc->GetWorldPosition();
							Vec3 scale = lc->GetScale();
							Quaternion world_rot = lc->GetWorldRotation();
							Mat4 trans_mat;
							trans_mat.Identity();
							//world_rot.ToRotationMatrix(trans_mat);
							//trans_mat.SetTranslation(world_pos.x,world_pos.y,world_pos.z);
							trans_mat.SetTransformation(world_pos,world_rot,scale);
							

							for(int j = 0 ; j < mesh_data->NumVertex; j++)
							{
								mesh_data->VertexVector[j] = trans_mat*mesh_data->VertexVector[j];
							}
						}
						mesh_data_vec.push_back(mesh_data);
					}
				}
			}
			int tot_verts = 0;
			int tot_faces = 0;
			for(int i = 0; i < mesh_data_vec.size() ; i++)
			{
				tot_verts += mesh_data_vec[i]->NumVertex;
				tot_faces  += mesh_data_vec[i]->NumFaces;
			}
			if(tot_verts > 0 && tot_faces > 0 && sizeof(Float) == 8) //double precision
			{
				//copy data to float
				float* verts = new float[tot_verts*3];
				int* tris = new int[tot_faces*3];
				float* trinorms = new float[tot_faces*3];

				float* bmin = new float[3];
				float* bmax = new float[3];

				int face_index = 0;
				int vert_index = 0;
				int norm_index  = 0;

				for(int i = 0; i < mesh_data_vec.size() ; i++)
				{
					int base_index = vert_index/3;
					for(int j = 0 ; j < mesh_data_vec[i]->NumVertex; j++)
					{
						verts[vert_index++] = mesh_data_vec[i]->VertexVector[j].x;
						verts[vert_index++] = mesh_data_vec[i]->VertexVector[j].y;
						verts[vert_index++] = mesh_data_vec[i]->VertexVector[j].z;
					}

					for(int j = 0 ; j < mesh_data_vec[i]->NumFaces*3; j++)
					{
						tris[face_index++] = mesh_data_vec[i]->FaceVector[j]+base_index;
					}

					for(int j = 0 ; j < mesh_data_vec[i]->NumFaces*3; j += 3)
					{
						Vec3 p1 = mesh_data_vec[i]->VertexVector[mesh_data_vec[i]->FaceVector[j]];
						Vec3 p2 = mesh_data_vec[i]->VertexVector[mesh_data_vec[i]->FaceVector[j+1]];
						Vec3 p3 = mesh_data_vec[i]->VertexVector[mesh_data_vec[i]->FaceVector[j+2]];;
						Vec3 v1 = p1 - p3;
						Vec3 v2 = p2 - p3;
						Vec3 norm = Math::Cross(v1,v2);
						norm.FastNormalize();

						trinorms[norm_index++]=norm.x;
						trinorms[norm_index++]=norm.y;
						trinorms[norm_index++]=norm.z;

					}
					delete[] mesh_data_vec[i]->FaceVector;
					delete[] mesh_data_vec[i]->VertexVector;
					delete mesh_data_vec[i];
					mesh_data_vec[i] = NULL;
				}
				bmin[0] = m_MeshBounding.m_Min.x;
				bmin[1] = m_MeshBounding.m_Min.y;
				bmin[2] = m_MeshBounding.m_Min.z;

				bmax[0] = m_MeshBounding.m_Max.x;
				bmax[1] = m_MeshBounding.m_Max.y;
				bmax[2] = m_MeshBounding.m_Max.z;

				rnm_data.Verts =  verts;
				rnm_data.NumVerts =  tot_verts;
				rnm_data.TriNorm= trinorms;
				rnm_data.NumTris = tot_faces;
				rnm_data.Tris = tris;
				rnm_data.BMin = bmin;
				rnm_data.BMax = bmax;
				return true;
			}
		}
		return false;
	}

	void RecastNavigationMeshComponent::UpdateNavMeshVis()
	{
		std::vector<MeshVertex> nav_tris =  GetVisualNavMesh();

		
		m_NavVisLineMesh->VertexVector.clear();
		m_NavVisLineMesh->IndexVector.clear();
		m_NavVisTriMesh->VertexVector.clear();
		m_NavVisTriMesh->IndexVector.clear();

		m_NavVisLineMesh->Material = "WhiteTransparentNoLighting";
		m_NavVisTriMesh->Material = "WhiteTransparentNoLighting";

		/*MeshVertex vertex;
		vertex.TexCoord.Set(0,0);
		vertex.Color.Set(1,1,1,1);*/

		m_NavVisLineMesh->Type = LINE_LIST;
		for(int i = 0; i < nav_tris.size(); i += 3)
		{
			MeshVertex v1 = nav_tris[i];
			MeshVertex v2 = nav_tris[i+1];
			MeshVertex v3 = nav_tris[i+2];
			v1.Color.Set(0,0,1,1);
			v2.Color.Set(0,0,1,1);
			v3.Color.Set(0,0,1,1);

			m_NavVisLineMesh->VertexVector.push_back(v1);
			m_NavVisLineMesh->VertexVector.push_back(v2);
			m_NavVisLineMesh->VertexVector.push_back(v2);
			m_NavVisLineMesh->VertexVector.push_back(v3);
			m_NavVisLineMesh->VertexVector.push_back(v3);
			m_NavVisLineMesh->VertexVector.push_back(v1);
		}

		//vertex.Color.Set(0,0.7,0,float(m_Transparency)/100.0f);
		m_NavVisTriMesh->Type = TRIANGLE_LIST;

		m_NavVisTriMesh->VertexVector = nav_tris;
		for(int i = 0; i < nav_tris.size(); i++)
		{
			m_NavVisTriMesh->IndexVector.push_back(i);
		}

		SetShowMeshLines(m_ShowMeshLines);
		SetShowMeshSolid(m_ShowMeshSolid);
	}

	void RecastNavigationMeshComponent::SetShowMeshLines(bool value)
	{
		m_ShowMeshLines = value;
		if(m_NavMesh)
		{
			SceneObjectPtr obj = GetSceneObject()->GetChildByID("LINE_MESH_DEBUG_OBJECT");
			if(obj)
			{
				if(m_ShowMeshLines)
				{
					MessagePtr mesh_message(new ManualMeshDataMessage(m_NavVisLineMesh,-1,0.1));
					obj->PostMessage(mesh_message);
					//MessagePtr material_message(new MaterialMessage(Vec4(1,0,0,1),Vec3(1,1,1)));
					//obj->PostMessage(material_message);
				}
				else
				{
					obj->PostMessage(MessagePtr(new ClearManualMeshMessage()));
				}
			}
		}
	}


	void RecastNavigationMeshComponent::SetShowMeshSolid(bool value)
	{
		m_ShowMeshSolid = value;
		if(m_NavMesh)
		{
			SceneObjectPtr obj = GetSceneObject()->GetChildByID("SOLID_MESH_DEBUG_OBJECT");
			if(obj)
			{
				if(m_ShowMeshSolid)
				{
					MessagePtr mesh_message(new ManualMeshDataMessage(m_NavVisTriMesh,-1,0.1));
					obj->PostMessage(mesh_message);
					MessagePtr material_message(new MaterialMessage(Vec4(0,0,1,float(m_Transparency)/100.0f),Vec3(1,1,1)));
					obj->PostMessage(material_message);
				}
				else
				{
					//Hide mesh
					obj->PostMessage(MessagePtr(new ClearManualMeshMessage()));
				}
			}
		}
	}


	


	// from recast tile example

	static const int NAVMESHSET_MAGIC = 'M'<<24 | 'S'<<16 | 'E'<<8 | 'T'; //'MSET';
	static const int NAVMESHSET_VERSION = 1;

	struct NavMeshSetHeader
	{
		int magic;
		int version;
		int numTiles;
		dtNavMeshParams params;
	};

	struct NavMeshTileHeader
	{
		dtTileRef tileRef;
		int dataSize;
	};

	void RecastNavigationMeshComponent::SaveAllTiles(const char* path, const dtNavMesh* mesh)
	{
		if (!mesh) return;

		FILE* fp = fopen(path, "wb");
		if (!fp)
			return;

		// Store header.
		NavMeshSetHeader header;
		header.magic = NAVMESHSET_MAGIC;
		header.version = NAVMESHSET_VERSION;
		header.numTiles = 0;
		for (int i = 0; i < mesh->getMaxTiles(); ++i)
		{
			const dtMeshTile* tile = mesh->getTile(i);
			if (!tile || !tile->header || !tile->dataSize) continue;
			header.numTiles++;
		}
		memcpy(&header.params, mesh->getParams(), sizeof(dtNavMeshParams));
		fwrite(&header, sizeof(NavMeshSetHeader), 1, fp);

		// Store tiles.
		for (int i = 0; i < mesh->getMaxTiles(); ++i)
		{
			const dtMeshTile* tile = mesh->getTile(i);
			if (!tile || !tile->header || !tile->dataSize) continue;

			NavMeshTileHeader tileHeader;
			tileHeader.tileRef = mesh->getTileRef(tile);
			tileHeader.dataSize = tile->dataSize;
			fwrite(&tileHeader, sizeof(tileHeader), 1, fp);

			fwrite(tile->data, tile->dataSize, 1, fp);
		}
		fclose(fp);
	}


	dtNavMesh* RecastNavigationMeshComponent::LoadAll(const char* path)
	{
		FILE* fp = fopen(path, "rb");
		if (!fp) return 0;

		// Read header.
		NavMeshSetHeader header;
		fread(&header, sizeof(NavMeshSetHeader), 1, fp);
		if (header.magic != NAVMESHSET_MAGIC)
		{
			fclose(fp);
			return 0;
		}
		if (header.version != NAVMESHSET_VERSION)
		{
			fclose(fp);
			return 0;
		}

		dtNavMesh* mesh = dtAllocNavMesh();
		if (!mesh)
		{
			fclose(fp);
			return 0;
		}
		dtStatus status = mesh->init(&header.params);
		if (dtStatusFailed(status))
		{
			fclose(fp);
			return 0;
		}

		// Read tiles.
		for (int i = 0; i < header.numTiles; ++i)
		{
			NavMeshTileHeader tileHeader;
			fread(&tileHeader, sizeof(tileHeader), 1, fp);
			if (!tileHeader.tileRef || !tileHeader.dataSize)
				break;

			unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
			if (!data) break;
			memset(data, 0, tileHeader.dataSize);
			fread(data, tileHeader.dataSize, 1, fp);

			mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
		}

		fclose(fp);

		return mesh;
	}


	bool RecastNavigationMeshComponent::IsPointInside(const Vec3 &point) const
	{
		bool ret = false;
		if(m_NavMesh)
		{
			float p1[3];
			float p2[3];
			float ext[3];
			p1[0] =point.x; p1[1] =point.y; p1[2] =point.z;
			ext[0] = 0.1; ext[1] = 0.1; ext[2] = 0.1;

			dtPolyRef ref;
			dtQueryFilter filter;
			dtStatus status = m_NavQuery->findNearestPoly(p1, ext, &filter, &ref, p2);
			if(ref)
			{
				ret = true;
			}
		}
		return ret;
	}


	static float frand()
	{
		return (float)rand()/(float)RAND_MAX;
	}


	Vec3 RecastNavigationMeshComponent::GetRandomPoint() const
	{
		Vec3 ret(0,0,0);
		if(m_NavMesh)
		{
			float pt[3];
			dtPolyRef ref;
			dtQueryFilter filter;
			dtStatus status = m_NavQuery->findRandomPoint(&filter, frand, &ref, pt);
			if (dtStatusSucceed(status))
			{
				ret.Set(pt[0],pt[1],pt[2]);
			}
		}
		return ret;
	}


	std::vector<MeshVertex> RecastNavigationMeshComponent::GetVisualNavMesh()
	{
		std::vector<MeshVertex> tris;
		if(!m_NavMesh)
			return tris;

		for (int i = 0; i < m_NavMesh->getMaxTiles(); ++i)
		{
			const dtNavMesh* mesh =  m_NavMesh;
			const dtMeshTile* tile = mesh->getTile(i);
			if (!tile->header) continue;

			for (int j = 0; j < tile->header->polyCount; ++j)
			{
				const dtPoly* p = &tile->polys[j];
				if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)	// Skip off-mesh links.
					continue;


				Vec4 color(0.4,0.4,1,float(m_Transparency)/100.0f);
				if (p->flags & SAMPLE_POLYFLAGS_DISABLED)
				{
					color.Set(1,0,0,float(m_Transparency)/100.0f);
				}
				else if (p->flags & SAMPLE_POLYFLAGS_DOOR)
				{
					color.Set(0,1,0,float(m_Transparency)/100.0f);
				}

				const dtPolyDetail* pd = &tile->detailMeshes[j];
				//if (p->getArea() == 0) // Treat zero area type as default.
				for (int k = 0; k < pd->triCount; ++k)
				{
					const unsigned char* t = &tile->detailTris[(pd->triBase+k)*4];
					for (int l = 0; l < 3; ++l)
					{
						
						float* pos;
						if (t[l] < p->vertCount)
						{
							pos = &tile->verts[p->verts[t[l]]*3];
						}
						else
						{
							pos = &tile->detailVerts[(pd->vertBase+t[l]-p->vertCount)*3];
						}
						MeshVertex vertex;
						vertex.Pos  = Vec3(pos[0],pos[1],pos[2]);
						vertex.Color = color;
						vertex.TexCoord.Set(0,0);
						vertex.Normal = Vec3(0,1,0);
						tris.push_back(vertex);
					}
				}
			}
		}
		return tris;
	}
}
