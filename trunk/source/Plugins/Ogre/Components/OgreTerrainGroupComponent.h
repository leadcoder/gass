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

#pragma once

#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include <tinyxml.h>

#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Common.h"
#include "OgreTerrain.h"
#include "OgreTerrainGroup.h"
#include "OgreTerrainQuadTreeNode.h"
#include "OgreTerrainMaterialGeneratorA.h"
#include "OgreTerrainPaging.h"
#include "Plugins/Ogre/Helpers/OgreTerrainMaterialGeneratorB.h"



namespace GASS
{
	class CustomTerrainPageListener;
	class CollisionMesh;
	class OgreTerrainGroupComponent : public Reflection<OgreTerrainGroupComponent,BaseSceneComponent>, public ITerrainComponent, public IMeshComponent, public IGeometryComponent,  public boost::enable_shared_from_this<OgreTerrainGroupComponent>
	{
	public:
		OgreTerrainGroupComponent();
		~OgreTerrainGroupComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void GetMeshData(MeshDataPtr mesh_data);
		std::string GetFilename()const {return m_TerrainName;}


		virtual Float GetSizeX(){return m_WorldWidth;}
		virtual Float GetSizeZ(){return m_WorldHeight;}
		virtual void GetHeightAndNormal(Float x, Float z, Float &height,Vec3 &normal){}
		virtual bool CheckOnTerrain(Float x, Float z){return true;}
		virtual void GetBounds(Vec3 &min,Vec3 &max);
		virtual unsigned int GetSamplesX();
		virtual unsigned int GetSamplesZ();

		Float GetHeight(Float x, Float z);
		Float GetWorldWidth()const {return  m_WorldWidth;}
		Float GetWorldHeight()const {return m_WorldHeight;}
		int GetNodesPerSideAllPagesW() const {return m_NodesPerSideAllPagesW;}
		int GetNodesPerSideAllPagesH() const  {return m_NodesPerSideAllPagesH;}
		Vec3 GetScale() const {return m_Scale;}

		virtual GeometryCategory GetGeometryCategory() const;
		virtual AABox GetBoundingBox() const;
		virtual Sphere GetBoundingSphere() const;
		float* GetHeightData() {return NULL;}
		Ogre::TerrainGroup* GetTerrainGroup() const {return m_TerrainGroup;}
		int GetImportTerrainSize() const;

		//IXMLSerialize interface, we need a way to trigger terrain save!
		virtual void SaveXML(TiXmlElement *obj_elem);
		
	protected:
		void RemoveAllPages();
		std::string GetCustomMaterial() const;
		void SetCustomMaterial(const std::string &material);
		void SetSaveTerrain(const std::string &filename);
		std::string GetSaveTerrain() const;
		void SetLoadTerrain(const std::string &filename);
		std::string GetLoadTerrain() const;
		float GetImportScale() const;
		void SetImportScale(const float &value);

		void SetImportTerrainSize(const int &value);
		Float GetImportTerrainWorldSize() const;
		void SetImportTerrainWorldSize(const Float &value);
		void ConfigureTerrainDefaults();
		Vec2i GetPages() const;
		void CreatePages(const Vec2i  &size);
		void SetOrigin(const Vec3 &pos);
		Vec3 GetOrigin() const;
		void OnLoad(LoadGFXComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);

		void OnTerrainHeightModify(TerrainHeightModifyMessagePtr message);
		void OnTerrainLayerPaint(TerrainPaintMessagePtr message);
		void OnRoadMessage(RoadMessagePtr message);
	
		void DeformTerrain(Ogre::Terrain* terrain,const Ogre::Vector3& centrepos, Ogre::Real timeElapsed, float brush_size_terrain_space, float brush_inner_radius, float noise);
		void GetAverageHeight(Ogre::Terrain* terrain, const Ogre::Vector3& centrepos, const Ogre::Real  brush_size_terrain_space,Ogre::Real &avg_height);
		void SmoothTerrain(Ogre::Terrain* terrain,const Ogre::Vector3& centrepos, const Ogre::Real intensity, const Ogre::Real brush_size_terrain_space, const Ogre::Real brush_inner_radius, const Ogre::Real average_height);
		void PaintTerrain(Ogre::Terrain* terrain,const Ogre::Vector3& centrepos, const Ogre::Real intensity, const Ogre::Real brush_size_terrain_space, const Ogre::Real brush_inner_radius, int layer_index, float noise);
		void FlattenTerrain(Ogre::Terrain* terrain,const Ogre::Vector3& centrepos, Ogre::Real intensity, float brush_size_terrain_space, float brush_inner_radius);
	
		bool GetFadeDetail() const {return m_FadeDetail;}
		Ogre::Real GetDetailFadeDist() const {return m_DetailFadeDist;}
		bool GetFadeOutColor() const {return m_FadeOutColor;}
		Ogre::Real GetNearColorWeight() const {return m_NearColorWeight;}

		void SetFadeDetail(bool value) ;
		void SetDetailFadeDist(float value) ;
		void SetFadeOutColor(bool  value) ;
		void SetNearColorWeight(float value) ;


		Vec3 m_Scale;
		bool m_Center;
		Float m_WorldWidth;
		Float m_WorldHeight;
		int m_NodesPerSideAllPagesW;
		int m_NodesPerSideAllPagesH;
		bool m_CreateCollisionMesh;

		Ogre::SceneManager* m_OgreSceneManager;

		//Helpers to access terrain height very fast
		//TODO: Put this in a heightmap class for all terrain managers
		float *m_HeightData;
		//Heightmap dimensions
		int m_HMDim;
		float m_MaxHeight;

		Ogre::TerrainGlobalOptions* m_TerrainGlobals;
		Ogre::TerrainGroup* m_TerrainGroup;
		bool m_Paging;
		Ogre::TerrainPaging* m_TerrainPaging;
		Ogre::PageManager* m_PageManager;

		int m_TerrainSize;
		float m_TerrainWorldSize;
		std::string m_TerrainName;
		std::string m_CustomMaterial;
		Vec3 m_Origin;


		bool m_FadeDetail;
		float m_DetailFadeDist;
		bool m_FadeOutColor;
		float m_NearColorWeight;
		Ogre::TerrainMaterialGeneratorB::SM2Profile* m_TerrainProfile;

	};
	typedef boost::shared_ptr<OgreTerrainGroupComponent> OgreTerrainGroupComponentPtr;
}

