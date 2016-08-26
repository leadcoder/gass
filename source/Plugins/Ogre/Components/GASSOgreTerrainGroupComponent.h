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

#include "Sim/GASSCommon.h"
#include "Plugins/Ogre/GASSOgreCommon.h"
#include "Plugins/Ogre/GASSOgreTerrainIncludes.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSICollisionComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSBaseSceneComponent.h"
#include <tinyxml2.h>

namespace GASS
{
	class CustomTerrainPageListener;
	class CollisionMesh;
	class OgreTerrainGroupComponent : public Reflection<OgreTerrainGroupComponent,BaseSceneComponent>, public IMeshComponent, public IGeometryComponent
	{
	public:
		OgreTerrainGroupComponent();
		~OgreTerrainGroupComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		
		//IMeshComponent
		virtual GraphicsMesh GetMeshData() const;
	
		//IGeometryComponent
		virtual GeometryFlags GetGeometryFlags() const;
		virtual void SetGeometryFlags(GeometryFlags flags);
		virtual AABox GetBoundingBox() const;
		virtual Sphere GetBoundingSphere() const;
		virtual bool GetCollision() const;
		virtual void SetCollision(bool value);

		
		//IXMLSerialize interface overload, we need a way to trigger terrain save!
		virtual void SaveXML(tinyxml2::XMLElement *obj_elem);

		Ogre::TerrainGroup* GetTerrainGroup() const {return m_TerrainGroup;}
		int GetImportTerrainSize() const;
		Vec3 GetScale() const {return m_Scale;}
	protected:
		ADD_PROPERTY(bool, EnableLightmap);
		ResourceHandle GetTerrainResource()const {return m_TerrainResource;}
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
	
		void OnTerrainHeightModify(TerrainHeightModifyRequestPtr message);
		void OnTerrainLayerPaint(TerrainPaintRequestPtr message);
		void OnRoadMessage(RoadRequestPtr message);
	
		void DeformTerrain(Ogre::Terrain* terrain,const Ogre::Vector3& centrepos, Ogre::Real timeElapsed, float brush_size_terrain_space, float brush_inner_radius, float noise);
		void GetAverageHeight(Ogre::Terrain* terrain, const Ogre::Vector3& centrepos, const Ogre::Real  brush_size_terrain_space,Ogre::Real &avg_height);
		void SmoothTerrain(Ogre::Terrain* terrain,const Ogre::Vector3& centrepos, const Ogre::Real intensity, const Ogre::Real brush_size_terrain_space, const Ogre::Real brush_inner_radius, const Ogre::Real average_height);
		void PaintTerrain(Ogre::Terrain* terrain,const Ogre::Vector3& centrepos, const Ogre::Real intensity, const Ogre::Real brush_size_terrain_space, const Ogre::Real brush_inner_radius, int layer_index, float noise);
		void FlattenTerrain(Ogre::Terrain* terrain,const Ogre::Vector3& centrepos, Ogre::Real intensity, float brush_size_terrain_space, float brush_inner_radius);
		void FlattenTerrain(Ogre::Terrain* terrain,const Ogre::Vector3& start, Ogre::Vector3& end);
	
		bool GetFadeDetail() const {return m_FadeDetail;}
		Ogre::Real GetDetailFadeDist() const {return m_DetailFadeDist;}
		bool GetFadeOutColor() const {return m_FadeOutColor;}
		Ogre::Real GetNearColorWeight() const {return m_NearColorWeight;}

		void SetFadeDetail(bool value) ;
		void SetDetailFadeDist(float value) ;
		void SetFadeOutColor(bool  value) ;
		void SetNearColorWeight(float value) ;

		bool GetEnableLayerNormal() const {return m_EnableLayerNormal;}
		void SetEnableLayerNormal(bool value) {m_EnableLayerNormal = value;}
		bool GetEnableLayerSpecular() const {return m_EnableLayerSpecular;}
		void SetEnableLayerSpecular(bool value) {m_EnableLayerSpecular = value;}
		bool GetEnableLayerParallax() const {return m_EnableLayerParallax;}
		void SetEnableLayerParallax(bool value) {m_EnableLayerParallax = value;}

		bool m_EnableLayerParallax;
		bool m_EnableLayerSpecular;
		bool m_EnableLayerNormal;
		Vec3 m_Scale;
		bool m_Center;
		Float m_WorldWidth;
		Float m_WorldHeight;
		int m_NodesPerSideAllPagesW;
		int m_NodesPerSideAllPagesH;
		bool m_CreateCollisionMesh;
		float m_TerrainScale;
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
		ResourceHandle m_TerrainResource;
		std::string m_CustomMaterial;
		Vec3 m_Origin;
		bool m_FadeDetail;
		float m_DetailFadeDist;
		bool m_FadeOutColor;
		float m_NearColorWeight;
		//Ogre::TerrainMaterialGeneratorB::SM2Profile* m_TerrainProfile;
		Ogre::TerrainMaterialGeneratorC::SM2Profile* m_TerrainProfile;
		//Ogre::TerrainMaterialGeneratorA::SM2Profile* m_TerrainProfile;
		GeometryFlags m_GeomFlags;
		CollisionComponentPtr m_Collision;
	};
	typedef GASS_SHARED_PTR<OgreTerrainGroupComponent> OgreTerrainGroupComponentPtr;
}