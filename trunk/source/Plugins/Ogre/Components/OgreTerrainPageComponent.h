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
	class OgreTerrainPageComponent : public Reflection<OgreTerrainPageComponent,BaseSceneComponent>, public ITerrainComponent, public IMeshComponent, public IGeometryComponent
	{
		friend class OgreTerrainGroupComponent;
	public:
		OgreTerrainPageComponent();
		~OgreTerrainPageComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();

		//IMeshComponent interface
		virtual std::string GetFilename() const;
		virtual void GetMeshData(MeshDataPtr mesh_data) const;

		//ITerrainComponent
		virtual Float GetHeight(Float x, Float z) const;
		virtual void GetHeightAndNormal(Float x, Float z, Float &height,Vec3 &normal) const{}
		virtual unsigned int GetSamplesX() const;
		virtual unsigned int GetSamplesZ() const;
		virtual float* GetHeightData() const;
		
		//IGeometryComponent
		virtual AABox GetBoundingBox() const;
		virtual Sphere GetBoundingSphere() const;
		virtual GeometryFlags GetGeometryFlags() const;
		virtual void SetGeometryFlags(GeometryFlags flags);
	protected:
		void LoadFromFile();
		std::string GetHeightMap()const {return m_HeightMapFile;}
		void SetHeightMap(const std::string &filename);
		std::string GetFromResourceSystem(const std::string &filename);
		void UpdatePosition();
		int GetIndexX() const;
		void SetIndexX(int index);
		int GetIndexY() const;
		void SetIndexY(int index);
		void SetPosition(const Vec3 &pos);
		Vec3 GetPosition() const; 
		void SetColorMap(const std::string &colormap);
		std::string GetColorMap() const; 
		void SetDiffuseLayer0(const std::string &diffuse);
		std::string GetDiffuseLayer0() const;
		void SetNormalLayer0(const std::string &diffuse);
		std::string GetNormalLayer0() const;
		void SetDiffuseLayer1(const std::string &diffuse);
		std::string GetDiffuseLayer1() const;
		void SetNormalLayer1(const std::string &diffuse);
		std::string GetNormalLayer1() const;
		void SetDiffuseLayer2(const std::string &diffuse);
		std::string GetDiffuseLayer2() const;
		void SetNormalLayer2(const std::string &diffuse);
		std::string GetNormalLayer2() const;
		void SetDiffuseLayer3(const std::string &diffuse);
		std::string GetDiffuseLayer3() const;
		void SetNormalLayer3(const std::string &diffuse);
		std::string GetNormalLayer3() const;
		void SetDiffuseLayer4(const std::string &diffuse);
		std::string GetDiffuseLayer4() const;
		void SetNormalLayer4(const std::string &diffuse);
		std::string GetNormalLayer4() const;
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
		void SetMask(const std::string &mask);
		std::string GetMask() const;
		void SetMaskLayer1(const std::string &mask);
		std::string GetMaskLayer1() const;
		void SetMaskLayer2(const std::string &mask);
		std::string GetMaskLayer2() const;

		//import functions that take full path to data, functions will only import data, 
		//file path is not saved, ues SetHeightMap etc instead if ypu want to ,
		//reimport data every load
		void ImportHeightMap(const std::string &filename);
		std::string GetImportHeightMap() const {return "";}
		void ImportColorMap(const std::string &filename);
		std::string GetImportColorMap() const {return "";}
		void ImportDetailMask(const std::string &mask);
		std::string GetImportDetailMask() const {return "";}
		void OnTerrainLayerMessage(TerrainLayerMessagePtr message);
		
		bool m_CreateCollisionMesh;
		std::string m_HeightMapFile;
		std::string m_ColorMap;
		std::string m_Mask;
		std::string m_MaskLayer1;
		std::string m_MaskLayer2;
		std::string m_DiffuseLayer0;
		std::string m_NormalLayer0;
		std::string m_DiffuseLayer1;
		std::string m_NormalLayer1;
		std::string m_DiffuseLayer2;
		std::string m_NormalLayer2;
		std::string m_DiffuseLayer3;
		std::string m_NormalLayer3;
		std::string m_DiffuseLayer4;
		std::string m_NormalLayer4;
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

