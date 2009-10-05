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
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Common.h"
#include <OgreTerrainPageSource.h>
#include <OgreTerrainSceneManager.h>
#include "tinyxml.h"

namespace GASS
{
	class CustomTerrainPageListener;
	class CollisionMesh;
	class OgreSceneManagerTerrainComponent : public Reflection<OgreSceneManagerTerrainComponent,BaseSceneComponent>, public ITerrainComponent, public IGeometryComponent,  Ogre::TerrainPageSourceListener
	{
	public:
		OgreSceneManagerTerrainComponent();
		~OgreSceneManagerTerrainComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void GetMeshData(MeshDataPtr mesh_data);
		virtual void pageConstructed(Ogre::TerrainSceneManager* manager, size_t pagex, size_t pagez, Ogre::Real* heightData);

		std::string GetFilename()const {return m_TerrainConfigFile;}
		void SetFilename(const std::string &filename) {m_TerrainConfigFile = filename;}	
		virtual float GetSizeX(){return m_WorldWidth;}
		virtual float GetSizeZ(){return m_WorldHeight;}
		virtual void GetHeightAndNormal(float x, float z, float &height,Vec3 &normal){}
		virtual bool CheckOnTerrain(float x, float z){return true;}
		
		virtual void GetBounds(Vec3 &min,Vec3 &max);
		virtual unsigned int GetSamplesX();
		virtual unsigned int GetSamplesZ();
		
		float GetHeight(float x, float z);
		float GetWorldWidth()const {return  m_WorldWidth;}
		float GetWorldHeight()const {return m_WorldHeight; }
		int GetNodesPerSideAllPagesW()const {return m_NodesPerSideAllPagesW;}
		int GetNodesPerSideAllPagesH()const  {return m_NodesPerSideAllPagesH;}
		Vec3 GetScale() const {return m_Scale;}
		void CreateHeightData(Ogre::TerrainSceneManager* manager, size_t pagex, size_t pagez, Ogre::Real* heightData);
		AABox GetBoundingBox() const;
		Sphere GetBoundingSphere() const;
	protected:
		void OnLoad(LoadGFXComponentsMessagePtr message);
		bool m_PageListenerAdded;
		Vec3 m_Scale;
		bool m_Center;
		float m_WorldWidth;
		float m_WorldHeight;
		int m_NodesPerSideAllPagesW;
		int m_NodesPerSideAllPagesH;
		bool m_CreateCollisionMesh;
		std::string m_TerrainConfigFile;

		//Helpers to access terrain height very fast
		//TODO: Put this in a heightmap class for all terrain managers
		float *m_HeightData;
		//Heightmap dimensions
		int m_HMDim;
		float m_MaxHeight;
	};


}

