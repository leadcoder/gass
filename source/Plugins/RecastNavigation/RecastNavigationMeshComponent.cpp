/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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
#include "RecastOffmeshMeshConnectionComponent.h"
#include "RecastConvexVolumeComponent.h"
#include "RecastIncludes.h"
#include "Core/Serialize/tinyxml2.h"
#include "Core/Utils/GASSFileUtils.h"
#include "Core/Math/GASSMath.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Sim/GASSPhysicsMesh.h"
#include <cfloat> //FLT_MIN/MAX

namespace GASS
{
	RecastNavigationMeshComponent::RecastNavigationMeshComponent() :m_NavVisTriMesh(new GraphicsMesh()),
		m_NavVisLineMesh(new GraphicsMesh()),
		m_NavMesh(NULL),
		m_ShowMeshLines(false),
		m_ShowMeshSolid(false),
		m_Transparency(30),
		m_Visible(true),
		m_Initialized(false),
		m_AutoCollectMeshes(true),
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
		m_GridSize(0),
		m_VertsPerPoly ( 6.0f),
		m_DetailSampleDist ( 6.0f),
		m_DetailSampleMaxError ( 1.0f),
		m_TileSize ( 64),
		m_NavQuery ( dtAllocNavMeshQuery()),
		m_Geom ( new InputGeom()),
		m_Ctx( new rcContext(true)),
		m_UseBoudingBox(true),
		m_MonotonePartitioning ( false),
		m_LocalOrigin(0,0,0)
	{
		m_MeshBounding = AABox();
	}

	RecastNavigationMeshComponent::~RecastNavigationMeshComponent()
	{
		delete m_Geom;
		dtFreeNavMeshQuery(m_NavQuery);
		dtFreeNavMesh(m_NavMesh);
	}

	void RecastNavigationMeshComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<RecastNavigationMeshComponent>();

		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("RecastNavigationMeshComponent", OF_VISIBLE)));
		RegisterMember("AutoCollectMeshes", &RecastNavigationMeshComponent::m_AutoCollectMeshes, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("Build", &RecastNavigationMeshComponent::GetBuild, &RecastNavigationMeshComponent::SetBuild,PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("CellSize", &RecastNavigationMeshComponent::GetCellSize, &RecastNavigationMeshComponent::SetCellSize, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("CellHeight", &RecastNavigationMeshComponent::GetCellHeight, &RecastNavigationMeshComponent::SetCellHeight, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("AgentHeight", &RecastNavigationMeshComponent::GetAgentHeight, &RecastNavigationMeshComponent::SetAgentHeight, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("AgentRadius", &RecastNavigationMeshComponent::GetAgentRadius, &RecastNavigationMeshComponent::SetAgentRadius, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("AgentMaxClimb", &RecastNavigationMeshComponent::GetAgentMaxClimb, &RecastNavigationMeshComponent::SetAgentMaxClimb, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("AgentMaxSlope", &RecastNavigationMeshComponent::GetAgentMaxSlope, &RecastNavigationMeshComponent::SetAgentMaxSlope, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("RegionMinSize", &RecastNavigationMeshComponent::GetRegionMinSize, &RecastNavigationMeshComponent::SetRegionMinSize, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("RegionMergeSize", &RecastNavigationMeshComponent::GetRegionMergeSize, &RecastNavigationMeshComponent::SetRegionMergeSize, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("EdgeMaxLen", &RecastNavigationMeshComponent::GetEdgeMaxLen, &RecastNavigationMeshComponent::SetEdgeMaxLen, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("EdgeMaxError", &RecastNavigationMeshComponent::GetEdgeMaxError, &RecastNavigationMeshComponent::SetEdgeMaxError, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("VertsPerPoly", &RecastNavigationMeshComponent::GetVertsPerPoly, &RecastNavigationMeshComponent::SetVertsPerPoly, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("DetailSampleDist", &RecastNavigationMeshComponent::GetDetailSampleDist, &RecastNavigationMeshComponent::SetDetailSampleDist, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("DetailSampleMaxError", &RecastNavigationMeshComponent::GetDetailSampleMaxError, &RecastNavigationMeshComponent::SetDetailSampleMaxError, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("GridSize", &RecastNavigationMeshComponent::GetGridSize, &RecastNavigationMeshComponent::SetGridSize, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("Visible", &RecastNavigationMeshComponent::GetVisible, &RecastNavigationMeshComponent::SetVisible, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("ShowMeshLines", &RecastNavigationMeshComponent::GetShowMeshLines, &RecastNavigationMeshComponent::SetShowMeshLines, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("ShowMeshSolid", &RecastNavigationMeshComponent::GetShowMeshSolid, &RecastNavigationMeshComponent::SetShowMeshSolid, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("MeshBoundingMin", &RecastNavigationMeshComponent::GetMeshBoundingMin, &RecastNavigationMeshComponent::SetMeshBoundingMin, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("MeshBoundingMax", &RecastNavigationMeshComponent::GetMeshBoundingMax, &RecastNavigationMeshComponent::SetMeshBoundingMax, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("TileSize", &RecastNavigationMeshComponent::GetTileSize, &RecastNavigationMeshComponent::SetTileSize, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("Transparency", &RecastNavigationMeshComponent::GetTransparency, &RecastNavigationMeshComponent::SetTransparency, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("BoundingBoxFromShape", &RecastNavigationMeshComponent::GetBoundingBoxFromShape, &RecastNavigationMeshComponent::SetBoundingBoxFromShape, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("ImportMesh", &RecastNavigationMeshComponent::GetImportMesh, &RecastNavigationMeshComponent::SetImportMesh, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("ExportMesh", &RecastNavigationMeshComponent::GetExportMesh, &RecastNavigationMeshComponent::SetExportMesh, PF_VISIBLE | PF_EDITABLE);
		RegisterGetSet("Transparency", &RecastNavigationMeshComponent::GetTransparency, &RecastNavigationMeshComponent::SetTransparency, PF_VISIBLE | PF_EDITABLE);

		RegisterMember("LocalOrigin", &RecastNavigationMeshComponent::m_LocalOrigin, PF_VISIBLE);
		auto mesh_prop = RegisterGetSet("MeshSelection", &RecastNavigationMeshComponent::GetMeshSelection, &RecastNavigationMeshComponent::SetMeshSelection, PF_VISIBLE | PF_EDITABLE, "");
		mesh_prop->SetObjectOptionsFunction(&RecastNavigationMeshComponent::GetMeshSelectionEnum);
		RegisterMember("UseBoudingBox", &RecastNavigationMeshComponent::m_UseBoudingBox, PF_VISIBLE | PF_EDITABLE);
	}

	void RecastNavigationMeshComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RecastNavigationMeshComponent::OnEditPosition,EditPositionMessage,1));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS( RecastNavigationMeshComponent::OnSceneObjectCreated,PostSceneObjectInitializedEvent,0));

		const std::string filename = m_NavMeshFilePath + GetSceneObject()->GetName() + ".bin";
		if(FILE*fp = fopen(filename.c_str(),"rt"))
		{
			fclose(fp);
			m_NavMesh = LoadAll(filename.c_str());
			m_NavQuery->init(m_NavMesh, 2048);
		}

		std::vector<SceneObjectRef>::iterator iter = m_SelectedMeshes.begin();
		//remove invalid pointers!
		while(iter!=  m_SelectedMeshes.end())
		{
			if(!(*iter).IsValid())
				iter = m_SelectedMeshes.erase(iter);
			else
				iter++;
		}
		m_Initialized = true;
	}

	void RecastNavigationMeshComponent::OnSceneObjectCreated(PostSceneObjectInitializedEventPtr message)
	{
		if(message->GetSceneObject() != GetSceneObject())
			return;

		if(m_NavMesh)
			UpdateNavMeshVis();
		//initialize visibility
		SetVisible(m_Visible);
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
			GASSToRecast(pos,point);
			//point[0] =pos.x; point[1] =pos.y; point[2] =pos.z;
			ext[0] = 0.1f; ext[1] = 0.1f; ext[2] = 0.1f;
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
/*	static int convexhull(const float* pts, int npts, int* out)
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
	}*/



	void RecastNavigationMeshComponent::UpdateConvexVolumes()
	{
		for(int i = 0; i < m_Geom->getConvexVolumeCount();i++)
		{
			m_Geom->deleteConvexVolume(i);
		}
		ComponentContainer::ComponentVector components;
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<RecastConvexVolumeComponent>(components,true);

		for(size_t i = 0; i < components.size(); i++)
		{
			RecastConvexVolumeComponentPtr comp = GASS_DYNAMIC_PTR_CAST<RecastConvexVolumeComponent>(components[i]);
			GeometryComponentPtr geom = comp->GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
			if(geom)
			{
				LandCoverType area = comp->GetLandCoverType().GetValue();

				AABox box = geom->GetBoundingBox();
				std::vector<Vec3> corners = box.GetCorners();

				LocationComponentPtr loc_comp = comp->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
				assert(loc_comp);
				Mat4 trans(loc_comp->GetWorldRotation(), loc_comp->GetWorldPosition());

				std::vector<float> points;

				float minh,maxh;

				minh = FLT_MAX;
				maxh = -FLT_MAX;

				float vertex[3];
				for(size_t j = 0; j < 8; j++)
				{
					Vec3 pos = trans*corners[j];
					GASSToRecast(pos,vertex);
					points.push_back(vertex[0]);
					points.push_back(vertex[1]);
					points.push_back(vertex[2]);
					if(vertex[1] < minh)
						minh = vertex[1];
					if(vertex[1] > maxh)
						maxh = vertex[1];
				}

				m_Geom->addConvexVolume(&points[0], 4, minh, maxh, static_cast<unsigned char>(area));

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
		for(int i = 0; i < m_Geom->getOffMeshConnectionCount();i++)
		{
			m_Geom->deleteOffMeshConnection(i);
		}

		const unsigned char area = LAND_COVER_JUMP;
		const unsigned short flags = SAMPLE_POLYFLAGS_JUMP;
		float p1[3];
		float p2[3];
		ComponentContainer::ComponentVector components;
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<RecastOffmeshMeshConnectionComponent>(components,true);
		for(size_t i = 0; i < components.size(); i++)
		{
			RecastOffmeshMeshConnectionComponentPtr comp = GASS_DYNAMIC_PTR_CAST<RecastOffmeshMeshConnectionComponent>(components[i]);
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
			GASSToRecast(pos1,p1);
			GASSToRecast(pos2,p2);
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
		rnm_data.Area = NULL;
		rnm_data.NumTris = 0;
		rnm_data.NumVerts = 0;
		GetRawMeshData(rnm_data);

		if(rnm_data.NumVerts == 0)
			return false;
		m_Geom->createMesh(rnm_data.Verts, rnm_data.TriNorm, rnm_data.Area,rnm_data.Tris, rnm_data.NumVerts,rnm_data.NumTris);
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
		//float totalBuildTimeMs = m_Ctx->getAccumulatedTime(RC_TIMER_TEMP)/1000.0f;
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

		//tessellate mesh!

		static unsigned char magic_num = 128;
		if (m_GridSize > 0)
		{
			int checker_size = m_GridSize;
			if (checker_size > 0)
			{
				for (int y = 0; y < chf->height; ++y) {
					for (int x = 0; x < chf->width; ++x) {
						const rcCompactCell& c = chf->cells[x + y*chf->width];
						//for (int i = (int)c.index, ni = (int)(c.index+c.count); i < ni; ++i) {
						for (int i = (int)c.index, ni = (int)(c.index + c.count); i < ni; ++i) {
							if (chf->areas[i] != RC_NULL_AREA && (((x / checker_size) ^ (y / checker_size)) & 1))
								chf->areas[i] = chf->areas[i] + magic_num;
							//chf->areas[i] = checker(x,y,checker_size);
						}
					}
				}
			}
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

		//restore material ids

		if (m_GridSize > 0)
		{
			for (int i = 0; i < pmesh->npolys; ++i)
			{
				if (pmesh->areas[i] > magic_num - 1)
					pmesh->areas[i] = pmesh->areas[i] - magic_num;
			}

			for (int y = 0; y < chf->height; ++y) {
				for (int x = 0; x < chf->width; ++x) {
					const rcCompactCell& c = chf->cells[x + y*chf->width];
					for (int i = (int)c.index, ni = (int)(c.index + c.count); i < ni; ++i) {
						if (chf->areas[i] > magic_num - 1)
							chf->areas[i] = chf->areas[i] - magic_num;
					}
				}
			}
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
		//int tileMemUsage = navDataSize/1024.0f;

		m_Ctx->stopTimer(RC_TIMER_TOTAL);
		// Show performance stats.
		//duLogBuildTimes(*m_Ctx, m_Ctx->getAccumulatedTime(RC_TIMER_TOTAL));
		m_Ctx->log(RC_LOG_PROGRESS, ">> Polymesh: %d vertices  %d polygons", pmesh->nverts, pmesh->npolys);

		//Free mesh
		rcFreePolyMesh(pmesh);
		rcFreePolyMeshDetail(dmesh);


//		float tileBuildTime = m_Ctx->getAccumulatedTime(RC_TIMER_TOTAL)/1000.0f;
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
			GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(value)));
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
		return m_MeshBounding.Min;
	}

	void RecastNavigationMeshComponent::SetMeshBoundingMin(const Vec3 &min)
	{
		m_MeshBounding.Min = min;
	}

	Vec3 RecastNavigationMeshComponent::GetMeshBoundingMax() const
	{
		return m_MeshBounding.Max;
	}

	void RecastNavigationMeshComponent::SetMeshBoundingMax(const Vec3 &max)
	{
		m_MeshBounding.Max = max;
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

	void RecastNavigationMeshComponent::SetBuild(const bool /*value*/)
	{
		if(m_Initialized)
		{
			GenerateTiles();
			UpdateNavMeshVis();
		}
	}

	FilePath RecastNavigationMeshComponent::_GetFilePath() const
	{
		ScenePtr  scene = GetSceneObject()->GetScene();
		std::string scene_path = scene->GetSceneFolder().GetFullPath();
		std::string filename = GetSceneObject()->GetName() + ".bin";
		FilePath full_path(scene_path + "/" + filename);
		return full_path;
	}

	void RecastNavigationMeshComponent::SaveXML(tinyxml2::XMLElement *obj_elem)
	{
		//temp fix because obj_elem->GetDocument()->GetFileName() dont work for save after port to tinyxml2
		//m_NavMeshFilePath = obj_elem->GetDocument()->GetFileName();
		//m_NavMeshFilePath = FileUtils::RemoveFilename(m_NavMeshFilePath);
		if(m_NavMesh)
		{
			//std::string filename = m_NavMeshFilePath + GetSceneObject()->GetName() + ".bin";
			//SaveAllTiles(filename.c_str(),m_NavMesh);
			SaveAllTiles(_GetFilePath().GetFullPath().c_str(),m_NavMesh);
		}
		Component::SaveXML(obj_elem);
	}

	void RecastNavigationMeshComponent::LoadXML(tinyxml2::XMLElement *obj_elem)
	{
		tinyxml2::XMLDocument *rootXMLDoc = obj_elem->GetDocument();
		m_NavMeshFilePath = rootXMLDoc->GetFileName();
		m_NavMeshFilePath = FileUtils::RemoveFilename(m_NavMeshFilePath);
		Component::LoadXML(obj_elem);
	}

	void RecastNavigationMeshComponent::SetBoundingBoxFromShape(const std::string &value)
	{
		m_BBShape = value;

		if(GetSceneObject()) //initialized?
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
						Mat4 trans_mat(world_rot, world_pos);
						//world_rot.ToRotationMatrix(trans_mat);
						//trans_mat.SetTranslation(world_pos);
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

	std::vector<SceneObjectRef> RecastNavigationMeshComponent::GetMeshSelectionEnum()
	{
		std::vector<SceneObjectRef> objs;
		if(GetSceneObject())
		{
			ComponentContainer::ComponentVector components;
			GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<IMeshComponent>(components, true);
			for(size_t i = 0; i < components.size() ; i++)
			{
				BaseSceneComponentPtr comp = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(components[i]);
				objs.push_back(comp->GetSceneObject());
			}
		}
		return objs;
	}

	void RecastNavigationMeshComponent::SetMeshSelection(const std::vector<SceneObjectRef> &value)
	{
		m_SelectedMeshes = value;
		//update bounds from mesh selection
		/*if(GetSceneObject())
		{
			//m_MeshBounding = AABox();
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
					//m_MeshBounding.Union(box);
				}
			}
		}*/
	}



	bool RecastNavigationMeshComponent::GetRawMeshData(RawNavMeshData &rnm_data)
	{
		AABox bbox;

		if(m_AutoCollectMeshes)
		{
			std::vector<SceneObjectPtr> objs;
			m_SelectedMeshes.clear();
			//m_MeshBounding = AABox();
			if(GetSceneObject())
			{
				ComponentContainer::ComponentVector components;
				GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<IMeshComponent>(components, true);
				for(size_t i = 0; i < components.size() ; i++)
				{
					BaseSceneComponentPtr comp = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(components[i]);
					m_SelectedMeshes.push_back(comp->GetSceneObject());
				}
			}
		}

		if(m_SelectedMeshes.size()>0)
		{
			std::vector<PhysicsMeshPtr> mesh_data_vec;
			for(size_t i = 0;  i <  m_SelectedMeshes.size(); i++)
			{
				SceneObjectPtr obj = m_SelectedMeshes[i].GetRefObject();
				if(obj)
				{
					MeshComponentPtr mesh = obj->GetFirstComponentByClass<IMeshComponent>();
					GeometryComponentPtr geom = obj->GetFirstComponentByClass<IGeometryComponent>();

					if(geom && geom->GetGeometryFlags() & (GEOMETRY_FLAG_GROUND | GEOMETRY_FLAG_STATIC_OBJECT))
					{
						//AABox box = geom->GetBoundingBox();
						GraphicsMesh gfx_mesh_data = mesh->GetMeshData();
						PhysicsMeshPtr physics_mesh(new PhysicsMesh(gfx_mesh_data));
						LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
						if(lc)
						{
							Mat4 trans_mat = Mat4::CreateTransformationSRT(lc->GetScale(),lc->GetWorldRotation(), lc->GetWorldPosition());

							for(size_t j = 0 ; j < physics_mesh->PositionVector.size(); j++)
							{
								Vec3 pos = trans_mat*physics_mesh->PositionVector[j];
								physics_mesh->PositionVector[j] = pos;
								bbox.Union(pos);
							}
						}
						mesh_data_vec.push_back(physics_mesh);
					}
				}
			}

			if(m_UseBoudingBox)
			{
				bbox = m_MeshBounding;
			}

			unsigned int tot_verts = 0;
			unsigned int tot_faces = 0;
			for(unsigned int i = 0; i < mesh_data_vec.size() ; i++)
			{
				tot_verts += static_cast<unsigned int>(mesh_data_vec[i]->PositionVector.size());
				tot_faces  += static_cast<unsigned int>(mesh_data_vec[i]->IndexVector.size())/3;
			}

			m_LocalOrigin.Set(0,0,0);
			double mult = 1.0 / static_cast<double>(tot_verts);
			for(size_t i = 0; i < mesh_data_vec.size() ; i++)
			{
				for(size_t j = 0 ; j < mesh_data_vec[i]->PositionVector.size(); j++)
				{
					m_LocalOrigin += (mesh_data_vec[i]->PositionVector[j]*mult);
				}
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

				for(size_t i = 0; i < mesh_data_vec.size() ; i++)
				{
					int base_index = vert_index/3;
					for(size_t j = 0 ; j < mesh_data_vec[i]->PositionVector.size(); j++)
					{
						GASSToRecast(mesh_data_vec[i]->PositionVector[j],&verts[vert_index]);
						vert_index += 3;
					}

					for(size_t j = 0 ; j < mesh_data_vec[i]->IndexVector.size(); j++)
					{
						tris[face_index++] = mesh_data_vec[i]->IndexVector[j]+base_index;
					}

					for(size_t j = 0 ; j < mesh_data_vec[i]->IndexVector.size(); j += 3)
					{
						Vec3 p1 = mesh_data_vec[i]->PositionVector[mesh_data_vec[i]->IndexVector[j]];
						Vec3 p2 = mesh_data_vec[i]->PositionVector[mesh_data_vec[i]->IndexVector[j+1]];
						Vec3 p3 = mesh_data_vec[i]->PositionVector[mesh_data_vec[i]->IndexVector[j+2]];;
						Vec3 v1 = p1 - p3;
						Vec3 v2 = p2 - p3;
						Vec3 norm = Vec3::Cross(v1,v2);
						norm.Normalize();

						trinorms[norm_index++] = static_cast<float>(norm.x);
						trinorms[norm_index++] = static_cast<float>(norm.y);
						trinorms[norm_index++] = static_cast<float>(norm.z);
					}

				}
				GASSToRecast(bbox.Min,bmin);
				GASSToRecast(bbox.Max,bmax);

				rnm_data.Verts =  verts;
				rnm_data.NumVerts =  tot_verts;
				rnm_data.TriNorm= trinorms;
				rnm_data.NumTris = tot_faces;
				rnm_data.Tris = tris;
				rnm_data.BMin = bmin;
				rnm_data.BMax = bmax;

				rnm_data.Area = new int[rnm_data.NumTris];
				for(int i = 0; i< rnm_data.NumTris; i++)
				{
					rnm_data.Area[i] = 0;
				}

				return true;
			}
		}
		return false;
	}

	void RecastNavigationMeshComponent::UpdateNavMeshVis()
	{
		std::vector<Vec3> nav_tris =  GetVisualNavMesh();

		m_NavVisLineMesh->SubMeshVector.clear();
		m_NavVisTriMesh->SubMeshVector.clear();

		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		m_NavVisLineMesh->SubMeshVector.push_back(sub_mesh_data);
		sub_mesh_data->Type = LINE_LIST;
		sub_mesh_data->MaterialName = "WhiteNoLighting";
		ColorRGBA color(0.4,0.4,1,float(m_Transparency)/100.0f);

		for(size_t i = 0; i < nav_tris.size(); i += 3)
		{
			Vec3 v1 = nav_tris[i];
			Vec3 v2 = nav_tris[i+1];
			Vec3 v3 = nav_tris[i+2];

			sub_mesh_data->PositionVector.push_back(v1);
			sub_mesh_data->ColorVector.push_back(color);

			sub_mesh_data->PositionVector.push_back(v2);
			sub_mesh_data->ColorVector.push_back(color);

			sub_mesh_data->PositionVector.push_back(v2);
			sub_mesh_data->ColorVector.push_back(color);

			sub_mesh_data->PositionVector.push_back(v3);
			sub_mesh_data->ColorVector.push_back(color);

			sub_mesh_data->PositionVector.push_back(v3);
			sub_mesh_data->ColorVector.push_back(color);

			sub_mesh_data->PositionVector.push_back(v1);
			sub_mesh_data->ColorVector.push_back(color);
		}

		sub_mesh_data = GraphicsSubMeshPtr(new GraphicsSubMesh());
		m_NavVisTriMesh->SubMeshVector.push_back(sub_mesh_data);
		sub_mesh_data->Type = TRIANGLE_LIST;
		sub_mesh_data->MaterialName = "WhiteNoLighting";

		for(size_t i = 0; i < nav_tris.size(); i++)
		{
			sub_mesh_data->PositionVector.push_back(nav_tris[i]);
			sub_mesh_data->ColorVector.push_back(color);
		}

		for(size_t i = 0; i < nav_tris.size(); i++)
		{
			sub_mesh_data->IndexVector.push_back(static_cast<int>(i));
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
					obj->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(m_NavVisLineMesh)));
					obj->PostRequest(PositionRequestPtr(new PositionRequest(m_LocalOrigin)));
				}
				else
				{
					obj->PostRequest(ClearManualMeshRequestPtr(new ClearManualMeshRequest()));
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
					obj->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(m_NavVisTriMesh)));
					obj->PostRequest(PositionRequestPtr(new PositionRequest(m_LocalOrigin)));
				}
				else
				{
					//Hide mesh
					obj->PostRequest(ClearManualMeshRequestPtr(new ClearManualMeshRequest()));
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
		GASS_MUTEX_LOCK(m_Mutex);
		bool ret = false;
		if(m_NavMesh)
		{

			float p1[3];
			float p2[3];
			float ext[3];
			GASSToRecast(point,p1);
			ext[0] = 0.1f; ext[1] = 0.1f; ext[2] = 0.1f;

			dtPolyRef ref;
			dtQueryFilter filter;
			/*dtStatus status = */m_NavQuery->findNearestPoly(p1, ext, &filter, &ref, p2);
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
		GASS_MUTEX_LOCK(m_Mutex);
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


	bool RecastNavigationMeshComponent::GetClosestPointOnMeshForPlatform(const PlatformType platform_type, const GASS::Vec2 &in_pos, const float search_radius, GASS::Vec3 &out_pos) const
	{
		bool ret = false;
		if (m_NavMesh)
		{
			GASS::Vec3 in_pos_v3(in_pos.x, 0, in_pos.y);
			float rescast_pos[3];
			GASSToRecast(in_pos_v3, rescast_pos);

			dtQueryFilter filter;
			dtPolyRef poly_ref = 0;
			dtStatus ret = 0;
			float polyPickExt[3];
			polyPickExt[0] = search_radius;
			polyPickExt[1] = FLT_MAX / 3.0f; //should be large enough, is later clamped to tile bounding box
			polyPickExt[2] = search_radius;
			ret = m_NavQuery->findNearestPoly(rescast_pos, polyPickExt, &filter, &poly_ref, 0);
			if (dtStatusFailed(ret))
				return false;
		}
		return ret;
	}

	bool RecastNavigationMeshComponent::GetRandomPointInCircle(const Vec3 &circle_center, const float radius, Vec3 &point) const
	{
		if(!m_NavMesh)
			return false;
		float ext[3];
		dtPolyRef start_ref;
		float c_pos[3];

		ext[0] = 1;
		ext[1] = 1;
		ext[2] = 1;

		GASSToRecast(circle_center,c_pos);

		float q_pos[3];
		dtQueryFilter filter;

		m_NavQuery->findNearestPoly(c_pos, ext, &filter, &start_ref, q_pos);
		if(start_ref)
		{
			dtStatus status = DT_FAILURE;
			dtPolyRef end_ref;
			float end_pos[3];
			Float dist =  radius+1;
			int tries = 0;
			while(dist > radius && tries < 200)
			{
				tries++;
				status = m_NavQuery->findRandomPointAroundCircle(start_ref, q_pos, radius, &filter, frand, &end_ref, end_pos);
				if (dtStatusSucceed(status))
				{
					RecastToGASS(end_pos,point);
					dist = (circle_center - point).Length();
				}
			}
			return true;
		}
		return false;
	}


	std::vector<Vec3> RecastNavigationMeshComponent::GetVisualNavMesh()
	{
		std::vector<Vec3> tris;
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

				/*if (p->flags & SAMPLE_POLYFLAGS_DISABLED)
				{
				color.Set(1,0,0,float(m_Transparency)/100.0f);
				}
				else if (p->flags & SAMPLE_POLYFLAGS_DOOR)
				{
				color.Set(0,1,0,float(m_Transparency)/100.0f);
				}*/

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

						Vec3 gpos;
						RecastToGASS(pos,gpos);
						//return local navmesh positions,  we transform navigation mesh by scene node instead
						gpos -= m_LocalOrigin;
						tris.push_back(gpos);
					}
				}
			}
		}
		return tris;
	}


	void RecastNavigationMeshComponent::GASSToRecast(const GASS::Vec3 &in_pos, float* out_pos) const
	{
		out_pos[0] = static_cast<float>(in_pos.x - m_LocalOrigin.x);
		out_pos[1] = static_cast<float>(in_pos.y - m_LocalOrigin.y);
		out_pos[2] = static_cast<float>(in_pos.z - m_LocalOrigin.z);
	}

	void RecastNavigationMeshComponent::RecastToGASS(float* in_pos,GASS::Vec3 &out_pos) const
	{
		out_pos.x = in_pos[0] + m_LocalOrigin.x;
		out_pos.y = in_pos[1] + m_LocalOrigin.y;
		out_pos.z = in_pos[2] + m_LocalOrigin.z;
	}

	bool RecastNavigationMeshComponent::GetShortestPathForPlatform(const PlatformType platform_type, const Vec3 &from, const Vec3 &to, NavigationPath &path) const
	{
		return GetShortestPath(from, to, path);
	}

	bool RecastNavigationMeshComponent::GetShortestPath(const Vec3 &from, const Vec3 &to, NavigationPath &path) const
	{
		GASS_MUTEX_LOCK(m_Mutex);
		if(!m_NavMesh)
			GASS_EXCEPT(GASS::Exception::ERR_ITEM_NOT_FOUND, "m_NavMesh not initialized","RecastNavigationMeshComponent::GetShortestPath");

		float rescast_from_pos[3];
		float rescast_to_pos[3];
		GASSToRecast(from,rescast_from_pos);
		GASSToRecast(to,rescast_to_pos);
		float polyPickExt[3];
		polyPickExt[0] = 4; polyPickExt[1] = 4; polyPickExt[2] = 4;


		dtQueryFilter filter;
		for(int i = 0; i < DT_MAX_AREAS; i++)
			filter.setAreaCost(i,1.0f);


		dtPolyRef start_ref = 0;
		dtPolyRef end_ref  = 0;
		dtStatus ret  =0;
		ret = m_NavQuery->findNearestPoly(rescast_from_pos, polyPickExt, &filter, &start_ref, 0);
		if(dtStatusFailed(ret)) //cast exception
			GASS_EXCEPT(GASS::Exception::ERR_ITEM_NOT_FOUND, "Failed to find start polygon","RecastNavigationMeshComponent::GetShortestPath");
		if(start_ref)
		{
			ret = m_NavQuery->findNearestPoly(rescast_to_pos, polyPickExt, &filter, &end_ref, 0);
			if(dtStatusFailed(ret)) //cast exception
				GASS_EXCEPT(GASS::Exception::ERR_ITEM_NOT_FOUND, "Failed to find end polygon","RecastNavigationMeshComponent::GetShortestPath");
			if(end_ref)
			{
				dtPolyRef polys[MAX_POLYS_IN_PATH];
				int num_polys = 0;
				ret = m_NavQuery->findPath(start_ref , end_ref , rescast_from_pos, rescast_to_pos, &filter, polys, &num_polys , MAX_POLYS_IN_PATH);
				if(dtStatusFailed(ret)) //cast exception
					GASS_EXCEPT(GASS::Exception::ERR_ITEM_NOT_FOUND, "Failed to findPath","RecastNavigationMeshComponent::GetShortestPath");
				if(num_polys)
				{
					// In case of partial path, make sure the end point is clamped to the last polygon.
					float epos[3];
					dtVcopy(epos, rescast_to_pos);
					if (polys[num_polys-1] != end_ref)
						m_NavQuery->closestPointOnPoly(polys[num_polys-1], rescast_to_pos, epos);

					float straight_path[MAX_POLYS_IN_PATH*3];
					unsigned char straight_path_flags[MAX_POLYS_IN_PATH];
					dtPolyRef straight_polys[MAX_POLYS_IN_PATH];
					int num_straight_polys = 0;

					ret = m_NavQuery->findStraightPath(rescast_from_pos, epos, polys, num_polys, straight_path, straight_path_flags, straight_polys, &num_straight_polys, MAX_POLYS_IN_PATH);//, straight_path_options);
					if(dtStatusFailed(ret)) //cast exception
						GASS_EXCEPT(GASS::Exception::ERR_ITEM_NOT_FOUND, "Failed to findStraightPath","RecastNavigationMeshComponent::GetShortestPath");
					for(int i = 0; i < num_straight_polys; i++)
					{
						GASS::Vec3 pos;
						RecastToGASS(&straight_path[i*3],pos);
						path.push_back(pos);
					}
					if(num_straight_polys > 0)
						return true;
				}
			}
		}
		return false;
	}
}
