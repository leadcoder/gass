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
#include <boost/filesystem.hpp>
#include <OgreSceneNode.h>
#include <OgreConfigFile.h>

#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSIResourceSystem.h"
#include "Sim/GASSSimSystemManager.h"

#include "Sim/GASSSimEngine.h"
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
		m_TilingLayer3(5),
		m_TilingLayer4(5),
		m_GeomFlags(GEOMETRY_FLAG_UNKOWN)
	{

	}

	OgreTerrainPageComponent::~OgreTerrainPageComponent()
	{

	}

	void OgreTerrainPageComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("OgreTerrainPageComponent",new Creator<OgreTerrainPageComponent, IComponent>);
		RegisterProperty<std::string>("HeightMap", &GASS::OgreTerrainPageComponent::GetHeightMap, &GASS::OgreTerrainPageComponent::SetHeightMap);
		RegisterProperty<std::string>("ColorMap", &GASS::OgreTerrainPageComponent::GetColorMap, &GASS::OgreTerrainPageComponent::SetColorMap);
		RegisterProperty<std::string>("DetailMask", &GASS::OgreTerrainPageComponent::GetMask, &GASS::OgreTerrainPageComponent::SetMask);
		//RegisterProperty<std::string>("MaskLayer1", &GASS::OgreTerrainPageComponent::GetMaskLayer1, &GASS::OgreTerrainPageComponent::SetMaskLayer1);
		//RegisterProperty<std::string>("MaskLayer2", &GASS::OgreTerrainPageComponent::GetMaskLayer2, &GASS::OgreTerrainPageComponent::SetMaskLayer2);
		RegisterProperty<std::string>("DiffuseLayer0", &GASS::OgreTerrainPageComponent::GetDiffuseLayer0, &GASS::OgreTerrainPageComponent::SetDiffuseLayer0);
		RegisterProperty<std::string>("NormalLayer0", &GASS::OgreTerrainPageComponent::GetNormalLayer0, &GASS::OgreTerrainPageComponent::SetNormalLayer0);
		RegisterProperty<std::string>("DiffuseLayer1", &GASS::OgreTerrainPageComponent::GetDiffuseLayer1, &GASS::OgreTerrainPageComponent::SetDiffuseLayer1);
		RegisterProperty<std::string>("NormalLayer1", &GASS::OgreTerrainPageComponent::GetNormalLayer1, &GASS::OgreTerrainPageComponent::SetNormalLayer1);
		RegisterProperty<std::string>("DiffuseLayer2", &GASS::OgreTerrainPageComponent::GetDiffuseLayer2, &GASS::OgreTerrainPageComponent::SetDiffuseLayer2);
		RegisterProperty<std::string>("NormalLayer2", &GASS::OgreTerrainPageComponent::GetNormalLayer2, &GASS::OgreTerrainPageComponent::SetNormalLayer2);
		RegisterProperty<std::string>("DiffuseLayer3", &GASS::OgreTerrainPageComponent::GetDiffuseLayer3, &GASS::OgreTerrainPageComponent::SetDiffuseLayer3);
		RegisterProperty<std::string>("NormalLayer3", &GASS::OgreTerrainPageComponent::GetNormalLayer3, &GASS::OgreTerrainPageComponent::SetNormalLayer3);
		RegisterProperty<std::string>("DiffuseLayer4", &GASS::OgreTerrainPageComponent::GetDiffuseLayer4, &GASS::OgreTerrainPageComponent::SetDiffuseLayer4);
		RegisterProperty<std::string>("NormalLayer4", &GASS::OgreTerrainPageComponent::GetNormalLayer4, &GASS::OgreTerrainPageComponent::SetNormalLayer4);
		RegisterProperty<float>("TilingLayer0", &GASS::OgreTerrainPageComponent::GetTilingLayer0, &GASS::OgreTerrainPageComponent::SetTilingLayer0);
		RegisterProperty<float>("TilingLayer1", &GASS::OgreTerrainPageComponent::GetTilingLayer1, &GASS::OgreTerrainPageComponent::SetTilingLayer1);
		RegisterProperty<float>("TilingLayer2", &GASS::OgreTerrainPageComponent::GetTilingLayer2, &GASS::OgreTerrainPageComponent::SetTilingLayer2);
		RegisterProperty<float>("TilingLayer3", &GASS::OgreTerrainPageComponent::GetTilingLayer3, &GASS::OgreTerrainPageComponent::SetTilingLayer3);
		RegisterProperty<float>("TilingLayer4", &GASS::OgreTerrainPageComponent::GetTilingLayer3, &GASS::OgreTerrainPageComponent::SetTilingLayer4);
		
		RegisterProperty<Vec3>("Position", &GASS::OgreTerrainPageComponent::GetPosition, &GASS::OgreTerrainPageComponent::SetPosition);
		RegisterProperty<int>("IndexX", &GASS::OgreTerrainPageComponent::GetIndexX, &GASS::OgreTerrainPageComponent::SetIndexX);
		RegisterProperty<int>("IndexY", &GASS::OgreTerrainPageComponent::GetIndexY, &GASS::OgreTerrainPageComponent::SetIndexY);

		//import functions, can be used from editor, use full-path to resource and execute import
		RegisterProperty<std::string>("ImportHeightMap", &GASS::OgreTerrainPageComponent::GetImportHeightMap, &GASS::OgreTerrainPageComponent::ImportHeightMap);
		RegisterProperty<std::string>("ImportColorMap", &GASS::OgreTerrainPageComponent::GetImportColorMap, &GASS::OgreTerrainPageComponent::ImportColorMap);
		RegisterProperty<std::string>("ImportDetailMask", &GASS::OgreTerrainPageComponent::GetImportDetailMask, &GASS::OgreTerrainPageComponent::ImportDetailMask);

	}

	void OgreTerrainPageComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreTerrainPageComponent::OnTerrainLayerMessage,TerrainLayerMessage,0));
	
		OgreGraphicsSceneManagerPtr ogsm =  GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OgreGraphicsSceneManager>();
		assert(ogsm);
		m_OgreSceneManager = ogsm->GetSceneManger();
		OgreTerrainGroupComponentPtr terrain_man = GetSceneObject()->GetFirstComponentByClass<OgreTerrainGroupComponent>();

		if(!terrain_man) //try parent
			terrain_man = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<OgreTerrainGroupComponent>();
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
					if(m_HeightMapFile != "") //import height map
						ImportHeightMap(GetFromResourceSystem(m_HeightMapFile));
				}
				m_TerrainGroup->loadTerrain(m_IndexX, m_IndexY,true);
				m_Terrain = m_TerrainGroup->getTerrain(m_IndexX, m_IndexY);
				
				//m_Terrain->setRenderQueueGroup(Ogre::RENDER_QUEUE_WORLD_GEOMETRY_1);
				//m_TerrainGroup->convertTerrainSlotToWorldPosition(m_IndexX, m_IndexY, &newpos);
				//SetPosition(m_Pos);
				UpdatePosition();
				if(m_ColorMap !="")
					ImportColorMap(GetFromResourceSystem(m_ColorMap));

				if(m_Mask != "") 
					ImportDetailMask(GetFromResourceSystem(m_Mask));

				if(m_DiffuseLayer0 != "")
					SetDiffuseLayer0(m_DiffuseLayer0);

				if(m_NormalLayer0 != "")
					SetNormalLayer0(m_NormalLayer0);

				if(m_TilingLayer0)
					SetTilingLayer0(m_TilingLayer0);

				if(m_DiffuseLayer1 != "")
					SetDiffuseLayer1(m_DiffuseLayer1);

				if(m_NormalLayer1 != "")
					SetNormalLayer1(m_NormalLayer1);

				if(m_TilingLayer1)
					SetTilingLayer1(m_TilingLayer1);

				if(m_DiffuseLayer2 != "")
					SetDiffuseLayer2(m_DiffuseLayer2);

				if(m_NormalLayer2 != "")
					SetNormalLayer2(m_NormalLayer2);

				if(m_TilingLayer2)
					SetTilingLayer2(m_TilingLayer2);

				if(m_DiffuseLayer3 != "")
					SetDiffuseLayer3(m_DiffuseLayer3);

				if(m_NormalLayer3 != "")
				{
					SetNormalLayer3(m_NormalLayer3);
				}

				if(m_TilingLayer3)
					SetTilingLayer3(m_TilingLayer3);

				if(m_DiffuseLayer4 != "")
					SetDiffuseLayer4(m_DiffuseLayer4);

				if(m_NormalLayer4 != "")
					SetNormalLayer4(m_NormalLayer4);

				if(m_TilingLayer4)
					SetTilingLayer4(m_TilingLayer4);
				
				//std::cout << "load world size:" << m_TerrainGroup->getTerrainWorldSize() << "\n";
				//std::cout << "load size:" << m_Terrain->getWorldSize() << "\n";
				//std::cout << "bb size:" << m_Terrain->getAABB().getMaximum().x <<" "<< m_Terrain->getAABB().getMaximum().y <<" " << m_Terrain->getAABB().getMaximum().z << "\n";
				//Ogre::MaterialPtr ptr = m_Terrain->getMaterial();
				//std::string name = ptr->getName();	
				//Terrain page created
			}
		}
	}

	void OgreTerrainPageComponent::OnDelete()
	{
		if(m_TerrainGroup)
		{
			m_TerrainGroup->removeTerrain(m_IndexX, m_IndexY);
		}
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

	void OgreTerrainPageComponent::SetHeightMap(const std::string &filename) 
	{
		m_HeightMapFile = filename;
	}

	void OgreTerrainPageComponent::OnTerrainLayerMessage(TerrainLayerMessagePtr message)
	{
		switch(	message->GetLayer())
		{
		case TL_1:
			SetDiffuseLayer1(message->GetTexture());
			SetTilingLayer1(message->GetTiling());
			break;
		case TL_2:
			SetDiffuseLayer2(message->GetTexture());
			SetTilingLayer2(message->GetTiling());
			break;
		case TL_3:
			SetDiffuseLayer3(message->GetTexture());
			SetTilingLayer3(message->GetTiling());
			break;
		case TL_4:
			SetDiffuseLayer4(message->GetTexture());
			SetTilingLayer4(message->GetTiling());
			break;
		}
	}

	std::string OgreTerrainPageComponent::GetFromResourceSystem(const std::string &filename)
	{
		boost::filesystem::path boost_path(filename);
		if(!boost::filesystem::exists(filename))
		{
			//try get resource from ResourceSystem
			IResourceSystem* rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>().get();
			std::string full_path;
			if(!rs->GetFullPath(filename,full_path))
			{
				return std::string("");
			}
			return full_path;
		}
		return std::string("");
	}

	void OgreTerrainPageComponent::ImportHeightMap(const std::string &filename)
	{
		if(m_OgreSceneManager && filename != "")
		{
			std::fstream fstr(filename.c_str(), std::ios::in|std::ios::binary);
			Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));

			Ogre::Image img;
			img.load(stream);

			m_TerrainGroup->defineTerrain(m_IndexX, m_IndexY, &img);

			// sync load since we want everything in place when we start
			m_TerrainGroup->loadAllTerrains(true);
			GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(boost::shared_dynamic_cast<IGeometryComponent>(shared_from_this()))));
		}
	}

	

	std::string OgreTerrainPageComponent::GetFilename() const
	{
		std::stringstream ss;
		ss << "OgreTerrainPageComponent_" << m_IndexX << "_" << m_IndexY;
		return ss.str();
	}

	void OgreTerrainPageComponent::LoadFromFile()
	{
		m_TerrainGroup->defineTerrain(m_IndexX, m_IndexY);
		m_TerrainGroup->loadTerrain(m_IndexX, m_IndexY,true);
		m_Terrain = m_TerrainGroup->getTerrain(m_IndexX, m_IndexY);
	}

	void OgreTerrainPageComponent::SetColorMap(const std::string &filename)
	{
		std::string color_filename = GetFromResourceSystem(filename);
		if(color_filename != "")
		{
			m_ColorMap = filename;
		}
		else 
			color_filename = filename;
		ImportColorMap(color_filename);
	}

	void OgreTerrainPageComponent::ImportColorMap(const std::string &filename)
	{
		if(m_Terrain && filename != "")
		{

			//std::cout << "try to load" << colormap << "\n";
			std::fstream fstr(filename.c_str(), std::ios::in|std::ios::binary);
			Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));

			Ogre::Image colourMap;
			colourMap.load(stream);
			m_Terrain->setGlobalColourMapEnabled(true);
			//colourMap.load(m_ColorMap, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			m_Terrain->getGlobalColourMap()->unload();
			m_Terrain->getGlobalColourMap()->loadImage(colourMap);


			m_Terrain->setGlobalColourMapEnabled(true);
			/*m_Terrain->getGlobalColourMap()->unload();
			Ogre::TexturePtr colourMap  = m_Terrain->getGlobalColourMap();
			Ogre::Image colourMapImage;
			colourMapImage.load(m_ColorMap,m_Terrain->getResourceGroup());
			m_Terrain->getGlobalColourMap()->setUsage(Ogre::TU_STATIC | Ogre::TU_AUTOMIPMAP);
			m_Terrain->getGlobalColourMap()->setNumMipmaps(10);
			m_Terrain->getGlobalColourMap()->loadImage(colourMapImage);*/

		}
	}

	std::string OgreTerrainPageComponent::GetColorMap() const 
	{
		return m_ColorMap;
	}

	void OgreTerrainPageComponent::SetDiffuseLayer0(const std::string &diffuse)
	{
		m_DiffuseLayer0 = diffuse;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_DiffuseLayer0 != "")
		{
			m_Terrain->setLayerTextureName(0,0,m_DiffuseLayer0);
		}
	}

	std::string OgreTerrainPageComponent::GetDiffuseLayer0() const
	{
		return m_DiffuseLayer0;
	}

	void OgreTerrainPageComponent::SetNormalLayer0(const std::string &normal)
	{
		m_NormalLayer0 = normal;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_NormalLayer0 != "")
		{
			m_Terrain->setLayerTextureName(0,1,m_NormalLayer0);
		}
	}

	std::string OgreTerrainPageComponent::GetNormalLayer0() const
	{
		return m_NormalLayer0;
	}



	void OgreTerrainPageComponent::SetDiffuseLayer1(const std::string &diffuse)
	{
		m_DiffuseLayer1 = diffuse;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_DiffuseLayer1 != "")
		{
			m_Terrain->setLayerTextureName(1,0,m_DiffuseLayer1);
		}
	}

	std::string OgreTerrainPageComponent::GetDiffuseLayer1() const
	{
		return m_DiffuseLayer1;
	}

	void OgreTerrainPageComponent::SetNormalLayer1(const std::string &normal)
	{
		m_NormalLayer1 = normal;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_NormalLayer1 != "")
		{
			m_Terrain->setLayerTextureName(1,1,m_NormalLayer1);
		}
	}

	std::string OgreTerrainPageComponent::GetNormalLayer1() const
	{
		return m_NormalLayer1;
	}

	void OgreTerrainPageComponent::SetDiffuseLayer2(const std::string &diffuse)
	{
		m_DiffuseLayer2 = diffuse;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_DiffuseLayer2 != "")
		{
			m_Terrain->setLayerTextureName(2,0,m_DiffuseLayer2);
		}
	}

	std::string OgreTerrainPageComponent::GetDiffuseLayer2() const
	{
		return m_DiffuseLayer2;
	}

	void OgreTerrainPageComponent::SetNormalLayer2(const std::string &normal)
	{
		m_NormalLayer2 = normal;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_NormalLayer2 != "")
		{
			m_Terrain->setLayerTextureName(2,1,m_NormalLayer2);
		}
	}

	std::string OgreTerrainPageComponent::GetNormalLayer2() const
	{
		return m_NormalLayer2;
	}


	void OgreTerrainPageComponent::SetDiffuseLayer3(const std::string &diffuse)
	{
		m_DiffuseLayer3 = diffuse;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_DiffuseLayer3 != "")
		{
			m_Terrain->setLayerTextureName(3,0,m_DiffuseLayer3);
		}
	}

	std::string OgreTerrainPageComponent::GetDiffuseLayer3() const
	{
		return m_DiffuseLayer3;
	}

	void OgreTerrainPageComponent::SetNormalLayer3(const std::string &normal)
	{
		m_NormalLayer3 = normal;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_NormalLayer3 != "")
		{
			m_Terrain->setLayerTextureName(3,1,m_NormalLayer3);
		}
	}

	std::string OgreTerrainPageComponent::GetNormalLayer3() const
	{
		return m_NormalLayer3;
	}

	void OgreTerrainPageComponent::SetDiffuseLayer4(const std::string &diffuse)
	{
		m_DiffuseLayer4 = diffuse;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_DiffuseLayer4 != "")
		{
			m_Terrain->setLayerTextureName(4,0,m_DiffuseLayer4);
		}
	}

	std::string OgreTerrainPageComponent::GetDiffuseLayer4() const
	{
		return m_DiffuseLayer4;
	}

	void OgreTerrainPageComponent::SetNormalLayer4(const std::string &normal)
	{
		m_NormalLayer4 = normal;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_NormalLayer4 != "")
		{
			m_Terrain->setLayerTextureName(4,1,m_NormalLayer4);
		}
	}

	std::string OgreTerrainPageComponent::GetNormalLayer4() const
	{
		return m_NormalLayer4;
	}

	void OgreTerrainPageComponent::SetMask(const std::string &mask)
	{
		m_Mask = mask;//Misc::GetFilename(mask);
	}

	
	void OgreTerrainPageComponent::ImportDetailMask(const std::string &mask)
	{
		if(m_Terrain && mask != "")
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
			float *blend_data_3 = m_Terrain->getLayerBlendMap(3)->getBlendPointer();

			for(int y = 0;y < m_Terrain->getLayerBlendMapSize();y++)
			{
				for(int x = 0;x < m_Terrain->getLayerBlendMapSize();x++)
				{
					cval = img.getColourAt(x, y, 0);
					*blend_data_1  = cval.r;
					*blend_data_2  = cval.g;
					*blend_data_3  = cval.b;
					++blend_data_1;
					++blend_data_2;
					++blend_data_3;
				}
			}
			Ogre::Rect drect(0, 0, m_Terrain->getLayerBlendMapSize(), m_Terrain->getLayerBlendMapSize());
			m_Terrain->getLayerBlendMap(1)->dirtyRect(drect);
			m_Terrain->getLayerBlendMap(1)->update();
			m_Terrain->getLayerBlendMap(2)->dirtyRect(drect);
			m_Terrain->getLayerBlendMap(2)->update();
			m_Terrain->getLayerBlendMap(3)->dirtyRect(drect);
			m_Terrain->getLayerBlendMap(3)->update();
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

	void OgreTerrainPageComponent::SetTilingLayer3(float value)
	{
		m_TilingLayer3 = value;
		if(m_Terrain)
		{
			m_Terrain->setLayerWorldSize(3,value);
		}
	}

	float OgreTerrainPageComponent::GetTilingLayer3() const
	{
		return m_TilingLayer3;
	}


	void OgreTerrainPageComponent::SetTilingLayer4(float value)
	{
		m_TilingLayer4 = value;
		if(m_Terrain)
		{
			m_Terrain->setLayerWorldSize(4,value);
		}
	}

	float OgreTerrainPageComponent::GetTilingLayer4() const
	{
		return m_TilingLayer4;
	}


	AABox OgreTerrainPageComponent::GetBoundingBox() const
	{
		AABox aabox;
		if(m_TerrainGroup)
		{
			aabox = Convert::ToGASS(m_Terrain->getAABB());
			//Hack to get terrain size before loaded
			Float size = m_TerrainGroup->getTerrainWorldSize()*0.5;
			Vec3 pos = GetPosition();
			aabox.m_Min = Vec3(pos.x -size,aabox.m_Min.y, pos.z -size);
			aabox.m_Max = Vec3(pos.x +size,aabox.m_Max.y, pos.z +size);
		}
		return aabox;
	}

	Sphere OgreTerrainPageComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		if(m_Terrain)
		{
			sphere.m_Pos = Convert::ToGASS(m_Terrain->getPosition()) + GetPosition();
			sphere.m_Radius = m_Terrain->getBoundingRadius();
		}
		return sphere;
	}

	unsigned int OgreTerrainPageComponent::GetSamplesX() const
	{
		OgreTerrainGroupComponentPtr terrain_man = GetSceneObject()->GetFirstComponentByClass<OgreTerrainGroupComponent>();
		if(!terrain_man) //try parent
			terrain_man = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<OgreTerrainGroupComponent>();
		if(terrain_man) //try parent
			return terrain_man->GetImportTerrainSize();
		else return 0;
	}

	unsigned int OgreTerrainPageComponent::GetSamplesZ() const
	{
		return GetSamplesX();
	}


	void OgreTerrainPageComponent::GetMeshData(MeshDataPtr mesh_data) const
	{

		if(!m_Terrain)
			return;
		size_t tWidth = m_Terrain->getSize();
		size_t tHeight = m_Terrain->getSize();

		//Create indices
		size_t index_size = (tWidth - 1) * (tHeight - 1) * 6;
		mesh_data->FaceVector = new unsigned int[index_size];
		mesh_data->NumFaces = index_size/3;
		for( size_t x = 0; x < tWidth - 1; x++)
		{
			for( size_t y=0; y < tHeight - 1; y++)
			{
				mesh_data->FaceVector[(x+y*(tWidth-1))*6+2] = x+y * tWidth;
				mesh_data->FaceVector[(x+y*(tWidth-1))*6+1] = (x+1)+y * tWidth;
				mesh_data->FaceVector[(x+y*(tWidth-1))*6] = (x+1)+(y+1) * tWidth;
				
				mesh_data->FaceVector[(x+y*(tWidth-1))*6+5] = x+(y+1) * tWidth;
				mesh_data->FaceVector[(x+y*(tWidth-1))*6+4] = x+y * tWidth;
				mesh_data->FaceVector[(x+y*(tWidth-1))*6+3] = (x+1)+(y+1) * tWidth;

			}
		}

		// Create vertices
		size_t vertex_size = tWidth * tHeight;
		mesh_data->VertexVector = new Vec3[vertex_size];
		mesh_data->NumVertex = vertex_size;
		size_t index = 0;
		
		Vec3 center = Convert::ToGASS(m_Terrain->getPosition());
		Vec3 offset = center - Vec3(m_Terrain->getWorldSize()/2.0,0,-m_Terrain->getWorldSize()/2.0);

		Float scale = m_Terrain->getWorldSize()/(m_Terrain->getSize()-1);
		for(size_t x = 0; x < tWidth; x++)
		{
			for(size_t z = 0; z < tHeight; z++)
			{
				/*Ogre::Real wx, wy, wz;
				wx = Ogre::Real(x)/(m_Terrain->getSize()-1);
				wy = 0;
				wz = Ogre::Real(z)/(m_Terrain->getSize()-1);
				Ogre::Vector3 outWSpos;
				m_Terrain->getPositionAlign(wx,wz,wy, Ogre::Terrain::ALIGN_X_Z, &outWSpos);*/
				//Ogre::Vector3 terrain_space_pos; 
				//void getPosition(Real x, Real y, Real z, &terrain_space_pos);
				//Ogre::Vector3 pos = Ogre::Vector3(x*m_Scale.x,m_HeightData[z*m_HMDim+x],z*m_Scale.z)+offset;
				Float fx = x;
				Float fz = z;
				mesh_data->VertexVector[index] = Vec3(fx*scale, m_Terrain->getHeightAtPoint(x,z),-fz * scale) + offset;
				index++;
			}
		}
	}

	Float OgreTerrainPageComponent::GetHeight(Float x, Float z) const
	{
		if(m_Terrain)
			return m_Terrain->getHeightAtWorldPosition(x,10000,z);
		return 0;
	}

	float* OgreTerrainPageComponent::GetHeightData() const
	{
		if(m_Terrain)
			return m_Terrain->getHeightData();
		return NULL;
	}


	void OgreTerrainPageComponent::UpdatePosition()
	{
		if(m_Terrain)
		{
			GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(boost::shared_dynamic_cast<IGeometryComponent>(shared_from_this()))));
		}
	}

	void OgreTerrainPageComponent::SetPosition(const Vec3 &pos)
	{
		/*	m_Pos = pos;
		if(m_Terrain)
		{
		m_Terrain->setPosition(Convert::ToOgre(pos));
		GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(shared_from_this())));
		}*/
	}

	Vec3 OgreTerrainPageComponent::GetPosition() const
	{
		Vec3 pos(0,0,0);
		if(m_TerrainGroup)
			pos= Convert::ToGASS(m_TerrainGroup->getOrigin());
		return pos;
	}

	GeometryFlags OgreTerrainPageComponent::GetGeometryFlags() const
	{
		return m_GeomFlags;
	}

	void OgreTerrainPageComponent::SetGeometryFlags(GeometryFlags flags)
	{
		m_GeomFlags = flags;
	}

}
