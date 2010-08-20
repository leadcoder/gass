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

#include "HydraxWaterComponent.h"
#include "HydraxRTTListener.h"
#include "Plugins/Ogre/OgreConvert.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/SimEngine.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include <Ogre.h>

#include "Hydrax/Hydrax.h"
#include "Hydrax/Noise/Perlin/Perlin.h"
#include "Hydrax/Noise/FFT/FFT.h"
#include "Hydrax/Modules/ProjectedGrid/ProjectedGrid.h"
#include "Hydrax/Modules/SimpleGrid/SimpleGrid.h"
#include "Hydrax/Modules/RadialGrid/RadialGrid.h"


namespace GASS
{

	HydraxWaterComponent::HydraxWaterComponent(void) : m_Hydrax(NULL) , m_Rot(0,0,0), m_Perlin(NULL), m_FFT(NULL), m_ProjectedGridGeometryModuleVertex(NULL),m_Target(NULL)
	{

	}

	HydraxWaterComponent::~HydraxWaterComponent(void)
	{

	}

	void HydraxWaterComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("HydraxWaterComponent",new Creator<HydraxWaterComponent, IComponent>);
		RegisterProperty<std::string>("ConfigurationFile", &HydraxWaterComponent::GetConfigurationFile, &HydraxWaterComponent::SetConfigurationFile);
		RegisterProperty<Vec3>("Position", &HydraxWaterComponent::GetPosition, &HydraxWaterComponent::SetPosition);
		RegisterProperty<Vec3>("Rotation", &HydraxWaterComponent::GetRotation, &HydraxWaterComponent::SetRotation);
		RegisterProperty<Float>("GlobalTransparency", &HydraxWaterComponent::GetGlobalTransparency, &HydraxWaterComponent::SetGlobalTransparency);
		RegisterProperty<Float>("FullReflectionDistance", &HydraxWaterComponent::GetFullReflectionDistance, &HydraxWaterComponent::SetFullReflectionDistance);
		RegisterProperty<Vec3>("WaterColor", &HydraxWaterComponent::GetWaterColor, &HydraxWaterComponent::SetWaterColor);
		RegisterProperty<Float>("NormalDistortion", &HydraxWaterComponent::GetNormalDistortion, &HydraxWaterComponent::SetNormalDistortion);
		RegisterProperty<Vec3>("SunPosition", &HydraxWaterComponent::GetSunPosition, &HydraxWaterComponent::SetSunPosition);
		RegisterProperty<Float>("SunStrength", &HydraxWaterComponent::GetSunStrength, &HydraxWaterComponent::SetSunStrength);
		RegisterProperty<Float>("SunArea", &HydraxWaterComponent::GetSunArea, &HydraxWaterComponent::SetSunArea);
		RegisterProperty<Vec3>("SunColor", &HydraxWaterComponent::GetSunColor, &HydraxWaterComponent::SetSunColor);
		RegisterProperty<Float>("FoamMaxDistance", &HydraxWaterComponent::GetFoamMaxDistance, &HydraxWaterComponent::SetFoamMaxDistance);
		RegisterProperty<Float>("FoamScale", &HydraxWaterComponent::GetFoamScale, &HydraxWaterComponent::SetFoamScale);
		RegisterProperty<Float>("FoamStart", &HydraxWaterComponent::GetFoamStart, &HydraxWaterComponent::SetFoamStart);
		RegisterProperty<Float>("FoamTransparency", &HydraxWaterComponent::GetFoamTransparency, &HydraxWaterComponent::SetFoamTransparency);
		RegisterProperty<Float>("DepthLimit", &HydraxWaterComponent::GetDepthLimit, &HydraxWaterComponent::SetDepthLimit);
		RegisterProperty<Float>("SmoothPower", &HydraxWaterComponent::GetSmoothPower, &HydraxWaterComponent::SetSmoothPower);
		RegisterProperty<Float>("CausticsScale", &HydraxWaterComponent::GetCausticsScale, &HydraxWaterComponent::SetCausticsScale);
		RegisterProperty<Float>("CausticsPower", &HydraxWaterComponent::GetCausticsPower, &HydraxWaterComponent::SetCausticsPower);
		RegisterProperty<Float>("CausticsEnd", &HydraxWaterComponent::GetCausticsEnd, &HydraxWaterComponent::SetCausticsEnd);
		RegisterProperty<Vec3>("GodRaysExposure", &HydraxWaterComponent::GetGodRaysExposure, &HydraxWaterComponent::SetGodRaysExposure);
		RegisterProperty<Float>("GodRaysIntensity", &HydraxWaterComponent::GetGodRaysIntensity, &HydraxWaterComponent::SetGodRaysIntensity);
		RegisterProperty<Float>("UnderwaterCameraSwitchDelta", &HydraxWaterComponent::GetUnderwaterCameraSwitchDelta, &HydraxWaterComponent::SetUnderwaterCameraSwitchDelta);
		RegisterProperty<Float>("PerlinScale", &HydraxWaterComponent::GetPerlinScale, &HydraxWaterComponent::SetPerlinScale);
		RegisterProperty<Float>("PerlinSpeed", &HydraxWaterComponent::GetPerlinSpeed, &HydraxWaterComponent::SetPerlinSpeed);
		RegisterProperty<Float>("FFTScale", &HydraxWaterComponent::GetFFTScale, &HydraxWaterComponent::SetFFTScale);
		RegisterProperty<Float>("FFTSpeed", &HydraxWaterComponent::GetFFTSpeed, &HydraxWaterComponent::SetFFTSpeed);
		RegisterProperty<Float>("Strength", &HydraxWaterComponent::GetStrength, &HydraxWaterComponent::SetStrength);
		RegisterProperty<Float>("Elevation", &HydraxWaterComponent::GetElevation, &HydraxWaterComponent::SetElevation);
		RegisterProperty<std::string>("SaveConfiguration", &HydraxWaterComponent::GetSave, &HydraxWaterComponent::SetSave);
		RegisterProperty<Float>("PlanesError", &HydraxWaterComponent::GetPlanesError, &HydraxWaterComponent::SetPlanesError);
	}

	void HydraxWaterComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(HydraxWaterComponent::OnLoad,LoadGFXComponentsMessage,2));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HydraxWaterComponent::OnUnload,UnloadComponentsMessage,0));
	}

	void HydraxWaterComponent::SetSave(const std::string &value)
	{
		if(m_Hydrax)
		{
			m_ConfigurationFile = value;
			std::string filename = Misc::GetFilename(value);
			std::string path = Misc::RemoveFilename(value);

			m_Hydrax->saveCfg(filename, path);
		}
	}

	std::string HydraxWaterComponent::GetSave() const 
	{
		return m_ConfigurationFile;
	}

	void HydraxWaterComponent::SetPlanesError(const Float &value)
	{
		if(m_Hydrax)
		{
			m_Hydrax->setPlanesError(value);
		}
	}


	Float HydraxWaterComponent::GetPlanesError() const 
	{
		if(m_Hydrax)
		{
			return m_Hydrax->getPlanesError();
		}
		return 0;
	}


	void HydraxWaterComponent::SetGPUStrength(const Float &value)
	{
		if(m_Hydrax)
		{
	//		m_Hydrax->_setStrength(value);
		}
	}

	Float HydraxWaterComponent::GetGPUStrength() const 
	{
		return 0;
	}

	void HydraxWaterComponent::SetStrength(const Float &value)
	{
		if(m_ProjectedGridGeometryModuleVertex)
		{
			Hydrax::Module::ProjectedGrid::Options  options = m_ProjectedGridGeometryModuleVertex->getOptions();
			options.Strength = value;
			m_ProjectedGridGeometryModuleVertex->setOptions(options);
		}
	}

	Float HydraxWaterComponent::GetStrength() const 
	{
		if(m_ProjectedGridGeometryModuleVertex)
		{
			Hydrax::Module::ProjectedGrid::Options  options = m_ProjectedGridGeometryModuleVertex->getOptions();
			return options.Strength;
		}
		return 0;
	}

	void HydraxWaterComponent::SetElevation(const Float &value)
	{
		if(m_ProjectedGridGeometryModuleVertex)
		{
			Hydrax::Module::ProjectedGrid::Options  options = m_ProjectedGridGeometryModuleVertex->getOptions();
			options.Elevation = value;
			m_ProjectedGridGeometryModuleVertex->setOptions(options);
		}
	}


	Float HydraxWaterComponent::GetElevation() const 
	{
		if(m_ProjectedGridGeometryModuleVertex)
		{
			Hydrax::Module::ProjectedGrid::Options  options = m_ProjectedGridGeometryModuleVertex->getOptions();
			return options.Elevation;
		}
		return 0;
	}

	void HydraxWaterComponent::SetPerlinScale(const Float &value)
	{
		if(m_Perlin)
		{
			Hydrax::Noise::Perlin::Options PN_Options = m_Perlin->getOptions();
			PN_Options.Scale = value;
			m_Perlin->setOptions(PN_Options);
		}
	}




	Float HydraxWaterComponent::GetPerlinScale() const
	{
		if(m_Perlin)
		{
			Hydrax::Noise::Perlin::Options PN_Options = m_Perlin->getOptions();
			return PN_Options.Scale;
		}
		return 0;
	}


	void HydraxWaterComponent::SetPerlinSpeed(const Float &value)
	{
		if(m_Perlin)
		{
			Hydrax::Noise::Perlin::Options PN_Options = m_Perlin->getOptions();
			PN_Options.Animspeed = value;
			m_Perlin->setOptions(PN_Options);
		}
	}

	Float HydraxWaterComponent::GetPerlinSpeed() const
	{
		if(m_Perlin)
		{
			Hydrax::Noise::Perlin::Options PN_Options = m_Perlin->getOptions();
			return PN_Options.Animspeed;
		}
		return 0;
	}


	void HydraxWaterComponent::SetFFTScale(const Float &value)
	{
		if(m_FFT)
		{
			Hydrax::Noise::FFT::Options FFT_Options = m_FFT->getOptions();
			FFT_Options.Scale = value;
			m_FFT->setOptions(FFT_Options);
		}
	}

	Float HydraxWaterComponent::GetFFTScale() const
	{
		if(m_FFT)
		{
			Hydrax::Noise::FFT::Options FFT_Options = m_FFT->getOptions();
			return FFT_Options.Scale;
		}
		return 0;
	}

	void HydraxWaterComponent::SetFFTSpeed(const Float &value)
	{
		if(m_FFT)
		{
			Hydrax::Noise::FFT::Options FFT_Options = m_FFT->getOptions();
			FFT_Options.AnimationSpeed = value;
			m_FFT->setOptions(FFT_Options);
		}
	}

	Float HydraxWaterComponent::GetFFTSpeed() const
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
			m_Hydrax->setPosition(Convert::ToOgre(value));
	}

	Vec3 HydraxWaterComponent::GetPosition() const
	{
		if(m_Hydrax)
			return Convert::ToGASS(m_Hydrax->getPosition());
		return Vec3(0,0,0);
	}

	void HydraxWaterComponent::SetRotation(const Vec3 &value)
	{
		m_Rot = value;
		if(m_Hydrax)
			m_Hydrax->rotate(Convert::ToOgre(Quaternion(value)));
	}

	Vec3 HydraxWaterComponent::GetRotation() const
	{
		return m_Rot;
	}

	void HydraxWaterComponent::SetGlobalTransparency(Float value)
	{
		if(m_Hydrax)
			m_Hydrax->setGlobalTransparency(value);
	}

	Float HydraxWaterComponent::GetGlobalTransparency() const
	{
		if(m_Hydrax)
			return m_Hydrax->getGlobalTransparency();
		return 0;
	}
	void HydraxWaterComponent::SetFullReflectionDistance(Float value)
	{
		if(m_Hydrax)
			m_Hydrax->setFullReflectionDistance(value);
	}

	Float HydraxWaterComponent::GetFullReflectionDistance() const
	{
		if(m_Hydrax)
			return m_Hydrax->getFullReflectionDistance();
		return 0;
	}

	void HydraxWaterComponent::SetWaterColor(const Vec3& value)
	{
		if(m_Hydrax)
			m_Hydrax->setWaterColor(Convert::ToOgre(value));
	}
	Vec3 HydraxWaterComponent::GetWaterColor() const
	{
		if(m_Hydrax)
			return Convert::ToGASS(m_Hydrax->getWaterColor());			
		return Vec3(0,0,0);
	}
	void HydraxWaterComponent::SetNormalDistortion(Float value)
	{
		if(m_Hydrax)
			m_Hydrax->setNormalDistortion(value);			
	}

	Float HydraxWaterComponent::GetNormalDistortion() const
	{
		if(m_Hydrax)
			return m_Hydrax->getNormalDistortion();
		return 0;
	}

	void HydraxWaterComponent::SetSunPosition(const Vec3 & value)
	{
		if(m_Hydrax)
			m_Hydrax->setSunPosition(Convert::ToOgre(value));
	}

	Vec3 HydraxWaterComponent::GetSunPosition() const
	{
		if(m_Hydrax)
			return Convert::ToGASS(m_Hydrax->getSunPosition());			
		return Vec3(0,0,0);

	}

	void HydraxWaterComponent::SetSunStrength(Float value)
	{
		if(m_Hydrax)
			m_Hydrax->setSunStrength(value);			
	}

	Float HydraxWaterComponent::GetSunStrength() const
	{
		if(m_Hydrax)
			return m_Hydrax->getSunStrength();
		return 0;
	}

	void HydraxWaterComponent::SetSunArea(Float value)
	{
		if(m_Hydrax)
			m_Hydrax->setSunArea(value);			

	}

	Float HydraxWaterComponent::GetSunArea() const
	{
		if(m_Hydrax)
			return m_Hydrax->getSunArea();
		return 0;
	}

	void HydraxWaterComponent::SetSunColor(const Vec3 &value)
	{
		if(m_Hydrax)
			m_Hydrax->setSunColor(Convert::ToOgre(value));

	}

	Vec3 HydraxWaterComponent::GetSunColor() const
	{
		if(m_Hydrax)
			return Convert::ToGASS(m_Hydrax->getSunColor());			
		return Vec3(0,0,0);
	}


	void HydraxWaterComponent::SetFoamMaxDistance(const Float &FoamMaxDistance)
	{
		if(m_Hydrax)
			m_Hydrax->setFoamMaxDistance(FoamMaxDistance);
	}

	void HydraxWaterComponent::SetFoamScale(const Float &FoamScale)
	{
		if(m_Hydrax)
			m_Hydrax->setFoamScale(FoamScale);

	}

	void HydraxWaterComponent::SetFoamStart(const Float &FoamStart)
	{
		if(m_Hydrax)
			m_Hydrax->setFoamScale(FoamStart);
	}

	void HydraxWaterComponent::SetFoamTransparency(const Float &FoamTransparency)
	{
		if(m_Hydrax)
			m_Hydrax->setFoamTransparency(FoamTransparency);
	}

	void HydraxWaterComponent::SetDepthLimit(const Float &DepthLimit)

	{
		if(m_Hydrax)
			m_Hydrax->setDepthLimit(DepthLimit);
	}


	void HydraxWaterComponent::SetSmoothPower(const Float &SmoothPower)
	{
		if(m_Hydrax)
			m_Hydrax->setSmoothPower(SmoothPower);
	}

	void HydraxWaterComponent::SetCausticsScale(const Float &CausticsScale)
	{
		if(m_Hydrax)
			m_Hydrax->setCausticsScale(CausticsScale);
	}

	void HydraxWaterComponent::SetCausticsPower(const Float &CausticsPower)
	{
		if(m_Hydrax)
			m_Hydrax->setCausticsPower(CausticsPower);
	}

	void HydraxWaterComponent::SetCausticsEnd(const Float &CausticsEnd)
	{
		if(m_Hydrax)
			m_Hydrax->setCausticsEnd(CausticsEnd);
	}

	void HydraxWaterComponent::SetGodRaysExposure(const Vec3 &value)
	{
		if(m_Hydrax)
			m_Hydrax->setGodRaysExposure(Convert::ToOgre(value));
	}


	void HydraxWaterComponent::SetGodRaysIntensity(const Float &GodRaysIntensity)
	{
		if(m_Hydrax)
			m_Hydrax->setGodRaysIntensity(GodRaysIntensity);
	}

	void HydraxWaterComponent::SetUnderwaterCameraSwitchDelta(const Float& value)
	{
		if(m_Hydrax)
			m_Hydrax->setUnderwaterCameraSwitchDelta(value);
	}

	Float HydraxWaterComponent::GetFoamMaxDistance() const
	{
		if(m_Hydrax)
			return m_Hydrax->getFoamMaxDistance();
		return 0;
	}

	Float HydraxWaterComponent::GetFoamScale() const
	{
		if(m_Hydrax)
			return m_Hydrax->getFoamScale();
		return 0;
	}

	Float HydraxWaterComponent::GetFoamStart() const
	{
		if(m_Hydrax)
			return m_Hydrax->getFoamStart();
		return 0;
	}

	Float HydraxWaterComponent::GetFoamTransparency() const
	{
		if(m_Hydrax)
			return m_Hydrax->getFoamTransparency();
		return 0;
	}

	Float HydraxWaterComponent::GetDepthLimit() const
	{
		if(m_Hydrax)
			return m_Hydrax->getDepthLimit();
		return 0;
	}

	Float HydraxWaterComponent::GetSmoothPower() const
	{
		if(m_Hydrax)
			return m_Hydrax->getSmoothPower();
		return 0;
	}

	Float HydraxWaterComponent::GetCausticsScale() const
	{
		if(m_Hydrax)
			return m_Hydrax->getCausticsScale();
		return 0;
	}

	Float HydraxWaterComponent::GetCausticsPower() const
	{
		if(m_Hydrax)
			return m_Hydrax->getCausticsPower();
		return 0;
	}

	Float HydraxWaterComponent::GetCausticsEnd() const
	{
		if(m_Hydrax)
			return m_Hydrax->getCausticsEnd();
		return 0;
	}
	Vec3 HydraxWaterComponent::GetGodRaysExposure() const
	{
		if(m_Hydrax)
			return Convert::ToGASS(m_Hydrax->getGodRaysExposure());
		return Vec3(0,0,0);
	}

	Float HydraxWaterComponent::GetGodRaysIntensity() const
	{
		if(m_Hydrax)
			return m_Hydrax->getGodRaysIntensity();
		return 0;
	}
	Float HydraxWaterComponent::GetUnderwaterCameraSwitchDelta() const
	{
		if(m_Hydrax)
			return m_Hydrax->getUnderwaterCameraSwitchDelta();
		return 0;
	}

	void HydraxWaterComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		Ogre::Root::getSingleton().removeFrameListener(this);
		m_Hydrax->remove();
		delete m_Hydrax;
	}

	void HydraxWaterComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		Ogre::Camera* ocam = sm->getCameraIterator().getNext();
		
		Ogre::Root::getSingleton().addFrameListener(this);
		
		m_Hydrax = new Hydrax::Hydrax(sm, ocam, ocam->getViewport());
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
			new Hydrax::Noise::Perlin(),
			// Base plane
			Ogre::Plane(Ogre::Vector3(0,1,0), Ogre::Vector3(0,0,0)),
			// Normal mode
			Hydrax::MaterialManager::NM_VERTEX,
			// Projected grid options (Can be updated each frame -> setOptions(...))
			Hydrax::Module::ProjectedGrid::Options());
		AddGeometryModule(m_ProjectedGridGeometryModuleVertex);

		// Add projected grid geometry module to the manager(Rtt normals)
		Hydrax::Module::ProjectedGrid *ProjectedGridGeometryModuleRtt
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
		AddGeometryModule(ProjectedGridGeometryModuleRtt);

		// Add the simple grid geometry module to our manager(Vertex normals)
		Hydrax::Module::SimpleGrid *SimpleGridGeometryModuleVertex
			= new Hydrax::Module::SimpleGrid(// Hydrax parent pointer
			m_Hydrax,
			// Noise module
			new Hydrax::Noise::Perlin(),
			// Normal mode
			Hydrax::MaterialManager::NM_VERTEX,
			// Simple grid options (Can be updated each frame -> setOptions(...))
			Hydrax::Module::SimpleGrid::Options(512, Hydrax::Size(10000,10000) ));
		AddGeometryModule(SimpleGridGeometryModuleVertex);

		// Add the simple grid geometry module to our manager(Rtt normals)
		Hydrax::Module::SimpleGrid *SimpleGridGeometryModuleRtt
			= new Hydrax::Module::SimpleGrid(// Hydrax parent pointer
			m_Hydrax,
			// Noise module
			new Hydrax::Noise::Perlin(),
			// Normal mode
			Hydrax::MaterialManager::NM_RTT,
			// Simple grid options (Can be updated each frame -> setOptions(...))
			Hydrax::Module::SimpleGrid::Options(128, Hydrax::Size(10000,10000) ));
		AddGeometryModule(SimpleGridGeometryModuleRtt);

		// Add the radial grid geometry module to our manager(Vertex normals)
		Hydrax::Module::RadialGrid *RadialGridGeometryModuleVertex
			= new Hydrax::Module::RadialGrid(// Hydrax parent pointer
			m_Hydrax,
			// Noise module
			new Hydrax::Noise::Perlin(),
			// Normal mode
			Hydrax::MaterialManager::NM_VERTEX,
			// Simple grid options (Can be updated each frame -> setOptions(...))
			Hydrax::Module::RadialGrid::Options(250, 250, 6000  ));
		AddGeometryModule(RadialGridGeometryModuleVertex);

		// Add the radial grid geometry module to our manager(Rtt normals)
		Hydrax::Module::RadialGrid *RadialGridGeometryModuleRtt
			= new Hydrax::Module::RadialGrid(// Hydrax parent pointer
			m_Hydrax,
			// Noise module
			new Hydrax::Noise::Perlin(),
			// Normal mode
			Hydrax::MaterialManager::NM_RTT,
			// Simple grid options (Can be updated each frame -> setOptions(...))
			Hydrax::Module::RadialGrid::Options(80, 10, 6000 ));
		AddGeometryModule(RadialGridGeometryModuleRtt);


		// Set our module
		m_Hydrax->setModule(static_cast<Hydrax::Module::Module*>(ProjectedGridGeometryModuleRtt));
		// Load all parameters from config file
		// Remarks: The config file must be in Hydrax resource group.
		// All parameters can be set/updated directly by code(Like previous versions),
		// but due to the high number of customizable parameters, since 0.4 version, Hydrax allows save/load config files.
		// Create water
		//m_Hydrax->create();

		m_Hydrax->loadCfg("DefaultEditorWater.hdx");
		// Create water
		m_Hydrax->create();

		if(m_ConfigurationFile != "")
		{
			SetConfigurationFile(m_ConfigurationFile);
		}
	}

	void HydraxWaterComponent::SetConfigurationFile(const std::string &cfg_file) 
	{
		m_ConfigurationFile = cfg_file;
		std::string filename = Misc::GetFilename(cfg_file);
		std::string path = Misc::RemoveFilename(cfg_file);

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

		if(m_Hydrax && m_ConfigurationFile != "")
		{
			Ogre::ConfigFile CfgFile;
			CfgFile.load(Ogre::ResourceGroupManager::getSingleton().openResource(filename, "Hydrax"));
			m_Hydrax->setModule(GetGeometryModule(CfgFile.getSetting("Module")), false);
			m_Hydrax->getModule()->setNoise(GetNoiseModule(CfgFile.getSetting("Noise")), m_Hydrax->getGPUNormalMapManager(), false);
			m_Hydrax->loadCfg(filename);
			m_Hydrax->create();
		}
	}

	bool  HydraxWaterComponent::frameStarted(const Ogre::FrameEvent& evt)
	{
		double c_time = SimEngine::Get().GetTime();
		static double prev_time = 0;
		if(prev_time == 0)
			m_Hydrax->update(0.1);
		else
			m_Hydrax->update(c_time - prev_time);

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


