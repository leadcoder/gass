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

#include <boost/bind.hpp>
#include <osg/Billboard>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/AlphaFunc>
#include <osgText/Text>

#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/Components/OSGTextComponent.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"
#include "Plugins/OSG/OSGNodeMasks.h"

namespace GASS
{

	OSGTextComponent::OSGTextComponent() :	m_OSGText(NULL),
		m_CharSize(32.0f),
		m_Font("arial.ttf")
	{

	}	

	OSGTextComponent::~OSGTextComponent()
	{
		
	}

	void OSGTextComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("TextComponent",new GASS::Creator<OSGTextComponent, IComponent>);
		RegisterProperty<std::string>("Font", &GetFont, &SetFont);
		RegisterProperty<float>("CharacterSize", &OSGTextComponent::GetCharacterSize, &OSGTextComponent::SetCharacterSize);
	}

	void OSGTextComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGTextComponent::OnLoad,LoadGFXComponentsMessage,1));
		//GetSceneObject()->RegisterForMessage(OBJECT_RM_TEXT_PARAMETER, TYPED_MESSAGE_FUNC(OSGTextComponent::OnParameterMessage));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGTextComponent::OnTextCaptionMessage, TextCaptionMessage,0));
		//mm.RegisterForMessage(MESSAGE_UPDATE, address,  boost::bind( &LocationComponent::OnUpdate, this, _1 ),m_InitPriority);
	}

	void OSGTextComponent::OnTextCaptionMessage(GASS::TextCaptionMessagePtr message)
	{
		
		std::string caption = message->GetCaption();
		m_OSGText->setText(caption.c_str());
	}

	void OSGTextComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		m_OSGText = new osgText::Text;

		SetFont(m_Font);

		m_OSGText->setCharacterSize(m_CharSize);
		m_OSGText->setAxisAlignment(osgText::Text::SCREEN);
		m_OSGText->setCharacterSizeMode(osgText::Text::OBJECT_COORDS_WITH_MAXIMUM_SCREEN_SIZE_CAPPED_BY_FONT_HEIGHT);
		
		m_OSGGeode = new osg::Geode;
		m_OSGGeode->addDrawable(m_OSGText.get());
    
		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		lc->GetOSGNode()->addChild(m_OSGGeode.get());

		m_OSGGeode->setNodeMask(~NM_RECEIVE_SHADOWS & m_OSGGeode->getNodeMask());
		m_OSGGeode->setNodeMask(~NM_CAST_SHADOWS & m_OSGGeode->getNodeMask());
		
	}

	void OSGTextComponent::SetFont(const std::string &font) 
	{
		m_Font = font;
		if(m_OSGText.valid())
		{
			std::string full_path;
			ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>();
			if(!rs->GetFullPath(m_Font,full_path))
			{
				GASS_EXCEPT(Exception::ERR_FILE_NOT_FOUND,"Failed to find texture:" + full_path,"OSGTextComponent::SetFont");
			}
			else
				m_OSGText->setFont(full_path);
		}
	}

	void OSGTextComponent::SetCharacterSize(float size) 
	{
		m_CharSize = size;
		if(m_OSGText.valid())
		{
			m_OSGText->setCharacterSize(size);
		}
	}

	float OSGTextComponent::GetCharacterSize() const  
	{
		return m_CharSize;
	}

	AABox OSGTextComponent::GetBoundingBox() const
	{
		float max = Math::Max(m_Width,m_Height);
		AABox box(Vec3(-max/2.0,-max/2.0,-max/2.0),Vec3(max/2.0,max/2.0,max/2.0));
		return box;
	}
	Sphere OSGTextComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		sphere.m_Pos = Vec3(0,0,0);
		sphere.m_Radius = Math::Max(m_Width,m_Height)/2.0;
		return sphere;
	}

	void OSGTextComponent::GetMeshData(MeshDataPtr mesh_data)
	{

	}

}
