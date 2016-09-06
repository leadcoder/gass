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

#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSScriptSystemMessages.h"
#include <osg/ref_ptr>
#include "MyGUI_WidgetDefines.h"


namespace GASS
{
	class MyGUIOSGTexture;

	class MyGUILayoutComponent : public Reflection<MyGUILayoutComponent,BaseSceneComponent>
	{
	public:
		MyGUILayoutComponent(void);
		~MyGUILayoutComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
	protected:
		void OnGeometryChanged(GeometryChangedEventPtr message);
		void OnGUILoaded(GUILoadedEventPtr message);
		void _SetupLayout();
		std::string GetTextureName() const;
		void SetTextureName(const std::string &value);
		std::string GetLayoutFile() const;
		void SetLayoutFile(const std::string &value);
		
		std::string m_TextureName;
		std::string m_LayoutFile;
		MyGUI::VectorWidgetPtr m_Layout;

		osg::ref_ptr<MyGUIOSGTexture> m_OSGRTTTexture;
	};
	typedef GASS_SHARED_PTR<MyGUILayoutComponent> MyGUILayoutComponentPtr;
}

