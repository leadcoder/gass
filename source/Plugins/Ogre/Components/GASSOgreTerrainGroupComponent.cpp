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

#include "Plugins/Ogre/Components/GASSOgreTerrainGroupComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSFileUtils.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Plugins/Ogre/Components/GASSOgreTerrainPageComponent.h"
#include "Plugins/Ogre/GASSOgreGraphicsSceneManager.h"
#include "Plugins/Ogre/Components/GASSOgreGASSTerrainMaterialGenerator.h"
#include "Plugins/Ogre/GASSOgreConvert.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif

namespace GASS
{
	OgreTerrainGroupComponent::OgreTerrainGroupComponent() : m_CreateCollisionMesh (true),
		m_TerrainGlobals(NULL),
		m_OgreSceneManager(NULL),
		m_TerrainGroup(NULL),
		m_TerrainWorldSize(5000),
		m_TerrainSize(513),
		m_Origin(0,0,0),
		m_FadeDetail(true),
		m_DetailFadeDist(20.0f),
		m_FadeOutColor(true),
		m_NearColorWeight(0.2f),
		m_TerrainProfile(NULL),
		m_EnableLayerParallax(true),
		m_EnableLayerSpecular(true),
		m_EnableLayerNormal(true),
		m_TerrainScale(10),
		m_EnableLightmap(false),
		m_GeomFlags(GEOMETRY_FLAG_GROUND)
	{
		m_TerrainResource.SetName("OgrePagedTerrain");
	}

	OgreTerrainGroupComponent::~OgreTerrainGroupComponent()
	{

	}

	void OgreTerrainGroupComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OgreTerrainGroupComponent>();
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("OgreTerrainGroupComponent", OF_VISIBLE)));
		RegisterProperty<float>("ImportScale", &GASS::OgreTerrainGroupComponent::GetImportScale, &GASS::OgreTerrainGroupComponent::SetImportScale,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<int>("ImportTerrainSize", &GASS::OgreTerrainGroupComponent::GetImportTerrainSize, &GASS::OgreTerrainGroupComponent::SetImportTerrainSize,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Float>("ImportTerrainWorldSize", &GASS::OgreTerrainGroupComponent::GetImportTerrainWorldSize, &GASS::OgreTerrainGroupComponent::SetImportTerrainWorldSize,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<std::string>("SaveTerrain", &GASS::OgreTerrainGroupComponent::GetSaveTerrain, &GASS::OgreTerrainGroupComponent::SetSaveTerrain);
		RegisterProperty<std::string>("LoadTerrain", &GASS::OgreTerrainGroupComponent::GetLoadTerrain, &GASS::OgreTerrainGroupComponent::SetLoadTerrain);
		RegisterProperty<std::string>("CustomMaterial", &GASS::OgreTerrainGroupComponent::GetCustomMaterial, &GASS::OgreTerrainGroupComponent::SetCustomMaterial,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec2i>("CreatePages", &GASS::OgreTerrainGroupComponent::GetPages, &GASS::OgreTerrainGroupComponent::CreatePages,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec3>("Origin", &GASS::OgreTerrainGroupComponent::GetOrigin, &GASS::OgreTerrainGroupComponent::SetOrigin,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("FadeDetail", &GASS::OgreTerrainGroupComponent::GetFadeDetail, &GASS::OgreTerrainGroupComponent::SetFadeDetail,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("DetailFadeDist", &GASS::OgreTerrainGroupComponent::GetDetailFadeDist, &GASS::OgreTerrainGroupComponent::SetDetailFadeDist,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("FadeOutColor", &GASS::OgreTerrainGroupComponent::GetFadeOutColor, &GASS::OgreTerrainGroupComponent::SetFadeOutColor,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("NearColorWeight", &GASS::OgreTerrainGroupComponent::GetNearColorWeight, &GASS::OgreTerrainGroupComponent::SetNearColorWeight,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<bool>("EnableLayerNormal", &GASS::OgreTerrainGroupComponent::GetEnableLayerNormal, &GASS::OgreTerrainGroupComponent::SetEnableLayerNormal,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("EnableLayerSpecular", &GASS::OgreTerrainGroupComponent::GetEnableLayerSpecular, &GASS::OgreTerrainGroupComponent::SetEnableLayerSpecular,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("EnableLayerParallax", &GASS::OgreTerrainGroupComponent::GetEnableLayerParallax, &GASS::OgreTerrainGroupComponent::SetEnableLayerParallax,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterMember("EnableLightmap", &GASS::OgreTerrainGroupComponent::m_EnableLightmap,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));

	}

	void OgreTerrainGroupComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreTerrainGroupComponent::OnTerrainHeightModify,TerrainHeightModifyRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreTerrainGroupComponent::OnTerrainLayerPaint,TerrainPaintRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreTerrainGroupComponent::OnRoadMessage,RoadRequest,0));

		OgreGraphicsSceneManagerPtr ogsm =  GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OgreGraphicsSceneManager>();
		assert(ogsm);
		m_OgreSceneManager = ogsm->GetOgreSceneManager();
		ConfigureTerrainDefaults();

		m_Collision = GetSceneObject()->GetFirstComponentByClass<ICollisionComponent>();
	}

	void OgreTerrainGroupComponent::OnDelete()
	{
		delete m_TerrainGroup;
	}

	void OgreTerrainGroupComponent::SaveXML(tinyxml2::XMLElement *obj_elem)
	{
		BaseSceneComponent::SaveXML(obj_elem);

		if(m_TerrainGroup)
		{
			//reset resource group!!
			m_TerrainGroup->setResourceGroup(GetSceneObject()->GetScene()->GetResourceGroupName());
			m_TerrainGroup->setFilenameConvention(m_TerrainResource.Name(), "dat");
			m_TerrainGroup->saveAllTerrains(false);
		}
		//also save terrain to data file?
	}

	void OgreTerrainGroupComponent::SetOrigin(const Vec3 &pos)
	{
		m_Origin = pos;
		if(m_TerrainGroup)
		{
			m_TerrainGroup->setOrigin(OgreConvert::ToOgre(pos));

			//update page positions
			ComponentContainer::ComponentContainerIterator children = GetSceneObject()->GetChildren();
			while(children.hasMoreElements())
			{
				SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(children.getNext());
				OgreTerrainPageComponentPtr page = child->GetFirstComponentByClass<OgreTerrainPageComponent>();
				if(page)
				{
					page->UpdatePosition();
				}
			}
		}
	}

	Vec3 OgreTerrainGroupComponent::GetOrigin() const
	{
		return m_Origin;
	}

	Vec2i OgreTerrainGroupComponent::GetPages() const
	{
		Vec2i size;
		size.x = 0;
		size.y = 0;
		return size;
	}

	void OgreTerrainGroupComponent::CreatePages(const Vec2i  &size)
	{

		if(GetSceneObject())
		{
			RemoveAllPages();

			for(int x = 0 ;x < size.x; x++)
			{
				for(int y = 0 ;y < size.y ;y++)
				{
					SceneObjectPtr so = SimEngine::Get().CreateObjectFromTemplate("OgreTerrainPageObject");
					if(so)
					{
						OgreTerrainPageComponentPtr comp = so->GetFirstComponentByClass<OgreTerrainPageComponent>();
						comp->SetIndexX(x);
						comp->SetIndexY(y);
						comp->SetPosition(Vec3(m_TerrainWorldSize*x,0,m_TerrainWorldSize*y));
						GetSceneObject()->AddChildSceneObject(so,true);

					}
				}
			}
		}
	}

	std::string OgreTerrainGroupComponent::GetCustomMaterial() const
	{
		return m_CustomMaterial;
	}

	void OgreTerrainGroupComponent::SetCustomMaterial(const std::string &material)
	{
		m_CustomMaterial = material;
	}

	std::string OgreTerrainGroupComponent::GetLoadTerrain() const
	{
		return m_TerrainResource.Name();
	}

	void OgreTerrainGroupComponent::SetLoadTerrain(const std::string &filename) 
	{
		m_TerrainResource.SetName(filename);
		if(m_TerrainGroup)
		{
			m_TerrainGroup->setFilenameConvention(m_TerrainResource.Name(), "dat");

			//Get all components
			ComponentContainer::ComponentVector comps;
			GetSceneObject()->GetComponentsByClassName(comps, "OgreTerrainPageComponent");

			for(int i = 0 ;  i < comps.size(); i++)
			{
				OgreTerrainPageComponentPtr page = GASS_DYNAMIC_PTR_CAST<OgreTerrainPageComponent>(comps[i]);
				if(page)
				{
					page->LoadFromFile();
				}
			}
		}
	}

	void OgreTerrainGroupComponent::ConfigureTerrainDefaults()
	{
		// Configure global
		//
		delete m_TerrainGroup;

		m_TerrainGlobals = Ogre::TerrainGlobalOptions::getSingletonPtr();

		if(!m_TerrainGlobals)
			m_TerrainGlobals = new  Ogre::TerrainGlobalOptions();
		m_TerrainGroup = new Ogre::TerrainGroup(m_OgreSceneManager, Ogre::Terrain::ALIGN_X_Z, static_cast<Ogre::uint16>(m_TerrainSize), static_cast<Ogre::Real>(m_TerrainWorldSize));
		SetOrigin(m_Origin);
		m_TerrainGroup->setResourceGroup(GetSceneObject()->GetScene()->GetResourceGroupName());

		if(m_CustomMaterial != "")
		{
			m_TerrainGlobals->setDefaultMaterialGenerator(Ogre::SharedPtr<Ogre::TerrainMaterialGenerator>( OGRE_NEW GASSTerrainMaterialGenerator(m_CustomMaterial)));
			GASSTerrainMaterialGenerator::CustomMaterialProfile *profile = static_cast<GASSTerrainMaterialGenerator::CustomMaterialProfile*>(m_TerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());
			profile->setCompositeMapEnabled(false);
			profile->setLightmapEnabled(false);
		}
		else
		{
			//m_TerrainGlobals->setDefaultMaterialGenerator(Ogre::SharedPtr<Ogre::TerrainMaterialGenerator>( OGRE_NEW Ogre::TerrainMaterialGeneratorB()));
			//m_TerrainProfile =	static_cast<Ogre::TerrainMaterialGeneratorB::SM2Profile*>(m_TerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());

			m_TerrainGlobals->setDefaultMaterialGenerator(Ogre::SharedPtr<Ogre::TerrainMaterialGenerator>( OGRE_NEW Ogre::TerrainMaterialGeneratorC()));
			m_TerrainProfile =	static_cast<Ogre::TerrainMaterialGeneratorC::SM2Profile*>(m_TerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());
			//m_TerrainProfile =	static_cast<Ogre::TerrainMaterialGeneratorA::SM2Profile*>(m_TerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());

			//we don't support composite maps, recursion bug in when doing synchronized load -> stack overflow.  
			m_TerrainProfile->setCompositeMapEnabled(false);
			m_TerrainProfile->setLightmapEnabled(m_EnableLightmap);

			m_TerrainProfile->setReceiveDynamicShadowsEnabled(true);
			m_TerrainProfile->setReceiveDynamicShadowsDepth(true);
			m_TerrainProfile->setReceiveDynamicShadowsLowLod(true);

			m_TerrainProfile->SetFadeDetail(GetFadeDetail());
			m_TerrainProfile->SetDetailFadeDist(GetDetailFadeDist());
			m_TerrainProfile->SetFadeOutColor(GetFadeOutColor());
			m_TerrainProfile->SetNearColorWeight(GetNearColorWeight());

			m_TerrainProfile->setLayerNormalMappingEnabled(m_EnableLayerNormal);
			m_TerrainProfile->setLayerParallaxMappingEnabled(m_EnableLayerParallax);
			m_TerrainProfile->setLayerSpecularMappingEnabled(m_EnableLayerSpecular);

		}

		m_TerrainGlobals->setMaxPixelError(8);
		// testing composite map
		m_TerrainGlobals->setCompositeMapDistance(20000);


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
		defaultimp.terrainSize = static_cast<Ogre::uint16>(m_TerrainSize);
		defaultimp.worldSize = static_cast<Ogre::Real>(m_TerrainWorldSize);
		defaultimp.inputScale = m_TerrainScale;
		defaultimp.minBatchSize = 33;
		defaultimp.maxBatchSize = 65;

		defaultimp.layerList.resize(5);
		defaultimp.layerList[0].worldSize = 10;
		defaultimp.layerList[0].textureNames.push_back("detail_default.dds");

		defaultimp.layerList[1].worldSize = 10;
		defaultimp.layerList[1].textureNames.push_back("detail_default.dds");

		defaultimp.layerList[2].worldSize = 10;
		defaultimp.layerList[2].textureNames.push_back("detail_default.dds");

		defaultimp.layerList[3].worldSize = 10;
		defaultimp.layerList[3].textureNames.push_back("detail_default.dds");

		defaultimp.layerList[4].worldSize = 10;
		defaultimp.layerList[4].textureNames.push_back("detail_default.dds");

		m_TerrainGroup->setFilenameConvention(m_TerrainResource.Name(), "dat");
	}

	float OgreTerrainGroupComponent::GetImportScale() const
	{
		return m_TerrainScale;
	}

	void OgreTerrainGroupComponent::SetImportScale(const float &value)
	{
		m_TerrainScale = value;
		if(m_TerrainGroup)
		{
			Ogre::Terrain::ImportData& defaultimp = m_TerrainGroup->getDefaultImportSettings();
			defaultimp.inputScale = value;
		}
	}

	int OgreTerrainGroupComponent::GetImportTerrainSize() const
	{
		return m_TerrainSize;
	}

	void OgreTerrainGroupComponent::SetImportTerrainSize(const int &value)
	{
		m_TerrainSize = value;
		if(m_TerrainGroup)
		{
			Ogre::Terrain::ImportData& defaultimp = m_TerrainGroup->getDefaultImportSettings();
			defaultimp.terrainSize = static_cast<Ogre::uint16>(value);

			RemoveAllPages();
			ConfigureTerrainDefaults();
		}
	}

	Float OgreTerrainGroupComponent::GetImportTerrainWorldSize() const
	{
		return m_TerrainWorldSize;
	}

	void OgreTerrainGroupComponent::SetImportTerrainWorldSize(const Float &value)
	{
		m_TerrainWorldSize = static_cast<float>(value);
		if(m_TerrainGroup)
		{

			Ogre::Terrain::ImportData& defaultimp = m_TerrainGroup->getDefaultImportSettings();
			defaultimp.worldSize = static_cast<Ogre::Real>(value);
			RemoveAllPages();
			ConfigureTerrainDefaults();
		}
	}

	void OgreTerrainGroupComponent::RemoveAllPages()
	{
		//remove all children
		ComponentContainer::ComponentContainerIterator children = GetSceneObject()->GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(children.getNext());
			OgreTerrainPageComponentPtr page = child->GetFirstComponentByClass<OgreTerrainPageComponent>();
			if(page)
			{
				GetSceneObject()->RemoveChildSceneObject(child);
				children = GetSceneObject()->GetChildren();
			}
		}
	}

	void OgreTerrainGroupComponent::SetSaveTerrain(const std::string &filename)
	{
		m_TerrainResource.SetName(FileUtils::GetFilename(filename));
		if(m_TerrainGroup)
		{
			m_TerrainGroup->setFilenameConvention(m_TerrainResource.Name(), "dat");
			m_TerrainGroup->saveAllTerrains(false);
		}
	}

	std::string OgreTerrainGroupComponent::GetSaveTerrain() const
	{
		return m_TerrainResource.Name();
	}

	AABox OgreTerrainGroupComponent::GetBoundingBox() const
	{
		AABox aabox;

		if(m_TerrainGroup)
		{
			//Get all components
			ComponentContainer::ComponentVector comps;
			GetSceneObject()->GetComponentsByClassName(comps, "OgreTerrainPageComponent");

			for(int i = 0 ;  i < comps.size(); i++)
			{
				OgreTerrainPageComponentPtr page = GASS_DYNAMIC_PTR_CAST<OgreTerrainPageComponent>(comps[i]);
				if(page)
				{

					AABox page_bb = page->GetBoundingBox();
					aabox.Union(page_bb);
				}
			}
		}
		return aabox;
	}

	Sphere OgreTerrainGroupComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		if(m_TerrainGroup)
		{
			//Get all components
			ComponentContainer::ComponentVector comps;
			GetSceneObject()->GetComponentsByClassName(comps, "OgreTerrainPageComponent");

			for(int i = 0 ;  i < comps.size(); i++)
			{
				OgreTerrainPageComponentPtr page = GASS_DYNAMIC_PTR_CAST<OgreTerrainPageComponent>(comps[i]);
				if(page)
				{

					Sphere page_sphere = page->GetBoundingSphere();
					sphere.Union(page_sphere);
				}
			}
		}
		return sphere;
	}

	GraphicsMesh OgreTerrainGroupComponent::GetMeshData() const
	{
		GraphicsMesh data;
		return data;
	}

	void OgreTerrainGroupComponent::OnTerrainLayerPaint(TerrainPaintRequestPtr message)
	{
		// figure out which terrains this affects
		Ogre::TerrainGroup::TerrainList terrainList;
		Ogre::Real brush_size = message->GetBrushSize();
		Ogre::Real inner_radius = message->GetBrushInnerSize()*0.5f;
		if(inner_radius > brush_size*0.5f)
			inner_radius = brush_size*0.5f;
		Ogre::Vector3 position = OgreConvert::ToOgre(message->GetPosition());
		float intensity = message->GetIntensity();
		Ogre::Sphere sphere(position, brush_size);
		m_TerrainGroup->sphereIntersects(sphere, &terrainList);

		for (Ogre::TerrainGroup::TerrainList::iterator ti = terrainList.begin();ti != terrainList.end(); ++ti)
			PaintTerrain(*ti, position, intensity,brush_size/m_TerrainGroup->getTerrainWorldSize(),inner_radius/m_TerrainGroup->getTerrainWorldSize(), (int)message->GetLayer(),message->GetNoise());

		m_TerrainGroup->update();
	}

	void OgreTerrainGroupComponent::OnTerrainHeightModify(TerrainHeightModifyRequestPtr message)
	{
		// figure out which terrains this affects
		Ogre::TerrainGroup::TerrainList terrainList;
		Ogre::Real brush_size = message->GetBrushSize();
		Ogre::Real inner_radius = message->GetBrushInnerSize()*0.5f;
		if(inner_radius > brush_size*0.5f)
			inner_radius = brush_size*0.5f;
		Ogre::Vector3 position = OgreConvert::ToOgre(message->GetPosition());
		float intensity = message->GetIntensity();
		Ogre::Sphere sphere(position, brush_size);
		m_TerrainGroup->sphereIntersects(sphere, &terrainList);

		if(message->GetModifyType() == TerrainHeightModifyRequest::MT_DEFORM)
		{
			for (Ogre::TerrainGroup::TerrainList::iterator ti = terrainList.begin();ti != terrainList.end(); ++ti)
				DeformTerrain(*ti, position, intensity,brush_size/m_TerrainGroup->getTerrainWorldSize(),inner_radius/m_TerrainGroup->getTerrainWorldSize(), message->GetNoise());
		}
		else if(message->GetModifyType() == TerrainHeightModifyRequest::MT_FLATTEN)
		{
			for (Ogre::TerrainGroup::TerrainList::iterator ti = terrainList.begin();ti != terrainList.end(); ++ti)
				FlattenTerrain(*ti, position,intensity,brush_size/m_TerrainGroup->getTerrainWorldSize(),inner_radius/m_TerrainGroup->getTerrainWorldSize());
		}
		else if(message->GetModifyType() == TerrainHeightModifyRequest::MT_SMOOTH)
		{
			Ogre::Real avg_height = 0;
			for (Ogre::TerrainGroup::TerrainList::iterator ti = terrainList.begin();ti != terrainList.end(); ++ti)
				GetAverageHeight(*ti, position,brush_size/m_TerrainGroup->getTerrainWorldSize(),avg_height);

			for (Ogre::TerrainGroup::TerrainList::iterator ti = terrainList.begin();ti != terrainList.end(); ++ti)
				SmoothTerrain(*ti, position, intensity ,brush_size/m_TerrainGroup->getTerrainWorldSize(),inner_radius/m_TerrainGroup->getTerrainWorldSize(),avg_height);
		}
		m_TerrainGroup->update();
	}

	void OgreTerrainGroupComponent::OnRoadMessage(RoadRequestPtr message)
	{
		// figure out which terrains this affects
		Ogre::Real brush_size = (std::max)(message->GetFlattenWidth(),message->GetPaintWidth());
		std::vector<Vec3> rwps = message->GetRoadWaypoints();
		for(size_t i = 0; i < rwps.size()-1; i++)
		{
			const Vec3 line = rwps[i+1] - rwps[i];
			const Float  length = line.Length();
			const Vec3 dir = line*(1.0/length);
			const Float  step_size = 0.1;
			Float dist = 0;
			while(dist < length)
			{
				Vec3 pos = rwps[i] + dir*dist;
				dist += step_size;

				Ogre::TerrainGroup::TerrainList terrainList;
				Ogre::Vector3 position = OgreConvert::ToOgre(pos);
				Ogre::Sphere sphere(position, brush_size);
				m_TerrainGroup->sphereIntersects(sphere, &terrainList);
				for (Ogre::TerrainGroup::TerrainList::iterator ti = terrainList.begin();ti != terrainList.end(); ++ti)
				{
					if(message->GetFlattenWidth() > 0)
						FlattenTerrain(*ti, position,1.0f, message->GetFlattenWidth()/m_TerrainGroup->getTerrainWorldSize(),message->GetFlattenWidth()*0.5f/m_TerrainGroup->getTerrainWorldSize());
					if(message->GetPaintWidth())
						PaintTerrain(*ti,position, message->GetPaintIntensity(), message->GetPaintWidth()/m_TerrainGroup->getTerrainWorldSize(),message->GetPaintWidth()*0.5f * 0.5f/m_TerrainGroup->getTerrainWorldSize(), message->GetLayer(), 0);
				}
			}
		}
		m_TerrainGroup->update();
	}

	void OgreTerrainGroupComponent::FlattenTerrain(Ogre::Terrain* terrain,const Ogre::Vector3& start, Ogre::Vector3& end)
	{
		Ogre::Vector3 ts_start;
		Ogre::Vector3 ts_end;
		terrain->getTerrainPosition(start, &ts_start);
		terrain->getTerrainPosition(end, &ts_end);
		Ogre::Real terrainSize = static_cast<Ogre::Real>(terrain->getSize() - 1);
		long start_x = static_cast<long>(ts_start.x  * terrainSize);
		long start_y = static_cast<long>(ts_start.y  * terrainSize);
		long end_x = static_cast<long>(ts_end.x * terrainSize);
		long end_y= static_cast<long>(ts_end.y * terrainSize);

		float x_dist = static_cast<float>(end_x - start_x);
		float y_dist = static_cast<float>(end_y - start_y);

		if(fabs(x_dist) > fabs(y_dist))
		{

			Ogre::Vector3 dir,start_pos;
			if(start_x > end_x) //swap
			{
				long temp = end_x;
				end_x = start_x;
				start_x = temp;

				temp = end_y;
				end_y = start_y;
				start_y = temp;

				dir = start-end;
				start_pos = end;
			}
			else
			{
				dir = end-start;
				start_pos = start;
			}

			//Ogre::Real dist = dir.normalise();

			x_dist = static_cast<float>(end_x - start_x);
			y_dist = static_cast<float>(end_y - start_y);

			float step = y_dist/x_dist;

			int count = 0;
			for (long x = start_x; x <= end_x; ++x)
			{
				long y = start_y + static_cast<long>(step*count);

				//Ogre::Real tsXdist = (x / terrainSize);
				//Ogre::Real tsYdist = (y / terrainSize);

				Ogre::Vector3 pos = start_pos + dir * Ogre::Real(count)/Ogre::Real(x_dist);
				terrain->setHeightAtPoint(x, y, pos.y);//*weight + current_height*(1-weight));
				count++;
			}
		}
		else
		{

			Ogre::Vector3 dir,start_pos;
			if(start_y > end_y) //swap
			{
				long temp = end_y;
				end_y = start_y;
				start_y = temp;

				temp = end_x;
				end_x = start_x;
				start_x = temp;

				dir = start-end;
				start_pos = end;
			}
			else
			{
				dir = end-start;
				start_pos = start;
			}

			//Ogre::Real dist = dir.normalise();

			x_dist = end_x - start_x;
			y_dist = end_y - start_y;

			float step = x_dist/y_dist;

			int count = 0;
			for (long y = start_y; y <= end_y; ++y)
			{
				long x = start_x + step*count;

				Ogre::Vector3 pos = start_pos + dir * Ogre::Real(count)/Ogre::Real(y_dist);
				terrain->setHeightAtPoint(x, y, pos.y);//*weight + current_height*(1-weight));
				count++;
			}
		}
	}

	void OgreTerrainGroupComponent::FlattenTerrain(Ogre::Terrain* terrain,const Ogre::Vector3& centrepos, Ogre::Real /*intensity*/, float brush_size_terrain_space, float brush_inner_radius)
	{
		Ogre::Vector3 tsPos;
		terrain->getTerrainPosition(centrepos, &tsPos);
		//#if OGRE_PLATFORM != OGRE_PLATFORM_IPHONE
		// we need point coords
		Ogre::Real terrainSize = (terrain->getSize() - 1);
		long startx = (tsPos.x - brush_size_terrain_space) * terrainSize;
		long starty = (tsPos.y - brush_size_terrain_space) * terrainSize;
		long endx = (tsPos.x + brush_size_terrain_space) * terrainSize;
		long endy= (tsPos.y + brush_size_terrain_space) * terrainSize;
		startx = (std::max)(startx, 0L);
		starty = (std::max)(starty, 0L);
		endx = (std::min)(endx, (long)terrainSize);
		endy = (std::min)(endy, (long)terrainSize);
		float newheight = centrepos.y;
		for (long y = starty; y <= endy; ++y)
		{
			for (long x = startx; x <= endx; ++x)
			{
				Ogre::Real tsXdist = (x / terrainSize) - tsPos.x;
				Ogre::Real tsYdist = (y / terrainSize)  - tsPos.y;

				Ogre::Real dist = Ogre::Math::Sqrt(tsYdist * tsYdist + tsXdist * tsXdist);
				Ogre::Real weight = (std::min)((Ogre::Real)1.0,((dist - brush_inner_radius )/ Ogre::Real(0.5 * brush_size_terrain_space - brush_inner_radius)));

				//Ogre::Real weight = (std::min)((Ogre::Real)1.0, 
				//	(Ogre::Math::Sqrt(tsYdist * tsYdist + tsXdist * tsXdist)- brush_inner_radius )/ Ogre::Real(0.5 * brush_size_terrain_space - brush_inner_radius));
				if( weight < 0) weight = 0;
				weight = 1.0 - (weight * weight);

				//float addedHeight = weight * intensity;

				//float current_h = terrain->getHeightAtPoint(x, y);
				if(weight >= 1)
				{
					terrain->setHeightAtPoint(x, y, newheight);
				}
				/*else
				{
				if((current_h + addedHeight) < newheight)
				terrain->setHeightAtPoint(x, y, current_h + addedHeight);
				else
				terrain->setHeightAtPoint(x, y, current_h - addedHeight);
				}*/
			}
		}
	}

	void OgreTerrainGroupComponent::DeformTerrain(Ogre::Terrain* terrain,const Ogre::Vector3& centrepos, Ogre::Real timeElapsed, float brush_size_terrain_space, float brush_inner_radius, float noise)
	{
		Ogre::Vector3 tsPos;
		terrain->getTerrainPosition(centrepos, &tsPos);
		//#if OGRE_PLATFORM != OGRE_PLATFORM_IPHONE
		// we need point coords
		Ogre::Real terrainSize = (terrain->getSize() - 1);
		long startx = (tsPos.x - brush_size_terrain_space) * terrainSize;
		long starty = (tsPos.y - brush_size_terrain_space) * terrainSize;
		long endx = (tsPos.x + brush_size_terrain_space) * terrainSize;
		long endy= (tsPos.y + brush_size_terrain_space) * terrainSize;
		startx = (std::max)(startx, 0L);
		starty = (std::max)(starty, 0L);
		endx = (std::min)(endx, (long)terrainSize);
		endy = (std::min)(endy, (long)terrainSize);
		//Ogre::Real inner_radius = brush_size_terrain_space*0.5*0.7;
		for (long y = starty; y <= endy; ++y)
		{
			for (long x = startx; x <= endx; ++x)
			{
				Ogre::Real tsXdist = (x / terrainSize) - tsPos.x;
				Ogre::Real tsYdist = (y / terrainSize)  - tsPos.y;

				Ogre::Real weight = (std::min)((Ogre::Real)1.0, 
					(Ogre::Math::Sqrt(tsYdist * tsYdist + tsXdist * tsXdist)- brush_inner_radius )/ Ogre::Real(0.5 * brush_size_terrain_space - brush_inner_radius));
				if( weight < 0) weight = 0;
				weight = 1.0 - (weight * weight);

				float rand_w = Ogre::Math::RangeRandom(0, 1);

				float addedHeight = weight * timeElapsed + weight*rand_w*noise*timeElapsed;
				float newheight;
				newheight = terrain->getHeightAtPoint(x, y) + addedHeight;
				terrain->setHeightAtPoint(x, y, newheight);
			}
		}
	}

	void OgreTerrainGroupComponent::GetAverageHeight(Ogre::Terrain* terrain, const Ogre::Vector3& centrepos, const Ogre::Real  brush_size_terrain_space,Ogre::Real &avg_height)
	{
		Ogre::Vector3 tsPos;
		terrain->getTerrainPosition(centrepos, &tsPos);

		Ogre::Real terrainSize = (terrain->getSize() - 1);
		long startx = (tsPos.x - brush_size_terrain_space) * terrainSize;
		long starty = (tsPos.y - brush_size_terrain_space) * terrainSize;
		long endx = (tsPos.x + brush_size_terrain_space) * terrainSize;
		long endy= (tsPos.y + brush_size_terrain_space) * terrainSize;
		startx = (std::max)(startx, 0L);
		starty = (std::max)(starty, 0L);
		endx = (std::min)(endx, (long)terrainSize);
		endy = (std::min)(endy, (long)terrainSize);

		long count = 0;
		for (long y = starty; y <= endy; ++y)
		{
			for (long x = startx; x <= endx; ++x)
			{
				count++;
				//Ogre::Real tsXdist = (x / terrainSize) - tsPos.x;
				//Ogre::Real tsYdist = (y / terrainSize)  - tsPos.y;
				avg_height += terrain->getHeightAtPoint(x, y);
			}
		}
		avg_height = avg_height/Ogre::Real(count);
	}

	void OgreTerrainGroupComponent::SmoothTerrain(Ogre::Terrain* terrain,const Ogre::Vector3& centrepos, const Ogre::Real intensity, const Ogre::Real brush_size_terrain_space, const Ogre::Real brush_inner_radius, const Ogre::Real average_height)
	{
		Ogre::Vector3 tsPos;
		terrain->getTerrainPosition(centrepos, &tsPos);
		const Ogre::Real terrainSize = (terrain->getSize() - 1);
		long startx = (tsPos.x - brush_size_terrain_space) * terrainSize;
		long starty = (tsPos.y - brush_size_terrain_space) * terrainSize;
		long endx = (tsPos.x + brush_size_terrain_space) * terrainSize;
		long endy= (tsPos.y + brush_size_terrain_space) * terrainSize;
		startx = (std::max)(startx, 0L);
		starty = (std::max)(starty, 0L);
		endx = (std::min)(endx, (long)terrainSize);
		endy = (std::min)(endy, (long)terrainSize);

		for (long y = starty; y <= endy; ++y)
		{
			for (long x = startx; x <= endx; ++x)
			{
				Ogre::Real tsXdist = (x / terrainSize) - tsPos.x;
				Ogre::Real tsYdist = (y / terrainSize)  - tsPos.y;

				Ogre::Real weight = (std::min)((Ogre::Real)1.0, 
					(Ogre::Math::Sqrt(tsYdist * tsYdist + tsXdist * tsXdist)- brush_inner_radius )/ Ogre::Real(0.5 * brush_size_terrain_space - brush_inner_radius));
				if( weight < 0) weight = 0;
				weight = 1.0 - (weight * weight);

				float height = terrain->getHeightAtPoint(x, y);
				float newheight = average_height - height;
				newheight = newheight * weight * intensity*0.05;
				terrain->setHeightAtPoint(x, y, height + newheight);
			}
		}
	}

	void OgreTerrainGroupComponent::PaintTerrain(Ogre::Terrain* terrain,const Ogre::Vector3& centrepos, const Ogre::Real intensity, const Ogre::Real brush_size_terrain_space, const Ogre::Real brush_inner_radius, int layer_index, float noise)
	{
		Ogre::TerrainLayerBlendMap* layer = terrain->getLayerBlendMap(layer_index);
		// we need image coords
		Ogre::Vector3 tsPos;
		terrain->getTerrainPosition(centrepos, &tsPos);
		Ogre::Real imgSize = terrain->getLayerBlendMapSize();
		long startx = (tsPos.x - brush_size_terrain_space) * imgSize;
		long starty = (tsPos.y - brush_size_terrain_space) * imgSize;
		long endx = (tsPos.x + brush_size_terrain_space) * imgSize;
		long endy= (tsPos.y + brush_size_terrain_space) * imgSize;
		startx = (std::max)(startx, 0L);
		starty = (std::max)(starty, 0L);
		endx = (std::min)(endx, (long)imgSize);
		endy = (std::min)(endy, (long)imgSize);
		for (long y = starty; y <= endy; ++y)
		{
			for (long x = startx; x <= endx; ++x)
			{
				Ogre::Real tsXdist = (x / imgSize) - tsPos.x;
				Ogre::Real tsYdist = (y / imgSize)  - tsPos.y;


				Ogre::Real weight = (std::min)((Ogre::Real)1.0, 
					(Ogre::Math::Sqrt(tsYdist * tsYdist + tsXdist * tsXdist)- brush_inner_radius )/ Ogre::Real(0.5 * brush_size_terrain_space - brush_inner_radius));


				if( weight < 0) 
					weight = 0;

				weight = 1.0 - (weight * weight);

				float rand_w = Ogre::Math::RangeRandom(0, 1);
				weight -= rand_w*noise*0.5;

				if( weight < 0) 
					weight = 0;

				float paint = weight * intensity;
				size_t imgY = imgSize - y;
				float val;
				val = layer->getBlendValue(x, imgY) + paint;
				val = Ogre::Math::Clamp(val, 0.0f, 1.0f);
				layer->setBlendValue(x, imgY, val);
				layer->update();
			}
		}
	}

	GeometryFlags OgreTerrainGroupComponent::GetGeometryFlags() const
	{
		return m_GeomFlags;
	}

	void OgreTerrainGroupComponent::SetGeometryFlags(GeometryFlags flags)
	{
		m_GeomFlags = flags;
	}

	void OgreTerrainGroupComponent::SetFadeDetail(bool value) 
	{
		m_FadeDetail = value;
		if(m_TerrainProfile)
			m_TerrainProfile->SetFadeDetail(GetFadeDetail());
	}

	void OgreTerrainGroupComponent::SetDetailFadeDist(float value) 
	{
		m_DetailFadeDist = value;
		if(m_TerrainProfile)
			m_TerrainProfile->SetDetailFadeDist(GetDetailFadeDist());

	}
	void OgreTerrainGroupComponent::SetFadeOutColor(bool  value) 
	{
		m_FadeOutColor= value;
		if(m_TerrainProfile)
			m_TerrainProfile->SetFadeOutColor(GetFadeOutColor());
	}

	void OgreTerrainGroupComponent::SetNearColorWeight(float value) 
	{
		m_NearColorWeight= value;
		if(m_TerrainProfile)
			m_TerrainProfile->SetNearColorWeight(GetNearColorWeight());
	}

	bool OgreTerrainGroupComponent::GetCollision() const
	{
		if(m_Collision)
			return m_Collision->GetActive();
		return false;
	}

	void OgreTerrainGroupComponent::SetCollision(bool value)
	{
		if(m_Collision)
			m_Collision->SetActive(value);
	}
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif
