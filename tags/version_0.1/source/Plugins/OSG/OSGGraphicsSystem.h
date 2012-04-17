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

#pragma once

#include "Core/MessageSystem/MessageType.h"
#include "Sim/Systems/Graphics/IGraphicsSystem.h"
#include "Sim/Systems/SimSystem.h"
#include "Sim/Systems/Messages/CoreSystemMessages.h"
#include "Sim/Systems/Messages/GraphicsSystemMessages.h"
#include "Sim/Scheduling/ITaskListener.h"
#include <string>
#include <osgViewer/Viewer>
#include <osgShadow/ShadowTechnique>

class TextBox;
namespace GASS
{
	class OSGCameraComponent;
	typedef boost::shared_ptr<OSGCameraComponent> OSGCameraComponentPtr;
	class OSGGraphicsSystem : public Reflection<OSGGraphicsSystem,SimSystem> , public IGraphicsSystem, public ITaskListener
	{
		friend class OSGGraphicsSceneManager;
	public:
		OSGGraphicsSystem();
		virtual ~OSGGraphicsSystem();
		static void RegisterReflection();
		virtual void OnCreate();
		SystemType GetSystemType() {return "GraphicsSystem";}
		void GetMainWindowInfo(unsigned int &width, unsigned int &height, int &left, int &top) const;
		osgViewer::CompositeViewer*  GetViewer() {return m_Viewer ;}
		//ITaskListener interface
		void Update(double delta);
		TaskGroup GetTaskGroup() const;

		void CreateRenderWindow(const std::string &name, int width, int height, void* handle, void* main_handle = 0);
		void CreateViewport(const std::string &name, const std::string &render_window, float  left, float top, float width, float height);
	protected:
		void OnDebugPrint(DebugPrintMessagePtr message);
		void OnViewportMovedOrResized(ViewportMovedOrResizedNotifyMessagePtr message);
		bool GetCreateMainWindowOnInit() const {return m_CreateMainWindowOnInit;}
		void SetCreateMainWindowOnInit(bool value){m_CreateMainWindowOnInit = value;}
		
		void SetActiveData(osg::Group* root);
		void OnInit(InitSystemMessagePtr message);		
		void LoadShadowSettings(TiXmlElement *shadow_elem);
		osg::ref_ptr<osgShadow::ShadowTechnique> GetShadowTechnique() const {return m_ShadowTechnique;}
		void SetShadowSettingsFile(const std::string& file_name) {m_ShadowSettingsFile = file_name;}
		std::string GetShadowSettingsFile() const {return m_ShadowSettingsFile;}
		void ChangeCamera(const std::string &viewport, OSGCameraComponentPtr cam_comp);
	private:
		osgViewer::CompositeViewer* m_Viewer;

		std::map<std::string,osg::ref_ptr<osg::GraphicsContext> > m_Windows;
		bool m_CreateMainWindowOnInit;
		osg::ref_ptr<osgShadow::ShadowTechnique> m_ShadowTechnique;
		std::string m_ShadowSettingsFile;

		TextBox* m_DebugTextBox;
		
	};
	typedef boost::shared_ptr<OSGGraphicsSystem>  OSGGraphicsSystemPtr;
	typedef boost::weak_ptr<OSGGraphicsSystem>  OSGGraphicsSystemWeakPtr;
}
