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

#include "EnvironmentManagerComponent.h"
#include "SkyXComponent.h"
#include "HydraxWaterComponent.h"
#include "HydraxRttListener.h"
#include <Hydrax/Hydrax.h>
#include <SkyX/SkyX.h>

#include "Plugins/Ogre/OgreConvert.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/SimEngine.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"

#include <Ogre.h>


namespace GASS
{
	EnvironmentManagerComponent::EnvironmentManagerComponent(void) :m_SkyX(NULL),m_Hydrax(NULL), m_Target(NULL),m_CurrentCamera(NULL)
	{
		
		m_WaterGradientValues.push_back(Vec3(0.058209*0.4,0.535822*0.4,0.779105*0.4));
		m_WaterGradientValues.push_back(Vec3(0.058209*0.3,0.535822*0.3,0.779105*0.3));
		m_WaterGradientValues.push_back(Vec3(0.058209*0.25,0.535822*0.25,0.679105*0.25));
		m_WaterGradientValues.push_back(Vec3(0.058209*0.2,0.535822*0.2,0.679105*0.2));
		m_WaterGradientValues.push_back(Vec3(0.058209*0.1,0.535822*0.1,0.679105*0.1));
		m_WaterGradientValues.push_back(Vec3(0.058209*0.1,0.535822*0.1,0.679105*0.025));

		m_WaterGradientWeights.push_back(1);
		m_WaterGradientWeights.push_back(0.8);
		m_WaterGradientWeights.push_back(0.6);
		m_WaterGradientWeights.push_back(0.5);
		m_WaterGradientWeights.push_back(0.45);
		m_WaterGradientWeights.push_back(0);
	}

	EnvironmentManagerComponent::~EnvironmentManagerComponent(void)
	{

	}

	void EnvironmentManagerComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("EnvironmentManagerComponent",new Creator<EnvironmentManagerComponent, IComponent>);
		RegisterVectorProperty<Vec3>("WaterGradient", &EnvironmentManagerComponent::GetWaterGradient, &EnvironmentManagerComponent::SetWaterGradient);
		RegisterVectorProperty<float>("WaterGradientWeights", &EnvironmentManagerComponent::GetWaterGradientWeights, &EnvironmentManagerComponent::SetWaterGradientWeights);
	}

	void EnvironmentManagerComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(EnvironmentManagerComponent::OnLoad,LoadGFXComponentsMessage,4));
		GetSceneObject()->RegisterForMessage(REG_TMESS(EnvironmentManagerComponent::OnUnload,UnloadComponentsMessage,0));
	}

	void EnvironmentManagerComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		Ogre::Root::getSingleton().removeFrameListener(this);
		GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->UnregisterForMessage(UNREG_TMESS( EnvironmentManagerComponent::OnChangeCamera,CameraChangedNotifyMessage));
	}

	void EnvironmentManagerComponent::SetWaterGradient(const std::vector<Vec3> &value)
	{
		m_WaterGradientValues = value;
		m_WaterGradient.clear();
		m_WaterGradient = SkyX::ColorGradient();

		if(m_WaterGradientWeights.size() != m_WaterGradientValues.size())
		{
			for(int i = 0; i < value.size(); i++)
			{
				m_WaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(value[i].x,value[i].y,value[i].z), float(i) / float(m_WaterGradientValues.size()-1.0)));
			}
		}
		else
		{
			for(int i = 0; i < value.size(); i++)
			{
				m_WaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(value[i].x,value[i].y,value[i].z), m_WaterGradientWeights[i]));
			}
		}
	}


	std::vector<Vec3>  EnvironmentManagerComponent::GetWaterGradient() const 
	{
		return m_WaterGradientValues;
	}


	void EnvironmentManagerComponent::SetWaterGradientWeights(const std::vector<float> &value)
	{
		m_WaterGradientWeights = value;
		m_WaterGradient.clear();
		m_WaterGradient = SkyX::ColorGradient();
		if(m_WaterGradientWeights.size() == m_WaterGradientValues.size())
		{
			SetWaterGradient(m_WaterGradientValues);
		}
	}


	std::vector<float>  EnvironmentManagerComponent::GetWaterGradientWeights() const 
	{
		return m_WaterGradientWeights;
	}


	void EnvironmentManagerComponent::OnChangeCamera(CameraChangedNotifyMessagePtr message)
	{
		
		m_CurrentCamera = static_cast<Ogre::Camera*> (message->GetUserData());

		/*if(m_SkyX)
		{
			Ogre::Camera * cam = static_cast<Ogre::Camera*> (message->GetUserData());
			Init(cam);
		}*/
	}

	void EnvironmentManagerComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		//Ogre::Camera* ocam = sm->getCameraIterator().getNext();
		
		
		Ogre::Root::getSingleton().addFrameListener(this);

		
		SkyXComponentPtr skyx = GetSceneObject()->GetFirstComponentByClass<SkyXComponent>();
		HydraxWaterComponentPtr hydrax = GetSceneObject()->GetFirstComponentByClass<HydraxWaterComponent>();

		if(hydrax)
			m_Hydrax = hydrax->GetHydrax();
		if(skyx)
			m_SkyX = skyx->GetSkyX();
		if(hydrax && skyx)
		{
			
			hydrax->GetHydrax()->getRttManager()->addRttListener(new HydraxRttListener(skyx->GetSkyX(), hydrax->GetHydrax()));
		}

		 // Light
		m_SunLight= sm->createLight("SunLight");
		m_SunLight->setType(Ogre::Light::LT_DIRECTIONAL);

		// Color gradients
		// Water
		m_WaterGradient = SkyX::ColorGradient();

		SetWaterGradient(m_WaterGradientValues);

		// Sun
		m_SunGradient = SkyX::ColorGradient();
		m_SunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8,0.75,0.55)*1.5, 1.0f));
		m_SunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8,0.75,0.55)*1.4, 0.75f));
		m_SunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8,0.75,0.55)*1.3, 0.5625f));
		m_SunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.6,0.5,0.2)*1.5, 0.5f));
		m_SunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.5,0.5,0.5)*0.25, 0.45f));
		m_SunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.5,0.5,0.5)*0.01, 0.0f));
		// Ambient
		m_AmbientGradient = SkyX::ColorGradient();
		m_AmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*1, 1.0f));
		m_AmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*1, 0.6f));
		m_AmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.6, 0.5f));
		m_AmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.3, 0.45f));
		m_AmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.1, 0.35f));
		m_AmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.05, 0.0f));

		GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->RegisterForMessage(REG_TMESS( EnvironmentManagerComponent::OnChangeCamera,CameraChangedNotifyMessage,0));
	}


	bool  EnvironmentManagerComponent::frameStarted(const Ogre::FrameEvent& evt)
	{
		UpdateEnvironmentLighting();
		return true;
	}

	void EnvironmentManagerComponent::UpdateEnvironmentLighting()
	{
		Ogre::Vector3 lightDir = m_SkyX->getAtmosphereManager()->getSunDirection();
		float point = (-lightDir.y + 1.0f) / 2.0f;

		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		//Ogre::Camera* ocam = sm->getCameraIterator().getNext();
		/*bool preForceDisableShadows = m_ForceDisableShadows;
		m_ForceDisableShadows = (lightDir.y > 0.15f) ? true : false;

		if (preForceDisableShadows != m_ForceDisableShadows)
		{
			setShadowMode(mSceneMgr, static_cast<ShadowMode>(mShadowMode));
		}*/
		// Calculate current color gradients point

		if(m_Hydrax)
		{
			
			m_Hydrax->setWaterColor(m_WaterGradient.getColor(point));
		}

		Ogre::Vector3 sunPos(0,0,0);
		if(m_CurrentCamera)
			sunPos = m_CurrentCamera->getDerivedPosition() - lightDir*m_SkyX->getMeshManager()->getSkydomeRadius()*0.1;
		if(m_Hydrax)
			m_Hydrax->setSunPosition(sunPos);

		//Light0->setPosition(mCamera->getDerivedPosition() - lightDir*mSkyX->getMeshManager()->getSkydomeRadius()*0.02);
		m_SunLight->setDirection(lightDir);

		Ogre::Vector3 sunCol = m_SunGradient.getColor(point);
		m_SunLight->setSpecularColour(sunCol.x, sunCol.y, sunCol.z);
		Ogre::Vector3 ambientCol = m_AmbientGradient.getColor(point);
		m_SunLight->setDiffuseColour(ambientCol.x, ambientCol.y, ambientCol.z);

		if(m_Hydrax)
			m_Hydrax->setSunColor(sunCol);
	}

}


