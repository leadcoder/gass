/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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
#include <OgreEntity.h>
#include <Ogre.h>
#include <OgreSceneManager.h>
#include <OgreHardwarePixelBuffer.h>
#include "TreeGeometryComponent.h"
#include "PagedGeometry.h"
#include "ImpostorPage.h"
#include "BatchPage.h"
#include "TreeLoader2D.h"
#include "TreeLoader3D.h"
#include "StaticBillboardSet.h"
#include "GrassLoader.h"
#include "DensityMapComponent.h"

#include "Plugins/Ogre/IOgreSceneManagerProxy.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSScene.h"



//#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"


namespace GASS
{
	IHeightmapTerrainComponent* TreeGeometryComponent::m_Terrain = NULL;

	void TreeGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("TreeGeometryComponent",new Creator<TreeGeometryComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ObjectMetaDataPtr(new ObjectMetaData("TreeGeometryComponent", OF_VISIBLE)));

		RegisterProperty<std::string>("Mesh", &TreeGeometryComponent::GetMesh, &TreeGeometryComponent::SetMesh,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE)));
		RegisterProperty<std::string>("ColorMap", &TreeGeometryComponent::GetColorMap, &TreeGeometryComponent::SetColorMap,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE)));
		RegisterProperty<float>("DensityFactor", &TreeGeometryComponent::GetDensityFactor, &TreeGeometryComponent::SetDensityFactor,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("MeshDistance", &TreeGeometryComponent::GetMeshDistance, &TreeGeometryComponent::SetMeshDistance,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("MeshFadeDistance", &TreeGeometryComponent::GetMeshFadeDistance, &TreeGeometryComponent::SetMeshFadeDistance,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("ImposterDistance", &TreeGeometryComponent::GetImposterDistance, &TreeGeometryComponent::SetImposterDistance,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("ImposterFadeDistance", &TreeGeometryComponent::GetImposterFadeDistance, &TreeGeometryComponent::SetImposterFadeDistance,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec4>("CustomBounds", &TreeGeometryComponent::GetCustomBounds, &TreeGeometryComponent::SetCustomBounds,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE)));
		RegisterProperty<Vec2>("MaxMinScale", &TreeGeometryComponent::GetMaxMinScale, &TreeGeometryComponent::SetMaxMinScale,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("CastShadows", &TreeGeometryComponent::GetCastShadows, &TreeGeometryComponent::SetCastShadows,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		//RegisterProperty<bool>("CreateShadowMap", &TreeGeometryComponent::GetCreateShadowMap, &TreeGeometryComponent::SetCreateShadowMap);
		RegisterProperty<bool>("SetHeightAtStartup", &TreeGeometryComponent::GetPrecalcHeight, &TreeGeometryComponent::SetPrecalcHeight,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("PageSize", &TreeGeometryComponent::GetPageSize, &TreeGeometryComponent::SetPageSize,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("ImposterAlphaRejectionValue", &TreeGeometryComponent::GetImposterAlphaRejectionValue, &TreeGeometryComponent::SetImposterAlphaRejectionValue,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<int>("ImposterResolution", &TreeGeometryComponent::GetImposterResolution, &TreeGeometryComponent::SetImposterResolution,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("RegenerateAllImpostors", &TreeGeometryComponent::GetRegenerateAllImpostors, &TreeGeometryComponent::SetRegenerateAllImpostors,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("DynamicImpostorLighting", &TreeGeometryComponent::GetDynamicImpostorLighting, &TreeGeometryComponent::SetDynamicImpostorLighting,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Enable experimental dynamic impostor lighting. Note that this will effect all TreeGeometryComponent",PF_VISIBLE | PF_EDITABLE)));
		
	}


	TreeGeometryComponent::TreeGeometryComponent(void) : m_CustomBounds(0,0,0,0), 
		m_TreeLoader2d(NULL),
		m_TreeLoader3d(NULL),
		m_DensityFactor(0.001),
		m_MaxMinScale(1.1, 0.9),
		m_CastShadows(true),
		m_MeshDist(100),
		m_ImposterDist(500),
		m_PrecalcHeight(true),
		m_PageSize(80),
		m_MeshFadeDist(0),
		m_ImposterFadeDist(0),
		m_ImposterAlphaRejectionValue(50),
		m_CreateShadowMap(false),
		m_TreeEntity(NULL),
		m_ImposterResolution(128),
		m_DynamicImpostorLighting(false)
	{
		m_RandomTable = new RandomTable();

	}

	TreeGeometryComponent::~TreeGeometryComponent(void)
	{
		delete m_RandomTable;
	}


	void TreeGeometryComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(TreeGeometryComponent::OnPaint,GrassPaintMessage,0));
		
		SetDynamicImpostorLighting(m_DynamicImpostorLighting);
		ImpostorPage::setImpostorBackgroundColor(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.0f));
		ImpostorPage::setImpostorResolution(m_ImposterResolution);
		
		Ogre::SceneManager* sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<IOgreSceneManagerProxy>()->GetOgreSceneManager();
		Ogre::Camera* ocam = NULL;
		if(sm->hasCamera("DummyCamera"))
			ocam = sm->getCamera("DummyCamera");
		else
			ocam = sm->createCamera("DummyCamera");

		Ogre::RenderSystem::RenderTargetIterator iter = Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator();
		while (iter.hasMoreElements())
		{
			//only render windows for now
			Ogre::RenderWindow* target = dynamic_cast<Ogre::RenderWindow*>(iter.getNext());
			if(target)
				target->addListener(this);
		}

		bool user_bounds = true;
		if(m_CustomBounds.x == 0 && m_CustomBounds.y == 0 && m_CustomBounds.z == 0 && m_CustomBounds.w == 0)
		{
			user_bounds = false;
			m_MapBounds = TBounds(m_CustomBounds.x, m_CustomBounds.y, m_CustomBounds.z, m_CustomBounds.w);
		}

		if(!user_bounds)
		{
			SceneObjectPtr root = GetSceneObject()->GetScene()->GetRootSceneObject();
			HeightmapTerrainComponentPtr terrain = root->GetFirstComponentByClass<IHeightmapTerrainComponent>(true);
			if(terrain)
			{
				GeometryComponentPtr geom = DYNAMIC_PTR_CAST<IGeometryComponent>(terrain);
				AABox aabox = geom->GetBoundingBox();

				//for speed we save the raw pointer , we will access this for each height callback
				m_Terrain = terrain.get();
				m_MapBounds = TBounds(aabox.m_Min.x, aabox.m_Min.z, aabox.m_Max.x, aabox.m_Max.z);
			}
		}

		m_PagedGeometry = new PagedGeometry(ocam, m_PageSize);
		m_PagedGeometry->setShadersEnabled(true);
		m_DensityMap = GetSceneObject()->GetFirstComponentByClass<DensityMapComponent>();
		if(m_DensityMap)
			m_DensityMap->SetMapBounds(m_MapBounds);



		//ImpostorPage::setImpostorColor(Ogre::ColourValue(0,0,0,1));
		ImpostorPage::setImpostorBackgroundColor(Ogre::ColourValue(0,0,0,1));
		assert(m_PagedGeometry);
		m_PagedGeometry->setImposterAlphaRejectValue(m_ImposterAlphaRejectionValue);
		if(m_MeshDist > 0)
		{
			//m_PagedGeometry->addDetailLevel<BatchPage>(m_MeshDist*0.1,m_MeshFadeDist,Ogre::Any(1));
			m_PagedGeometry->addDetailLevel<BatchPage>(m_MeshDist*0.5,m_MeshFadeDist,Ogre::Any(0));
			m_PagedGeometry->addDetailLevel<BatchPage>(m_MeshDist,m_MeshFadeDist,Ogre::Any(1));
			//m_PagedGeometry->addDetailLevel<BatchPage>(m_MeshDist,0,Ogre::Any(3));
		}
		if(m_ImposterDist >  0)
		{
			GeometryPageManager* geom_man = m_PagedGeometry->addDetailLevel<ImpostorPage>(m_ImposterDist,m_ImposterFadeDist);
		}

		if(m_PrecalcHeight)
		{
			m_TreeLoader3d = new TreeLoader3D(m_PagedGeometry, m_MapBounds);
			if(m_ColorMapFilename!= "") 
				m_TreeLoader3d->setColorMap(m_ColorMapFilename);

			m_PagedGeometry->setPageLoader(m_TreeLoader3d);
		}
		else
		{
			m_TreeLoader2d = new TreeLoader2D(m_PagedGeometry, m_MapBounds);
			m_TreeLoader2d->setHeightFunction(TreeGeometryComponent::GetTerrainHeight);
			if(m_ColorMapFilename!= "") 
				m_TreeLoader2d->setColorMap(m_ColorMapFilename);
			m_PagedGeometry->setPageLoader(m_TreeLoader2d);
		}
		float volume = m_MapBounds.width() * m_MapBounds.height();
		unsigned int treeCount = m_DensityFactor * volume;

		//TerrainComponentPtr terrain = GetSceneObject()->GetFirstComponentByClass<ITerrainComponent>();
		//m_Terrain = terrain.get();
		/*Image shadowMap;
		int shadow_size = 4096;
		if(m_CreateShadowMap)
		{
			shadowMap.Allocate(shadow_size, shadow_size, 24);
		}*/
		static unsigned int tree_id = 0;
		tree_id++;
		std::stringstream ss;
		std::string name;
		ss << GetName() << tree_id;
		ss >> name;
		
		m_TreeEntity = sm->createEntity(name, m_MeshFileName);
		m_TreeEntity->setCastShadows(m_CastShadows);
		//add one dummy tree to allocate grid list
		if(m_TreeLoader3d)
		{
			m_TreeLoader3d->addTree(m_TreeEntity,  Ogre::Vector3(0, 0, 0));
			m_TreeLoader3d->deleteTrees(Ogre::Vector3(0, 0, 0),10, m_TreeEntity);
		}
		else if(m_TreeLoader2d)
		{
			m_TreeLoader2d->addTree(m_TreeEntity,  Ogre::Vector3(0, 0, 0));
			m_TreeLoader2d->deleteTrees(Ogre::Vector3(0, 0, 0),10, m_TreeEntity);
		}

		//m_RandomTable->resetRandomIndex();
		UpdateArea(m_MapBounds.left, m_MapBounds.top,m_MapBounds.right, m_MapBounds.bottom);
		m_PagedGeometry->update();
	
	}

	void TreeGeometryComponent::OnDelete()
	{
		if(m_PagedGeometry)
		{
			if(m_PagedGeometry->getPageLoader())
				delete m_PagedGeometry->getPageLoader();
			delete m_PagedGeometry;
		}

		Ogre::SceneManager* sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<IOgreSceneManagerProxy>()->GetOgreSceneManager();
		sm->destroyEntity(m_TreeEntity);
		
		Ogre::RenderSystem::RenderTargetIterator iter = Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator();
		while (iter.hasMoreElements())
		{
			//only render windows for now
			Ogre::RenderWindow* target = dynamic_cast<Ogre::RenderWindow*>(iter.getNext());
			if(target)
				target->removeListener(this);
		}
	}

	

	void TreeGeometryComponent::OnPaint(GrassPaintMessagePtr message)
	{
		Paint(message->GetPosition(), message->GetBrushSize(), message->GetBrushInnerSize(), message->GetIntensity());
	}

	void TreeGeometryComponent::Paint(const Vec3 &world_pos, float brush_size, float brush_inner_size , float intensity)
	{
		float radius = brush_size;
		int minPageX = Ogre::Math::Floor(((world_pos.x-radius) - m_MapBounds.left) / m_PageSize);
		int minPageZ = Ogre::Math::Floor(((world_pos.z-radius) - m_MapBounds.top) / m_PageSize);
		int maxPageX = Ogre::Math::Ceil(((world_pos.x+radius) - m_MapBounds.left) / m_PageSize);
		int maxPageZ = Ogre::Math::Ceil(((world_pos.z+radius) - m_MapBounds.top) / m_PageSize);
	
		Forests::TBounds bounds(m_MapBounds.left + minPageX*m_PageSize, 
									m_MapBounds.top + minPageZ*m_PageSize, 
									m_MapBounds.left + maxPageX*m_PageSize,
									m_MapBounds.top + maxPageZ*m_PageSize);

		//update page by page
		for(int i = minPageX ; i < maxPageX; i++)
		{
			for(int j = minPageZ ; j < maxPageZ ; j++)
			{
				Forests::TBounds bounds(m_MapBounds.left + i*m_PageSize, 
									m_MapBounds.top + j*m_PageSize, 
									m_MapBounds.left + (i+1)*m_PageSize,
									m_MapBounds.top + (j+1)*m_PageSize);
				UpdateArea(bounds.left, bounds.top,bounds.right, bounds.bottom);
			}
		}
	}

	void TreeGeometryComponent::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
	{
		Ogre::Viewport *vp = evt.source;
		m_PagedGeometry->update();
		if(vp)
			m_PagedGeometry->setCamera(vp->getCamera());
	}

	float TreeGeometryComponent::GetTerrainHeight(float x, float z, void* user_data)
	{
		if(m_Terrain)
			return m_Terrain->GetHeightAtWorldLocation(x,z);
		else
			return 0;
	}

	void TreeGeometryComponent::SetRegenerateAllImpostors(bool value)
	{
		if(value)
		{
			ImpostorPage::setImpostorBackgroundColor(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.0f));
			ImpostorPage::setImpostorResolution(m_ImposterResolution);
			ImpostorPage::regenerateAll();
		}
	}

	bool TreeGeometryComponent::GetRegenerateAllImpostors() const
	{
		return false;
	}


	void TreeGeometryComponent::SetDynamicImpostorLighting(bool value)
	{
		m_DynamicImpostorLighting  = value;
		if(value)
		{
			StaticBillboardSet::setDynamicLighting(value);
		}
	}

	bool TreeGeometryComponent::GetDynamicImpostorLighting() const
	{
		return m_DynamicImpostorLighting;
	}

	void TreeGeometryComponent::UpdateArea(Float start_x,Float start_z,Float end_x,Float end_z)
	{
		m_RandomTable->resetRandomIndex();
		Float width = end_x - start_x;
		Float height = end_z - start_z;

		Float volume = width * height;
		unsigned int treeCount = m_DensityFactor * volume;

		//delete all trees in area!
		if(m_TreeLoader3d)
			m_TreeLoader3d->deleteTrees(TBounds(start_x, start_z,end_x,end_z),m_TreeEntity);
		else	
			m_TreeLoader2d->deleteTrees(TBounds(start_x, start_z,end_x,end_z),m_TreeEntity);

		if (m_DensityMap)
		{
			for (int i = 0; i < treeCount; i++)
			{
				//Determine whether this grass will be added based on the local density.
				//For example, if localDensity is .32, grasses will be added 32% of the time.
				Float x, y, z, yaw, scale;

				x = Ogre::Math::RangeRandom(start_x, end_x);
				z = Ogre::Math::RangeRandom(start_z, end_z);
				
				//x = m_RandomTable->getRangeRandom(start_x, end_x);
				//z = m_RandomTable->getRangeRandom(start_z, end_z);
				float density = m_DensityMap->GetDensityAt(x, z);
				if (density  > 0 && Ogre::Math::UnitRandom() <= density)
				{
					yaw = m_RandomTable->getRangeRandom(0, 360);
					scale = m_RandomTable->getRangeRandom(m_MaxMinScale.x, m_MaxMinScale.y);
					y = 0;
					if(m_PrecalcHeight)
					{
						if(m_Terrain)
							y = m_Terrain->GetHeightAtWorldLocation(x,z);
						m_TreeLoader3d->addTree(m_TreeEntity,  Ogre::Vector3(x, y,z) ,Ogre::Degree(yaw), scale);
					}
					else
						m_TreeLoader2d->addTree(m_TreeEntity,  Ogre::Vector3(x, y,z) ,Ogre::Degree(yaw), scale);
				}
			}
		}
	}
}


