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

#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreSceneNode.h>
#include <OgreConfigFile.h>
#include <OgreAVTerrainSceneManager.h>
#include <OgreAVTerrain.h>


#include "Plugins/AdvantageTerrain/AdvantageTerrainComponent.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"

//#include "AdvantageTerrainManager.H"

namespace GASS
{
	AdvantageTerrainComponent::AdvantageTerrainComponent(void) : 
		mAVTerrainSceneMgr(NULL),
		m_Offset(0.4)
	{

	}

	AdvantageTerrainComponent::~AdvantageTerrainComponent(void)
	{
		//if(mAVTerrainSceneMgr)
			//Root::Get().GetMainWindow()->removeListener(this);
	}

	
	void AdvantageTerrainComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AdvantageTerrainComponent",new Creator<AdvantageTerrainComponent, IComponent>);
		//RegisterProperty<std::string>("RenderQueue", &GetRenderQueue, &SetRenderQueue);
		RegisterProperty<std::string>("TerrainDataFile", &GetFilename, &SetFilename);
		//RegisterProperty<bool>("CastShadow", &GetCastShadow, &SetCastShadow);
	}

	void AdvantageTerrainComponent::OnCreate()
	{
		int obj_id = (int) this;
		
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_LOAD_GFX_COMPONENTS, MESSAGE_FUNC(AdvantageTerrainComponent::OnLoad),1);
		GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->RegisterForMessage(ScenarioScene::SCENARIO_NM_CAMERA_CHANGED,  MESSAGE_FUNC(AdvantageTerrainComponent::OnChangeCamera));
		
	}

	void AdvantageTerrainComponent::Shutdown()
	{

	}

	void AdvantageTerrainComponent::GetMeshData(MeshDataPtr mesh_data)
	{

		//impossible to return all meshdata. 
		//Should we return a low res version of the terrain 
		//or highres of terrain in memory?

	}

	void AdvantageTerrainComponent::OnChangeCamera(MessagePtr message)
	{
		if(mAVTerrainSceneMgr)
		{
			Ogre::Camera* ocam = boost::any_cast<Ogre::Camera*>(message->GetData("OgreCamera"));
			mAVTerrainSceneMgr->setPrimaryCamera(ocam);
			float viewRange = ocam->getFarClipDistance();//Root::Get().GetLevel()->GetViewDist();
			mAVTerrainSceneMgr->setViewRange(viewRange);
		}
	}

	void AdvantageTerrainComponent::OnLoad(MessagePtr message)
	{
		ResourceSystemPtr rs = SimEngine::GetPtr()->GetSystemManager()->GetFirstSystem<IResourceSystem>();
		assert(rs);

		std::string full_path;
		if(!rs->GetFullPath(GetFilename(),full_path))
			Log::Error("Faild to load terrain %s",GetFilename().c_str());

		Ogre::SceneManager* sm = boost::any_cast<Ogre::SceneManager*>(message->GetData("OgreSceneManager"));
		mAVTerrainSceneMgr = static_cast<Ogre::AVTerrainSceneManager*>(sm);
		std::string base_path = Misc::RemoveFilename(full_path);

		//What camera should be used?
	/*	Ogre::Camera* ocam = NULL;
		if(Root::Get().GetDefaultLocalPlayer()) //check if we have a player, use his cam in that case
		{
			ICameraNode* cam = Root::Get().GetDefaultLocalPlayer()->GetCamera();
			assert(cam);
			ocam = cam->GetOgreCamera();
		}
		else //get first camera object
		{
			Ogre::Viewport* vp = Root::Get().GetMainWindow()->getViewport(0);
			ocam = vp->getCamera();
		}

		
		mAVTerrainSceneMgr->setPrimaryCamera(ocam);*/


		//printf("Camera added");


		//delete dataStreamPtr; 

		/*	if(m_CreateCollisionMesh)
		{
		m_PageListener = new CustomTerrainPageListener(this); // your custom page source listener class
		Ogre::TerrainPageSourceListenerManager::getSingleton().addListener(m_PageListener);
		}*/

		Ogre::Camera* ocam = sm->getCameraIterator().getNext();

		//ocam = sm->getCamera("TheCamera");
		mAVTerrainSceneMgr->setPrimaryCamera(ocam);
	
		
		sm->setWorldGeometry(full_path);

		float viewRange = ocam->getFarClipDistance();//Root::Get().GetLevel()->GetViewDist();
		mAVTerrainSceneMgr->setViewRange(viewRange);

		//printf("world geom set");


		

		//Root::Get().GetMainWindow()->addListener(this);

		AdVantage::MapInfo info = mAVTerrainSceneMgr->getAVTerrain()->getMapInfo();
		
		m_WorldMin.x = info.MapDims.MinX;
		m_WorldMin.y = info.MapDims.MinZ;
		m_WorldMin.z = info.MapDims.MinY;

		m_WorldMax.x = info.MapDims.SizeX + m_WorldMin.x;
		m_WorldMax.y = info.MapDims.SizeZ + m_WorldMin.y;
		m_WorldMax.z = info.MapDims.SizeY + m_WorldMin.z;

		m_SamplesX = info.RasterX;
		m_SamplesZ = info.RasterY;

		//printf("Done");

	}

	unsigned int AdvantageTerrainComponent::GetSamplesX()
	{
		return m_SamplesX;

	}

	unsigned int AdvantageTerrainComponent::GetSamplesZ()
	{
		return m_SamplesZ;
	}

	void AdvantageTerrainComponent::GetBounds(Vec3 &min,Vec3 &max)
	{
		min = m_WorldMin;
		max = m_WorldMax;
	}

	float AdvantageTerrainComponent::GetHeight(float x, float z)
	{
		//mAVTerrainSceneMgr->bidForCollisionData(Ogre::Vector3(x,0,z), 500);
		Ogre::Sphere sphere(Ogre::Vector3(x,0,z), 500);

		mAVTerrainSceneMgr->bidForCollisionData(sphere);
		Ogre::AVRayHitInfo rayHitInfo;
		if(mAVTerrainSceneMgr->intersectRayVertical(x, z, 0,rayHitInfo))
		{
			return rayHitInfo.hitPos.y + m_Offset;
		}
		return 0;
	}

	bool AdvantageTerrainComponent::CheckOnTerrain(float x, float z)
	{
		return true;
	}



	float AdvantageTerrainComponent::GetSizeZ()
	{
		return 0;
	}
	float AdvantageTerrainComponent::GetSizeX()
	{
		return 0;
	}
	void AdvantageTerrainComponent::GetHeightAndNormal(float x, float z, float &height,Vec3 &normal)
	{
		height = GetHeight(x, z);
		normal = Vec3(0,1,0);
	}

	void AdvantageTerrainComponent::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
	{
		Ogre::Viewport *vp = evt.source;
		if(vp) 
			mAVTerrainSceneMgr->setPrimaryCamera(vp->getCamera());
	}

	AABox AdvantageTerrainComponent::GetBoundingBox() const
	{
		AABox bbox;
		bbox.m_Min = m_WorldMin;
		bbox.m_Max = m_WorldMax;
		return bbox;

	}
	Sphere AdvantageTerrainComponent::GetBoundingSphere() const
	{
		//Sphere sphere;
		AABox bbox;
		bbox.m_Min = m_WorldMin;
		bbox.m_Max = m_WorldMax;
		return bbox.GetBoundingSphere();


	}
}


