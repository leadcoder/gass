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
#include "Plugins/Ogre/Components/OgreTerrainPageComponent.h"
#include "Plugins/Ogre/Components/OgreTerrainGroupComponent.h"

#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include "Plugins/Ogre/OgreConvert.h"


namespace GASS
{
	OgreTerrainPageComponent::OgreTerrainPageComponent() : m_CreateCollisionMesh (true),
		m_OgreSceneManager(NULL),
		m_IndexX(0),
		m_IndexY(0),
		m_Terrain(NULL),
		m_TerrainGroup(NULL),
		m_TilingLayer0(5),
		m_TilingLayer1(5),
		m_TilingLayer2(5),
		m_Pos(0,0,0)
	{

	}

	OgreTerrainPageComponent::~OgreTerrainPageComponent()
	{

	}

	void OgreTerrainPageComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("OgreTerrainPageComponent",new Creator<OgreTerrainPageComponent, IComponent>);
		RegisterProperty<std::string>("ImportTerrain", &GASS::OgreTerrainPageComponent::GetFilename, &GASS::OgreTerrainPageComponent::SetFilename);
		RegisterProperty<std::string>("ColorMap", &GASS::OgreTerrainPageComponent::GetColorMap, &GASS::OgreTerrainPageComponent::SetColorMap);
		RegisterProperty<std::string>("DiffuseLayer0", &GASS::OgreTerrainPageComponent::GetDiffuseLayer0, &GASS::OgreTerrainPageComponent::SetDiffuseLayer0);
		RegisterProperty<std::string>("DiffuseLayer1", &GASS::OgreTerrainPageComponent::GetDiffuseLayer1, &GASS::OgreTerrainPageComponent::SetDiffuseLayer1);
		RegisterProperty<std::string>("DiffuseLayer2", &GASS::OgreTerrainPageComponent::GetDiffuseLayer2, &GASS::OgreTerrainPageComponent::SetDiffuseLayer2);
		RegisterProperty<float>("TilingLayer0", &GASS::OgreTerrainPageComponent::GetTilingLayer0, &GASS::OgreTerrainPageComponent::SetTilingLayer0);
		RegisterProperty<float>("TilingLayer1", &GASS::OgreTerrainPageComponent::GetTilingLayer1, &GASS::OgreTerrainPageComponent::SetTilingLayer1);
		RegisterProperty<float>("TilingLayer2", &GASS::OgreTerrainPageComponent::GetTilingLayer2, &GASS::OgreTerrainPageComponent::SetTilingLayer2);
		RegisterProperty<std::string>("MaskLayer1", &GASS::OgreTerrainPageComponent::GetMaskLayer1, &GASS::OgreTerrainPageComponent::SetMaskLayer1);
		RegisterProperty<std::string>("MaskLayer2", &GASS::OgreTerrainPageComponent::GetMaskLayer2, &GASS::OgreTerrainPageComponent::SetMaskLayer2);
		RegisterProperty<Vec3>("Position", &GASS::OgreTerrainPageComponent::GetPosition, &GASS::OgreTerrainPageComponent::SetPosition);
		RegisterProperty<int>("IndexX", &GASS::OgreTerrainPageComponent::GetIndexX, &GASS::OgreTerrainPageComponent::SetIndexX);
		RegisterProperty<int>("IndexY", &GASS::OgreTerrainPageComponent::GetIndexY, &GASS::OgreTerrainPageComponent::SetIndexY);
	}


	int OgreTerrainPageComponent::GetIndexX() const
	{
		return m_IndexX;
	}

	void OgreTerrainPageComponent::SetIndexX(int index) 
	{
		m_IndexX=index;
	}


	int OgreTerrainPageComponent::GetIndexY() const
	{
		return m_IndexY;
	}


	void OgreTerrainPageComponent::SetIndexY(int index) 
	{
		m_IndexY=index;
	}

	void OgreTerrainPageComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreTerrainPageComponent::OnLoad,LoadGFXComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreTerrainPageComponent::OnUnload,UnloadComponentsMessage,0));
	}

	void OgreTerrainPageComponent::SetFilename(const std::string &filename) 
	{
		m_TerrainConfigFile = filename;
		ImportTerrain(m_TerrainConfigFile);
	}

	void OgreTerrainPageComponent::ImportTerrain(const std::string &filename)
	{
		if(m_OgreSceneManager && filename != "")
		{
			//unload previous terrain
			//IResourceSystem* rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>().get();
			std::fstream fstr(filename.c_str(), std::ios::in|std::ios::binary);
			Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));

			Ogre::Image img;
			img.load(stream);
			/*std::string full_path;
			if(!rs->GetFullPath(filename,full_path))
			{
			Log::Warning("Faild to load terrain %s",filename.c_str());
			return;
			}
			Ogre::Image img;
			img.load(full_path, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);*/

			m_TerrainGroup->defineTerrain(m_IndexX, m_IndexY, &img);

			// sync load since we want everything in place when we start
			m_TerrainGroup->loadAllTerrains(true);
			GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(shared_from_this())));
		}
	}

	void OgreTerrainPageComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		OgreGraphicsSceneManagerPtr ogsm = boost::shared_static_cast<OgreGraphicsSceneManager>(message->GetGFXSceneManager());
		assert(ogsm);
		m_OgreSceneManager = ogsm->GetSceneManger();
		OgreTerrainGroupComponentPtr terrain_man = GetSceneObject()->GetFirstComponent<OgreTerrainGroupComponent>();

		if(!terrain_man) //try parent
			terrain_man = GetSceneObject()->GetParentSceneObject()->GetFirstComponent<OgreTerrainGroupComponent>();
		if(terrain_man)
		{
			m_TerrainGroup = terrain_man->GetTerrainGroup();
			if(m_TerrainGroup)
			{
				Ogre::String filename = m_TerrainGroup->generateFilename(m_IndexX, m_IndexY);
				if (Ogre::ResourceGroupManager::getSingleton().resourceExists(m_TerrainGroup->getResourceGroup(), filename))
				{
					m_TerrainGroup->defineTerrain(m_IndexX, m_IndexY);
				}
				else
				{
					m_TerrainGroup->defineTerrain(m_IndexX, m_IndexY, 0.0f);
				}
				m_TerrainGroup->loadTerrain(m_IndexX, m_IndexY);
				m_Terrain = m_TerrainGroup->getTerrain(m_IndexX, m_IndexY);
				m_Terrain->setRenderQueueGroup(Ogre::RENDER_QUEUE_WORLD_GEOMETRY_1);
				//m_TerrainGroup->convertTerrainSlotToWorldPosition(m_IndexX, m_IndexY, &newpos);
				SetPosition(m_Pos);
				
			}
		}
	}

	void OgreTerrainPageComponent::LoadFromFile()
	{
		m_TerrainGroup->defineTerrain(m_IndexX, m_IndexY);
		m_TerrainGroup->loadTerrain(m_IndexX, m_IndexY,true);
		m_Terrain = m_TerrainGroup->getTerrain(m_IndexX, m_IndexY);
	}

	void OgreTerrainPageComponent::SetColorMap(const std::string &colormap)
	{
		m_ColorMap = Misc::GetFilename(colormap);
		if(m_Terrain && m_ColorMap != "")
		{

			std::fstream fstr(colormap.c_str(), std::ios::in|std::ios::binary);
			Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));

			Ogre::Image colourMap;
			colourMap.load(stream);
			m_Terrain->setGlobalColourMapEnabled(true);
			//colourMap.load(m_ColorMap, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			m_Terrain->getGlobalColourMap()->loadImage(colourMap);
		}
	}

	std::string OgreTerrainPageComponent::GetColorMap() const 
	{
		return m_ColorMap;
	}

	void OgreTerrainPageComponent::SetDiffuseLayer0(const std::string &diffuse)
	{
		m_DiffuseLayer0 = Misc::GetFilename(diffuse);
		if(m_Terrain && m_DiffuseLayer0 != "")
		{
			m_Terrain->setLayerTextureName(0,0,m_DiffuseLayer0);
		}
	}

	std::string OgreTerrainPageComponent::GetDiffuseLayer0() const
	{
		return m_DiffuseLayer0;
	}

	void OgreTerrainPageComponent::SetDiffuseLayer1(const std::string &diffuse)
	{
		m_DiffuseLayer1 = Misc::GetFilename(diffuse);
		if(m_Terrain && m_DiffuseLayer1 != "")
		{
			m_Terrain->setLayerTextureName(1,0,m_DiffuseLayer1);
		}
	}

	std::string OgreTerrainPageComponent::GetDiffuseLayer1() const
	{
		return m_DiffuseLayer1;
	}

	void OgreTerrainPageComponent::SetDiffuseLayer2(const std::string &diffuse)
	{
		m_DiffuseLayer2 = Misc::GetFilename(diffuse);
		if(m_Terrain && m_DiffuseLayer2 != "")
		{
			m_Terrain->setLayerTextureName(2,0,m_DiffuseLayer2);
		}
	}

	std::string OgreTerrainPageComponent::GetDiffuseLayer2() const
	{
		return m_DiffuseLayer2;
	}


	void OgreTerrainPageComponent::SetMask(const std::string &mask)
	{
		m_Mask = Misc::GetFilename(mask);
		if(m_Terrain && m_Mask != "")
		{
			std::fstream fstr(mask.c_str(), std::ios::in|std::ios::binary);
			Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));

			Ogre::Image img;
			img.load(stream);
			img.resize(m_Terrain->getLayerBlendMapSize(), m_Terrain->getLayerBlendMapSize());

			Ogre::ColourValue cval;

			//mBlendMap = m_Terrain->getLayerBlendMap(layerID);
			float *blend_data_1 = m_Terrain->getLayerBlendMap(1)->getBlendPointer();
			float *blend_data_2 = m_Terrain->getLayerBlendMap(2)->getBlendPointer();

			for(int y = 0;y < m_Terrain->getLayerBlendMapSize();y++)
			{
				for(int x = 0;x < m_Terrain->getLayerBlendMapSize();x++)
				{
					cval = img.getColourAt(x, y, 0);
					*blend_data_1  = cval.g;
					*blend_data_2  = cval.b;
					++blend_data_1;
					++blend_data_2;
				}
			}
			Ogre::Rect drect(0, 0, m_Terrain->getLayerBlendMapSize(), m_Terrain->getLayerBlendMapSize());
			m_Terrain->getLayerBlendMap(1)->dirtyRect(drect);
			m_Terrain->getLayerBlendMap(1)->update();
			m_Terrain->getLayerBlendMap(2)->dirtyRect(drect);
			m_Terrain->getLayerBlendMap(2)->update();
		}
	}

	std::string OgreTerrainPageComponent::GetMask() const
	{
		return m_Mask;
	}

	void OgreTerrainPageComponent::SetMaskLayer1(const std::string &mask)
	{
		m_MaskLayer1 = Misc::GetFilename(mask);
		if(m_Terrain && m_MaskLayer1 != "")
		{

			//m_Terrain->getLayerBlendMap(1)->loadImage(m_MaskLayer1,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

			std::fstream fstr(mask.c_str(), std::ios::in|std::ios::binary);
			Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));

			Ogre::Image img;
			img.load(stream);
			img.resize(m_Terrain->getLayerBlendMapSize(), m_Terrain->getLayerBlendMapSize());

			Ogre::ColourValue cval;

			//mBlendMap = m_Terrain->getLayerBlendMap(layerID);
			float *blend_data = m_Terrain->getLayerBlendMap(1)->getBlendPointer();

			for(int y = 0;y < m_Terrain->getLayerBlendMapSize();y++)
			{
				for(int x = 0;x < m_Terrain->getLayerBlendMapSize();x++)
				{
					cval = img.getColourAt(x, y, 0);
					*blend_data  = cval.r;
					++blend_data ;
				}
			}
			Ogre::Rect drect(0, 0, m_Terrain->getLayerBlendMapSize(), m_Terrain->getLayerBlendMapSize());
			m_Terrain->getLayerBlendMap(1)->dirtyRect(drect);
			m_Terrain->getLayerBlendMap(1)->update();
		}
	}

	std::string OgreTerrainPageComponent::GetMaskLayer1() const
	{
		return m_MaskLayer1;
	}

	void OgreTerrainPageComponent::SetMaskLayer2(const std::string &mask)
	{
		m_MaskLayer2 = Misc::GetFilename(mask);
		if(m_Terrain && m_MaskLayer2 != "")
		{
			//m_Terrain->getLayerBlendMap(1)->loadImage(m_MaskLayer1,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			std::fstream fstr(mask.c_str(), std::ios::in|std::ios::binary);
			Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));

			Ogre::Image img;
			img.load(stream);
			img.resize(m_Terrain->getLayerBlendMapSize(), m_Terrain->getLayerBlendMapSize());

			Ogre::ColourValue cval;

			//mBlendMap = m_Terrain->getLayerBlendMap(layerID);
			float *blend_data = m_Terrain->getLayerBlendMap(2)->getBlendPointer();

			for(int y = 0;y < m_Terrain->getLayerBlendMapSize();y++)
			{
				for(int x = 0;x < m_Terrain->getLayerBlendMapSize();x++)
				{
					cval = img.getColourAt(x, y, 0);
					*blend_data  = cval.r;
					++blend_data ;
				}
			}
			Ogre::Rect drect(0, 0, m_Terrain->getLayerBlendMapSize(), m_Terrain->getLayerBlendMapSize());
			m_Terrain->getLayerBlendMap(2)->dirtyRect(drect);
			m_Terrain->getLayerBlendMap(2)->update();
		}
	}

	std::string OgreTerrainPageComponent::GetMaskLayer2() const
	{
		return m_MaskLayer2;
	}

	void OgreTerrainPageComponent::SetTilingLayer0(float value)
	{
		m_TilingLayer0 = value;
		if(m_Terrain)
		{
			m_Terrain->setLayerWorldSize(0,value);
		}
	}

	float OgreTerrainPageComponent::GetTilingLayer0() const
	{
		return m_TilingLayer0;
	}


	void OgreTerrainPageComponent::SetTilingLayer1(float value)
	{
		m_TilingLayer1 = value;
		if(m_Terrain)
		{
			m_Terrain->setLayerWorldSize(1,value);
		}
	}

	float OgreTerrainPageComponent::GetTilingLayer1() const
	{
		return m_TilingLayer1;
	}

	void OgreTerrainPageComponent::SetTilingLayer2(float value)
	{
		m_TilingLayer2 = value;
		if(m_Terrain)
		{
			m_Terrain->setLayerWorldSize(2,value);
		}
	}

	float OgreTerrainPageComponent::GetTilingLayer2() const
	{
		return m_TilingLayer2;
	}
	void OgreTerrainPageComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		if(m_TerrainGroup)
		{
			m_TerrainGroup->removeTerrain(m_IndexX, m_IndexY);
		}
	}

	void OgreTerrainPageComponent::GetBounds(Vec3 &min,Vec3 &max)
	{
		AABox aabox;
		if(m_Terrain)
			aabox = Convert::ToGASS(m_Terrain->getAABB());
		min = aabox.m_Min + m_Pos;
		max = aabox.m_Max + m_Pos;
	}

	AABox OgreTerrainPageComponent::GetBoundingBox() const
	{
		AABox aabox;
		
		if(m_Terrain)
			aabox = Convert::ToGASS(m_Terrain->getAABB());

		aabox.m_Min += m_Pos;
		aabox.m_Max += m_Pos;

		return aabox;
	}

	Sphere OgreTerrainPageComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		if(m_Terrain)
		{
			sphere.m_Pos = Convert::ToGASS(m_Terrain->getPosition()) + m_Pos;
			sphere.m_Radius = m_Terrain->getBoundingRadius();
		}
		return sphere;
	}

	unsigned int OgreTerrainPageComponent::GetSamplesX()
	{
		if(m_Terrain)
			return m_Terrain->getSize();
		else return 0;
	}

	unsigned int OgreTerrainPageComponent::GetSamplesZ()
	{
		if(m_Terrain)
			return m_Terrain->getSize();
		else return 0;
	}

	void OgreTerrainPageComponent::GetMeshData(MeshDataPtr mesh_data)
	{

	}

	Float OgreTerrainPageComponent::GetHeight(Float x, Float z)
	{
		if(m_Terrain)
			return m_Terrain->getHeightAtWorldPosition(x,10000,z);
		return 0;
	}

	void OgreTerrainPageComponent::SetPosition(const Vec3 &pos)
	{
		m_Pos = pos;
		if(m_Terrain)
		{
			m_Terrain->setPosition(Convert::ToOgre(pos));
			GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(shared_from_this())));
		}
	}

	Vec3 OgreTerrainPageComponent::GetPosition() const
	{
		return m_Pos;
	}

	float* OgreTerrainPageComponent::GetHeightData()
	{
			if(m_Terrain)
				return m_Terrain->getHeightData();
			return NULL;
	}
}
