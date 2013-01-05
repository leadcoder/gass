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
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"

#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSIResourceSystem.h"
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
		m_TimeToLive(-1),
		m_GeomFlags(GEOMETRY_FLAG_UNKOWN)
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

	void OgreParticleSystemComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreParticleSystemComponent::OnLocationLoaded,LocationLoadedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreParticleSystemComponent::OnParameterMessage,ParticleSystemParameterMessage,0));
	}

	void OgreParticleSystemComponent::OnLocationLoaded(LocationLoadedMessagePtr message)
	{
		
		OgreGraphicsSceneManagerPtr ogsm =  GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OgreGraphicsSceneManager>();
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
			GetSceneObject()->GetScene()->PostMessage(remove_msg);
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


	GeometryFlags OgreParticleSystemComponent::GetGeometryFlags() const
	{
		return m_GeomFlags;
	}

	void OgreParticleSystemComponent::SetGeometryFlags(GeometryFlags flags)
	{
		m_GeomFlags = flags;
	}
}
