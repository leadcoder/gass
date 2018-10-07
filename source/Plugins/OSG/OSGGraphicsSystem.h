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


#include "Sim/GASS.h"
#include "Sim/GASSGraphicsMaterial.h"
#include "Plugins/OSG/OSGCommon.h"
#include "IOSGGraphicsSystem.h"

class TextBox;
namespace GASS
{
	GASS_FORWARD_DECL(OSGRenderWindow)
	GASS_FORWARD_DECL(OSGCameraComponent)

	class OSGGraphicsSystem : public Reflection<OSGGraphicsSystem,SimSystem> , public IGraphicsSystem, public IOSGGraphicsSystem
	{
		friend class OSGGraphicsSceneManager;
	public:
		OSGGraphicsSystem();
		virtual ~OSGGraphicsSystem();
		static void RegisterReflection();
		virtual void Init();
		virtual std::string GetSystemName() const {return "OSGGraphicsSystem";}
		virtual void OnSystemUpdate(double delta_time);

		virtual RenderWindowPtr GetMainRenderWindow() const;
		virtual RenderWindowVector GetRenderWindows() const;
		virtual RenderWindowPtr CreateRenderWindow(const std::string &name, int width, int height, void* external_handle = 0);
		virtual std::vector<std::string> GetMaterialNames(std::string resource_group = "") const;

		virtual void AddMaterial(const GraphicsMaterial &material,const std::string &base_mat_name = "");
		virtual bool HasMaterial(const std::string &mat_name) const;
		virtual void RemoveMaterial(const std::string &mat_name);
		virtual GraphicsMaterial GetMaterial(const std::string &mat_name);

	public:
		//osg specific
		osgViewer::CompositeViewer*  GetViewer() const {return m_Viewer ;}
		static void SetOSGStateSet(const GraphicsMaterial &material,osg::ref_ptr<osg::StateSet> state);
		static void SetGASSMaterial(osg::ref_ptr<osg::StateSet> state_set,GraphicsMaterial &material);
		osg::ref_ptr<osg::StateSet> GetStateSet(const std::string &material_name);
		TextBox* GetDebugText() const { return m_DebugTextBox; }
	protected:
		ADD_PROPERTY(bool,FlipDDS);
		void LoadXML(tinyxml2::XMLElement *elem);
		void OnInitializeTextBox(CreateTextBoxRequestPtr message);
		void OnDebugPrint(DebugPrintRequestPtr message);
		void OnViewportMovedOrResized(ViewportMovedOrResizedEventPtr message);
		
		//void SetActiveData(osg::Group* root);
		void LoadShadowSettings(tinyxml2::XMLElement *shadow_elem);
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
		typedef std::map<std::string, osg::ref_ptr<osg::StateSet> > MaterialMap;
		MaterialMap m_Materials;
		std::vector<std::string> m_DebugVec;
	};
	typedef GASS_SHARED_PTR<OSGGraphicsSystem>  OSGGraphicsSystemPtr;
	typedef GASS_WEAK_PTR<OSGGraphicsSystem>  OSGGraphicsSystemWeakPtr;
}
