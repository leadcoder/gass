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

#include "SkyXComponent.h"
#include "SkyXVolumeCloudComponent.h"
#include "SkyXCloudLayerComponent.h"
#include "Plugins/Ogre/GASSIOgreCameraProxy.h"
#include "Plugins/Ogre/GASSIOgreSceneManagerProxy.h"
#include "Plugins/Ogre/GASSOgreConvert.h"

#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSIViewport.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include <Ogre.h>

namespace GASS
{
	SkyXComponent::SkyXComponent(void) : m_TimeMultiplier(1), 
		m_MoonSize(1),
		m_SkyX(NULL), 
		m_Radius(5000),
		m_Target(NULL), 
		m_SkyDomeFog(false),
		m_Initialized (false),
		m_BasicController(NULL),
		m_Time(12,7,18)
	{
				
	}

	SkyXComponent::~SkyXComponent(void)
	{

	}

	void SkyXComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("SkyXComponent",new Creator<SkyXComponent, Component>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("SkyXComponent", OF_VISIBLE )));
		RegisterProperty<double>("TimeMultiplier", &SkyXComponent::GetTimeMultiplier, &SkyXComponent::SetTimeMultiplier,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec3>("Time", &SkyXComponent::GetTime, &SkyXComponent::SetTime,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Float>("SunInnerRadius", &SkyXComponent::GetInnerRadius, &SkyXComponent::SetInnerRadius,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Float>("SunOuterRadius", &SkyXComponent::GetOuterRadius, &SkyXComponent::SetOuterRadius,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Float>("SunExposure", &SkyXComponent::GetExposure, &SkyXComponent::SetExposure,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Float>("HeightPosition", &SkyXComponent::GetHeightPosition, &SkyXComponent::SetHeightPosition,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Float>("RayleighMultiplier", &SkyXComponent::GetRayleighMultiplier, &SkyXComponent::SetRayleighMultiplier,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Float>("MieMultiplier", &SkyXComponent::GetMieMultiplier, &SkyXComponent::SetMieMultiplier,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<int>("NumberOfSamples", &SkyXComponent::GetNumberOfSamples, &SkyXComponent::SetNumberOfSamples,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Float>("MoonSize", &SkyXComponent::GetMoonSize, &SkyXComponent::SetMoonSize,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("SkyDomeFog", &SkyXComponent::GetSkyDomeFog, &SkyXComponent::SetSkyDomeFog,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
	}

	void SkyXComponent::OnInitialize()
	{
		if(m_Initialized)
			return;

		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(SkyXComponent::OnTimeOfDayRequest,TimeOfDayRequest,0));
		// Create SkyX object
		_Init();
		m_Initialized = true;
	}

	void SkyXComponent::_Init()
	{
	    Ogre::SceneManager* sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<IOgreSceneManagerProxy>()->GetOgreSceneManager();

		delete m_BasicController;
		m_BasicController = new SkyX::BasicController();
		
		delete m_SkyX;
		m_SkyX = new SkyX::SkyX(sm, m_BasicController);

		// No smooth fading
		//m_SkyX->getMeshManager()->setSkydomeFadingParameters(false);
		
		
		// A little change to default atmosphere settings :)
		SkyX::AtmosphereManager::Options atOpt = m_SkyX->getAtmosphereManager()->getOptions();
		atOpt.RayleighMultiplier = 0.003075f;
		atOpt.MieMultiplier = 0.00125f;
		atOpt.InnerRadius = 9.92f;
		atOpt.OuterRadius = 10.3311f;
		m_SkyX->getAtmosphereManager()->setOptions(atOpt);
		
		UpdateOptions();

		m_SkyX->setTimeMultiplier(m_TimeMultiplier);
		SetTime(m_Time);
		//m_SkyXOptions = m_SkyX->getAtmosphereManager()->getOptions();
		m_MoonSize = m_SkyX->getMoonManager()->getMoonSize();

		// Create the sky
		m_SkyX->create();
		Ogre::RenderSystem::RenderTargetIterator iter = Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator();
		while (iter.hasMoreElements())
		{
			Ogre::RenderWindow* target = dynamic_cast<Ogre::RenderWindow*>(iter.getNext());
			if(target)// && target->getNumViewports() > 0 && target->getViewport(0)->getCamera() )
			{
				
				target->addListener(m_SkyX);
				break;
			}
		}

		Ogre::Root::getSingletonPtr()->addFrameListener(m_SkyX);

		SetSkyDomeFog(m_SkyDomeFog);
		//ocam->setFarClipDistance(save_clip);

		//create clouds
		ComponentContainer::ComponentVector components;
		GetSceneObject()->GetComponentsByClassName(components, "SkyXCloudLayerComponent", true);
		for(int i = 0 ;  i < components.size(); i++)
		{
			SkyXCloudLayerComponentPtr layer = GASS_DYNAMIC_PTR_CAST<SkyXCloudLayerComponent>(components[i]);
			layer->CreateLayer();
		}

		components.clear();
		GetSceneObject()->GetComponentsByClassName(components, "SkyXVolumeCloudComponent", true);
		for(int i = 0 ;  i < components.size(); i++)
		{
			SkyXVolumeCloudComponentPtr volume = GASS_DYNAMIC_PTR_CAST<SkyXVolumeCloudComponent>(components[i]);
			volume->CreateVolume();
		}
		m_SkyX->update(0);
	}

	void SkyXComponent::SetSkyDomeFog(bool value)
	{
		m_SkyDomeFog  = value;
		if(m_SkyX)
		{
			Ogre::String mat_name = m_SkyX->getMeshManager()->getMaterialName();
			Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(mat_name);

			if(!mat.isNull() && mat->getNumTechniques() > 0)
			{
				Ogre::Technique * technique = mat->getTechnique(0);
				if(technique && technique->getNumPasses() > 0)
				{
					Ogre::Pass* pass = technique->getPass(0);
					if(pass)
						pass->setFog(!value);
				}
			}
		}
	}


	void SkyXComponent::OnDelete()
	{
		//Ogre::Root::getSingleton().removeFrameListener(this);
	    Ogre::Root::getSingleton().removeFrameListener(m_SkyX);

		Ogre::RenderSystem::RenderTargetIterator iter = Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator();
		while (iter.hasMoreElements())
		{
			Ogre::RenderWindow* target = dynamic_cast<Ogre::RenderWindow*>(iter.getNext());
			if(target)
			{
				target->removeListener(m_SkyX);
				break;
			}
		}

		delete m_SkyX;
		m_SkyX = NULL;
		//SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS( SkyXComponent::OnChangeCamera,CameraChangedEvent));
	}

	void SkyXComponent::SetMoonSize(const Float &value)
	{
		m_MoonSize = value;
		if(m_SkyX)
		{
			m_SkyX->getMoonManager()->setMoonSize(value);
		}
	}

	Float SkyXComponent::GetMoonSize() const 
	{
		return m_MoonSize;
	}

	void SkyXComponent::SetInnerRadius(const Float &value)
	{
		m_SkyXOptions.InnerRadius = value;
		UpdateOptions();
	}

	Float SkyXComponent::GetInnerRadius() const 
	{
		return m_SkyXOptions.InnerRadius;
	}

	void SkyXComponent::SetOuterRadius(const Float &value)
	{
		m_SkyXOptions.OuterRadius = value;
		UpdateOptions();
	}

	Float SkyXComponent::GetOuterRadius() const 
	{
		return m_SkyXOptions.OuterRadius;
	}

	void SkyXComponent::SetExposure(const Float &value)
	{
		m_SkyXOptions.Exposure = value;
		UpdateOptions();
	}

	Float SkyXComponent::GetExposure() const 
	{
		return m_SkyXOptions.Exposure;
	}


	void SkyXComponent::SetHeightPosition(const Float &value)
	{
		m_SkyXOptions.HeightPosition = value;
		UpdateOptions();
	}

	Float SkyXComponent::GetHeightPosition() const 
	{
		return m_SkyXOptions.HeightPosition;
	}

	void SkyXComponent::SetRayleighMultiplier(const Float &value)
	{
		m_SkyXOptions.RayleighMultiplier = value;
		UpdateOptions();
	}

	Float SkyXComponent::GetRayleighMultiplier() const 
	{
		return m_SkyXOptions.RayleighMultiplier;
	}

	Float SkyXComponent::GetMieMultiplier() const 
	{
		return m_SkyXOptions.MieMultiplier;
	}

	void SkyXComponent::SetMieMultiplier(const Float &value)
	{
		m_SkyXOptions.MieMultiplier = value;
		UpdateOptions();
	}


	
	int SkyXComponent::GetNumberOfSamples() const 
	{
		return m_SkyXOptions.NumberOfSamples;
	}

	void SkyXComponent::SetNumberOfSamples(const int &value)
	{
		m_SkyXOptions.NumberOfSamples = value;
		UpdateOptions();
	}

	void SkyXComponent::SetTimeMultiplier(const double &value)
	{
		if(m_SkyX)
		{
			m_SkyX->setTimeMultiplier(value);
		}
		m_TimeMultiplier = value;
	}

	double SkyXComponent::GetTimeMultiplier() const 
	{
		return m_TimeMultiplier;
	}


	Vec3 SkyXComponent::GetTime() const 
	{
		//if(m_BasicController)
		//	return OgreConvert::ToGASS(m_BasicController->getTime());
		return m_Time;
	}

	void SkyXComponent::SetTime(const Vec3 &value)
	{
		m_Time = value;
		if(m_BasicController)
			m_BasicController->setTime(OgreConvert::ToOgre(value));
	}


	void SkyXComponent::UpdateOptions()
	{
		if(m_SkyX)
		{
			m_SkyX->getAtmosphereManager()->setOptions(m_SkyXOptions);
		}
	}

	/*void SkyXComponent::OnChangeCamera(CameraChangedEventPtr message)
	{
		if(m_SkyX)
		{
			OgreCameraProxyPtr camera_proxy = GASS_DYNAMIC_PTR_CAST<IOgreCameraProxy>(message->GetViewport()->GetCamera());
			//Ogre::Camera * cam = static_cast<Ogre::Camera*> (message->GetUserData());
			//Init(camera_proxy->GetOgreCamera());
		}
	}*/


	/*bool  SkyXComponent::frameStarted(const Ogre::FrameEvent& evt)
	{
		double c_time = SimEngine::Get().GetTime();
		static double prev_time = 0;

		if(prev_time == 0)
			m_SkyX->update(0.1);
		else
			m_SkyX->update(c_time - prev_time);
		
		prev_time = c_time;
		return true;
	}*/

	void SkyXComponent::OnTimeOfDayRequest(TimeOfDayRequestPtr message)
	{
		SetTimeMultiplier(message->GetSpeed());
		SetTime(Vec3(message->GetTime(),message->GetSunRise(),message->GetSunSet()));
		//std::cout << "Set time of day, time:" << message->GetTime() << " rise:" << message->GetSunRise() << " Set:" << message->GetSunSet() << " speed:" << message->GetSpeed() << "\n";
	}
}


