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

#include "TreeGeometryComponent.h"
#include "DensityMapComponent.h"
#include "Plugins/Ogre/GASSIOgreSceneManagerProxy.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSScene.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	IHeightmapTerrainComponent* TreeGeometryComponent::m_Terrain = NULL;

	

	TreeGeometryComponent::TreeGeometryComponent(void) : m_CustomBounds(0,0,0,0), 
		m_TreeLoader2d(NULL),
		m_TreeLoader3d(NULL),
		m_DensityFactor(0.001f),
		m_MaxMinScale(1.1f, 0.9f),
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
		m_DynamicImpostorLighting(false),
		m_SceneMan(NULL)
	{
		m_RandomTable = new RandomTable();

	}

	TreeGeometryComponent::~TreeGeometryComponent(void)
	{
		delete m_RandomTable;
	}

	void TreeGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<TreeGeometryComponent>();
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("TreeGeometryComponent", OF_VISIBLE)));

		RegisterGetSet("Mesh", &TreeGeometryComponent::GetMesh, &TreeGeometryComponent::SetMesh,PF_VISIBLE,"");
		RegisterGetSet("ColorMap", &TreeGeometryComponent::GetColorMap, &TreeGeometryComponent::SetColorMap,PF_VISIBLE,"");
		RegisterGetSet("DensityFactor", &TreeGeometryComponent::GetDensityFactor, &TreeGeometryComponent::SetDensityFactor,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("MeshDistance", &TreeGeometryComponent::GetMeshDistance, &TreeGeometryComponent::SetMeshDistance,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("MeshFadeDistance", &TreeGeometryComponent::GetMeshFadeDistance, &TreeGeometryComponent::SetMeshFadeDistance,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("ImposterDistance", &TreeGeometryComponent::GetImposterDistance, &TreeGeometryComponent::SetImposterDistance,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("ImposterFadeDistance", &TreeGeometryComponent::GetImposterFadeDistance, &TreeGeometryComponent::SetImposterFadeDistance,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("CustomBounds", &TreeGeometryComponent::GetCustomBounds, &TreeGeometryComponent::SetCustomBounds,PF_VISIBLE,"");
		RegisterGetSet("MaxMinScale", &TreeGeometryComponent::GetMaxMinScale, &TreeGeometryComponent::SetMaxMinScale,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("CastShadows", &TreeGeometryComponent::GetCastShadows, &TreeGeometryComponent::SetCastShadows,PF_VISIBLE | PF_EDITABLE,"");
		//RegisterGetSet("CreateShadowMap", &TreeGeometryComponent::GetCreateShadowMap, &TreeGeometryComponent::SetCreateShadowMap);
		RegisterGetSet("SetHeightAtStartup", &TreeGeometryComponent::GetPrecalcHeight, &TreeGeometryComponent::SetPrecalcHeight,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("PageSize", &TreeGeometryComponent::GetPageSize, &TreeGeometryComponent::SetPageSize,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("ImposterAlphaRejectionValue", &TreeGeometryComponent::GetImposterAlphaRejectionValue, &TreeGeometryComponent::SetImposterAlphaRejectionValue,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("ImposterResolution", &TreeGeometryComponent::GetImposterResolution, &TreeGeometryComponent::SetImposterResolution,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("RegenerateAllImpostors", &TreeGeometryComponent::GetRegenerateAllImpostors, &TreeGeometryComponent::SetRegenerateAllImpostors,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("DynamicImpostorLighting", &TreeGeometryComponent::GetDynamicImpostorLighting, &TreeGeometryComponent::SetDynamicImpostorLighting,PF_VISIBLE | PF_EDITABLE,"Enable experimental dynamic impostor lighting. Note that this will effect all TreeGeometryComponent");
		
	}

	void TreeGeometryComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(TreeGeometryComponent::OnPaint,GrassPaintMessage,0));
		
		SetDynamicImpostorLighting(m_DynamicImpostorLighting);
		ImpostorPage::setImpostorBackgroundColor(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.0f));
		ImpostorPage::setImpostorResolution(m_ImposterResolution);
		
		m_SceneMan = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<IOgreSceneManagerProxy>()->GetOgreSceneManager();
		Ogre::Camera* ocam = NULL;
		if(m_SceneMan->hasCamera("DummyCamera"))
			ocam = m_SceneMan->getCamera("DummyCamera");
		else
			ocam = m_SceneMan->createCamera("DummyCamera");

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
				GeometryComponentPtr geom = GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(terrain);
				AABox aabox = geom->GetBoundingBox();

				//for speed we save the raw pointer , we will access this for each height callback
				m_Terrain = terrain.get();
				m_MapBounds = TBounds(static_cast<float>(aabox.Min.x), 
									  static_cast<float>(aabox.Min.z), 
									  static_cast<float>(aabox.Max.x), 
									  static_cast<float>(aabox.Max.z));
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
			m_PagedGeometry->addDetailLevel<BatchPage>(m_MeshDist*0.5f,m_MeshFadeDist,Ogre::Any(0));
			m_PagedGeometry->addDetailLevel<BatchPage>(m_MeshDist,m_MeshFadeDist,Ogre::Any(1));
			//m_PagedGeometry->addDetailLevel<BatchPage>(m_MeshDist,0,Ogre::Any(3));
		}
		if(m_ImposterDist >  0)
		{
			/*GeometryPageManager* geom_man = */m_PagedGeometry->addDetailLevel<ImpostorPage>(m_ImposterDist,m_ImposterFadeDist);
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
		//float volume = m_MapBounds.width() * m_MapBounds.height();
		//unsigned int treeCount = m_DensityFactor * volume;

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
		
		m_TreeEntity = m_SceneMan->createEntity(name, m_MeshFileName);
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
		UpdateArea(message->GetPosition(), message->GetBrushSize());
	}

	void TreeGeometryComponent::UpdateArea(const Vec3 &world_pos, float radius)
	{
		int minPageX = static_cast<int>(Ogre::Math::Floor(((static_cast<float>(world_pos.x) - radius) - m_MapBounds.left) / m_PageSize));
		int minPageZ = static_cast<int>(Ogre::Math::Floor(((static_cast<float>(world_pos.z) - radius) - m_MapBounds.top) / m_PageSize));
		int maxPageX = static_cast<int>(Ogre::Math::Ceil(((static_cast<float>(world_pos.x) + radius) - m_MapBounds.left) / m_PageSize));
		int maxPageZ = static_cast<int>(Ogre::Math::Ceil(((static_cast<float>(world_pos.z) + radius) - m_MapBounds.top) / m_PageSize));
	
		/*Forests::TBounds bounds(m_MapBounds.left + minPageX*m_PageSize, 
									m_MapBounds.top + minPageZ*m_PageSize, 
									m_MapBounds.left + maxPageX*m_PageSize,
									m_MapBounds.top + maxPageZ*m_PageSize);
									*/
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
		if(vp && vp->getCamera() && m_SceneMan == vp->getCamera()->getSceneManager())
			m_PagedGeometry->setCamera(vp->getCamera());
	}

	float TreeGeometryComponent::GetTerrainHeight(float x, float z, void* /*user_data*/)
	{
		if(m_Terrain)
			return static_cast<float>(m_Terrain->GetHeightAtWorldLocation(x,z));
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
	//		StaticBillboardSet::setDynamicLighting(value);
		}
	}

	bool TreeGeometryComponent::GetDynamicImpostorLighting() const
	{
		return m_DynamicImpostorLighting;
	}

	void TreeGeometryComponent::UpdateArea(float start_x, float start_z, float end_x, float end_z)
	{
		m_RandomTable->resetRandomIndex();
		float width = end_x - start_x;
		float height = end_z - start_z;

		float volume = width * height;
		unsigned int treeCount = static_cast<unsigned int>(m_DensityFactor * volume);

		//delete all trees in area!
		if(m_TreeLoader3d)
			m_TreeLoader3d->deleteTrees(TBounds(start_x, start_z,end_x,end_z),m_TreeEntity);
		else	
			m_TreeLoader2d->deleteTrees(TBounds(start_x, start_z,end_x,end_z),m_TreeEntity);

		if (m_DensityMap)
		{
			for (unsigned int i = 0; i < treeCount; i++)
			{
				//Determine whether this grass will be added based on the local density.
				//For example, if localDensity is .32, grasses will be added 32% of the time.
				float x, y, z, yaw, scale;

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
							y = static_cast<float>(m_Terrain->GetHeightAtWorldLocation(x,z));
						m_TreeLoader3d->addTree(m_TreeEntity,  Ogre::Vector3(x, y,z) ,Ogre::Degree(yaw), scale);
					}
					else
						m_TreeLoader2d->addTree(m_TreeEntity,  Ogre::Vector3(x, y,z) ,Ogre::Degree(yaw), scale);
				}
			}
		}
	}
}


