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
#include <OgreTerrainMaterialGeneratorA.h>

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
#include "Plugins/Ogre/Components/OgreTerrainGroupComponent.h"
#include "Plugins/Ogre/Components/OgreTerrainPageComponent.h"


#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include "Plugins/Ogre/Components/OgreGASSTerrainMaterialGenerator.h"
#include "Plugins/Ogre/OgreConvert.h"


namespace GASS
{
	OgreTerrainGroupComponent::OgreTerrainGroupComponent() : m_CreateCollisionMesh (true),
		m_TerrainGlobals(NULL),
		m_OgreSceneManager(NULL),
		m_TerrainGroup(NULL),
		m_TerrainWorldSize(5000),
		m_TerrainSize(513),
		m_TerrainName("UnkownTerrain")
	{


	}

	OgreTerrainGroupComponent::~OgreTerrainGroupComponent()
	{
	
	}

	void OgreTerrainGroupComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("OgreTerrainGroupComponent",new Creator<OgreTerrainGroupComponent, IComponent>);
		
		RegisterProperty<float>("ImportScale", &GASS::OgreTerrainGroupComponent::GetImportScale, &GASS::OgreTerrainGroupComponent::SetImportScale);
		RegisterProperty<int>("ImportTerrainSize", &GASS::OgreTerrainGroupComponent::GetImportTerrainSize, &GASS::OgreTerrainGroupComponent::SetImportTerrainSize);
		RegisterProperty<Float>("ImportTerrainWorldSize", &GASS::OgreTerrainGroupComponent::GetImportTerrainWorldSize, &GASS::OgreTerrainGroupComponent::SetImportTerrainWorldSize);
		RegisterProperty<std::string>("SaveTerrain", &GASS::OgreTerrainGroupComponent::GetSaveTerrain, &GASS::OgreTerrainGroupComponent::SetSaveTerrain);
		RegisterProperty<std::string>("LoadTerrain", &GASS::OgreTerrainGroupComponent::GetLoadTerrain, &GASS::OgreTerrainGroupComponent::SetLoadTerrain);
		RegisterProperty<std::string>("CustomMaterial", &GASS::OgreTerrainGroupComponent::GetCustomMaterial, &GASS::OgreTerrainGroupComponent::SetCustomMaterial);
	}

	void OgreTerrainGroupComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreTerrainGroupComponent::OnLoad,LoadGFXComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreTerrainGroupComponent::OnUnload,UnloadComponentsMessage,0));
	}


	std::string OgreTerrainGroupComponent::GetCustomMaterial() const
	{
		return m_CustomMaterial;
	}

	void OgreTerrainGroupComponent::SetCustomMaterial(const std::string &material)
	{
		m_CustomMaterial=material;
	}

	std::string OgreTerrainGroupComponent::GetLoadTerrain() const
	{
		return m_TerrainName;
	}

	void OgreTerrainGroupComponent::SetLoadTerrain(const std::string &filename) 
	{
		m_TerrainName = filename;
		if(m_TerrainGroup)
		{
			m_TerrainGroup->setFilenameConvention(m_TerrainName, "dat");

			//Get all components
			IComponentContainer::ComponentVector comps;
			GetSceneObject()->GetComponentsByClass(comps, "OgreTerrainPageComponent");

			for(int i = 0 ;  i < comps.size(); i++)
			{
				OgreTerrainPageComponentPtr page = boost::shared_dynamic_cast<OgreTerrainPageComponent>(comps[i]);
				if(page)
				{
					page->LoadFromFile();
				}
			}
			//m_TerrainGroup->defineTerrain(x, y);
		}
	}

	//void OgreTerrainGroupComponent::ConfigureTerrainDefaults(Light* l)
	void OgreTerrainGroupComponent::ConfigureTerrainDefaults()
	{
		// Configure global
		//
		if(m_CustomMaterial != "")
		{
			m_TerrainGlobals->setDefaultMaterialGenerator(Ogre::SharedPtr<Ogre::TerrainMaterialGenerator>( OGRE_NEW GASSTerrainMaterialGenerator(m_CustomMaterial)));
			GASSTerrainMaterialGenerator::CustomMaterialProfile* matProfile =	static_cast<GASSTerrainMaterialGenerator::CustomMaterialProfile*>(m_TerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());
			matProfile->setCompositeMapEnabled(false);
			matProfile->setLightmapEnabled(false);
		}
		else
		{
			Ogre::TerrainMaterialGeneratorA::SM2Profile* matProfile =	static_cast<Ogre::TerrainMaterialGeneratorA::SM2Profile*>(m_TerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());
			matProfile->setLightmapEnabled(false);
		}

		m_TerrainGlobals->setMaxPixelError(8);
		// testing composite map
		m_TerrainGlobals->setCompositeMapDistance(3000);
		
		//mTerrainGlobals->setUseRayBoxDistanceCalculation(true);
		//mTerrainGlobals->getDefaultMaterialGenerator()->setDebugLevel(1);
		//mTerrainGlobals->setLightMapSize(256);

		//matProfile->setLightmapEnabled(false);
		// Important to set these so that the terrain knows what to use for derived (non-realtime) data
		//m_TerrainGlobals->setLightMapDirection(l->getDerivedDirection());
		
		//m_TerrainGlobals->setCompositeMapAmbient(m_OgreSceneManager->getAmbientLight());
		m_TerrainGlobals->setLayerBlendMapSize(1024);
		//mTerrainGlobals->setCompositeMapAmbient(ColourValue::Red);
		//m_TerrainGlobals->setCompositeMapDiffuse(l->getDiffuseColour());

		// Configure default import settings for if we use imported image
		Ogre::Terrain::ImportData& defaultimp = m_TerrainGroup->getDefaultImportSettings();
		defaultimp.terrainSize = m_TerrainSize;
		defaultimp.worldSize = m_TerrainWorldSize;
		defaultimp.inputScale = 600;
		defaultimp.minBatchSize = 33;
		defaultimp.maxBatchSize = 65;
	
		defaultimp.layerList.resize(3);
		defaultimp.layerList[0].worldSize = 10;
		defaultimp.layerList[0].textureNames.push_back("default.dds");

		defaultimp.layerList[1].worldSize = 10;
		defaultimp.layerList[1].textureNames.push_back("default.dds");

		defaultimp.layerList[2].worldSize = 10;
		defaultimp.layerList[2].textureNames.push_back("default.dds");
		
	}

	float OgreTerrainGroupComponent::GetImportScale() const
	{
		if(m_TerrainGroup)
		{
			Ogre::Terrain::ImportData& defaultimp = m_TerrainGroup->getDefaultImportSettings();
			return defaultimp.inputScale;
		}
		return 1;
	}

	void OgreTerrainGroupComponent::SetImportScale(const float &value)
	{
		if(m_TerrainGroup)
		{
			Ogre::Terrain::ImportData& defaultimp = m_TerrainGroup->getDefaultImportSettings();
			defaultimp.inputScale =value;
		}

	}


	int OgreTerrainGroupComponent::GetImportTerrainSize() const
	{
		if(m_TerrainGroup)
		{
			Ogre::Terrain::ImportData& defaultimp = m_TerrainGroup->getDefaultImportSettings();
			return defaultimp.terrainSize;
		}
		return 1;
	}

	void OgreTerrainGroupComponent::SetImportTerrainSize(const int &value)
	{
		if(m_TerrainGroup)
		{
			Ogre::Terrain::ImportData& defaultimp = m_TerrainGroup->getDefaultImportSettings();
			defaultimp.terrainSize = value;
		}
		
	}

	Float OgreTerrainGroupComponent::GetImportTerrainWorldSize() const
	{
		if(m_TerrainGroup)
		{
			Ogre::Terrain::ImportData& defaultimp = m_TerrainGroup->getDefaultImportSettings();
			return defaultimp.worldSize;
		}
		return 1;
	}

	void OgreTerrainGroupComponent::SetImportTerrainWorldSize(const Float &value)
	{
		if(m_TerrainGroup)
		{
			Ogre::Terrain::ImportData& defaultimp = m_TerrainGroup->getDefaultImportSettings();
			defaultimp.worldSize = value;
		}
	}

	void OgreTerrainGroupComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		OgreGraphicsSceneManagerPtr ogsm = boost::shared_static_cast<OgreGraphicsSceneManager>(message->GetGFXSceneManager());
		assert(ogsm);
		m_OgreSceneManager = ogsm->GetSceneManger();

		m_TerrainGlobals = new  Ogre::TerrainGlobalOptions();
		m_TerrainGroup = new Ogre::TerrainGroup(m_OgreSceneManager, Ogre::Terrain::ALIGN_X_Z, m_TerrainSize, m_TerrainWorldSize);
		m_TerrainGroup->setOrigin(Ogre::Vector3(0,0,0));
		ConfigureTerrainDefaults();

		if(m_TerrainName == "UnkownTerrain")
			m_TerrainGroup->setFilenameConvention(m_TerrainName, "dat");
		else
		{
			//try to load terrain
			SetLoadTerrain(m_TerrainName);
		}
	}

	void OgreTerrainGroupComponent::SetSaveTerrain(const std::string &filename)
	{
		m_TerrainName = Misc::GetFilename(filename);

		if(m_TerrainGroup)
		{
			m_TerrainGroup->setFilenameConvention(m_TerrainName, "dat");
			m_TerrainGroup->saveAllTerrains(false);
		}
	}

	std::string OgreTerrainGroupComponent::GetSaveTerrain() const
	{
		return m_TerrainName;
	}

	void OgreTerrainGroupComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		delete m_TerrainGroup;
		delete m_TerrainGlobals;	
	}

	void OgreTerrainGroupComponent::GetBounds(Vec3 &min,Vec3 &max)
	{
		/*min.x = 0;
		min.y = 0;
		min.z = 0;

		max.x = m_WorldWidth;
		max.y = m_MaxHeight;
		max.z = m_WorldHeight;*/
	}

	AABox OgreTerrainGroupComponent::GetBoundingBox() const
	{
		AABox aabox;
		/*aabox.m_Min.x = 0;
		aabox.m_Min.y = 0;
		aabox.m_Min.z = 0;

		aabox.m_Max.x = m_WorldWidth;
		aabox.m_Max.y = m_MaxHeight;
		aabox.m_Max.z = m_WorldHeight;*/
		return aabox;
	}

	Sphere OgreTerrainGroupComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		//sphere.m_Pos = Vec3(m_WorldWidth/2.0,m_MaxHeight/2.0,m_WorldHeight/2.0);
		//sphere.m_Radius = sqrt(m_WorldWidth*m_WorldWidth);
		return sphere;
	}

	unsigned int OgreTerrainGroupComponent::GetSamplesX()
	{
		return 0;
	}
	unsigned int OgreTerrainGroupComponent::GetSamplesZ()
	{
		return 0;
	}

	void OgreTerrainGroupComponent::GetMeshData(MeshDataPtr mesh_data)
	{
	
	}

	Float OgreTerrainGroupComponent::GetHeight(Float x, Float z)
	{
		return 0;
	}
}
