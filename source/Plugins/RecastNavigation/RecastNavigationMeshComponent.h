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

#pragma once
#include "Sim/GASS.h"
#include "Core/Utils/GASSEnumBinder.h"
#include "Plugins/Base/CoreMessages.h"
#include "Modules/Editor/EditorMessages.h"
#include "Sim/Interface/GASSINavigationMeshComponent.h"
#include "LandCoverType.h"

class dtNavMesh;
class dtTileCache;
class dtNavMeshQuery;
class InputGeom;
class rcContext;
namespace GASS
{
	class RawNavMeshData
	{
	public:
		float* Verts;
		int NumVerts;
		float* TriNorm;
		int* Tris;
		int* Area;
		int NumTris;
		float* BMin;
		float* BMax;
	};

	/*enum SamplePolyAreas
	{
		SAMPLE_POLYAREA_OBSTACLE,
		SAMPLE_POLYAREA_GROUND,
		SAMPLE_POLYAREA_WATER,
		SAMPLE_POLYAREA_ROAD,
		SAMPLE_POLYAREA_DOOR,
		SAMPLE_POLYAREA_GRASS,
		SAMPLE_POLYAREA_JUMP,
	};*/

	enum SamplePolyFlags
	{
		SAMPLE_POLYFLAGS_WALK = 0x01,      // Ability to walk (ground, grass, road)
		SAMPLE_POLYFLAGS_SWIM = 0x02,      // Ability to swim (water).
		SAMPLE_POLYFLAGS_DOOR = 0x04,      // Ability to move through doors.
		SAMPLE_POLYFLAGS_JUMP = 0x08,      // Ability to jump.
		SAMPLE_POLYFLAGS_DISABLED	= 0x10,		// Disabled polygon
		SAMPLE_POLYFLAGS_ALL = 0xffff      // All abilities.
	};


	START_ENUM_BINDER(LandCoverType,LandCoverTypeBinder)
		BIND(LAND_COVER_OBSTACLE)
		BIND(LAND_COVER_DEFAULT)
		BIND(LAND_COVER_WATER_C1)
		BIND(LAND_COVER_ROAD_C1)
		BIND(LAND_COVER_FOREST_C1)
		BIND(LAND_COVER_GRASS_C1)
		BIND(LAND_COVER_URBAN_C1)
	END_ENUM_BINDER(LandCoverType,LandCoverTypeBinder)

	class RecastNavigationMeshComponent : public Reflection<RecastNavigationMeshComponent, BaseSceneComponent>, public INavigationMeshComponent
	{
	public:
		RecastNavigationMeshComponent();
		~RecastNavigationMeshComponent() override;
		static void RegisterReflection();

		void OnInitialize() override;
		void OnDelete() override;
		
		//INavigationMeshComponent
		bool GetShortestPath(const Vec3 &from, const Vec3 &to, NavigationPath &path) const override;
		bool GetShortestPathForPlatform(const PlatformType platform_type, const Vec3 &from, const Vec3 &to, NavigationPath &path) const override;
		bool GetClosestPointOnMeshForPlatform(const PlatformType platform_type, const GASS::Vec2 &in_pos, const float search_radius, GASS::Vec3 &out_pos) const override;
		bool Raycast(const PlatformType platform_type, const GASS::Vec3 &from_pos, const GASS::Vec3 &to_pos, GASS::Vec3 &hit_pos) const override { return false; }
		
		Vec3 GetRandomPoint() const;
		bool GetRandomPointInCircle(const Vec3 &circle_center, const float radius, Vec3 &point) const;
		bool IsPointInside(const Vec3 &point) const;

		//Internal

		dtNavMesh* GetNavMesh() const { return m_NavMesh; }
		dtNavMeshQuery* GetNavMeshQuery() const { return m_NavQuery; }
		//temp public to allow debug updates from outside
		void UpdateNavMeshVis();
		//need to be public for enumerator access it
		std::vector<SceneObjectPtr> GetMeshSelectionEnum();
	protected:
		static const int MAX_POLYS_IN_PATH = 2048;
		void GASSToRecast(const GASS::Vec3 &in_pos, float* out_pos) const;
		void RecastToGASS(float* in_pos, GASS::Vec3 &out_pos) const;
		void UpdateOffmeshConnections();
		void UpdateConvexVolumes();
		
		float GetCellSize() const;
		float GetCellHeight() const;
		float GetAgentHeight() const;
		float GetAgentRadius() const;
		float GetAgentMaxClimb() const;
		float GetAgentMaxSlope() const;
		float GetRegionMinSize() const;
		float GetRegionMergeSize() const;
		float GetEdgeMaxLen() const;
		float GetEdgeMaxError() const;
		float GetVertsPerPoly() const;
		float GetDetailSampleDist() const;
		float GetDetailSampleMaxError() const;
		int GetTileSize() const;
		int GetGridSize()const { return m_GridSize; }

		void SetCellSize(const float value);
		void SetCellHeight(const float value);
		void SetAgentHeight(const float value);
		void SetAgentRadius(const float value);
		void SetAgentMaxClimb(const float value);
		void SetAgentMaxSlope(const float value);
		void SetRegionMinSize(const float value);
		void SetRegionMergeSize(const float value);
		void SetEdgeMaxLen(const float value);
		void SetEdgeMaxError(const float value);
		void SetVertsPerPoly(const float value);
		void SetDetailSampleDist(const float value);
		void SetDetailSampleMaxError(const float value);
		void SetTileSize(const int size);
		void SetGridSize(const int size) { m_GridSize = size; }
		
		
		void SetMeshSelection(const std::vector<SceneObjectRef>  &value);
		
		std::vector<SceneObjectRef>  GetMeshSelection() const {return m_SelectedMeshes;}
		bool GetBuild() const;
		void SetBuild(const bool value);
		void SetShowMeshLines(bool value);
		bool GetShowMeshLines() const{return m_ShowMeshLines;}
		void SetShowMeshSolid(bool value);
		bool GetShowMeshSolid() const{return m_ShowMeshSolid;}
		Vec3 GetMeshBoundingMin() const;
		void SetMeshBoundingMin(const Vec3 &min);
		Vec3 GetMeshBoundingMax() const;
		void SetMeshBoundingMax(const Vec3 &max);
		int GetTransparency() const;
		void SetTransparency(int value);
		void SaveXML(tinyxml2::XMLElement *obj_elem) override;
		void LoadXML(tinyxml2::XMLElement *obj_elem) override;
		void SaveAllTiles(const char* path, const dtNavMesh* mesh);
		dtNavMesh* LoadAll(const char* path);
		std::vector<Vec3> GetVisualNavMesh();
		bool GetRawMeshData(RawNavMeshData &nm_data);
		bool GenerateTiles();
		void BuildAllTiles();
		unsigned char* BuildTileMesh(const int tx, const int ty, const float* bmin, const float* bmax, int& dataSize);
		void SetVisible(bool value);
		bool GetVisible() const;
		void SetExportMesh(const FilePath &value);
		FilePath GetExportMesh() const;
		void SetImportMesh(const FilePath &value);
		FilePath GetImportMesh() const;
		void SetBoundingBoxFromShape(const std::string &value);
		std::string GetBoundingBoxFromShape() const;
		FilePath _GetFilePath() const;
		std::vector<std::string> GetMeshSelectionEnum2();

		void OnEditPosition(EditPositionMessagePtr message);
		void OnSceneObjectCreated(PostSceneObjectInitializedEventPtr message);

		GraphicsMeshPtr m_NavVisTriMesh;
		GraphicsMeshPtr m_NavVisLineMesh;
		std::vector<SceneObjectRef> m_SelectedMeshes;
		bool m_ShowMeshLines;
		bool m_ShowMeshSolid;
		bool m_Visible;
		AABox m_MeshBounding;
		int m_Transparency;
		std::string m_BBShape;
		std::string m_NavMeshFilePath;
		dtNavMesh* m_NavMesh;
		//dtTileCache* m_TileCache;
		//settings
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
		int m_GridSize;
		int m_TileSize;
		dtNavMeshQuery* m_NavQuery;
		InputGeom* m_Geom;
		rcContext* m_Ctx;
		bool m_MonotonePartitioning;
		bool m_Initialized;
		bool m_AutoCollectMeshes;
		bool m_UseBoudingBox;
		Vec3 m_LocalOrigin;

		mutable GASS_MUTEX m_Mutex; 
	};
	typedef GASS_SHARED_PTR<RecastNavigationMeshComponent> RecastNavigationMeshComponentPtr;
	typedef GASS_WEAK_PTR<RecastNavigationMeshComponent> RecastNavigationMeshComponentWeakPtr;
}

