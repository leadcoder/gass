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
#include <OgreParticleSystem.h>
#include <OgreParticleEmitter.h>
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/SimEngine.h"
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/Components/OgreParticleSystemComponent.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include "Plugins/Ogre/OgreConvert.h"

using namespace Ogre;

namespace GASS
{

	OgreParticleSystemComponent::OgreParticleSystemComponent() :
		m_CastShadow(true),
		m_ParticleSystem(NULL),
		m_TimeToLive(-1)
		
		
	{

	}

	OgreParticleSystemComponent::~OgreParticleSystemComponent()
	{

	}

	void OgreParticleSystemComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("ParticleSystemComponent",new GASS::Creator<OgreParticleSystemComponent, IComponent>);
		RegisterProperty<std::string>("ParticleTemplate", &GASS::OgreParticleSystemComponent::GetParticleTemplate, &GASS::OgreParticleSystemComponent::SetParticleTemplate);
		RegisterProperty<bool>("CastShadow", &GASS::OgreParticleSystemComponent::GetCastShadow, &GASS::OgreParticleSystemComponent::SetCastShadow);
		RegisterProperty<float>("TimeToLive", &GASS::OgreParticleSystemComponent::GetTimeToLive, &GASS::OgreParticleSystemComponent::SetTimeToLive);
	}

	void OgreParticleSystemComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreParticleSystemComponent::OnLoad,LoadGFXComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreParticleSystemComponent::OnParameterMessage,ParticleSystemParameterMessage,1));
	}

	void OgreParticleSystemComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		
		OgreGraphicsSceneManagerPtr ogsm = boost::shared_static_cast<OgreGraphicsSceneManager>(message->GetGFXSceneManager());
		assert(ogsm);
		OgreLocationComponent * lc = GetSceneObject()->GetFirstComponentByClass<OgreLocationComponent>().get();

		static unsigned int obj_id = 0;
		obj_id++;
		std::stringstream ss;
		std::string name;
		ss << GetName() << obj_id;
		ss >> name;

		m_ParticleSystem  = ogsm->GetSceneManger()->createParticleSystem(name, m_ParticleTemplate);
		lc->GetOgreNode()->attachObject((Ogre::MovableObject*) m_ParticleSystem);

		if(m_TimeToLive > -1)
		{
			//Send remove message with delay
			MessagePtr remove_msg(new RemoveSceneObjectMessage(GetSceneObject()));
			remove_msg->SetDeliverDelay(m_TimeToLive);
			GetSceneObject()->GetSceneObjectManager()->GetScenario()->PostMessage(remove_msg);
		}
		//m_ParticleSystem->getEmitter(0)->setEmissionRate();
	}

	void OgreParticleSystemComponent::OnParameterMessage(ParticleSystemParameterMessagePtr message)
	{
		ParticleSystemParameterMessage::ParticleSystemParameterType type = message->GetParameter();
		switch(type)
		{
		case ParticleSystemParameterMessage::EMISSION_RATE:
			{
			int emitter = message->GetEmitter();
			float rate = message->GetValue();
			m_ParticleSystem->getEmitter(emitter)->setEmissionRate(rate);
			}
			break;
		case ParticleSystemParameterMessage::PARTICLE_LIFE_TIME:
			{
			int emitter = message->GetEmitter();
			float duration = message->GetValue();
			m_ParticleSystem->getEmitter(emitter)->setTimeToLive(duration);
			}
			break;
		}
	}

	AABox OgreParticleSystemComponent::GetBoundingBox() const
	{
		assert(m_ParticleSystem);
		return Convert::ToGASS(m_ParticleSystem->getBoundingBox());
	}

	Sphere OgreParticleSystemComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		assert(m_ParticleSystem);
		sphere.m_Pos = Vec3(0,0,0);
		sphere.m_Radius = m_ParticleSystem->getBoundingRadius();
		return sphere;
	}

	void OgreParticleSystemComponent::GetMeshData(MeshDataPtr mesh_data)
	{

	}

	GeometryCategory OgreParticleSystemComponent::GetGeometryCategory() const
	{
		return GeometryCategory(GT_REGULAR);
	}


}
