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
#include <boost/bind.hpp>

#include <OgreSceneNode.h>
#include <OgreConfigFile.h>

#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/Systems/SimSystemManager.h"

#include "Sim/SimEngine.h"
#include "Plugins/Ogre/Components/OgreSceneManagerTerrainComponent.h"

#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include "Plugins/Ogre/OgreConvert.h"


namespace GASS
{
	OgreSceneManagerTerrainComponent::OgreSceneManagerTerrainComponent() :
		m_CreateCollisionMesh (true),
		m_PageListenerAdded (false)
	{


	}

	OgreSceneManagerTerrainComponent::~OgreSceneManagerTerrainComponent()
	{
		if(m_PageListenerAdded)
		{
			Ogre::TerrainPageSourceListenerManager::getSingleton().removeListener(this);
		}
	}

	void OgreSceneManagerTerrainComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("OgreTerrainComponent",new Creator<OgreSceneManagerTerrainComponent, IComponent>);
		RegisterProperty<std::string>("TerrainConfigFile", &GASS::OgreSceneManagerTerrainComponent::GetFilename, &GASS::OgreSceneManagerTerrainComponent::SetFilename);
	}

	void OgreSceneManagerTerrainComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreSceneManagerTerrainComponent::OnLoad,LoadGFXComponentsMessage,0));
	}

	void OgreSceneManagerTerrainComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		OgreGraphicsSceneManagerPtr ogsm = boost::shared_static_cast<OgreGraphicsSceneManager>(message->GetGFXSceneManager());
		assert(ogsm);
		Ogre::SceneManager* sm = ogsm->GetSceneManger();

		IResourceSystem* rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>().get();

		std::string full_path;
		if(!rs->GetFullPath(m_TerrainConfigFile,full_path))
			Log::Error("Faild to load terrain %s",m_TerrainConfigFile.c_str());

		std::string base_path = Misc::RemoveFilename(full_path);

		if(m_CreateCollisionMesh)
		{
			Ogre::TerrainPageSourceListenerManager::getSingleton().addListener(this);
			m_PageListenerAdded = true;
		}
		sm->setWorldGeometry(full_path);
		Ogre::Vector3 scale = Ogre::Vector3::ZERO;
		int nodes_per_side = 0;
		int nodes_per_side_all_pagesW = 0;
		int nodes_per_side_all_pagesH = 0;
		sm->getOption("Scale", &scale);
		sm->getOption("PageSize", &nodes_per_side);

		//Ogre::Real worldWidth, worldHeight;

		if(Misc::GetExtension(m_TerrainConfigFile) == "cfg")
		{
			if (nodes_per_side == 0)
			{
				Ogre::ConfigFile config;
				Ogre::String val;

				config.load(full_path, "=", true);

				val = config.getSetting( "PageSize" );
				assert( !val.empty() );
				nodes_per_side = atoi( val.c_str() );

				val = config.getSetting( "PageWorldX" );
				assert( !val.empty() );
				scale.x = atof( val.c_str() ) / (nodes_per_side-1);

				val = config.getSetting( "MaxHeight" );
				assert( !val.empty() );
				scale.y = atof( val.c_str() );

				val = config.getSetting( "PageWorldZ" );
				assert( !val.empty() );
				scale.z = atof( val.c_str() ) / (nodes_per_side-1);

			}
		}

		m_Scale = Convert::ToGASS(scale);
		m_MaxHeight = m_Scale.y;
		m_WorldWidth  = m_Scale.x * (nodes_per_side-1);
		m_WorldHeight = m_Scale.z * (nodes_per_side-1);
		m_NodesPerSideAllPagesW = nodes_per_side;
		m_NodesPerSideAllPagesH = nodes_per_side;
	}


	void OgreSceneManagerTerrainComponent::GetBounds(Vec3 &min,Vec3 &max)
	{
		min.x = 0;
		min.y = 0;
		min.z = 0;

		max.x = m_WorldWidth;
		max.y = m_MaxHeight;
		max.z = m_WorldHeight;
	}

	AABox OgreSceneManagerTerrainComponent::GetBoundingBox() const
	{
		AABox aabox;
		aabox.m_Min.x = 0;
		aabox.m_Min.y = 0;
		aabox.m_Min.z = 0;

		aabox.m_Max.x = m_WorldWidth;
		aabox.m_Max.y = m_MaxHeight;
		aabox.m_Max.z = m_WorldHeight;
		return aabox;
	}

	Sphere OgreSceneManagerTerrainComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		sphere.m_Pos = Vec3(m_WorldWidth/2.0,m_MaxHeight/2.0,m_WorldHeight/2.0);
		sphere.m_Radius = sqrt(m_WorldWidth*m_WorldWidth);
		return sphere;
	}

	unsigned int OgreSceneManagerTerrainComponent::GetSamplesX()
	{
		return m_NodesPerSideAllPagesW;
	}
	unsigned int OgreSceneManagerTerrainComponent::GetSamplesZ()
	{
		return m_NodesPerSideAllPagesH;
	}

	void OgreSceneManagerTerrainComponent::pageConstructed(Ogre::TerrainSceneManager* manager, size_t pagex, size_t pagez, Ogre::Real* heightData)
	{
		Ogre::Vector3 scale = manager->getScale();
		//Get the terrain's Size
		m_HMDim = manager->getPageSize();
		float terrain_size = scale.x*m_HMDim;
		// Create vertices
		size_t vertex_size = m_HMDim * m_HMDim;
		m_HeightData = new float[vertex_size];
		Ogre::Vector3 offset(0,0,0);
		for(size_t x = 0; x < m_HMDim; x++)
		{
			for(size_t z = 0; z < m_HMDim; z++)
			{
				m_HeightData[z*m_HMDim+x] = heightData[z*m_HMDim+x]*scale.y;
			}
		}
	}

	void OgreSceneManagerTerrainComponent::GetMeshData(MeshDataPtr mesh_data)
		//(Ogre::TerrainSceneManager* manager, size_t pagex, size_t pagez, Ogre::Real* heightData)
	{
		size_t tWidth = m_HMDim;
		size_t tHeight = m_HMDim;

		//Create indices
		size_t index_size = (tWidth - 1) * (tHeight - 1) * 6;
		mesh_data->FaceVector = new unsigned int[index_size];
		mesh_data->NumFaces = index_size/3;
		for( size_t x = 0; x < tWidth - 1; x++)
		{
			for( size_t y=0; y < tHeight - 1; y++)
			{
				mesh_data->FaceVector[(x+y*(tWidth-1))*6] = x+y * tWidth;
				mesh_data->FaceVector[(x+y*(tWidth-1))*6+1] = (x+1)+y * tWidth;
				mesh_data->FaceVector[(x+y*(tWidth-1))*6+2] = (x+1)+(y+1) * tWidth;

				mesh_data->FaceVector[(x+y*(tWidth-1))*6+3] = x+(y+1) * tWidth;
				mesh_data->FaceVector[(x+y*(tWidth-1))*6+4] = x+y * tWidth;
				mesh_data->FaceVector[(x+y*(tWidth-1))*6+5] = (x+1)+(y+1) * tWidth;
			}
		}

		// Create vertices
		size_t vertex_size = tWidth * tHeight;
		mesh_data->VertexVector = new Vec3[vertex_size];
		mesh_data->NumVertex = vertex_size;
		size_t index = 0;
		Ogre::Vector3 offset(0,0,0);
		for(size_t x = 0; x < tWidth; x++)
		{
			for(size_t z = 0; z < tHeight; z++)
			{
//				m_HeightData[z*m_HMDim+x] = heightData[z*m_HMDim+x]*scale.y;
				//Ogre::Vector3 pos = Ogre::Vector3(x,heightData[z*tWidth+x],z)*scale+offset;
				Ogre::Vector3 pos = Ogre::Vector3(x*m_Scale.x,m_HeightData[z*m_HMDim+x],z*m_Scale.z)+offset;
				mesh_data->VertexVector[index] = Convert::ToGASS(pos);
				index++;
			}
		}
	}

	#ifndef LERP
		#define LERP(a, b, t) (a + (b - a) * t)
	#endif

	Float OgreSceneManagerTerrainComponent::GetHeight(Float x, Float z)
	{
		//Factor in our horizontal scaling
		x /= m_Scale.x;
		z /= m_Scale.z;

		if( x < 0 || z < 0 )
			return -1.0;
		if( (x > m_HMDim) || (z > m_HMDim) )
			return -1.0;

		//algo:
		//Find the 4 corner points of the quad that the cam is directly above
		//Find the cam's distance from these 4 points
		//Lerp or Bilerp the height value at camera point from above values
		//return value

		int x0, z0, x1, z1;
		x0 = int(x);
		x1 = x0 + 1;
		z0 = int(z);
		z1 = z0 + 1;

		//we are just slightly off the edge of the terrain if x1 or z1 is off the terrain
		if( x0 < 0.0 || z0 < 0.0 )
			return -1.0;

		if( x1 >= m_HMDim || z1 >= m_HMDim)
			return -1.0;


		Float h00, h01, h10, h11;


		h00 = m_HeightData[z0*m_HMDim+x0];
		h01 = m_HeightData[z0*m_HMDim+x1];
		h10 = m_HeightData[z1*m_HMDim+x0];
		h11 = m_HeightData[z1*m_HMDim+x1];

		Float tx, ty;
		tx = x - x0;
		ty = z - z0;


		Float height = LERP(LERP(h00, h01, tx), LERP(h10, h11, tx), ty);

		return height;
	}

	float* OgreSceneManagerTerrainComponent::GetHeightData()
	{
		return m_HeightData;
	}
}
