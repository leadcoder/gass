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
#ifndef HYDRAX_WATER_COMPONENT_H
#define HYDRAX_WATER_COMPONENT_H

#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include <OgreFrameListener.h>


namespace Hydrax
{
	class Hydrax;

	namespace Module
	{
		class Module;
		class ProjectedGrid;
		class SimpleGrid;
		class RadialGrid;
	}


	namespace Noise
	{
		class Noise;
		class Perlin;
		class FFT;
	}
}

namespace GASS
{

	class HydraxWaterComponent : public Reflection<HydraxWaterComponent,BaseSceneComponent> , public Ogre::FrameListener
	{
	public:
		HydraxWaterComponent(void);
		~HydraxWaterComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		Hydrax::Hydrax* GetHydrax() const {return m_Hydrax;}
	protected:
		void OnChangeCamera(CameraChangedNotifyMessagePtr message);
		void CreateHydrax(Ogre::SceneManager* sm, Ogre::Camera* ocam, Ogre::Viewport* vp);
		bool frameStarted(const Ogre::FrameEvent& evt);
		void SetConfigurationFile(const std::string &cfg_file);
		std::string GetConfigurationFile() const {return m_ConfigurationFile;}
		void SetPosition(const Vec3 &);
		Vec3 GetPosition() const;
		void SetRotation(const Vec3 &);
		Vec3 GetRotation() const;
		void SetGlobalTransparency(Float);
		Float GetGlobalTransparency() const;
		void SetFullReflectionDistance(Float);
		Float GetFullReflectionDistance() const;
		void SetWaterColor(const Vec3&);
		Vec3 GetWaterColor() const;
		void SetNormalDistortion(Float);
		Float GetNormalDistortion() const;
		void SetSunPosition(const Vec3 &);
		Vec3 GetSunPosition() const;
		void SetSunStrength(Float);
		Float GetSunStrength() const;
		void SetSunArea(Float);
		Float GetSunArea() const;
		void SetSunColor(const Vec3 &);
		Vec3 GetSunColor() const;


		/** Set foam max distance
		@param FoamMaxDistance Foam max distance
		*/
		void SetFoamMaxDistance(const Float &FoamMaxDistance);

		/** Set foam scale
		@param FoamScale Foam scale
		*/
		void SetFoamScale(const Float &FoamScale);

		/** Set foam start
		@param FoamStart Foam start
		*/
		void SetFoamStart(const Float &FoamStart);

		/** Set foam transparency
		@param FoamTransparency Foam transparency
		*/
		void SetFoamTransparency(const Float &FoamTransparency);

		/** Set depth limit
		@param DepthLimit Depth limit
		*/
		void SetDepthLimit(const Float &DepthLimit);

		/** Set smooth power
		@param SmoothPower Smooth power
		@remarks Less values more transition distance, hight values short transition values, 1-50 range(aprox.)
		*/
		void SetSmoothPower(const Float &SmoothPower);

		/** Set caustics scale
		@param CausticsScale Caustics scale
		*/
		void SetCausticsScale(const Float &CausticsScale);

		/** Set caustics power
		@param CausticsPower Caustics power
		*/
		void SetCausticsPower(const Float &CausticsPower);

		/** Set caustics end
		@param CausticsEnd Caustics end
		*/
		void SetCausticsEnd(const Float &CausticsEnd);

		/** Set god rays exposure
		@param GodRaysExposure God rays exposure
		*/
		void SetGodRaysExposure(const Vec3 &);


		/** Set god rays intensity
		@param GodRaysIntensity God rays intensity
		*/
		void SetGodRaysIntensity(const Float &GodRaysIntensity);

		/** Set the y-displacement under the water needed to change between underwater and overwater mode
		@param UnderwaterCameraSwitchDelta Underwater camera switch delta factor
		@remarks Useful to get a nice underwater-overwater transition, it depends of the world scale
		*/
		void SetUnderwaterCameraSwitchDelta(const Float&);


		/** Get foam max distance
		@return Foam max distance
		*/
		Float GetFoamMaxDistance() const;

		/** Get foam scale
		@return Foam scale
		*/
		Float GetFoamScale() const;

		/** Get foam start
		@return Foam start
		*/
		Float GetFoamStart() const;

		/** Get foam transparency
		@return Foam scale
		*/
		Float GetFoamTransparency() const;

		/** Get depth limit
		@return Depth limit
		*/
		Float GetDepthLimit() const;

		/** Get smooth power
		@return Smooth power
		*/
		Float GetSmoothPower() const;

		/** Get caustics scale
		@return Caustics scale
		*/
		Float GetCausticsScale() const;

		/** Get caustics power
		@return Caustics power
		*/
		Float GetCausticsPower() const;

		/** Get caustics end
		@return Caustics end
		*/
		Float GetCausticsEnd() const;

		/** Get God rays exposure factors
		@return God rays exposure factors
		*/
		Vec3 GetGodRaysExposure() const;

		/** Get God rays intensity
		@return God rays intensity
		*/
		Float GetGodRaysIntensity() const;

		/** Get the y-displacement under the water needed to change between underwater and overwater mode
		@return Underwater camera switch delta
		@remarks Useful to get a nice underwater-overwater transition, it depends of the world scale
		*/
		Float GetUnderwaterCameraSwitchDelta() const;



		

		/** Add geometry module to the manager
		@m Hydrax::Module::Module*
		*/
		void AddGeometryModule(Hydrax::Module::Module* m);

		/** Remove geometry module from manager
		@m Hydrax::Module::Module*
		*/
		void RemoveGeometryModule(Hydrax::Module::Module* m);

		/** Remove all geometry modules
		*/
		void RemoveGeometryModules();

		/** Add noise module to the manager
		@n Hydrax::Noise::Noise*
		*/
		void AddNoiseModule(Hydrax::Noise::Noise* n);

		/** Remove noise module from manager
		@n Hydrax::Noise::Noise*
		*/
		void RemoveNoiseModule(Hydrax::Noise::Noise* n);

		/** Remove all noise modules
		*/
		void RemoveNoiseModules();


		/** Get geometry module 
		@Name Geometry module name
		@return Geometry module, NULL if the given name isn't in the list
		*/
		Hydrax::Module::Module* GetGeometryModule(const Ogre::String& Name);

		/** Get noise module 
		@Name Noise module name
		@return Noise module, NULL if the given name isn't in the list
		*/
		Hydrax::Noise::Noise* GetNoiseModule(const Ogre::String& Name);


		void SetPerlinScale(const Float &value);
		Float GetPerlinScale() const;
		void SetPerlinSpeed(const Float &value);
		Float GetPerlinSpeed() const;
		void SetFFTScale(const Float &value);
		Float GetFFTScale() const;
		void SetFFTSpeed(const Float &value);
		Float GetFFTSpeed() const;
		void SetStrength(const Float &value);
		Float GetStrength() const;
		void SetElevation(const Float &value);
		Float GetElevation() const;

		void SetSave(const std::string &value);
		std::string GetSave() const;
		void SetPlanesError(const Float &value);
		Float GetPlanesError() const;
		void SetGPUStrength(const Float &value);
		Float GetGPUStrength() const;
		FilePath GetResourceLocation() const;
		void SetResourceLocation(const FilePath &value);
		void SetActiveModule(const std::string &module);
		std::string GetActiveModule() const;

		void SetActiveNoise(const std::string &module);
		std::string GetActiveNoise() const;
	
	private:
		std::string m_ConfigurationFile;
		Hydrax::Hydrax *m_Hydrax;
		Vec3 m_Rot;

		// Our Hydrax geometry modules vector
		std::vector<Hydrax::Module::Module*> mModules;
		/// Our Hydrax noise modules vector
		std::vector<Hydrax::Noise::Noise*> mNoises;

		Hydrax::Noise::Perlin* m_Perlin;
		Hydrax::Noise::FFT* m_FFT;
		Hydrax::Module::ProjectedGrid *m_ProjectedGridGeometryModuleVertex; 
		Hydrax::Module::ProjectedGrid *m_ProjectedGridGeometryModuleRtt;
		Hydrax::Module::SimpleGrid *m_SimpleGridGeometryModuleVertex;
		Hydrax::Module::SimpleGrid *m_SimpleGridGeometryModuleRtt;
		Hydrax::Module::RadialGrid *m_RadialGridGeometryModuleVertex;
		Hydrax::Module::RadialGrid *m_RadialGridGeometryModuleRtt;

		Ogre::RenderTarget* m_Target;
		FilePath m_ResourceLocation;
		std::string m_ActiveModule;
		std::string m_ActiveNoise;
		bool m_Initialized;

	};
	typedef boost::shared_ptr<HydraxWaterComponent> HydraxWaterComponentPtr;
}

#endif