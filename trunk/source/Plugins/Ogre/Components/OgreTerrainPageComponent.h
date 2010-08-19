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

namespace GASS
{
	class CustomTerrainPageListener;
	class CollisionMesh;
	class OgreTerrainPageComponent : public Reflection<OgreTerrainPageComponent,BaseSceneComponent>, public ITerrainComponent, public IMeshComponent, public IGeometryComponent,  public boost::enable_shared_from_this<OgreTerrainPageComponent>
	{
	public:
		OgreTerrainPageComponent();
		~OgreTerrainPageComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void GetMeshData(MeshDataPtr mesh_data);
		
		std::string GetFilename()const {return m_TerrainConfigFile;}
		void SetFilename(const std::string &filename);
		virtual Float GetSizeX(){return 0;}
		virtual Float GetSizeZ(){return 0;}
		virtual void GetHeightAndNormal(Float x, Float z, Float &height,Vec3 &normal){}
		virtual bool CheckOnTerrain(Float x, Float z){return true;}
		virtual void GetBounds(Vec3 &min,Vec3 &max);
		virtual unsigned int GetSamplesX();
		virtual unsigned int GetSamplesZ();

		Float GetHeight(Float x, Float z);
		Float GetWorldWidth()const {return  0;}
		Float GetWorldHeight()const {return 0;}
		int GetNodesPerSideAllPagesW() const {return 0;}
		int GetNodesPerSideAllPagesH() const  {return 0;}
		//Vec3 GetScale() const {return m_Scale;}
		AABox GetBoundingBox() const;
		Sphere GetBoundingSphere() const;
		float* GetHeightData() {return NULL;}
		
		void LoadFromFile();

	protected:
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
		void SetDiffuseLayer1(const std::string &diffuse);
		std::string GetDiffuseLayer1() const;
		void SetDiffuseLayer2(const std::string &diffuse);
		std::string GetDiffuseLayer2() const;
		void SetTilingLayer0(float value);
		float GetTilingLayer0() const;
		void SetTilingLayer1(float value);
		float GetTilingLayer1() const;
		void SetTilingLayer2(float value);
		float GetTilingLayer2() const;


		void SetMask(const std::string &mask);
		std::string GetMask() const;

		void SetMaskLayer1(const std::string &mask);
		std::string GetMaskLayer1() const;
		void SetMaskLayer2(const std::string &mask);
		std::string GetMaskLayer2() const;

		
		void ImportTerrain(const std::string &filename);
		void OnLoad(LoadGFXComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		bool m_CreateCollisionMesh;
		std::string m_TerrainConfigFile;
		std::string m_ColorMap;
		std::string m_Mask;
		std::string m_MaskLayer1;
		std::string m_MaskLayer2;
		std::string m_DiffuseLayer0;
		std::string m_DiffuseLayer1;
		std::string m_DiffuseLayer2;
		std::string m_DiffuseLayer3;
		Ogre::SceneManager* m_OgreSceneManager;

		Ogre::TerrainGroup* m_TerrainGroup;
		Ogre::Terrain* m_Terrain;
		
		int m_IndexX;
		int m_IndexY;

		float m_TilingLayer0;
		float m_TilingLayer1;
		float m_TilingLayer2;
		Vec3 m_Pos;
	};

	typedef boost::shared_ptr<OgreTerrainPageComponent> OgreTerrainPageComponentPtr;
}

