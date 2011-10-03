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
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include <tinyxml.h>

#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Common.h"
#include <OgreTerrainPageSource.h>
#include <OgreTerrainSceneManager.h>

namespace GASS
{
	class CustomTerrainPageListener;
	class CollisionMesh;
	class OgreSceneManagerTerrainComponent : public Reflection<OgreSceneManagerTerrainComponent,BaseSceneComponent>, public ITerrainComponent, public IMeshComponent, public IGeometryComponent,  Ogre::TerrainPageSourceListener
	{
	public:
		OgreSceneManagerTerrainComponent();
		~OgreSceneManagerTerrainComponent();
		static void RegisterReflection();
		virtual void OnCreate();

		//IMeshComponent
		virtual void GetMeshData(MeshDataPtr mesh_data) const;
		virtual void pageConstructed(Ogre::TerrainSceneManager* manager, size_t pagex, size_t pagez, Ogre::Real* heightData);
		virtual std::string GetFilename()const {return m_TerrainConfigFile;}
		virtual void SetFilename(const std::string &filename);

		//ITerrainComponent
		virtual void GetHeightAndNormal(Float x, Float z, Float &height,Vec3 &normal)const {}
		virtual Float GetHeight(Float x, Float z) const;
		virtual unsigned int GetSamplesX() const;
		virtual unsigned int GetSamplesZ() const;
		float* GetHeightData() const;
		
		//IGeometryComponent
		AABox GetBoundingBox() const;
		Sphere GetBoundingSphere() const;
		virtual GeometryCategory GetGeometryCategory() const;
		Vec3 GetScale() const {return m_Scale;}
	protected:
		void CreateHeightData(Ogre::TerrainSceneManager* manager, size_t pagex, size_t pagez, Ogre::Real* heightData);
		void LoadTerrain(const std::string &filename);
		void OnLoad(LoadGFXComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		bool m_PageListenerAdded;
		Vec3 m_Scale;
		bool m_Center;
		Float m_WorldWidth;
		Float m_WorldHeight;
		int m_NodesPerSideAllPagesW;
		int m_NodesPerSideAllPagesH;
		bool m_CreateCollisionMesh;
		std::string m_TerrainConfigFile;
		Ogre::SceneManager* m_OgreSceneManager;

		//Helpers to access terrain height very fast
		//TODO: Put this in a heightmap class for all terrain managers
		float *m_HeightData;
		//Heightmap dimensions
		int m_HMDim;
		float m_MaxHeight;
	};
}

