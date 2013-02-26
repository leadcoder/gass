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

#include "Sim/GASS.h"
#include <string>
#include <osgViewer/Viewer>
#include <osgShadow/ShadowTechnique>

class TextBox;
namespace GASS
{
	FDECL(OSGRenderWindow)
	FDECL(OSGCameraComponent)

	class OSGGraphicsSystem : public Reflection<OSGGraphicsSystem,SimSystem> , public IGraphicsSystem
	{
		friend class OSGGraphicsSceneManager;
	public:
		OSGGraphicsSystem();
		virtual ~OSGGraphicsSystem();
		static void RegisterReflection();
		virtual void Init();
		virtual std::string GetSystemName() const {return "OSGGraphicsSystem";}
		virtual void Update(double delta);

		virtual RenderWindowPtr GetMainRenderWindow() const;
		virtual RenderWindowVector GetRenderWindows() const;
		virtual RenderWindowPtr CreateRenderWindow(const std::string &name, int width, int height, void* external_handle = 0);
		
		osgViewer::CompositeViewer*  GetViewer() {return m_Viewer ;}
	protected:
		void OnInitializeTextBox(CreateTextBoxRequestPtr message);
		void OnDebugPrint(DebugPrintRequestPtr message);
		void OnViewportMovedOrResized(ViewportMovedOrResizedEventPtr message);
		
		//void SetActiveData(osg::Group* root);
		void LoadShadowSettings(TiXmlElement *shadow_elem);
		osg::ref_ptr<osgShadow::ShadowTechnique> GetShadowTechnique() const {return m_ShadowTechnique;}
		void SetShadowSettingsFile(const std::string& file_name) {m_ShadowSettingsFile = file_name;}
		std::string GetShadowSettingsFile() const {return m_ShadowSettingsFile;}
		void ChangeCamera(const std::string &viewport, OSGCameraComponentPtr cam_comp);
	private:
		osgViewer::CompositeViewer* m_Viewer;
		std::vector<OSGRenderWindowPtr> m_Windows;
		osg::ref_ptr<osgShadow::ShadowTechnique> m_ShadowTechnique;
		std::string m_ShadowSettingsFile;
		TextBox* m_DebugTextBox;
		std::map<std::string,TextBox*>  m_TextBoxes;
		
	};
	typedef SPTR<OSGGraphicsSystem>  OSGGraphicsSystemPtr;
	typedef WPTR<OSGGraphicsSystem>  OSGGraphicsSystemWeakPtr;
}
