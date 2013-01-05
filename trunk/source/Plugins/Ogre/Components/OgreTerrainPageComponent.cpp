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
		RegisterProperty<Resource>("HeightMap", &GASS::OgreTerrainPageComponent::GetHeightMap, &GASS::OgreTerrainPageComponent::SetHeightMap);
		RegisterProperty<Resource>("ColorMap", &GASS::OgreTerrainPageComponent::GetColorMap, &GASS::OgreTerrainPageComponent::SetColorMap);
		RegisterProperty<Resource>("DetailMask", &GASS::OgreTerrainPageComponent::GetMask, &GASS::OgreTerrainPageComponent::SetMask);
		RegisterProperty<Resource>("DiffuseLayer0", &GASS::OgreTerrainPageComponent::GetDiffuseLayer0, &GASS::OgreTerrainPageComponent::SetDiffuseLayer0);
		RegisterProperty<Resource>("NormalLayer0", &GASS::OgreTerrainPageComponent::GetNormalLayer0, &GASS::OgreTerrainPageComponent::SetNormalLayer0);
		RegisterProperty<Resource>("DiffuseLayer1", &GASS::OgreTerrainPageComponent::GetDiffuseLayer1, &GASS::OgreTerrainPageComponent::SetDiffuseLayer1);
		RegisterProperty<Resource>("NormalLayer1", &GASS::OgreTerrainPageComponent::GetNormalLayer1, &GASS::OgreTerrainPageComponent::SetNormalLayer1);
		RegisterProperty<Resource>("DiffuseLayer2", &GASS::OgreTerrainPageComponent::GetDiffuseLayer2, &GASS::OgreTerrainPageComponent::SetDiffuseLayer2);
		RegisterProperty<Resource>("NormalLayer2", &GASS::OgreTerrainPageComponent::GetNormalLayer2, &GASS::OgreTerrainPageComponent::SetNormalLayer2);
		RegisterProperty<Resource>("DiffuseLayer3", &GASS::OgreTerrainPageComponent::GetDiffuseLayer3, &GASS::OgreTerrainPageComponent::SetDiffuseLayer3);
		RegisterProperty<Resource>("NormalLayer3", &GASS::OgreTerrainPageComponent::GetNormalLayer3, &GASS::OgreTerrainPageComponent::SetNormalLayer3);
		RegisterProperty<Resource>("DiffuseLayer4", &GASS::OgreTerrainPageComponent::GetDiffuseLayer4, &GASS::OgreTerrainPageComponent::SetDiffuseLayer4);
		RegisterProperty<Resource>("NormalLayer4", &GASS::OgreTerrainPageComponent::GetNormalLayer4, &GASS::OgreTerrainPageComponent::SetNormalLayer4);
		RegisterProperty<float>("TilingLayer0", &GASS::OgreTerrainPageComponent::GetTilingLayer0, &GASS::OgreTerrainPageComponent::SetTilingLayer0);
		RegisterProperty<float>("TilingLayer1", &GASS::OgreTerrainPageComponent::GetTilingLayer1, &GASS::OgreTerrainPageComponent::SetTilingLayer1);
		RegisterProperty<float>("TilingLayer2", &GASS::OgreTerrainPageComponent::GetTilingLayer2, &GASS::OgreTerrainPageComponent::SetTilingLayer2);
		RegisterProperty<float>("TilingLayer3", &GASS::OgreTerrainPageComponent::GetTilingLayer3, &GASS::OgreTerrainPageComponent::SetTilingLayer3);
		RegisterProperty<float>("TilingLayer4", &GASS::OgreTerrainPageComponent::GetTilingLayer3, &GASS::OgreTerrainPageComponent::SetTilingLayer4);
		
		RegisterProperty<Vec3>("Position", &GASS::OgreTerrainPageComponent::GetPosition, &GASS::OgreTerrainPageComponent::SetPosition);
		RegisterProperty<int>("IndexX", &GASS::OgreTerrainPageComponent::GetIndexX, &GASS::OgreTerrainPageComponent::SetIndexX);
		RegisterProperty<int>("IndexY", &GASS::OgreTerrainPageComponent::GetIndexY, &GASS::OgreTerrainPageComponent::SetIndexY);

		//import functions, can be used from editor, use full-path to resource and execute import
		RegisterProperty<FilePath>("ImportHeightMap", &GASS::OgreTerrainPageComponent::GetImportHeightMap, &GASS::OgreTerrainPageComponent::ImportHeightMap);
		RegisterProperty<FilePath>("ImportColorMap", &GASS::OgreTerrainPageComponent::GetImportColorMap, &GASS::OgreTerrainPageComponent::ImportColorMap);
		RegisterProperty<FilePath>("ImportDetailMask", &GASS::OgreTerrainPageComponent::GetImportDetailMask, &GASS::OgreTerrainPageComponent::ImportDetailMask);
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
					if(m_HeightMapFile.Valid()) //import height map
						ImportHeightMap(m_HeightMapFile.GetFilePath());
				}
				m_TerrainGroup->loadTerrain(m_IndexX, m_IndexY,true);
				m_Terrain = m_TerrainGroup->getTerrain(m_IndexX, m_IndexY);
				
				//m_Terrain->setRenderQueueGroup(Ogre::RENDER_QUEUE_WORLD_GEOMETRY_1);
				//m_TerrainGroup->convertTerrainSlotToWorldPosition(m_IndexX, m_IndexY, &newpos);
				//SetPosition(m_Pos);
				UpdatePosition();
				if(m_ColorMap.Valid())
					ImportColorMap(m_ColorMap.GetFilePath());

				if(m_Mask.Valid()) 
					ImportDetailMask(m_Mask.GetFilePath());

				if(m_DiffuseLayer0.Valid())
					SetDiffuseLayer0(m_DiffuseLayer0);

				if(m_NormalLayer0.Valid())
					SetNormalLayer0(m_NormalLayer0);

				if(m_TilingLayer0)
					SetTilingLayer0(m_TilingLayer0);

				if(m_DiffuseLayer1.Valid())
					SetDiffuseLayer1(m_DiffuseLayer1);

				if(m_NormalLayer1.Valid())
					SetNormalLayer1(m_NormalLayer1);

				if(m_TilingLayer1)
					SetTilingLayer1(m_TilingLayer1);

				if(m_DiffuseLayer2.Valid())
					SetDiffuseLayer2(m_DiffuseLayer2);

				if(m_NormalLayer2.Valid())
					SetNormalLayer2(m_NormalLayer2);

				if(m_TilingLayer2)
					SetTilingLayer2(m_TilingLayer2);

				if(m_DiffuseLayer3.Valid())
					SetDiffuseLayer3(m_DiffuseLayer3);

				if(m_NormalLayer3.Valid())
				{
					SetNormalLayer3(m_NormalLayer3);
				}

				if(m_TilingLayer3)
					SetTilingLayer3(m_TilingLayer3);

				if(m_DiffuseLayer4.Valid())
					SetDiffuseLayer4(m_DiffuseLayer4);

				if(m_NormalLayer4.Valid())
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

	void OgreTerrainPageComponent::SetHeightMap(const Resource &res) 
	{
		m_HeightMapFile = res;
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

	void OgreTerrainPageComponent::ImportHeightMap(const FilePath &filename)
	{
		if(m_OgreSceneManager && filename.GetFullPath() != "")
		{
			std::fstream fstr(filename.GetFullPath().c_str(), std::ios::in|std::ios::binary);
			Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));

			Ogre::Image img;
			img.load(stream);

			m_TerrainGroup->defineTerrain(m_IndexX, m_IndexY, &img);

			// sync load since we want everything in place when we start
			m_TerrainGroup->loadAllTerrains(true);
			GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(boost::shared_dynamic_cast<IGeometryComponent>(shared_from_this()))));
		}
	}

	Resource OgreTerrainPageComponent::GetTerrainResource() const
	{
		std::stringstream ss;
		ss << "OgreTerrainPageComponent_" << m_IndexX << "_" << m_IndexY;
		Resource res(ss.str());
		return res;
	}

	void OgreTerrainPageComponent::LoadFromFile()
	{
		m_TerrainGroup->defineTerrain(m_IndexX, m_IndexY);
		m_TerrainGroup->loadTerrain(m_IndexX, m_IndexY,true);
		m_Terrain = m_TerrainGroup->getTerrain(m_IndexX, m_IndexY);
	}

	void OgreTerrainPageComponent::SetColorMap(const Resource &color_map_res)
	{
		m_ColorMap = color_map_res;
		ImportColorMap(m_ColorMap.GetFilePath());
	}

	void OgreTerrainPageComponent::ImportColorMap(const FilePath &filename)
	{
		if(m_Terrain && filename.GetFullPath() != "")
		{

			//std::cout << "try to load" << colormap << "\n";
			std::fstream fstr(filename.GetFullPath().c_str(), std::ios::in|std::ios::binary);
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

	Resource OgreTerrainPageComponent::GetColorMap() const 
	{
		return m_ColorMap;
	}

	void OgreTerrainPageComponent::SetDiffuseLayer0(const Resource &diffuse)
	{
		m_DiffuseLayer0 = diffuse;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_DiffuseLayer0.Valid())
		{
			m_Terrain->setLayerTextureName(0,0,m_DiffuseLayer0.Name());
		}
	}

	Resource OgreTerrainPageComponent::GetDiffuseLayer0() const
	{
		return m_DiffuseLayer0;
	}

	void OgreTerrainPageComponent::SetNormalLayer0(const Resource &normal)
	{
		m_NormalLayer0 = normal;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_NormalLayer0.Valid())
		{
			m_Terrain->setLayerTextureName(0,1,m_NormalLayer0.Name());
		}
	}

	Resource OgreTerrainPageComponent::GetNormalLayer0() const
	{
		return m_NormalLayer0;
	}

	void OgreTerrainPageComponent::SetDiffuseLayer1(const Resource &diffuse)
	{
		m_DiffuseLayer1 = diffuse;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_DiffuseLayer1.Valid())
		{
			m_Terrain->setLayerTextureName(1,0,m_DiffuseLayer1.Name());
		}
	}

	Resource OgreTerrainPageComponent::GetDiffuseLayer1() const
	{
		return m_DiffuseLayer1;
	}

	void OgreTerrainPageComponent::SetNormalLayer1(const Resource &normal)
	{
		m_NormalLayer1 = normal;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_NormalLayer1.Valid())
		{
			m_Terrain->setLayerTextureName(1,1,m_NormalLayer1.Name());
		}
	}

	Resource OgreTerrainPageComponent::GetNormalLayer1() const
	{
		return m_NormalLayer1;
	}

	void OgreTerrainPageComponent::SetDiffuseLayer2(const Resource &diffuse)
	{
		m_DiffuseLayer2 = diffuse;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_DiffuseLayer2.Valid())
		{
			m_Terrain->setLayerTextureName(2,0,m_DiffuseLayer2.Name());
		}
	}

	Resource OgreTerrainPageComponent::GetDiffuseLayer2() const
	{
		return m_DiffuseLayer2;
	}

	void OgreTerrainPageComponent::SetNormalLayer2(const Resource &normal)
	{
		m_NormalLayer2 = normal;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_NormalLayer2.Valid())
		{
			m_Terrain->setLayerTextureName(2,1,m_NormalLayer2.Name());
		}
	}

	Resource OgreTerrainPageComponent::GetNormalLayer2() const
	{
		return m_NormalLayer2;
	}


	void OgreTerrainPageComponent::SetDiffuseLayer3(const Resource &diffuse)
	{
		m_DiffuseLayer3 = diffuse;
		if(m_Terrain && m_DiffuseLayer3.Valid())
		{
			m_Terrain->setLayerTextureName(3,0,m_DiffuseLayer3.Name());
		}
	}

	Resource OgreTerrainPageComponent::GetDiffuseLayer3() const
	{
		return m_DiffuseLayer3;
	}

	void OgreTerrainPageComponent::SetNormalLayer3(const Resource &normal)
	{
		m_NormalLayer3 = normal;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_NormalLayer3.Valid())
		{
			m_Terrain->setLayerTextureName(3,1,m_NormalLayer3.Name());
		}
	}

	Resource OgreTerrainPageComponent::GetNormalLayer3() const
	{
		return m_NormalLayer3;
	}

	void OgreTerrainPageComponent::SetDiffuseLayer4(const Resource &diffuse)
	{
		m_DiffuseLayer4 = diffuse;
		if(m_Terrain && m_DiffuseLayer4.Valid())
		{
			m_Terrain->setLayerTextureName(4,0,m_DiffuseLayer4.Name());
		}
	}

	Resource OgreTerrainPageComponent::GetDiffuseLayer4() const
	{
		return m_DiffuseLayer4;
	}

	void OgreTerrainPageComponent::SetNormalLayer4(const Resource &normal)
	{
		m_NormalLayer4 = normal;//Misc::GetFilename(diffuse);
		if(m_Terrain && m_NormalLayer4.Valid())
		{
			m_Terrain->setLayerTextureName(4,1,m_NormalLayer4.Name());
		}
	}

	Resource OgreTerrainPageComponent::GetNormalLayer4() const
	{
		return m_NormalLayer4;
	}

	void OgreTerrainPageComponent::SetMask(const Resource &mask)
	{
		m_Mask = mask;
	}

	
	void OgreTerrainPageComponent::ImportDetailMask(const FilePath &mask)
	{
		if(m_Terrain && mask.GetFullPath() != "")
		{
			std::fstream fstr(mask.GetFullPath().c_str(), std::ios::in|std::ios::binary);
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

	Resource OgreTerrainPageComponent::GetMask() const
	{
		return m_Mask;
	}

	/*void OgreTerrainPageComponent::SetMaskLayer1(const FilePath &mask)
	{
		m_MaskLayer1 = Misc::GetFilename(mask);
		if(m_Terrain && m_MaskLayer1 != "")
		{
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
	}*/

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
		unsigned int tWidth = m_Terrain->getSize();
		unsigned int tHeight = m_Terrain->getSize();

		//Create indices
		unsigned int index_size = (tWidth - 1) * (tHeight - 1) * 6;
		mesh_data->FaceVector = new unsigned int[index_size];
		mesh_data->NumFaces = static_cast<unsigned int>(index_size/3);
		for( unsigned int x = 0; x < tWidth - 1; x++)
		{
			for( unsigned int y=0; y < tHeight - 1; y++)
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
		unsigned int vertex_size = tWidth * tHeight;
		mesh_data->VertexVector = new Vec3[vertex_size];
		mesh_data->NumVertex = vertex_size;
		size_t index = 0;
		
		Vec3 center = Convert::ToGASS(m_Terrain->getPosition());
		Vec3 offset = center - Vec3(m_Terrain->getWorldSize()/2.0,0,-m_Terrain->getWorldSize()/2.0);

		Float scale = m_Terrain->getWorldSize()/(m_Terrain->getSize()-1);
		for(unsigned int x = 0; x < tWidth; x++)
		{
			for(unsigned int z = 0; z < tHeight; z++)
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
