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

#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include <tinyxml.h>

#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSCommon.h"
#include "OgreTerrain.h"
#include "OgreTerrainGroup.h"
#include "OgreTerrainQuadTreeNode.h"
#include "OgreTerrainMaterialGeneratorA.h"
#include "OgreTerrainPaging.h"

namespace GASS
{
	class CustomTerrainPageListener;
	class CollisionMesh;
	class OgreTerrainPageComponent : public Reflection<OgreTerrainPageComponent,BaseSceneComponent>, public IHeightmapTerrainComponent, public IMeshComponent, public IGeometryComponent
	{
		friend class OgreTerrainGroupComponent;
	public:
		OgreTerrainPageComponent();
		~OgreTerrainPageComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();

		//IMeshComponent interface
		virtual Resource GetTerrainResource() const;
		virtual void GetMeshData(MeshDataPtr mesh_data) const;

		//IHeightmapTerrainComponent
		virtual Float GetHeightAtWorldLocation(Float x, Float z) const;
		virtual Float GetHeightAtPoint(int x, int y) const;
		virtual unsigned int GetSamples() const;
		virtual float* GetHeightData() const;
		
		//IGeometryComponent
		virtual AABox GetBoundingBox() const;
		virtual Sphere GetBoundingSphere() const;
		virtual GeometryFlags GetGeometryFlags() const;
		virtual void SetGeometryFlags(GeometryFlags flags);
	protected:
		void LoadFromFile();
		Resource GetHeightMap()const {return m_HeightMapFile;}
		void SetHeightMap(const Resource &filename);
		std::string GetFromResourceSystem(const std::string &filename);
		void UpdatePosition();
		int GetIndexX() const;
		void SetIndexX(int index);
		int GetIndexY() const;
		void SetIndexY(int index);
		void SetPosition(const Vec3 &pos);
		Vec3 GetPosition() const; 
		void SetColorMap(const Resource &colormap);
		Resource GetColorMap() const; 
		void SetDiffuseLayer0(const Resource &diffuse);
		Resource GetDiffuseLayer0() const;
		void SetNormalLayer0(const Resource &diffuse);
		Resource GetNormalLayer0() const;
		void SetDiffuseLayer1(const Resource &diffuse);
		Resource GetDiffuseLayer1() const;
		void SetNormalLayer1(const Resource &diffuse);
		Resource GetNormalLayer1() const;
		void SetDiffuseLayer2(const Resource &diffuse);
		Resource GetDiffuseLayer2() const;
		void SetNormalLayer2(const Resource &diffuse);
		Resource GetNormalLayer2() const;
		void SetDiffuseLayer3(const Resource &diffuse);
		Resource GetDiffuseLayer3() const;
		void SetNormalLayer3(const Resource &diffuse);
		Resource GetNormalLayer3() const;
		void SetDiffuseLayer4(const Resource &diffuse);
		Resource GetDiffuseLayer4() const;
		void SetNormalLayer4(const Resource &diffuse);
		Resource GetNormalLayer4() const;
		void SetTilingLayer0(float value);
		float GetTilingLayer0() const;
		void SetTilingLayer1(float value);
		float GetTilingLayer1() const;
		void SetTilingLayer2(float value);
		float GetTilingLayer2() const;
		void SetTilingLayer3(float value);
		float GetTilingLayer3() const;
		void SetTilingLayer4(float value);
		float GetTilingLayer4() const;
		void SetMask(const Resource &mask);
		Resource GetMask() const;
		//void SetMaskLayer1(const std::string &mask);
		//std::string GetMaskLayer1() const;
		//void SetMaskLayer2(const std::string &mask);
		//std::string GetMaskLayer2() const;

		//import functions that take full path to data, functions will only import data, 
		//file path is not saved, ues SetHeightMap etc instead if you want to ,
		//reimport data every load
		void ImportHeightMap(const FilePath &filename);
		FilePath GetImportHeightMap() const {return "";}
		void ImportColorMap(const FilePath &filename);
		FilePath GetImportColorMap() const {return "";}
		void ImportDetailMask(const FilePath &mask);
		FilePath GetImportDetailMask() const {return FilePath("");}
		void OnTerrainLayerMessage(TerrainLayerMessagePtr message);
		
		bool m_CreateCollisionMesh;
		Resource m_HeightMapFile;
		Resource m_ColorMap;
		Resource m_Mask;
		//Resource m_MaskLayer1;
		//Resource m_MaskLayer2;
		Resource m_DiffuseLayer0;
		Resource m_NormalLayer0;
		Resource m_DiffuseLayer1;
		Resource m_NormalLayer1;
		Resource m_DiffuseLayer2;
		Resource m_NormalLayer2;
		Resource m_DiffuseLayer3;
		Resource m_NormalLayer3;
		Resource m_DiffuseLayer4;
		Resource m_NormalLayer4;
		Ogre::SceneManager* m_OgreSceneManager;
		Ogre::TerrainGroup* m_TerrainGroup;
		Ogre::Terrain* m_Terrain;
		int m_IndexX;
		int m_IndexY;
		float m_TilingLayer0;
		float m_TilingLayer1;
		float m_TilingLayer2;
		float m_TilingLayer3;
		float m_TilingLayer4;
		GeometryFlags m_GeomFlags;		
	};

	typedef boost::shared_ptr<OgreTerrainPageComponent> OgreTerrainPageComponentPtr;
}

