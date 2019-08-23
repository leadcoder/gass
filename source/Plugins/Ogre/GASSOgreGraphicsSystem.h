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

#pragma once

#include "Plugins/Ogre/GASSOgreCommon.h"
#include "Sim/Interface/GASSIGraphicsSystem.h"
#include "Sim/GASSSimSystem.h"
#include "Sim/GASSGraphicsMaterial.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Plugins/Ogre/GASSOgreGraphicsSceneManager.h"

namespace Ogre
{
	class Root;
	class SceneManager;
	class ColourValue;
	class Viewport;
	class OverlaySystem;
}

namespace GASS
{
	class OgreRenderWindow;
	typedef GASS_SHARED_PTR<OgreRenderWindow> OgreRenderWindowPtr;
	class OgreResourceManager;
	typedef GASS_SHARED_PTR<OgreResourceManager> OgreResourceManagerPtr;

	class OgreDebugTextOutput;
	class OgrePostProcess;
	class OgreCameraComponent;
	typedef GASS_SHARED_PTR<OgrePostProcess> OgrePostProcesGASS_SHARED_PTR;
	typedef GASS_SHARED_PTR<OgreCameraComponent> OgreCameraComponentPtr;
	typedef std::vector< std::string > PostFilterVector;
	typedef std::vector< std::string > PluginVector;
	class OgreGraphicsSystem : public Reflection<OgreGraphicsSystem, SimSystem>, public IGraphicsSystem
	{
		friend class OgreGraphicsSceneManager;
	public:
		static void RegisterReflection();
		OgreGraphicsSystem(SimSystemManagerWeakPtr manager);
		~OgreGraphicsSystem() override;
		void OnSystemInit() override;
		void OnSystemUpdate(double time) override;
		std::string GetSystemName() const override {return "OgreGraphicsSystem";}
		
		//IGraphicsSystem
		RenderWindowPtr GetMainRenderWindow() const override;
		RenderWindowVector GetRenderWindows() const override;
		void PrintDebugText(const std::string &message) override;
		RenderWindowPtr CreateRenderWindow(const std::string &name, int width, int height, void* external_window_handle = 0) override;
		std::vector<std::string> GetMaterialNames(std::string resource_group) const override;
		void AddMaterial(const GraphicsMaterial &material,const std::string &base_mat_name = "") override;
		bool HasMaterial(const std::string &mat_name) const override;
		virtual void RemoveMaterial(const std::string &mat_name);
		virtual GraphicsMaterial GetMaterial(const std::string &mat_name);
	public: //ogre specific
		Ogre::SceneManager* GetBootSceneManager() const {return m_SceneMgr;}
		OgrePostProcesGASS_SHARED_PTR GetPostProcess() const {return m_PostProcess;}
		//void ChangeCamera(Ogre::Camera* camera, const std::string &vp_name);
		Ogre::OverlaySystem* GetOverlaySystem() const {return m_OverlaySystem;}

		static void SetOgreMaterial(const GraphicsMaterial &material, Ogre::MaterialPtr mat);
		static void SetGASSMaterial(Ogre::MaterialPtr mat , GraphicsMaterial &material);

	protected:
		
		//void OnReloadMaterial(ReloadMaterialPtr message);
		void OnInitializeTextBox(CreateTextBoxRequestPtr message);

		//Resource listeners
		void OnResourceGroupCreated(ResourceGroupCreatedEventPtr message);
		void OnResourceGroupRemoved(ResourceGroupRemovedEventPtr message);
		void OnResourceLocationAdded(ResourceLocationAddedEventPtr message);
		void OnResourceLocationRemoved(ResourceLocationRemovedEventPtr message);
		void OnResourceGroupReload(ResourceGroupReloadEventPtr event);
		void OnViewportMovedOrResized(ViewportMovedOrResizedEventPtr message);

		PluginVector GetPlugins() const {return m_Plugins;}
		void SetPlugins(const PluginVector &plugins){m_Plugins = plugins;}
		bool GetCreateMainWindowOnInit() const {return m_CreateMainWindowOnInit;}
		void SetCreateMainWindowOnInit(bool value){m_CreateMainWindowOnInit = value;}
		bool GetShowStats() const {return m_ShowStats;}
		void SetShowStats(bool value){m_ShowStats = value;}
		PostFilterVector GetPostFilters() const;
		void SetPostFilters(const PostFilterVector &filters);
		//void ReloadMaterials();
		void ReloadResources();

		std::string m_RenderSystem;
		Ogre::Root* m_Root;
		std::map<std::string, OgreRenderWindowPtr>	m_Windows;
		Ogre::SceneManager* m_SceneMgr;
		PluginVector m_Plugins;

		
		PostFilterVector m_PostFilters;
		OgreDebugTextOutput* m_DebugTextBox;
		bool m_CreateMainWindowOnInit;
		bool m_ShowStats;
		bool m_UpdateMessagePump;
		bool m_UseShaderCache;

		OgrePostProcesGASS_SHARED_PTR m_PostProcess;
		OgreResourceManagerPtr m_ResourceManager;
		Ogre::OverlaySystem* m_OverlaySystem;
	};
	typedef GASS_SHARED_PTR<OgreGraphicsSystem> OgreGraphicsSystemPtr;
}