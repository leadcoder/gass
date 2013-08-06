/****************************************************************************
*                                                                           *
* GASS                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: GASS@gmail.com                                               *
* Web page: http://n00b.dyndns.org/GASS                               *
*                                                                           *
* GASS is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the GASS license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 

#pragma once
#include "Sim/GASS.h"
#include "Plugins/Base/CoreMessages.h"
#include "Modules/Editor/EditorMessages.h"
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

	class RecastNavigationMeshComponent : public Reflection<RecastNavigationMeshComponent,BaseSceneComponent>, public IShape
	{
	public:
		RecastNavigationMeshComponent();
		virtual ~RecastNavigationMeshComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		
		dtNavMesh* GetNavMesh() const {return m_NavMesh;}
		dtNavMeshQuery* GetNavMeshQuery() const {return m_NavQuery;}
		virtual bool IsPointInside(const Vec3 &point) const;
		virtual Vec3 GetRandomPoint() const;
		//temp public to allow debug updates from outside
		void UpdateNavMeshVis();

		//need to be public for enumerator access it
		std::vector<SceneObjectPtr> GetMeshSelectionEnum();
	protected:
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

		void SetCellSize(const float value);
		void SetCellHeight(const float value);
		void SetAgentHeight(const float value);
		void SetAgentRadius (const float value);
		void SetAgentMaxClimb(const float value);
		void SetAgentMaxSlope(const float value);
		void SetRegionMinSize(const float value);
		void SetRegionMergeSize(const float value);
		void SetEdgeMaxLen(const float value);
		void SetEdgeMaxError(const float value);
		void SetVertsPerPoly(const float value);
		void SetDetailSampleDist(const float value);
		void SetDetailSampleMaxError(const float value);
		void SetSelectedMeshes(const std::vector<SceneObjectRef>  &value);
		void SetTileSize(const int size);
		std::vector<SceneObjectRef>  GetSelectedMeshes() const {return m_SelectedMeshes;}
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
		void SaveXML(TiXmlElement *obj_elem);
		void LoadXML(TiXmlElement *obj_elem);
		void OnEditPosition(EditPositionMessagePtr message);

		void SaveAllTiles(const char* path, const dtNavMesh* mesh);
		dtNavMesh* LoadAll(const char* path);
		std::vector<MeshVertex> GetVisualNavMesh();
		bool GetRawMeshData(RawNavMeshData &nm_data);
		bool GenerateTiles();
		void BuildAllTiles();
		unsigned char* BuildTileMesh(const int tx, const int ty, const float* bmin, const float* bmax, int& dataSize);
		void Cleanup();
		void SetVisible(bool value);
		bool GetVisible() const;
		void SetExportMesh(const FilePath &value);
		FilePath GetExportMesh() const;
		void SetImportMesh(const FilePath &value);
		FilePath GetImportMesh() const;
		void SetBoundingBoxFromShape(const std::string &value);
		std::string GetBoundingBoxFromShape() const;

		

		void OnChangeName(SceneObjectNameMessagePtr message);

		ManualMeshDataPtr m_NavVisTriMesh;
		ManualMeshDataPtr m_NavVisLineMesh;
		std::vector<SceneObjectRef> m_SelectedMeshes;
		bool m_ShowMeshLines;
		bool m_ShowMeshSolid;
		bool m_Visible;
		AABox m_MeshBounding;
		int m_Transparency;
		std::string m_BBShape;
		std::string m_NavMeshFilePath;
		dtNavMesh* m_NavMesh;
		dtTileCache* m_TileCache;
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
		int m_TileSize;
		dtNavMeshQuery* m_NavQuery;
		InputGeom* m_Geom;
		rcContext* m_Ctx;
		bool m_MonotonePartitioning;
		bool m_Initialized;
	};
	typedef SPTR<RecastNavigationMeshComponent> RecastNavigationMeshComponentPtr;
	typedef WPTR<RecastNavigationMeshComponent> RecastNavigationMeshComponentWeakPtr;
}

