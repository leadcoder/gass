/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
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


#include "MyGUIVehicleInstrumentsComponent.h"

#include <osg/NodeVisitor>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/Texture>
#include <osg/Geode>
#include <osg/Image>
#include "MyGUI_RTTLayer.h"
#include "MonitorPanel.h"
#include "MyGUI_OpenGLTexture.h"

#include "MyGUIOSGSystem.h"
#include "Plugins/Base/CoreMessages.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSSimEngine.h"
#include "Plugins/OSG/IOSGMesh.h"



namespace GASS
{
	MyGUIVehicleInstrumentsComponent::MyGUIVehicleInstrumentsComponent(void) 
	{

	}

	MyGUIVehicleInstrumentsComponent::~MyGUIVehicleInstrumentsComponent(void)
	{
		
	}

	void MyGUIVehicleInstrumentsComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("MyGUIVehicleInstrumentsComponent",new GASS::Creator<MyGUIVehicleInstrumentsComponent, Component>);
		//RegisterProperty<std::string>("TextureName", &GASS::MyGUIVehicleInstrumentsComponent::GetTextureName, &GASS::MyGUIVehicleInstrumentsComponent::SetTextureName);
	}

	void MyGUIVehicleInstrumentsComponent::OnInitialize()
	{
		//wait for mesh data to load!
		
		GetSceneObject()->RegisterForMessage(REG_TMESS(MyGUIVehicleInstrumentsComponent::OnLayoutLoaded,LayoutLoadedEvent,0));
	}

	void MyGUIVehicleInstrumentsComponent::OnLayoutLoaded(LayoutLoadedEventPtr message)
	{
		//SceneManagerListenerPtr listener = shared_from_this();
		//GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<BaseSceneManager>()->Register(listener);
	}
	
	/*void MyGUIVehicleInstrumentsComponent::SceneManagerTick(double delta_time)
	{
		static double progress = 0;
		progress += 1;
		if(progress > 100)
			progress = 0;
		MyGUI::ImageBox* image = test_layout.at(0)->findWidget("Ship")->castType<MyGUI::ImageBox>();
		//image = layout.at(0)->createWidget<MyGUI::ImageBox>("RotatingSkin", MyGUI::IntCoord(150, 150, 100, 150), MyGUI::Align::Default);
		//image->setImageTexture("Wallpaper.png");
		MyGUI::ISubWidget* main = image->getSubWidgetMain();
		MyGUI::RotatingSkin * rotato = main->castType<MyGUI::RotatingSkin>();
		rotato->setCenter(MyGUI::IntPoint(rotato->getWidth()*0.5,rotato->getHeight()*0.5));
		rotato->setAngle(progress*0.01);
	}

	std::string MyGUIVehicleInstrumentsComponent::GetTextureName() const
	{
		return m_TextureName;
	}

	void MyGUIVehicleInstrumentsComponent::SetTextureName(const std::string &value)
	{
		m_TextureName = value;
	}*/
}
