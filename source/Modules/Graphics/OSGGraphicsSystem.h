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
#include "Modules/Graphics/OSGCommon.h"

class TextBox;
namespace GASS
{
	GASS_FORWARD_DECL(OSGRenderWindow)
	GASS_FORWARD_DECL(OSGCameraComponent)

	class ExportOSG OSGGraphicsSystem : public Reflection<OSGGraphicsSystem,SimSystem> , public IGraphicsSystem
	{
		friend class OSGGraphicsSceneManager;
	public:
		static void RegisterReflection();
		OSGGraphicsSystem(SimSystemManagerWeakPtr manager);
		~OSGGraphicsSystem() override;
		void OnSystemInit() override;
		void OnSystemUpdate(double delta_time) override;
		std::string GetSystemName() const override { return "OSGGraphicsSystem"; }

		RenderWindowPtr GetMainRenderWindow() const override;
		RenderWindowVector GetRenderWindows() const override;
		RenderWindowPtr CreateRenderWindow(const std::string &name, int width, int height, void* external_handle = nullptr) override;
		std::vector<std::string> GetMaterialNames(std::string resource_group = "") const override;
		void AddMaterial(IGfxMaterialConfig* config, const std::string &name) override;
		bool HasMaterial(const std::string &mat_name) const override;
		virtual void RemoveMaterial(const std::string &mat_name);
		void PrintDebugText(const std::string &message) override;
	public:
		//osg specific
		osgViewer::CompositeViewer*  GetViewer() const {return m_Viewer ;}
		osg::ref_ptr<osg::StateSet> GetStateSet(const std::string &material_name);
		TextBox* GetDebugText() const { return m_DebugTextBox; }
	protected:
		void OnInitializeTextBox(CreateTextBoxRequestPtr message);
		void OnViewportMovedOrResized(ViewportMovedOrResizedEventPtr message);
		virtual void RegisterSceneManager();

		bool m_UseLogHandler = true;
	private:
		osgViewer::CompositeViewer* m_Viewer;
		std::vector<OSGRenderWindowPtr> m_Windows;
		TextBox* m_DebugTextBox;
		std::map<std::string,TextBox*>  m_TextBoxes;
		using MaterialMap = std::map<std::string, osg::ref_ptr<osg::StateSet>>;
		MaterialMap m_Materials;
		std::vector<std::string> m_DebugVec;
		bool m_FlipDDS;
		
		};
	using OSGGraphicsSystemPtr = std::shared_ptr<OSGGraphicsSystem>;
	using OSGGraphicsSystemWeakPtr = std::weak_ptr<OSGGraphicsSystem>;
}
