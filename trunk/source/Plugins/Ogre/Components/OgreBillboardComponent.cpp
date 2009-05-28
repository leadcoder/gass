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

#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreTextureUnitState.h>
#include <OgreSkeletonInstance.h>
#include <OgreBillboardSet.h>
#include <OgreBillboard.h>
#include <OgreMaterialManager.h>

#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Sim/SimEngine.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/Components/OgreBillboardComponent.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include "Plugins/Ogre/OgreConvert.h"


using namespace Ogre;

namespace GASS
{

	OgreBillboardComponent::OgreBillboardComponent() :
		m_OgreEntity(NULL),
		m_CastShadow(true),
		m_BillboardSet (NULL),
		m_Width(1.0f),
		m_Height(1.0f)
	{

	}	

	OgreBillboardComponent::~OgreBillboardComponent()
	{

	}

	void OgreBillboardComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("BillboardComponent",new GASS::Creator<OgreBillboardComponent, IComponent>);
		RegisterProperty<std::string>("RenderQueue", &GetRenderQueue, &SetRenderQueue);
		RegisterProperty<std::string>("Material", &GetMaterial, &SetMaterial);
		RegisterProperty<bool>("CastShadow", &GetCastShadow, &SetCastShadow);
	}

	void OgreBillboardComponent::OnCreate()
	{
		int obj_id = (int) this;
		MessageManager * mm = GetMessageManager();
		mm->RegisterForMessage(ScenarioScene::SM_MESSAGE_LOAD_GFX_COMPONENTS, obj_id,  boost::bind( &OgreBillboardComponent::OnLoad, this, _1 ),1);
		//mm.RegisterForMessage(MESSAGE_UPDATE, address,  boost::bind( &LocationComponent::OnUpdate, this, _1 ),m_InitPriority);
	}

	void OgreBillboardComponent::OnLoad(MessagePtr message)
	{
		OgreGraphicsSceneManager* ogsm = boost::any_cast<OgreGraphicsSceneManager*>(message->GetData("GraphicsSceneManager"));
		assert(ogsm);
	
		OgreLocationComponent * lc = GetSceneObject()->GetFirstComponent<OgreLocationComponent>().get();

		static unsigned int obj_id = 0;
		obj_id++;
		std::stringstream ss;
		std::string name;
		ss << GetName() << obj_id;
		ss >> name;

		if(m_Material != "")
		{
			std::string material_name = m_Material;
			Ogre::MaterialPtr material;
			if(Ogre::MaterialManager::getSingleton().resourceExists(material_name)) material = Ogre::MaterialManager::getSingleton().getByName(material_name);
			else 
			{
				material = Ogre::MaterialManager::getSingleton().create(material_name, "GASS",false, 0); // Manual, loader
				// Remove pre-created technique from defaults
				material->removeAllTechniques();
				// Create a techinique and a pass and a texture unit
				Ogre::Technique * technique = material->createTechnique();
				Ogre::Pass* pass = technique->createPass();
				material->setLightingEnabled(false);
				material->setDepthWriteEnabled(true);
				material->setCullingMode(Ogre::CULL_NONE);
				std::string fullpath;
				IResourceSystem* rs = SimEngine::GetPtr()->GetSystemManager()->GetFirstSystem<IResourceSystem>().get();
		
				if(rs->GetFullPath(m_Material,fullpath))
				{
					Ogre::TextureUnitState * textureUnit = pass->createTextureUnitState(m_Material,0);
					//pass->setSceneBlending(Ogre::SBF_SOURCE_ALPHA,Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
					//pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
					pass->setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL, 128);
				}
			}
			/*int elem = 0;
			//char billboard_set_name[255];
			sprintf(billboard_set_name,"%s_%s",m_Material.c_str(),GetName().c_str());
			
			if(ogsm->GetSceneManger()->hasBillboardSet(billboard_set_name))
			{
				m_BillboardSet = ogsm->GetSceneManger()->getBillboardSet(billboard_set_name);
			}
			else
			{
				m_BillboardSet = ogsm->GetSceneManger()->createBillboardSet(billboard_set_name);
				m_BillboardSet->setMaterialName(material_name);
			}*/

			m_BillboardSet = ogsm->GetSceneManger()->createBillboardSet(name);
			m_BillboardSet->setMaterialName(material_name);

			Vec3 pos = Vec3(0,m_Height/2.0,0);
			Ogre::ColourValue color = Ogre::ColourValue::White;
			Ogre::Billboard* billboard = m_BillboardSet->createBillboard(Convert::ToOgre(pos),color);
			billboard->mPosition = Convert::ToOgre(pos);
			billboard->setColour(color);
			billboard->setDimensions(m_Width,m_Height);
			billboard->setTexcoordRect(0, 0,1, 1);
			float bbsize = m_Height;
			if(m_Width > m_Height) bbsize = m_Width;
			bbsize *=  0.5f;
			m_BillboardSet->setBounds(Ogre::AxisAlignedBox(Ogre::Vector3(-bbsize,-bbsize + pos.y,-bbsize),Ogre::Vector3(bbsize,bbsize+ pos.y,bbsize)),bbsize*2);
			lc->GetOgreNode()->attachObject((Ogre::MovableObject*) m_BillboardSet);
		}
	}

	AABox OgreBillboardComponent::GetBoundingBox() const
	{
		float max = Math::Max(m_Width,m_Height);
		AABox box(Vec3(-max/2.0,-max/2.0,-max/2.0),Vec3(max/2.0,max/2.0,max/2.0));
		return box;

	}
	Sphere OgreBillboardComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		sphere.m_Pos = Vec3(0,0,0);
		sphere.m_Radius = Math::Max(m_Width,m_Height)/2.0;
		return sphere;
	}

	void OgreBillboardComponent::GetMeshData(MeshDataPtr mesh_data)
	{
	
	}

	
}
