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

#include "Plugins/Ogre/Components/GASSOgreTerrainPageComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSResourceManager.h"
#include "Sim/GASSFileResourceCallback.h"
#include "Sim/GASSSimEngine.h"
#include "Plugins/Ogre/Components/GASSOgreTerrainGroupComponent.h"
#include "Plugins/Ogre/GASSOgreGraphicsSceneManager.h"
#include "Plugins/Ogre/GASSOgreConvert.h"

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
		m_GeomFlags(GEOMETRY_FLAG_GROUND),
		m_Pos(0,0,0),
		m_RenderQueue(Ogre::RENDER_QUEUE_WORLD_GEOMETRY_1)
	{

	}

	OgreTerrainPageComponent::~OgreTerrainPageComponent()
	{

	}

	void OgreTerrainPageComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OgreTerrainPageComponent>();
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("OgreTerrainPageComponent", OF_VISIBLE)));

		RegisterGetSet("HeightMap", &GASS::OgreTerrainPageComponent::GetHeightMap, &GASS::OgreTerrainPageComponent::SetHeightMap,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("ColorMap", &GASS::OgreTerrainPageComponent::GetColorMap, &GASS::OgreTerrainPageComponent::SetColorMap,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("DetailMask", &GASS::OgreTerrainPageComponent::GetMask, &GASS::OgreTerrainPageComponent::SetMask,PF_VISIBLE | PF_EDITABLE,"");

		
		auto diff0 = RegisterGetSet("DiffuseLayer0", &GASS::OgreTerrainPageComponent::GetDiffuseLayer0, &GASS::OgreTerrainPageComponent::SetDiffuseLayer0, PF_VISIBLE, "DiffuseLayer texture map for layer 0");
		auto norm0 = RegisterGetSet("NormalLayer0", &GASS::OgreTerrainPageComponent::GetNormalLayer0, &GASS::OgreTerrainPageComponent::SetNormalLayer0, PF_VISIBLE);
		
		auto diff1 = RegisterGetSet("DiffuseLayer1", &GASS::OgreTerrainPageComponent::GetDiffuseLayer1, &GASS::OgreTerrainPageComponent::SetDiffuseLayer1, PF_VISIBLE);
		auto norm1 = RegisterGetSet("NormalLayer1", &GASS::OgreTerrainPageComponent::GetNormalLayer1, &GASS::OgreTerrainPageComponent::SetNormalLayer1, PF_VISIBLE);
		
		auto diff2 = RegisterGetSet("DiffuseLayer2", &GASS::OgreTerrainPageComponent::GetDiffuseLayer2, &GASS::OgreTerrainPageComponent::SetDiffuseLayer2, PF_VISIBLE);
		auto norm2 = RegisterGetSet("NormalLayer2", &GASS::OgreTerrainPageComponent::GetNormalLayer2, &GASS::OgreTerrainPageComponent::SetNormalLayer2, PF_VISIBLE);
		
		auto diff3 = RegisterGetSet("DiffuseLayer3", &GASS::OgreTerrainPageComponent::GetDiffuseLayer3, &GASS::OgreTerrainPageComponent::SetDiffuseLayer3, PF_VISIBLE);
		auto norm3 = RegisterGetSet("NormalLayer3", &GASS::OgreTerrainPageComponent::GetNormalLayer3, &GASS::OgreTerrainPageComponent::SetNormalLayer3, PF_VISIBLE);
		
		auto diff4 = RegisterGetSet("DiffuseLayer4", &GASS::OgreTerrainPageComponent::GetDiffuseLayer4, &GASS::OgreTerrainPageComponent::SetDiffuseLayer4, PF_VISIBLE);
		auto norm4 = RegisterGetSet("NormalLayer4", &GASS::OgreTerrainPageComponent::GetNormalLayer4, &GASS::OgreTerrainPageComponent::SetNormalLayer4, PF_VISIBLE);
	
		std::shared_ptr<FileResourceCallback<ResourceHandle>> option_callback = std::make_shared<FileResourceCallback<ResourceHandle>>("GASS_DETAIL_TEXTURES", "TEXTURE");
		diff0->SetOptionsCallback(option_callback);
		diff1->SetOptionsCallback(option_callback);
		diff2->SetOptionsCallback(option_callback);
		diff3->SetOptionsCallback(option_callback);
		diff4->SetOptionsCallback(option_callback);

		norm0->SetOptionsCallback(option_callback);
		norm1->SetOptionsCallback(option_callback);
		norm2->SetOptionsCallback(option_callback);
		norm3->SetOptionsCallback(option_callback);
		norm4->SetOptionsCallback(option_callback);

		RegisterGetSet("TilingLayer0", &GASS::OgreTerrainPageComponent::GetTilingLayer0, &GASS::OgreTerrainPageComponent::SetTilingLayer0,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("TilingLayer1", &GASS::OgreTerrainPageComponent::GetTilingLayer1, &GASS::OgreTerrainPageComponent::SetTilingLayer1,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("TilingLayer2", &GASS::OgreTerrainPageComponent::GetTilingLayer2, &GASS::OgreTerrainPageComponent::SetTilingLayer2,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("TilingLayer3", &GASS::OgreTerrainPageComponent::GetTilingLayer3, &GASS::OgreTerrainPageComponent::SetTilingLayer3,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("TilingLayer4", &GASS::OgreTerrainPageComponent::GetTilingLayer3, &GASS::OgreTerrainPageComponent::SetTilingLayer4,PF_VISIBLE | PF_EDITABLE,"");

		RegisterGetSet("Position", &GASS::OgreTerrainPageComponent::GetPosition, &GASS::OgreTerrainPageComponent::SetPosition,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("IndexX", &GASS::OgreTerrainPageComponent::GetIndexX, &GASS::OgreTerrainPageComponent::SetIndexX,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("IndexY", &GASS::OgreTerrainPageComponent::GetIndexY, &GASS::OgreTerrainPageComponent::SetIndexY,PF_VISIBLE | PF_EDITABLE,"");
		
		RegisterGetSet("RenderQueue", &GASS::OgreTerrainPageComponent::GetRenderQueue, &GASS::OgreTerrainPageComponent::SetRenderQueue, PF_VISIBLE | PF_EDITABLE, "Render Queue");
		//import functions, can be used from editor, use full-path to resource and execute import
		std::vector<std::string> hm_ext;
		hm_ext.push_back("png");
		RegisterGetSet("ImportHeightMap", &OgreTerrainPageComponent::GetImportHeightMap, &OgreTerrainPageComponent::ImportHeightMap, PF_VISIBLE | PF_EDITABLE, "Import height map, only png files supported",
			std::make_shared<FilePathPropertyMetaData>(FilePathPropertyMetaData::IMPORT_FILE, hm_ext));

		std::vector<std::string> color_ext;
		color_ext.push_back("png");
		color_ext.push_back("dds");
		color_ext.push_back("jpg");
		color_ext.push_back("*");

		RegisterGetSet("ImportColorMap", &GASS::OgreTerrainPageComponent::GetImportColorMap, &GASS::OgreTerrainPageComponent::ImportColorMap, PF_VISIBLE | PF_EDITABLE,"Import color map" ,
			std::make_shared<FilePathPropertyMetaData>(FilePathPropertyMetaData::IMPORT_FILE, color_ext));

		RegisterGetSet("ImportDetailMask", &GASS::OgreTerrainPageComponent::GetImportDetailMask, &GASS::OgreTerrainPageComponent::ImportDetailMask, PF_VISIBLE | PF_EDITABLE, "Import detail map (RGB = detail_layer_1,detail_layer_2,detail_layer_3",
			std::make_shared<FilePathPropertyMetaData>(FilePathPropertyMetaData::IMPORT_FILE, color_ext));

		RegisterGetSet("ExportDetailMask", &GASS::OgreTerrainPageComponent::GetExportDetailMask, &GASS::OgreTerrainPageComponent::ExportDetailMask, PF_VISIBLE | PF_EDITABLE, "Export detail map (RGB = detail_layer_1,detail_layer_2,detail_layer_3",
			std::make_shared < FilePathPropertyMetaData>(FilePathPropertyMetaData::EXPORT_FILE, color_ext));
		RegisterGetSet("DumpTextues", &GASS::OgreTerrainPageComponent::GetDumpTextues, &GASS::OgreTerrainPageComponent::SetDumpTextues,PF_VISIBLE | PF_EDITABLE,"");
	}

	void OgreTerrainPageComponent::SetDumpTextues(bool /*value*/)
	{
		std::string prefix = "OGRE_TERRAIN";
		std::string sufix = ".tga";
		if(m_Terrain)
			m_Terrain->_dumpTextures(prefix,sufix);
	}

	void OgreTerrainPageComponent::SetRenderQueue(const RenderQueueBinder &rq)
	{
		m_RenderQueue = rq;
		if(m_Terrain)
		{
			m_Terrain->setRenderQueueGroup(static_cast<Ogre::uint8>(m_RenderQueue.GetValue()));
		}
	}

	void OgreTerrainPageComponent::OnInitialize()
	{
		m_Collision = GetSceneObject()->GetFirstComponentByClass<ICollisionComponent>();

		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreTerrainPageComponent::OnTerrainLayerMessage,TerrainLayerRequest,0));
		OgreGraphicsSceneManagerPtr ogsm =  GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OgreGraphicsSceneManager>();
		assert(ogsm);
		m_OgreSceneManager = ogsm->GetOgreSceneManager();
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
				}
				m_TerrainGroup->loadTerrain(m_IndexX, m_IndexY,true);
				m_Terrain = m_TerrainGroup->getTerrain(m_IndexX, m_IndexY);

				if(m_HeightMapFile.Valid()) //import height map
					ImportHeightMap(m_HeightMapFile.GetResource()->Path());

				SetRenderQueue(m_RenderQueue);
				//m_Terrain->setRenderQueueGroup(Ogre::RENDER_QUEUE_WORLD_GEOMETRY_1);
				//m_TerrainGroup->convertTerrainSlotToWorldPosition(m_IndexX, m_IndexY, &newpos);

				if(m_Pos != Vec3(0,0,0))
					SetPosition(m_Pos);

				GASS_LOG(LINFO) << "UpdatePosition" << " OgreTerrainPageComponent::OnInitialize";
				UpdatePosition();

				//WaitWhileLoading();

				//GASS_LOG(LINFO) << "Start Import"<<  " OgreTerrainPageComponent::OnInitialize";

				if(m_ColorMap.Valid())
					ImportColorMap(m_ColorMap.GetResource()->Path());

				//WaitWhileLoading();

				if(m_Mask.Valid())
					ImportDetailMask(m_Mask.GetResource()->Path());

				//WaitWhileLoading();

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

				//GASS_LOG(LINFO) << "DONE!"<<  " OgreTerrainPageComponent::OnInitialize";
				//std::cout << "load world size:" << m_TerrainGroup->getTerrainWorldSize() << "\n";
				//std::cout << "load size:" << m_Terrain->getWorldSize() << "\n";
				//std::cout << "bb size:" << m_Terrain->getAABB().getMaximum().x <<" "<< m_Terrain->getAABB().getMaximum().y <<" " << m_Terrain->getAABB().getMaximum().z << "\n";
				//Ogre::MaterialPtr ptr = m_Terrain->getMaterial();
				//std::string name = ptr->getName();
				//Terrain page created
			}
		}
	}

	void OgreTerrainPageComponent::WaitWhileLoading()
	{
		m_TerrainGroup->loadAllTerrains(true);
		m_Terrain->update(true);
		while (m_Terrain->isDerivedDataUpdateInProgress() || !m_Terrain->isLoaded())
		{
			m_Terrain->update(true);
			// we need to wait for this to finish
			OGRE_THREAD_SLEEP(50);
			Ogre::Root::getSingleton().getWorkQueue()->processResponses();
		}

		/*m_TerrainGroup->update(true);
		bool bAllTerrainsLoaded;
		do
		{
		bAllTerrainsLoaded = true;
		Ogre::TerrainGroup::TerrainIterator ti = m_TerrainGroup->getTerrainIterator();
		while(ti.hasMoreElements())
		{
		Ogre::Terrain* t = ti.getNext()->instance;
		bAllTerrainsLoaded &= t->isLoaded() & !t->isModified() & !t->isDerivedDataUpdateInProgress();
		}
		OGRE_THREAD_SLEEP(50);
		Ogre::Root::getSingleton().getWorkQueue()->processResponses();
		} while(!bAllTerrainsLoaded);*/

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

	void OgreTerrainPageComponent::SetHeightMap(const ResourceHandle &res)
	{
		m_HeightMapFile = res;
	}

	void OgreTerrainPageComponent::OnTerrainLayerMessage(TerrainLayerRequestPtr message)
	{
		switch(	message->GetLayer())
		{
		case TL_0:
			break;
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


	void OgreTerrainPageComponent::ImportHeightMap(const FilePath &filename)
	{
		if(m_OgreSceneManager && filename.GetFullPath() != "")
		{
			if(filename.GetExtension() == "png")
			{
				std::fstream fstr(filename.GetFullPath().c_str(), std::ios::in|std::ios::binary);
				Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));

				Ogre::Image image;
				image.load(stream);

				bool new_load = false;
				if(new_load)
				{
					m_TerrainGroup->defineTerrain(m_IndexX, m_IndexY, &image);
					// sync load since we want everything in place when we start
					m_TerrainGroup->loadAllTerrains(true);
				}
				else
				{
					int hm_size = m_Terrain->getSize();
					image.resize(static_cast<Ogre::ushort>(hm_size), static_cast<Ogre::ushort>(hm_size));
					float *height_data = m_Terrain->getHeightData();
					for(int y = 0; y < hm_size ;y++)
					{
						for(int x = 0; x < hm_size ;x++)
						{
							Ogre::ColourValue color = image.getColourAt(x, y, 0);
							const Ogre::Terrain::ImportData& defaultimp = m_TerrainGroup->getDefaultImportSettings();
							*height_data = color.r * defaultimp.inputScale;
							++height_data;
						}
					}
				}
				stream.setNull();

				Ogre::Rect drect(0, 0, m_Terrain->getSize(), m_Terrain->getSize());
				m_Terrain->dirtyRect(drect);
				m_Terrain->update(true);
				GetSceneObject()->PostEvent(GeometryChangedEventPtr(new GeometryChangedEvent(GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
			}
		}
	}

	ResourceHandle OgreTerrainPageComponent::GetTerrainResource() const
	{
		std::stringstream ss;
		ss << "OgreTerrainPageComponent_" << m_IndexX << "_" << m_IndexY;
		ResourceHandle res(ss.str());
		return res;
	}

	void OgreTerrainPageComponent::LoadFromFile()
	{
		m_TerrainGroup->defineTerrain(m_IndexX, m_IndexY);
		m_TerrainGroup->loadTerrain(m_IndexX, m_IndexY,true);
		m_Terrain = m_TerrainGroup->getTerrain(m_IndexX, m_IndexY);
	}

	void OgreTerrainPageComponent::SetColorMap(const ResourceHandle &color_map_res)
	{
		m_ColorMap = color_map_res;
		if(m_ColorMap.Valid())
			ImportColorMap(m_ColorMap.GetResource()->Path());
	}

	void OgreTerrainPageComponent::ImportColorMap(const FilePath &filename)
	{
		if(m_Terrain && filename.GetFullPath() != "")
		{
			std::fstream fstr(filename.GetFullPath().c_str(), std::ios::in|std::ios::binary);
			Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));

			Ogre::Image colourMap;
			colourMap.load(stream);
			m_Terrain->setGlobalColourMapEnabled(true);
			m_Terrain->getGlobalColourMap()->unload();
			m_Terrain->getGlobalColourMap()->loadImage(colourMap);
			m_Terrain->setGlobalColourMapEnabled(true);
		}
	}

	ResourceHandle OgreTerrainPageComponent::GetColorMap() const
	{
		return m_ColorMap;
	}

	void OgreTerrainPageComponent::SetDiffuseLayer0(const ResourceHandle &diffuse)
	{
		m_DiffuseLayer0 = diffuse;
		if(m_Terrain && m_DiffuseLayer0.Valid())
		{
			m_Terrain->setLayerTextureName(0,0,m_DiffuseLayer0.Name());
		}
	}

	ResourceHandle OgreTerrainPageComponent::GetDiffuseLayer0() const
	{
		return m_DiffuseLayer0;
	}

	void OgreTerrainPageComponent::SetNormalLayer0(const ResourceHandle &normal)
	{
		m_NormalLayer0 = normal;
		if(m_Terrain && m_NormalLayer0.Valid())
		{
			m_Terrain->setLayerTextureName(0,1,m_NormalLayer0.Name());
		}
	}

	ResourceHandle OgreTerrainPageComponent::GetNormalLayer0() const
	{
		return m_NormalLayer0;
	}

	void OgreTerrainPageComponent::SetDiffuseLayer1(const ResourceHandle &diffuse)
	{
		m_DiffuseLayer1 = diffuse;
		if(m_Terrain && m_DiffuseLayer1.Valid())
		{
			m_Terrain->setLayerTextureName(1,0,m_DiffuseLayer1.Name());
		}
	}

	ResourceHandle OgreTerrainPageComponent::GetDiffuseLayer1() const
	{
		return m_DiffuseLayer1;
	}

	void OgreTerrainPageComponent::SetNormalLayer1(const ResourceHandle &normal)
	{
		m_NormalLayer1 = normal;
		if(m_Terrain && m_NormalLayer1.Valid())
		{
			m_Terrain->setLayerTextureName(1,1,m_NormalLayer1.Name());
		}
	}

	ResourceHandle OgreTerrainPageComponent::GetNormalLayer1() const
	{
		return m_NormalLayer1;
	}

	void OgreTerrainPageComponent::SetDiffuseLayer2(const ResourceHandle &diffuse)
	{
		m_DiffuseLayer2 = diffuse;
		if(m_Terrain && m_DiffuseLayer2.Valid())
		{
			m_Terrain->setLayerTextureName(2,0,m_DiffuseLayer2.Name());
		}
	}

	ResourceHandle OgreTerrainPageComponent::GetDiffuseLayer2() const
	{
		return m_DiffuseLayer2;
	}

	void OgreTerrainPageComponent::SetNormalLayer2(const ResourceHandle &normal)
	{
		m_NormalLayer2 = normal;
		if(m_Terrain && m_NormalLayer2.Valid())
		{
			m_Terrain->setLayerTextureName(2,1,m_NormalLayer2.Name());
		}
	}

	ResourceHandle OgreTerrainPageComponent::GetNormalLayer2() const
	{
		return m_NormalLayer2;
	}

	void OgreTerrainPageComponent::SetDiffuseLayer3(const ResourceHandle &diffuse)
	{
		m_DiffuseLayer3 = diffuse;
		if(m_Terrain && m_DiffuseLayer3.Valid())
		{
			m_Terrain->setLayerTextureName(3,0,m_DiffuseLayer3.Name());
		}
	}

	ResourceHandle OgreTerrainPageComponent::GetDiffuseLayer3() const
	{
		return m_DiffuseLayer3;
	}

	void OgreTerrainPageComponent::SetNormalLayer3(const ResourceHandle &normal)
	{
		m_NormalLayer3 = normal;
		if(m_Terrain && m_NormalLayer3.Valid())
		{
			m_Terrain->setLayerTextureName(3,1,m_NormalLayer3.Name());
		}
	}

	ResourceHandle OgreTerrainPageComponent::GetNormalLayer3() const
	{
		return m_NormalLayer3;
	}

	void OgreTerrainPageComponent::SetDiffuseLayer4(const ResourceHandle &diffuse)
	{
		m_DiffuseLayer4 = diffuse;
		if(m_Terrain && m_DiffuseLayer4.Valid())
		{
			m_Terrain->setLayerTextureName(4,0,m_DiffuseLayer4.Name());
		}
	}

	ResourceHandle OgreTerrainPageComponent::GetDiffuseLayer4() const
	{
		return m_DiffuseLayer4;
	}

	void OgreTerrainPageComponent::SetNormalLayer4(const ResourceHandle &normal)
	{
		m_NormalLayer4 = normal;
		if(m_Terrain && m_NormalLayer4.Valid())
		{
			m_Terrain->setLayerTextureName(4,1,m_NormalLayer4.Name());
		}
	}

	ResourceHandle OgreTerrainPageComponent::GetNormalLayer4() const
	{
		return m_NormalLayer4;
	}

	void OgreTerrainPageComponent::SetMask(const ResourceHandle &mask)
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

	void OgreTerrainPageComponent::ExportDetailMask(const FilePath &mask)
	{
		if(m_Terrain && mask.GetFullPath() != "")
		{
			Ogre::Image img;
			Ogre::ColourValue cval;

			float *blend_data_1 = m_Terrain->getLayerBlendMap(1)->getBlendPointer();
			float *blend_data_2 = m_Terrain->getLayerBlendMap(2)->getBlendPointer();
			float *blend_data_3 = m_Terrain->getLayerBlendMap(3)->getBlendPointer();

			int size = m_Terrain->getLayerBlendMapSize() * m_Terrain->getLayerBlendMapSize();
			Ogre::uchar *data = OGRE_ALLOC_T(Ogre::uchar, size * 3, Ogre::MEMCATEGORY_GENERAL);
			memset(data, 0, size * 3);

			img.loadDynamicImage(data, m_Terrain->getLayerBlendMapSize(), m_Terrain->getLayerBlendMapSize(), 1, Ogre::PF_R8G8B8, true);
			for(int y = 0;y < m_Terrain->getLayerBlendMapSize();y++)
			{
				for(int x = 0;x < m_Terrain->getLayerBlendMapSize();x++)
				{
					cval.r = *blend_data_1;
					cval.g = *blend_data_2;
					cval.b = *blend_data_3;
					++blend_data_1;
					++blend_data_2;
					++blend_data_3;
					img.setColourAt(cval,x, y, 0);
				}
			}
			img.save(mask.GetFullPath());
		}
	}

	ResourceHandle OgreTerrainPageComponent::GetMask() const
	{
		return m_Mask;
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
			aabox = OgreConvert::ToGASS(m_Terrain->getAABB());
			//Hack to get terrain size before loaded
			Float size = m_TerrainGroup->getTerrainWorldSize()*0.5;
			Vec3 pos = GetPosition();
			aabox.Min = Vec3(pos.x -size,aabox.Min.y, pos.z -size);
			aabox.Max = Vec3(pos.x +size,aabox.Max.y, pos.z +size);
		}
		return aabox;
	}

	Sphere OgreTerrainPageComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		if(m_Terrain)
		{
			sphere.m_Pos = OgreConvert::ToGASS(m_Terrain->getPosition()) + GetPosition();
			sphere.m_Radius = m_Terrain->getBoundingRadius();
		}
		return sphere;
	}

	unsigned int OgreTerrainPageComponent::GetNumSamplesH() const
	{
		return GetNumSamplesW();
	}

	unsigned int OgreTerrainPageComponent::GetNumSamplesW() const
	{
		OgreTerrainGroupComponentPtr terrain_man = GetSceneObject()->GetFirstComponentByClass<OgreTerrainGroupComponent>();
		if(!terrain_man) //try parent
			terrain_man = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<OgreTerrainGroupComponent>();
		if(terrain_man) //try parent
			return terrain_man->GetImportTerrainSize();
		else return 0;
	}


	GraphicsMesh OgreTerrainPageComponent::GetMeshData() const
	{
		GraphicsMesh mesh_data;
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data.SubMeshVector.push_back(sub_mesh_data);

		if(!m_Terrain)
			return mesh_data;
		unsigned int tWidth = m_Terrain->getSize();
		unsigned int tHeight = m_Terrain->getSize();

		//Create indices
		unsigned int index_size = (tWidth - 1) * (tHeight - 1) * 6;
		sub_mesh_data->IndexVector.resize(index_size);
		sub_mesh_data->Type = TRIANGLE_LIST;
		for( unsigned int x = 0; x < tWidth - 1; x++)
		{
			for( unsigned int y=0; y < tHeight - 1; y++)
			{
				sub_mesh_data->IndexVector[(x+y*(tWidth-1))*6+2] = x+y * tWidth;
				sub_mesh_data->IndexVector[(x+y*(tWidth-1))*6+1] = (x+1)+y * tWidth;
				sub_mesh_data->IndexVector[(x+y*(tWidth-1))*6] = (x+1)+(y+1) * tWidth;

				sub_mesh_data->IndexVector[(x+y*(tWidth-1))*6+5] = x+(y+1) * tWidth;
				sub_mesh_data->IndexVector[(x+y*(tWidth-1))*6+4] = x+y * tWidth;
				sub_mesh_data->IndexVector[(x+y*(tWidth-1))*6+3] = (x+1)+(y+1) * tWidth;

			}
		}

		// Create vertices
		unsigned int vertex_size = tWidth * tHeight;
		sub_mesh_data->PositionVector.resize(vertex_size);
		size_t index = 0;
		Vec3 center = OgreConvert::ToGASS(m_Terrain->getPosition());
		Vec3 offset = center - Vec3(m_Terrain->getWorldSize()/2.0,0,-m_Terrain->getWorldSize()/2.0);

		Float scale = m_Terrain->getWorldSize()/(m_Terrain->getSize()-1);
		for(unsigned int x = 0; x < tWidth; x++)
		{
			for(unsigned int z = 0; z < tHeight; z++)
			{
				Float fx = x;
				Float fz = z;
				sub_mesh_data->PositionVector[index] = Vec3(fx*scale, m_Terrain->getHeightAtPoint(x,z),-fz * scale) + offset;
				index++;
			}
		}
		return mesh_data;
	}

	Float OgreTerrainPageComponent::GetHeightAtSample(int x, int y) const
	{
		if(m_Terrain)
			return m_Terrain->getHeightAtPoint(x, m_Terrain->getSize() - y - 1);
		return 0;
	}

	Float OgreTerrainPageComponent::GetHeightAtWorldLocation(Float x, Float z) const
	{
		if(m_Terrain)
			return m_Terrain->getHeightAtWorldPosition(static_cast<Ogre::Real>(x), 10000.0f, static_cast<Ogre::Real>(z));
		return 0;
	}

	/*float* OgreTerrainPageComponent::GetHeightData() const
	{
		if(m_Terrain)
			return m_Terrain->getHeightData();
		return NULL;
	}*/

	void OgreTerrainPageComponent::UpdatePosition()
	{
		if(m_Terrain)
		{
			GetSceneObject()->PostEvent(GeometryChangedEventPtr(new GeometryChangedEvent(GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
		}
	}

	void OgreTerrainPageComponent::SetPosition(const Vec3 &pos)
	{
		m_Pos = pos;
		if(m_Terrain)
		{
			m_Terrain->setPosition(OgreConvert::ToOgre(pos));
		}
	}

	Vec3 OgreTerrainPageComponent::GetPosition() const
	{
		return m_Pos;
	}

	GeometryFlags OgreTerrainPageComponent::GetGeometryFlags() const
	{
		return m_GeomFlags;
	}

	void OgreTerrainPageComponent::SetGeometryFlags(GeometryFlags flags)
	{
		m_GeomFlags = flags;
	}

	bool OgreTerrainPageComponent::GetCollision() const
	{
		if(m_Collision)
			return m_Collision->GetActive();
		return false;
	}

	void OgreTerrainPageComponent::SetCollision(bool value)
	{
		if(m_Collision)
			m_Collision->SetActive(value);
	}
}
