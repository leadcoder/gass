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
#include <SkyX.h>

#include "Plugins/Ogre/OgreConvert.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"

#include <Ogre.h>


namespace GASS
{
	EnvironmentManagerComponent::EnvironmentManagerComponent(void) : m_Hydrax(NULL), 
		m_Target(NULL),
		m_CurrentCamera(NULL), 
		m_SunLight(NULL), 
		m_SpecularWeight(0),
		m_CloudFactor(0)
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

		
		m_SunGradientValues.push_back(Vec3(0.8,0.75,0.55)*1.0);
		m_SunGradientValues.push_back(Vec3(0.8,0.75,0.55)*1.0);
		m_SunGradientValues.push_back(Vec3(0.8,0.75,0.55)*1.0);
		m_SunGradientValues.push_back(Vec3(0.6,0.5,0.2)*1.0);
		m_SunGradientValues.push_back(Vec3(0.6,0.5,0.2)*1.0);
		m_SunGradientValues.push_back(Vec3(0.6,0.5,0.2)*1.0);

		m_SunGradientWeights.push_back(1.0);
		m_SunGradientWeights.push_back(0.75);
		m_SunGradientWeights.push_back(0.5625);
		m_SunGradientWeights.push_back(0.5);
		m_SunGradientWeights.push_back(0.45);
		m_SunGradientWeights.push_back(0.0);

		m_AmbientGradientValues.push_back(Vec3(1,1,1)*1.0);
		m_AmbientGradientValues.push_back(Vec3(1,1,1)*1.0);
		m_AmbientGradientValues.push_back(Vec3(1,1,1)*0.6);
		m_AmbientGradientValues.push_back(Vec3(1,1,1)*0.3);
		m_AmbientGradientValues.push_back(Vec3(1,1,1)*0.1);
		m_AmbientGradientValues.push_back(Vec3(1,1,1)*0.05);

		m_AmbientGradientWeights.push_back(1.0);
		m_AmbientGradientWeights.push_back(0.6);
		m_AmbientGradientWeights.push_back(0.5);
		m_AmbientGradientWeights.push_back(0.45);
		m_AmbientGradientWeights.push_back(0.35);
		m_AmbientGradientWeights.push_back(0.0);


		m_FogGradientValues.push_back(Vec3(0.49, 0.62, 0.753)*1.0);
		m_FogGradientValues.push_back(Vec3(0.49, 0.62, 0.753)*1.0);
		m_FogGradientValues.push_back(Vec3(0.49, 0.62, 0.753)*1.0);
		m_FogGradientValues.push_back(Vec3(0.49, 0.62, 0.753)*1.0);
		m_FogGradientValues.push_back(Vec3(0.49, 0.62, 0.753)*1.0);
		m_FogGradientValues.push_back(Vec3(0.49, 0.62, 0.753)*1.0);
		
		m_FogGradientWeights.push_back(1.0);
		m_FogGradientWeights.push_back(0.6);
		m_FogGradientWeights.push_back(0.5);
		m_FogGradientWeights.push_back(0.45);
		m_FogGradientWeights.push_back(0.35);
		m_FogGradientWeights.push_back(0.0);

	}

	EnvironmentManagerComponent::~EnvironmentManagerComponent(void)
	{

	}

	void EnvironmentManagerComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("EnvironmentManagerComponent",new Creator<EnvironmentManagerComponent, IComponent>);
		RegisterVectorProperty<Vec3>("WaterGradient", &EnvironmentManagerComponent::GetWaterGradient, &EnvironmentManagerComponent::SetWaterGradient);
		RegisterVectorProperty<float>("WaterGradientWeights", &EnvironmentManagerComponent::GetWaterGradientWeights, &EnvironmentManagerComponent::SetWaterGradientWeights);
		RegisterVectorProperty<Vec3>("SunGradient", &EnvironmentManagerComponent::GetSunGradient, &EnvironmentManagerComponent::SetSunGradient);
		RegisterVectorProperty<float>("SunGradientWeights", &EnvironmentManagerComponent::GetSunGradientWeights, &EnvironmentManagerComponent::SetSunGradientWeights);
		RegisterVectorProperty<Vec3>("AmbientGradient", &EnvironmentManagerComponent::GetAmbientGradient, &EnvironmentManagerComponent::SetAmbientGradient);
		RegisterVectorProperty<float>("AmbientGradientWeights", &EnvironmentManagerComponent::GetAmbientGradientWeights, &EnvironmentManagerComponent::SetAmbientGradientWeights);
		RegisterVectorProperty<Vec3>("FogGradient", &EnvironmentManagerComponent::GetFogGradient, &EnvironmentManagerComponent::SetFogGradient);
		//RegisterVectorProperty<float>("FogDistance", &EnvironmentManagerComponent::GetFogDistance, &EnvironmentManagerComponent::SetFogDistance);
		RegisterVectorProperty<float>("FogGradientWeights", &EnvironmentManagerComponent::GetFogGradientWeights, &EnvironmentManagerComponent::SetFogGradientWeights);
		RegisterProperty<float>("SpecularWeight", &EnvironmentManagerComponent::GetSpecularWeight, &EnvironmentManagerComponent::SetSpecularWeight);
	}

	void EnvironmentManagerComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(EnvironmentManagerComponent::OnLoad,LoadComponentsMessage,4));
		GetSceneObject()->RegisterForMessage(REG_TMESS(EnvironmentManagerComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(EnvironmentManagerComponent::OnWeatherMessage,WeatherMessage,0));

	}

	void EnvironmentManagerComponent::OnLoad(LoadComponentsMessagePtr message)
	{
		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		//Ogre::Camera* ocam = sm->getCameraIterator().getNext();
		
		
		Ogre::Root::getSingleton().addFrameListener(this);

		
		SkyXComponentPtr skyx = GetSceneObject()->GetFirstComponentByClass<SkyXComponent>();
		HydraxWaterComponentPtr hydrax = GetSceneObject()->GetFirstComponentByClass<HydraxWaterComponent>();

		if(hydrax)
			m_Hydrax = hydrax->GetHydrax();
		
		if(hydrax && skyx->GetSkyX())
		{
			
			hydrax->GetHydrax()->getRttManager()->addRttListener(new HydraxRttListener(skyx->GetSkyX(), hydrax->GetHydrax()));
		}

		 // Light
		m_SunLight = sm->createLight("EnvironmentManagerComponentSunLight");
		m_SunLight->setType(Ogre::Light::LT_DIRECTIONAL);

		// Color gradients
		// Water
		m_WaterGradient = SkyX::ColorGradient();

		SetWaterGradient(m_WaterGradientValues);
		SetSunGradient(m_SunGradientValues);
		SetAmbientGradient(m_AmbientGradientValues);
		SetFogGradient(m_FogGradientValues);
	
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS( EnvironmentManagerComponent::OnChangeCamera,CameraChangedNotifyMessage,0));
	}

	void EnvironmentManagerComponent::OnWeatherMessage(WeatherMessagePtr message)
	{
		m_CloudFactor = message->GetClouds();
	}

	void EnvironmentManagerComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		if(sm && m_SunLight)
			sm->destroyLight(m_SunLight);

		m_SunLight = NULL;
		Ogre::Root::getSingleton().removeFrameListener(this);
		GetSceneObject()->GetScene()->UnregisterForMessage(UNREG_TMESS( EnvironmentManagerComponent::OnChangeCamera,CameraChangedNotifyMessage));
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


	void EnvironmentManagerComponent::SetSunGradient(const std::vector<Vec3> &value)
	{
		m_SunGradientValues = value;
		m_SunGradient.clear();
		m_SunGradient = SkyX::ColorGradient();

		if(m_SunGradientWeights.size() != m_SunGradientValues.size())
		{
			for(int i = 0; i < value.size(); i++)
			{
				m_SunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(value[i].x,value[i].y,value[i].z), float(i) / float(m_SunGradientValues.size()-1.0)));
			}
		}
		else
		{
			for(int i = 0; i < value.size(); i++)
			{
				m_SunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(value[i].x,value[i].y,value[i].z), m_SunGradientWeights[i]));
			}
		}
	}


	std::vector<Vec3>  EnvironmentManagerComponent::GetSunGradient() const 
	{
		return m_SunGradientValues;
	}



	void EnvironmentManagerComponent::SetSunGradientWeights(const std::vector<float> &value)
	{
		m_SunGradientWeights = value;
		m_SunGradient.clear();
		m_SunGradient = SkyX::ColorGradient();
		if(m_SunGradientWeights.size() == m_SunGradientValues.size())
		{
			SetSunGradient(m_SunGradientValues);
		}
	}

	std::vector<float>  EnvironmentManagerComponent::GetSunGradientWeights() const 
	{
		return m_SunGradientWeights;
	}

	void EnvironmentManagerComponent::SetAmbientGradient(const std::vector<Vec3> &value)
	{
		m_AmbientGradientValues = value;
		m_AmbientGradient.clear();
		m_AmbientGradient = SkyX::ColorGradient();

		if(m_AmbientGradientWeights.size() != m_AmbientGradientValues.size())
		{
			for(int i = 0; i < value.size(); i++)
			{
				m_AmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(value[i].x,value[i].y,value[i].z), float(i) / float(m_AmbientGradientValues.size()-1.0)));
			}
		}
		else
		{
			for(int i = 0; i < value.size(); i++)
			{
				m_AmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(value[i].x,value[i].y,value[i].z), m_AmbientGradientWeights[i]));
			}
		}
	}


	std::vector<Vec3>  EnvironmentManagerComponent::GetAmbientGradient() const 
	{
		return m_AmbientGradientValues;
	}



	void EnvironmentManagerComponent::SetAmbientGradientWeights(const std::vector<float> &value)
	{
		m_AmbientGradientWeights = value;
		m_AmbientGradient.clear();
		m_AmbientGradient = SkyX::ColorGradient();
		if(m_AmbientGradientWeights.size() == m_AmbientGradientValues.size())
		{
			SetAmbientGradient(m_AmbientGradientValues);
		}
	}

	std::vector<float>  EnvironmentManagerComponent::GetAmbientGradientWeights() const 
	{
		return m_AmbientGradientWeights;
	}

	void EnvironmentManagerComponent::SetFogGradient(const std::vector<Vec3> &value)
	{
		m_FogGradientValues = value;
		m_FogGradient.clear();
		m_FogGradient = SkyX::ColorGradient();

		if(m_FogGradientWeights.size() != m_FogGradientValues.size())
		{
			for(int i = 0; i < value.size(); i++)
			{
				m_FogGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(value[i].x,value[i].y,value[i].z), float(i) / float(m_FogGradientValues.size()-1.0)));
			}
		}
		else
		{
			for(int i = 0; i < value.size(); i++)
			{
				m_FogGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(value[i].x,value[i].y,value[i].z), m_FogGradientWeights[i]));
			}
		}
	}


	std::vector<Vec3>  EnvironmentManagerComponent::GetFogGradient() const 
	{
		return m_FogGradientValues;
	}



	void EnvironmentManagerComponent::SetFogGradientWeights(const std::vector<float> &value)
	{
		m_FogGradientWeights = value;
		m_FogGradient.clear();
		m_FogGradient = SkyX::ColorGradient();
		if(m_FogGradientWeights.size() == m_FogGradientValues.size())
		{
			SetFogGradient(m_FogGradientValues);
		}
	}

	std::vector<float>  EnvironmentManagerComponent::GetFogGradientWeights() const 
	{
		return m_FogGradientWeights;
	}



	void EnvironmentManagerComponent::OnChangeCamera(CameraChangedNotifyMessagePtr message)
	{
		m_CurrentCamera = static_cast<Ogre::Camera*> (message->GetUserData());
	}

	


	bool  EnvironmentManagerComponent::frameStarted(const Ogre::FrameEvent& evt)
	{
		UpdateEnvironmentLighting();
		return true;
	}

	void EnvironmentManagerComponent::UpdateEnvironmentLighting()
	{
		SkyXComponentPtr skyx = GetSceneObject()->GetFirstComponentByClass<SkyXComponent>();
		if(!skyx)
			return;

		if(skyx->GetSkyX() == NULL)
			return;
		
		Ogre::Vector3 lightDir = skyx->GetSkyX()->getAtmosphereManager()->getSunDirection();
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
			sunPos = m_CurrentCamera->getDerivedPosition() - lightDir*skyx->GetSkyX()->getMeshManager()->getSkydomeRadius()*0.1;
		if(m_Hydrax)
			m_Hydrax->setSunPosition(sunPos);

		//Light0->setPosition(mCamera->getDerivedPosition() - lightDir*mSkyX->getMeshManager()->getSkydomeRadius()*0.02);
		m_SunLight->setDirection(lightDir);

		Ogre::Vector3 sunCol = m_SunGradient.getColor(point)*(1-m_CloudFactor);
		m_SunLight->setSpecularColour(sunCol.x*m_SpecularWeight, sunCol.y*m_SpecularWeight, sunCol.z*m_SpecularWeight);
		Ogre::Vector3 ambientCol = m_AmbientGradient.getColor(point);
		m_SunLight->setDiffuseColour(sunCol.x, sunCol.y, sunCol.z);
				
		sm->setAmbientLight(Ogre::ColourValue(ambientCol.x, ambientCol.y, ambientCol.z,1));
		
		if(m_FogGradientValues.size() > 0)
		{
			Ogre::Vector3 fogCol = m_FogGradient.getColor(point);
			Ogre::Real cloud_colour = ambientCol.x*0.5;
			
			fogCol.x = fogCol.x*(1-m_CloudFactor) +  cloud_colour*m_CloudFactor;
			fogCol.y = fogCol.y*(1-m_CloudFactor) +  cloud_colour*m_CloudFactor;
			fogCol.z = fogCol.z*(1-m_CloudFactor) +  cloud_colour*m_CloudFactor;

			Ogre::ColourValue fogColour(fogCol.x, fogCol.y, fogCol.z);

			sm->setFog(sm->getFogMode(),fogColour, sm->getFogDensity(), sm->getFogStart(), sm->getFogEnd());
		}
		if(m_Hydrax)
			m_Hydrax->setSunColor(sunCol);

		/*std::stringstream ss;
		ss << "Gradient value:" << point << "\n";
		ss << "Sun color:" << sunCol.x << " " << sunCol.y << " " << sunCol.z << "\n";
		ss << "Ambient color:" << ambientCol.x << " " << ambientCol.y << " " << ambientCol.z << "\n";
		//ss << "Fog color :" << fogColour.r << fogColour.g << fogColour.b << "\n";
		SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr( new DebugPrintMessage(ss.str())));*/
	}

}


