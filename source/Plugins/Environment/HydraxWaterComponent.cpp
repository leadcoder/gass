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

#include "HydraxWaterComponent.h"
#include "Plugins/Ogre/GASSOgreConvert.h"
#include "Plugins/Ogre/GASSIOgreCameraProxy.h"
#include "Plugins/Ogre/GASSIOgreSceneManagerProxy.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSFileUtils.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSICameraComponent.h"

/*#pragma warning(disable: 4512)
#include <Ogre.h>

#include "Hydrax/Hydrax.h"
#include "Hydrax/Noise/Perlin/Perlin.h"
#include "Hydrax/Noise/FFT/FFT.h"
#include "Hydrax/Modules/ProjectedGrid/ProjectedGrid.h"
#include "Hydrax/Modules/SimpleGrid/SimpleGrid.h"
#include "Hydrax/Modules/RadialGrid/RadialGrid.h"
*/

namespace GASS
{

	HydraxWaterComponent::HydraxWaterComponent(void) : m_Hydrax(NULL) , 
		m_Rot(0,0,0), 
		m_Perlin(NULL), 
		m_FFT(NULL), 
		m_ProjectedGridGeometryModuleVertex(NULL),
		m_ProjectedGridGeometryModuleRtt(NULL),
		m_SimpleGridGeometryModuleVertex(NULL),
		m_SimpleGridGeometryModuleRtt(NULL),
		m_RadialGridGeometryModuleVertex(NULL),
		m_RadialGridGeometryModuleRtt(NULL),
		m_Target(NULL),
		m_ResourceLocation("%GASS_DATA_HOME%/gfx/ogre/ExternalResources/hydrax"),
		m_ActiveModule("ProjectedGridRtt"),
		m_Initialized(false)
	{

	}

	HydraxWaterComponent::~HydraxWaterComponent(void)
	{

	}

	void HydraxWaterComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<HydraxWaterComponent>();
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("HydraxWaterComponent", OF_VISIBLE )));

		RegisterGetSet("ConfigurationFile", &HydraxWaterComponent::GetConfigurationFile, &HydraxWaterComponent::SetConfigurationFile,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("ActiveModule", &HydraxWaterComponent::GetActiveModule, &HydraxWaterComponent::SetActiveModule,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("ActiveNoise", &HydraxWaterComponent::GetActiveNoise, &HydraxWaterComponent::SetActiveNoise,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("Position", &HydraxWaterComponent::GetPosition, &HydraxWaterComponent::SetPosition,PF_VISIBLE | PF_EDITABLE,"");

		RegisterGetSet("Rotation", &HydraxWaterComponent::GetRotation, &HydraxWaterComponent::SetRotation,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("GlobalTransparency", &HydraxWaterComponent::GetGlobalTransparency, &HydraxWaterComponent::SetGlobalTransparency,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("FullReflectionDistance", &HydraxWaterComponent::GetFullReflectionDistance, &HydraxWaterComponent::SetFullReflectionDistance,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("WaterColor", &HydraxWaterComponent::GetWaterColor, &HydraxWaterComponent::SetWaterColor,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("NormalDistortion", &HydraxWaterComponent::GetNormalDistortion, &HydraxWaterComponent::SetNormalDistortion,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("SunPosition", &HydraxWaterComponent::GetSunPosition, &HydraxWaterComponent::SetSunPosition,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("SunStrength", &HydraxWaterComponent::GetSunStrength, &HydraxWaterComponent::SetSunStrength,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("SunArea", &HydraxWaterComponent::GetSunArea, &HydraxWaterComponent::SetSunArea,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("SunColor", &HydraxWaterComponent::GetSunColor, &HydraxWaterComponent::SetSunColor,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("FoamMaxDistance", &HydraxWaterComponent::GetFoamMaxDistance, &HydraxWaterComponent::SetFoamMaxDistance,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("FoamScale", &HydraxWaterComponent::GetFoamScale, &HydraxWaterComponent::SetFoamScale,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("FoamStart", &HydraxWaterComponent::GetFoamStart, &HydraxWaterComponent::SetFoamStart,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("FoamTransparency", &HydraxWaterComponent::GetFoamTransparency, &HydraxWaterComponent::SetFoamTransparency,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("DepthLimit", &HydraxWaterComponent::GetDepthLimit, &HydraxWaterComponent::SetDepthLimit,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("SmoothPower", &HydraxWaterComponent::GetSmoothPower, &HydraxWaterComponent::SetSmoothPower,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("CausticsScale", &HydraxWaterComponent::GetCausticsScale, &HydraxWaterComponent::SetCausticsScale,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("CausticsPower", &HydraxWaterComponent::GetCausticsPower, &HydraxWaterComponent::SetCausticsPower,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("CausticsEnd", &HydraxWaterComponent::GetCausticsEnd, &HydraxWaterComponent::SetCausticsEnd,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("GodRaysExposure", &HydraxWaterComponent::GetGodRaysExposure, &HydraxWaterComponent::SetGodRaysExposure,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("GodRaysIntensity", &HydraxWaterComponent::GetGodRaysIntensity, &HydraxWaterComponent::SetGodRaysIntensity,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("UnderwaterCameraSwitchDelta", &HydraxWaterComponent::GetUnderwaterCameraSwitchDelta, &HydraxWaterComponent::SetUnderwaterCameraSwitchDelta,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("PerlinScale", &HydraxWaterComponent::GetPerlinScale, &HydraxWaterComponent::SetPerlinScale,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("PerlinSpeed", &HydraxWaterComponent::GetPerlinSpeed, &HydraxWaterComponent::SetPerlinSpeed,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("FFTScale", &HydraxWaterComponent::GetFFTScale, &HydraxWaterComponent::SetFFTScale,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("FFTSpeed", &HydraxWaterComponent::GetFFTSpeed, &HydraxWaterComponent::SetFFTSpeed,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("Strength", &HydraxWaterComponent::GetStrength, &HydraxWaterComponent::SetStrength,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("Elevation", &HydraxWaterComponent::GetElevation, &HydraxWaterComponent::SetElevation,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("SaveConfiguration", &HydraxWaterComponent::GetSave, &HydraxWaterComponent::SetSave,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("PlanesError", &HydraxWaterComponent::GetPlanesError, &HydraxWaterComponent::SetPlanesError,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("ResourceLocation", &HydraxWaterComponent::GetResourceLocation, &HydraxWaterComponent::SetResourceLocation,PF_VISIBLE,"");
	}

	void HydraxWaterComponent::OnInitialize()
	{
		if(m_Initialized)
			return;

		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(HydraxWaterComponent::OnChangeCamera,CameraChangedEvent,0));
		Ogre::SceneManager* sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<IOgreSceneManagerProxy>()->GetOgreSceneManager();
		Ogre::Camera* ocam = NULL;
		//if(sm->hasCamera("DummyCamera"))
		//	ocam = sm->getCamera("DummyCamera");
		//else
		//	ocam = sm->createCamera("DummyCamera");
		//Ogre::RenderTarget* target = NULL;
		Ogre::Viewport* vp =NULL;
		Ogre::RenderSystem::RenderTargetIterator iter = Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator();
		while (iter.hasMoreElements())
		{
			Ogre::RenderWindow* target = dynamic_cast<Ogre::RenderWindow*>(iter.getNext());
			if(target)
			{
				if(target->getNumViewports() > 0)
				{
					ocam = target->getViewport(0)->getCamera();
					vp = target->getViewport(0);
					break;
				}
			}
			//target->addListener(this);
		}

		if(ocam == NULL)
		{
			if(sm->hasCamera("DummyCamera"))
				ocam = sm->getCamera("DummyCamera");
			else
				ocam = sm->createCamera("DummyCamera");

			vp->setCamera(ocam);
		}

		Ogre::Root::getSingleton().addFrameListener(this);
		CreateHydrax(sm, ocam, vp);
		m_Initialized = true;
	}

	void HydraxWaterComponent::OnDelete()
	{
		Ogre::Root::getSingleton().removeFrameListener(this);
		Ogre::CompositorManager& compMgr = Ogre::CompositorManager::getSingleton();
		compMgr.removeCompositor(m_Hydrax->getViewport(),"_Hydrax_Underwater_Compositor_Name");

		m_Hydrax->remove();
		delete m_Hydrax;
		Ogre::ResourceGroupManager::getSingletonPtr()->destroyResourceGroup("Hydrax");

		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS( HydraxWaterComponent::OnChangeCamera,CameraChangedEvent));
	}

	void HydraxWaterComponent::SetSave(const std::string &value)
	{
		if(m_Hydrax)
		{
			m_ConfigurationFile = value;
			std::string filename = FileUtils::GetFilename(value);
			std::string path = FileUtils::RemoveFilename(value);

			m_Hydrax->saveCfg(filename, path);
		}
	}

	
	std::string HydraxWaterComponent::GetSave() const 
	{
		return m_ConfigurationFile;
	}


	FilePath HydraxWaterComponent::GetResourceLocation() const 
	{
		return m_ResourceLocation;
	}


	void HydraxWaterComponent::SetResourceLocation(const FilePath &value)
	{
		m_ResourceLocation = value;
	}


	void HydraxWaterComponent::SetPlanesError(const float &value)
	{
		if(m_Hydrax)
		{
			m_Hydrax->setPlanesError(value);
		}
	}


	float HydraxWaterComponent::GetPlanesError() const 
	{
		if(m_Hydrax)
		{
			return m_Hydrax->getPlanesError();
		}
		return 0;
	}


	void HydraxWaterComponent::SetGPUStrength(const float &/*value*/)
	{
		if(m_Hydrax)
		{
	//		m_Hydrax->_setStrength(value);
		}
	}

	float HydraxWaterComponent::GetGPUStrength() const 
	{
		return 0;
	}

	void HydraxWaterComponent::SetStrength(const float &value)
	{
		if(m_Hydrax && m_ProjectedGridGeometryModuleVertex == m_Hydrax->getModule())
		{
			Hydrax::Module::ProjectedGrid::Options  options = m_ProjectedGridGeometryModuleVertex->getOptions();
			options.Strength = value;
			m_ProjectedGridGeometryModuleVertex->setOptions(options);
			m_ProjectedGridGeometryModuleRtt->setOptions(options);
		}
		else if(m_Hydrax && m_ProjectedGridGeometryModuleRtt == m_Hydrax->getModule())
		{
			Hydrax::Module::ProjectedGrid::Options  options = m_ProjectedGridGeometryModuleRtt->getOptions();
			options.Strength = value;
			m_ProjectedGridGeometryModuleRtt->setOptions(options);
		}
	}

	float HydraxWaterComponent::GetStrength() const 
	{
		if(m_Hydrax && m_ProjectedGridGeometryModuleVertex == m_Hydrax->getModule())
		{
			Hydrax::Module::ProjectedGrid::Options  options = m_ProjectedGridGeometryModuleVertex->getOptions();
			return options.Strength;
		}
		else if(m_Hydrax && m_ProjectedGridGeometryModuleRtt == m_Hydrax->getModule())
		{
			Hydrax::Module::ProjectedGrid::Options  options = m_ProjectedGridGeometryModuleRtt->getOptions();
			return options.Strength;
		}
		return 0;
	}

	void HydraxWaterComponent::SetElevation(const float &value)
	{
		if(m_ProjectedGridGeometryModuleVertex)
		{
			Hydrax::Module::ProjectedGrid::Options  options = m_ProjectedGridGeometryModuleVertex->getOptions();
			options.Elevation = value;
			m_ProjectedGridGeometryModuleVertex->setOptions(options);
		}
	}


	float HydraxWaterComponent::GetElevation() const 
	{
		if(m_ProjectedGridGeometryModuleVertex)
		{
			Hydrax::Module::ProjectedGrid::Options  options = m_ProjectedGridGeometryModuleVertex->getOptions();
			return options.Elevation;
		}
		return 0;
	}

	void HydraxWaterComponent::SetPerlinScale(const float &value)
	{
		if(m_Perlin)
		{
			Hydrax::Noise::Perlin::Options PN_Options = m_Perlin->getOptions();
			PN_Options.Scale = value;
			m_Perlin->setOptions(PN_Options);
		}
	}




	float HydraxWaterComponent::GetPerlinScale() const
	{
		if(m_Perlin)
		{
			Hydrax::Noise::Perlin::Options PN_Options = m_Perlin->getOptions();
			return PN_Options.Scale;
		}
		return 0;
	}


	void HydraxWaterComponent::SetPerlinSpeed(const float &value)
	{
		if(m_Perlin)
		{
			Hydrax::Noise::Perlin::Options PN_Options = m_Perlin->getOptions();
			PN_Options.Animspeed = value;
			m_Perlin->setOptions(PN_Options);
		}
	}

	float HydraxWaterComponent::GetPerlinSpeed() const
	{
		if(m_Perlin)
		{
			Hydrax::Noise::Perlin::Options PN_Options = m_Perlin->getOptions();
			return PN_Options.Animspeed;
		}
		return 0;
	}


	void HydraxWaterComponent::SetFFTScale(const float &value)
	{
		if(m_FFT)
		{
			Hydrax::Noise::FFT::Options FFT_Options = m_FFT->getOptions();
			FFT_Options.Scale = value;
			m_FFT->setOptions(FFT_Options);
		}
	}

	float HydraxWaterComponent::GetFFTScale() const
	{
		if(m_FFT)
		{
			Hydrax::Noise::FFT::Options FFT_Options = m_FFT->getOptions();
			return FFT_Options.Scale;
		}
		return 0;
	}

	void HydraxWaterComponent::SetFFTSpeed(const float &value)
	{
		if(m_FFT)
		{
			Hydrax::Noise::FFT::Options FFT_Options = m_FFT->getOptions();
			FFT_Options.AnimationSpeed = value;
			m_FFT->setOptions(FFT_Options);
		}
	}

	float HydraxWaterComponent::GetFFTSpeed() const
	{
		if(m_FFT)
		{
			Hydrax::Noise::FFT::Options FFT_Options = m_FFT->getOptions();
			return FFT_Options.AnimationSpeed;
		}
		return 0;
	}

	void HydraxWaterComponent::SetPosition(const Vec3 &value)
	{
		if(m_Hydrax)
			m_Hydrax->setPosition(OgreConvert::ToOgre(value));
	}

	Vec3 HydraxWaterComponent::GetPosition() const
	{
		if(m_Hydrax)
			return OgreConvert::ToGASS(m_Hydrax->getPosition());
		return Vec3(0,0,0);
	}

	void HydraxWaterComponent::SetRotation(const EulerRotation &value)
	{
		m_Rot = value;
		if(m_Hydrax)
			m_Hydrax->rotate(OgreConvert::ToOgre(value.GetQuaternion()));
	}

	EulerRotation HydraxWaterComponent::GetRotation() const
	{
		return m_Rot;
	}

	void HydraxWaterComponent::SetGlobalTransparency(float value)
	{
		if(m_Hydrax)
			m_Hydrax->setGlobalTransparency(value);
	}

	float HydraxWaterComponent::GetGlobalTransparency() const
	{
		if(m_Hydrax)
			return m_Hydrax->getGlobalTransparency();
		return 0;
	}
	void HydraxWaterComponent::SetFullReflectionDistance(float value)
	{
		if(m_Hydrax)
			m_Hydrax->setFullReflectionDistance(value);
	}

	float HydraxWaterComponent::GetFullReflectionDistance() const
	{
		if(m_Hydrax)
			return m_Hydrax->getFullReflectionDistance();
		return 0;
	}

	void HydraxWaterComponent::SetWaterColor(const Vec3& value)
	{
		if(m_Hydrax)
			m_Hydrax->setWaterColor(OgreConvert::ToOgre(value));
	}
	Vec3 HydraxWaterComponent::GetWaterColor() const
	{
		if(m_Hydrax)
			return OgreConvert::ToGASS(m_Hydrax->getWaterColor());			
		return Vec3(0,0,0);
	}
	void HydraxWaterComponent::SetNormalDistortion(float value)
	{
		if(m_Hydrax)
			m_Hydrax->setNormalDistortion(value);			
	}

	float HydraxWaterComponent::GetNormalDistortion() const
	{
		if(m_Hydrax)
			return m_Hydrax->getNormalDistortion();
		return 0;
	}

	void HydraxWaterComponent::SetSunPosition(const Vec3 & value)
	{
		if(m_Hydrax)
			m_Hydrax->setSunPosition(OgreConvert::ToOgre(value));
	}

	Vec3 HydraxWaterComponent::GetSunPosition() const
	{
		if(m_Hydrax)
			return OgreConvert::ToGASS(m_Hydrax->getSunPosition());			
		return Vec3(0,0,0);

	}

	void HydraxWaterComponent::SetSunStrength(float value)
	{
		if(m_Hydrax)
			m_Hydrax->setSunStrength(value);			
	}

	float HydraxWaterComponent::GetSunStrength() const
	{
		if(m_Hydrax)
			return m_Hydrax->getSunStrength();
		return 0;
	}

	void HydraxWaterComponent::SetSunArea(float value)
	{
		if(m_Hydrax)
			m_Hydrax->setSunArea(value);			

	}

	float HydraxWaterComponent::GetSunArea() const
	{
		if(m_Hydrax)
			return m_Hydrax->getSunArea();
		return 0;
	}

	void HydraxWaterComponent::SetSunColor(const Vec3 &value)
	{
		if(m_Hydrax)
			m_Hydrax->setSunColor(OgreConvert::ToOgre(value));

	}

	Vec3 HydraxWaterComponent::GetSunColor() const
	{
		if(m_Hydrax)
			return OgreConvert::ToGASS(m_Hydrax->getSunColor());			
		return Vec3(0,0,0);
	}


	void HydraxWaterComponent::SetFoamMaxDistance(const float &FoamMaxDistance)
	{
		if(m_Hydrax)
			m_Hydrax->setFoamMaxDistance(FoamMaxDistance);
	}

	void HydraxWaterComponent::SetFoamScale(const float &FoamScale)
	{
		if(m_Hydrax)
			m_Hydrax->setFoamScale(FoamScale);

	}

	void HydraxWaterComponent::SetFoamStart(const float &FoamStart)
	{
		if(m_Hydrax)
			m_Hydrax->setFoamStart(FoamStart);
	}

	void HydraxWaterComponent::SetFoamTransparency(const float &FoamTransparency)
	{
		if(m_Hydrax)
			m_Hydrax->setFoamTransparency(FoamTransparency);
	}

	void HydraxWaterComponent::SetDepthLimit(const float &DepthLimit)

	{
		if(m_Hydrax)
			m_Hydrax->setDepthLimit(DepthLimit);
	}


	void HydraxWaterComponent::SetSmoothPower(const float &SmoothPower)
	{
		if(m_Hydrax)
			m_Hydrax->setSmoothPower(SmoothPower);
	}

	void HydraxWaterComponent::SetCausticsScale(const float &CausticsScale)
	{
		if(m_Hydrax)
			m_Hydrax->setCausticsScale(CausticsScale);
	}

	void HydraxWaterComponent::SetCausticsPower(const float &CausticsPower)
	{
		if(m_Hydrax)
			m_Hydrax->setCausticsPower(CausticsPower);
	}

	void HydraxWaterComponent::SetCausticsEnd(const float &CausticsEnd)
	{
		if(m_Hydrax)
			m_Hydrax->setCausticsEnd(CausticsEnd);
	}

	void HydraxWaterComponent::SetGodRaysExposure(const Vec3 &value)
	{
		if(m_Hydrax)
			m_Hydrax->setGodRaysExposure(OgreConvert::ToOgre(value));
	}


	void HydraxWaterComponent::SetGodRaysIntensity(const float &GodRaysIntensity)
	{
		if(m_Hydrax)
			m_Hydrax->setGodRaysIntensity(GodRaysIntensity);
	}

	void HydraxWaterComponent::SetUnderwaterCameraSwitchDelta(const float& value)
	{
		if(m_Hydrax)
			m_Hydrax->setUnderwaterCameraSwitchDelta(value);
	}

	float HydraxWaterComponent::GetFoamMaxDistance() const
	{
		if(m_Hydrax)
			return m_Hydrax->getFoamMaxDistance();
		return 0;
	}

	float HydraxWaterComponent::GetFoamScale() const
	{
		if(m_Hydrax)
			return m_Hydrax->getFoamScale();
		return 0;
	}

	float HydraxWaterComponent::GetFoamStart() const
	{
		if(m_Hydrax)
			return m_Hydrax->getFoamStart();
		return 0;
	}

	float HydraxWaterComponent::GetFoamTransparency() const
	{
		if(m_Hydrax)
			return m_Hydrax->getFoamTransparency();
		return 0;
	}

	float HydraxWaterComponent::GetDepthLimit() const
	{
		if(m_Hydrax)
			return m_Hydrax->getDepthLimit();
		return 0;
	}

	float HydraxWaterComponent::GetSmoothPower() const
	{
		if(m_Hydrax)
			return m_Hydrax->getSmoothPower();
		return 0;
	}

	float HydraxWaterComponent::GetCausticsScale() const
	{
		if(m_Hydrax)
			return m_Hydrax->getCausticsScale();
		return 0;
	}

	float HydraxWaterComponent::GetCausticsPower() const
	{
		if(m_Hydrax)
			return m_Hydrax->getCausticsPower();
		return 0;
	}

	float HydraxWaterComponent::GetCausticsEnd() const
	{
		if(m_Hydrax)
			return m_Hydrax->getCausticsEnd();
		return 0;
	}
	Vec3 HydraxWaterComponent::GetGodRaysExposure() const
	{
		if(m_Hydrax)
			return OgreConvert::ToGASS(m_Hydrax->getGodRaysExposure());
		return Vec3(0,0,0);
	}

	float HydraxWaterComponent::GetGodRaysIntensity() const
	{
		if(m_Hydrax)
			return m_Hydrax->getGodRaysIntensity();
		return 0;
	}
	float HydraxWaterComponent::GetUnderwaterCameraSwitchDelta() const
	{
		if(m_Hydrax)
			return m_Hydrax->getUnderwaterCameraSwitchDelta();
		return 0;
	}

	

	void HydraxWaterComponent::OnChangeCamera(CameraChangedEventPtr message)
	{
		if(m_Hydrax)
		{
			OgreCameraProxyPtr camera_proxy = GASS_DYNAMIC_PTR_CAST<IOgreCameraProxy>(message->GetViewport()->GetCamera());
			Ogre::Camera * cam = camera_proxy->GetOgreCamera();
			if(cam)
			{
				m_Hydrax->setCamera(cam);
				if(m_ProjectedGridGeometryModuleVertex)
					m_ProjectedGridGeometryModuleVertex->setRenderingCamera(cam);
				if(m_ProjectedGridGeometryModuleRtt)
					m_ProjectedGridGeometryModuleRtt->setRenderingCamera(cam);
			}
			
			/*Ogre::Root::getSingleton().removeFrameListener(this);
			Ogre::CompositorManager& compMgr = Ogre::CompositorManager::getSingleton();
			compMgr.removeCompositor(m_Hydrax->getViewport(),"_Hydrax_Underwater_Compositor_Name");

			m_Hydrax->remove();
			delete m_Hydrax;
			Ogre::ResourceGroupManager::getSingletonPtr()->destroyResourceGroup("Hydrax");
			m_Hydrax = NULL;
			Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
			CreateHydrax(sm,cam,cam->getViewport());*/
		}
	}

	void HydraxWaterComponent::CreateHydrax(Ogre::SceneManager* sm, Ogre::Camera* ocam, Ogre::Viewport* vp)
	{
		if(m_ResourceLocation.GetFullPath() != "")
		{
			Ogre::ResourceGroupManager *mngr = Ogre::ResourceGroupManager::getSingletonPtr();
			mngr->addResourceLocation(m_ResourceLocation.GetFullPath() ,"FileSystem","Hydrax");
		}

		m_Hydrax = new Hydrax::Hydrax(sm, ocam, vp);

		// Create our projected grid module  

		Ogre::MaterialPtr terrain_mat = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("TerrainMat"));

		if(terrain_mat.get())
			m_Hydrax->getMaterialManager()->addDepthTechnique(terrain_mat->createTechnique());


		terrain_mat = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("TerrainPageMaterial"));
		if(terrain_mat.get())
			m_Hydrax->getMaterialManager()->addDepthTechnique(terrain_mat->createTechnique());

		// Add perlin noise module to our manager
		m_Perlin = new Hydrax::Noise::Perlin();
		AddNoiseModule(m_Perlin);

		// Add fft noise module to our manager
		m_FFT = new Hydrax::Noise::FFT();
		AddNoiseModule(m_FFT);

		// Add projected grid geometry module to the manager(Vertex normals)
		m_ProjectedGridGeometryModuleVertex 
			= new Hydrax::Module::ProjectedGrid(// Hydrax parent pointer
			m_Hydrax,
			// Noise module
			m_Perlin,
			// Base plane
			Ogre::Plane(Ogre::Vector3(0,1,0), Ogre::Vector3(0,0,0)),
			// Normal mode
			Hydrax::MaterialManager::NM_VERTEX,
			// Projected grid options (Can be updated each frame -> setOptions(...))
			Hydrax::Module::ProjectedGrid::Options());
		AddGeometryModule(m_ProjectedGridGeometryModuleVertex);

		// Add projected grid geometry module to the manager(Rtt normals)
		m_ProjectedGridGeometryModuleRtt
			= new Hydrax::Module::ProjectedGrid(// Hydrax parent pointer
			m_Hydrax,
			// Noise module
			m_Perlin,
			// Base plane
			Ogre::Plane(Ogre::Vector3(0,1,0), Ogre::Vector3(0,0,0)),
			// Normal mode
			Hydrax::MaterialManager::NM_RTT,
			// Projected grid options (Can be updated each frame -> setOptions(...))
			Hydrax::Module::ProjectedGrid::Options());
		AddGeometryModule(m_ProjectedGridGeometryModuleRtt);

		// Add the simple grid geometry module to our manager(Vertex normals)
		m_SimpleGridGeometryModuleVertex
			= new Hydrax::Module::SimpleGrid(// Hydrax parent pointer
			m_Hydrax,
			// Noise module
			m_Perlin,
			// Normal mode
			Hydrax::MaterialManager::NM_VERTEX,
			// Simple grid options (Can be updated each frame -> setOptions(...))
			Hydrax::Module::SimpleGrid::Options(512, Hydrax::Size(10000,10000) ));
		AddGeometryModule(m_SimpleGridGeometryModuleVertex);

		// Add the simple grid geometry module to our manager(Rtt normals)
		m_SimpleGridGeometryModuleRtt
			= new Hydrax::Module::SimpleGrid(// Hydrax parent pointer
			m_Hydrax,
			// Noise module
			m_Perlin,
			// Normal mode
			Hydrax::MaterialManager::NM_RTT,
			// Simple grid options (Can be updated each frame -> setOptions(...))
			Hydrax::Module::SimpleGrid::Options(128, Hydrax::Size(10000,10000) ));
		AddGeometryModule(m_SimpleGridGeometryModuleRtt);

		// Add the radial grid geometry module to our manager(Vertex normals)
		m_RadialGridGeometryModuleVertex
			= new Hydrax::Module::RadialGrid(// Hydrax parent pointer
			m_Hydrax,
			// Noise module
			m_Perlin,
			// Normal mode
			Hydrax::MaterialManager::NM_VERTEX,
			// Simple grid options (Can be updated each frame -> setOptions(...))
			Hydrax::Module::RadialGrid::Options(250, 250, 6000  ));
		AddGeometryModule(m_RadialGridGeometryModuleVertex);

		// Add the radial grid geometry module to our manager(Rtt normals)
		m_RadialGridGeometryModuleRtt
			= new Hydrax::Module::RadialGrid(// Hydrax parent pointer
			m_Hydrax,
			// Noise module
			m_Perlin,
			// Normal mode
			Hydrax::MaterialManager::NM_RTT,
			// Simple grid options (Can be updated each frame -> setOptions(...))
			Hydrax::Module::RadialGrid::Options(80, 10, 6000 ));
		AddGeometryModule(m_RadialGridGeometryModuleRtt);


		if(m_ConfigurationFile != "")
		{
			SetConfigurationFile(m_ConfigurationFile);
		}
		else
		{
			//force default load
			Ogre::ConfigFile CfgFile;
			CfgFile.load(Ogre::ResourceGroupManager::getSingleton().openResource("DefaultEditorWater.hdx", "Hydrax"));
			SetActiveModule(CfgFile.getSetting("Module"));
			SetActiveNoise(CfgFile.getSetting("Perlin"));
			m_Hydrax->loadCfg("DefaultEditorWater.hdx");
			// Create water
			m_Hydrax->create();
		}
	}


	void HydraxWaterComponent::SetActiveNoise(const std::string &module) 
	{
		m_ActiveNoise = module;
		if(m_Hydrax && m_Hydrax->getModule() && GetNoiseModule(module))
			m_Hydrax->getModule()->setNoise(GetNoiseModule(module),m_Hydrax->getGPUNormalMapManager(), false);
	}

	std::string HydraxWaterComponent::GetActiveNoise() const 
	{
		return m_ActiveNoise;
	}


	void HydraxWaterComponent::SetActiveModule(const std::string &module) 
	{
		m_ActiveModule = module;
		if(m_Hydrax && GetGeometryModule(module))
			m_Hydrax->setModule(GetGeometryModule(module), false);
	}

	std::string HydraxWaterComponent::GetActiveModule() const 
	{
		return m_ActiveModule;
	}

	void HydraxWaterComponent::SetConfigurationFile(const std::string &cfg_file) 
	{
		m_ConfigurationFile = cfg_file;
		
		std::string filename = FileUtils::GetFilename(cfg_file);
		std::string path = FileUtils::RemoveFilename(cfg_file);

		if(m_Hydrax && m_ConfigurationFile != "")
		{
			if (path != "" && path != filename && !Ogre::ResourceGroupManager::getSingleton().resourceExists("Hydrax", filename))
			{
				try
				{
					Ogre::ResourceGroupManager::getSingleton().removeResourceLocation(path, "Hydrax");
				}
				catch(...)
				{
				}

				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, "FileSystem", "Hydrax");
			}

			Ogre::ConfigFile CfgFile;
			//std::fstream fstr(cfg_file.c_str(), std::ios::in|std::ios::binary);
			//Ogre::DataStreamPtr stream = Ogre::DataStreamPtr(OGRE_NEW Ogre::FileStreamDataStream(&fstr, false));
			//CfgFile.load(stream);
			CfgFile.load(Ogre::ResourceGroupManager::getSingleton().openResource(filename, "Hydrax"));
			SetActiveModule(CfgFile.getSetting("Module"));
			//m_Hydrax->setModule(GetGeometryModule(CfgFile.getSetting("Module")), false);
			SetActiveNoise(CfgFile.getSetting("Noise"));

			m_Hydrax->loadCfg(filename);
			m_Hydrax->create();
		}
	}

	bool  HydraxWaterComponent::frameStarted(const Ogre::FrameEvent& /*evt*/)
	{
		double c_time = SimEngine::Get().GetSimulationTime();
		static double prev_time = 0;
		if(prev_time == 0)
			m_Hydrax->update(0.1f);
		else
			m_Hydrax->update(static_cast<float>(c_time - prev_time));

		prev_time = c_time;
		return true;
	}


	void HydraxWaterComponent::AddGeometryModule(Hydrax::Module::Module* m)
	{
		mModules.push_back(m);
	}

	Hydrax::Module::Module* HydraxWaterComponent::GetGeometryModule(const Ogre::String& Name)
	{
		std::vector<Hydrax::Module::Module*>::iterator It;

		for (It = mModules.begin(); It != mModules.end(); It++)
		{
			if ((*It)->getName() == Name)
			{
				return (*It);
			}
		}

		return static_cast<Hydrax::Module::Module*>(NULL);
	}

	void HydraxWaterComponent::RemoveGeometryModule(Hydrax::Module::Module* m)
	{
		std::vector<Hydrax::Module::Module*>::iterator It;

		for (It = mModules.begin(); It != mModules.end(); It++)
		{
			if ((*It) == m)
			{
				delete (*It);
				mModules.erase(It);

				return;
			}
		}
	}

	void HydraxWaterComponent::RemoveGeometryModules()
	{
		std::vector<Hydrax::Module::Module*>::iterator It;

		for (It = mModules.begin(); It != mModules.end(); It++)
		{
			delete (*It);
			
		}
		mModules.clear();
	}

	void HydraxWaterComponent::AddNoiseModule(Hydrax::Noise::Noise* n)
	{
		mNoises.push_back(n);
	}

	Hydrax::Noise::Noise* HydraxWaterComponent::GetNoiseModule(const Ogre::String& Name)
	{
		std::vector<Hydrax::Noise::Noise*>::iterator It;

		for (It = mNoises.begin(); It != mNoises.end(); It++)
		{
			if ((*It)->getName() == Name)
			{
				return (*It);
			}
		}

		return static_cast<Hydrax::Noise::Noise*>(NULL);
	}

	void HydraxWaterComponent::RemoveNoiseModule(Hydrax::Noise::Noise* n)
	{
		std::vector<Hydrax::Noise::Noise*>::iterator It;

		for (It = mNoises.begin(); It != mNoises.end(); It++)
		{
			if ((*It) == n)
			{
				delete (*It);
				mNoises.erase(It);

				return;
			}
		}
	}

	void HydraxWaterComponent::RemoveNoiseModules()
	{
		std::vector<Hydrax::Noise::Noise*>::iterator It;

		for (It = mNoises.begin(); It != mNoises.end(); It++)
		{
			delete (*It);
			
		}
		mNoises.clear();
	}
}


