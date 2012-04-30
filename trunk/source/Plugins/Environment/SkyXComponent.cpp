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

#include "SkyXComponent.h"
#include "SkyXVolumeCloudComponent.h"
#include "SkyXCloudLayerComponent.h"

#include "Plugins/Ogre/OgreConvert.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Scene/GASSScene.h"
#include <Ogre.h>

namespace GASS
{
	SkyXComponent::SkyXComponent(void) : m_TimeMultiplier(1), m_MoonSize(1),m_SkyX(NULL), m_Radius(5000),m_Target(NULL), m_SkyDomeFog(false)
	{
				
	}

	SkyXComponent::~SkyXComponent(void)
	{

	}

	void SkyXComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("SkyXComponent",new Creator<SkyXComponent, IComponent>);
		RegisterProperty<double>("TimeMultiplier", &SkyXComponent::GetTimeMultiplier, &SkyXComponent::SetTimeMultiplier);
		RegisterProperty<Vec3>("Time", &SkyXComponent::GetTime, &SkyXComponent::SetTime);
		RegisterProperty<Float>("SunInnerRadius", &SkyXComponent::GetInnerRadius, &SkyXComponent::SetInnerRadius);
		RegisterProperty<Float>("SunOuterRadius", &SkyXComponent::GetOuterRadius, &SkyXComponent::SetOuterRadius);
		RegisterProperty<Float>("SunExposure", &SkyXComponent::GetExposure, &SkyXComponent::SetExposure);
		RegisterProperty<Float>("HeightPosition", &SkyXComponent::GetHeightPosition, &SkyXComponent::SetHeightPosition);
		RegisterProperty<Float>("RayleighMultiplier", &SkyXComponent::GetRayleighMultiplier, &SkyXComponent::SetRayleighMultiplier);
		RegisterProperty<Float>("MieMultiplier", &SkyXComponent::GetMieMultiplier, &SkyXComponent::SetMieMultiplier);
		RegisterProperty<int>("NumberOfSamples", &SkyXComponent::GetNumberOfSamples, &SkyXComponent::SetNumberOfSamples);
		RegisterProperty<Float>("MoonSize", &SkyXComponent::GetMoonSize, &SkyXComponent::SetMoonSize);
		RegisterProperty<bool>("SkyDomeFog", &SkyXComponent::GetSkyDomeFog, &SkyXComponent::SetSkyDomeFog);
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
		return Convert::ToGASS(m_SkyXOptions.Time);
	}

	void SkyXComponent::SetTime(const Vec3 &value)
	{
		m_SkyXOptions.Time = Convert::ToOgre(value);
		UpdateOptions();
	}

	void SkyXComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(SkyXComponent::OnLoad,LoadGFXComponentsMessage,2));
		GetSceneObject()->RegisterForMessage(REG_TMESS(SkyXComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(SkyXComponent::OnTimeOfDayMessage,TimeOfDayMessage,0));
	}

	void SkyXComponent::UpdateOptions()
	{
		if(m_SkyX)
		{
			m_SkyX->getAtmosphereManager()->setOptions(m_SkyXOptions);
		}
	}

	void SkyXComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		Ogre::Root::getSingleton().removeFrameListener(this);
		delete m_SkyX;
		m_SkyX = NULL;
		GetSceneObject()->GetScene()->UnregisterForMessage(UNREG_TMESS( SkyXComponent::OnChangeCamera,CameraChangedNotifyMessage));

	}

	void SkyXComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		Ogre::Camera* ocam = sm->getCameraIterator().getNext();
		Ogre::Root::getSingleton().addFrameListener(this);
		float save_clip  = ocam->getFarClipDistance();
		//ocam->setFarClipDistance(m_Radius);
		
		// Create our projected grid module  
		Ogre::MaterialPtr terrain_mat = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("TerrainMat"));

		//if(terrain_mat.get())
		//m_Hydrax->getMaterialManager()->addDepthTechnique(terrain_mat->createTechnique());

		// Create SkyX object
		Init(ocam);

		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS( SkyXComponent::OnChangeCamera,CameraChangedNotifyMessage,0));
		
	}

	void SkyXComponent::OnChangeCamera(CameraChangedNotifyMessagePtr message)
	{
		if(m_SkyX)
		{
			Ogre::Camera * cam = static_cast<Ogre::Camera*> (message->GetUserData());
			Init(cam);
		}
	}

	void SkyXComponent::Init(Ogre::Camera* ocam)
	{
		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();

		delete m_SkyX;
		m_SkyX = new SkyX::SkyX(sm, ocam);
		// No smooth fading
		m_SkyX->getMeshManager()->setSkydomeFadingParameters(false);
		
		
		// A little change to default atmosphere settings :)
		SkyX::AtmosphereManager::Options atOpt = m_SkyX->getAtmosphereManager()->getOptions();
		atOpt.RayleighMultiplier = 0.003075f;
		atOpt.MieMultiplier = 0.00125f;
		atOpt.InnerRadius = 9.92f;
		atOpt.OuterRadius = 10.3311f;
		m_SkyX->getAtmosphereManager()->setOptions(atOpt);
		

		
		UpdateOptions();

		m_SkyX->setTimeMultiplier(m_TimeMultiplier);
		//m_SkyXOptions = m_SkyX->getAtmosphereManager()->getOptions();
		m_MoonSize = m_SkyX->getMoonManager()->getMoonSize();

		// Create the sky
		m_SkyX->create();

		Ogre::String mat_name = m_SkyX->getMeshManager()->getMaterialName();
		Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(mat_name);

		if(!mat.isNull() && mat->getNumTechniques() > 0)
		{
			Ogre::Technique * technique = mat->getTechnique(0);
			if(technique && technique->getNumPasses() > 0)
			{
				Ogre::Pass* pass = technique->getPass(0);
				if(pass)
					pass->setFog(m_SkyDomeFog);
			}
		}
		//ocam->setFarClipDistance(save_clip);

		//create clouds
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetComponentsByClass(components, "SkyXCloudLayerComponent", true);
		for(int i = 0 ;  i < components.size(); i++)
		{
			SkyXCloudLayerComponentPtr layer = boost::shared_dynamic_cast<SkyXCloudLayerComponent>(components[i]);
			layer->CreateLayer();
		}

		components.clear();
		GetSceneObject()->GetComponentsByClass(components, "SkyXVolumeCloudComponent", true);
		for(int i = 0 ;  i < components.size(); i++)
		{
			SkyXVolumeCloudComponentPtr volume = boost::shared_dynamic_cast<SkyXVolumeCloudComponent>(components[i]);
			volume->CreateVolume();
		}
	}

	bool  SkyXComponent::frameStarted(const Ogre::FrameEvent& evt)
	{
		double c_time = SimEngine::Get().GetTime();
		static double prev_time = 0;

		if(prev_time == 0)
			m_SkyX->update(0.1);
		else
			m_SkyX->update(c_time - prev_time);

		prev_time = c_time;
		return true;
	}

	void SkyXComponent::OnTimeOfDayMessage(TimeOfDayMessagePtr message)
	{
		SetTimeMultiplier(message->GetSpeed());
		SetTime(Vec3(message->GetTime(),message->GetSunRise(),message->GetSunSet()));
		//std::cout << "Set time of day, time:" << message->GetTime() << " rise:" << message->GetSunRise() << " Set:" << message->GetSunSet() << " speed:" << message->GetSpeed() << "\n";
	}
}


