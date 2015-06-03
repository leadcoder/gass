/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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

#include "Sim/Interface/GASSIGraphicsSystem.h"
#include "Sim/GASSSimSystem.h"
#include "Sim/GASSGraphicsMaterial.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Core/MessageSystem/GASSMessageType.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Core/Serialize/GASSSTDSerialize.h"
#include <OgreRenderWindow.h>
#include <string>

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
	typedef SPTR<OgreRenderWindow> OgreRenderWindowPtr;
	class OgreResourceManager;
	typedef SPTR<OgreResourceManager> OgreResourceManagerPtr;

	class OgreDebugTextOutput;
	class OgrePostProcess;
	class OgreCameraComponent;
	typedef SPTR<OgrePostProcess> OgrePostProcessPtr;
	typedef SPTR<OgreCameraComponent> OgreCameraComponentPtr;
	typedef std::vector< std::string > PostFilterVector;
	typedef std::vector< std::string > PluginVector;
	class OgreGraphicsSystem : public Reflection<OgreGraphicsSystem, SimSystem>, public IGraphicsSystem
	{
		
		friend class OgreGraphicsSceneManager;
	public:
		OgreGraphicsSystem();
		virtual ~OgreGraphicsSystem();
		static void RegisterReflection();
		virtual void Init();
		virtual std::string GetSystemName() const {return "OgreGraphicsSystem";}
		virtual void Update(double time, TaskNode* caller);
		//IGraphicsSystem
		virtual RenderWindowPtr GetMainRenderWindow() const;
		virtual RenderWindowVector GetRenderWindows() const;

		//move this to material manager?
		virtual std::vector<std::string> GetMaterialNames(std::string resource_group) const;
		virtual RenderWindowPtr CreateRenderWindow(const std::string &name, int width, int height, void* external_window_handle = 0);
		virtual void AddMaterial(const GraphicsMaterial &material,const std::string &base_mat_name = "");
		virtual bool HasMaterial(const std::string &mat_name) const;
		virtual void RemoveMaterial(const std::string &mat_name);
		virtual GraphicsMaterial GetMaterial(const std::string &mat_name);
	public: //ogre specific
		Ogre::SceneManager* GetBootSceneManager() const {return m_SceneMgr;}
		OgrePostProcessPtr GetPostProcess() const {return m_PostProcess;}
		void ChangeCamera(Ogre::Camera* camera, const std::string &vp_name);
		Ogre::OverlaySystem* GetOverlaySystem() const {return m_OverlaySystem;}

		static void SetOgreMaterial(const GraphicsMaterial &material, Ogre::MaterialPtr mat);
		static void SetGASSMaterial(Ogre::MaterialPtr mat , GraphicsMaterial &material);

	protected:
		ADD_PROPERTY(bool,UpdateMessagePump);
		ADD_PROPERTY(bool,UseShaderCache);
		void OnDebugPrint(DebugPrintRequestPtr message);
		void OnReloadMaterial(ReloadMaterialPtr message);
		void OnInitializeTextBox(CreateTextBoxRequestPtr message);

		//Resource listeners
		void OnResourceGroupCreated(ResourceGroupCreatedEventPtr message);
		void OnResourceGroupRemoved(ResourceGroupRemovedEventPtr message);
		void OnResourceLocationAdded(ResourceLocationAddedEventPtr message);
		void OnResourceLocationRemoved(ResourceLocationRemovedEventPtr message);
		void OnViewportMovedOrResized(ViewportMovedOrResizedEventPtr message);

		PluginVector GetPlugins() const {return m_Plugins;}
		void SetPlugins(const PluginVector &plugins){m_Plugins = plugins;}
		bool GetCreateMainWindowOnInit() const {return m_CreateMainWindowOnInit;}
		void SetCreateMainWindowOnInit(bool value){m_CreateMainWindowOnInit = value;}
		bool GetShowStats() const {return m_ShowStats;}
		void SetShowStats(bool value){m_ShowStats = value;}
		PostFilterVector GetPostFilters() const;
		void SetPostFilters(const PostFilterVector &filters);
		void ReloadMaterials();
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
		OgrePostProcessPtr m_PostProcess;
		OgreResourceManagerPtr m_ResourceManager;
		Ogre::OverlaySystem* m_OverlaySystem;
	};
	typedef SPTR<OgreGraphicsSystem> OgreGraphicsSystemPtr;
}