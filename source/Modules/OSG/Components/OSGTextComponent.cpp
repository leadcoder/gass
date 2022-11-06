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

#include "Modules/OSG/Components/OSGTextComponent.h"
#include "Modules/OSG/OSGGraphicsSceneManager.h"
#include "Modules/OSG/Components/OSGLocationComponent.h"
#include "Modules/OSG/OSGNodeMasks.h"
#include "Modules/OSG/OSGConvert.h"

namespace GASS
{
	OSGTextComponent::OSGTextComponent() :	m_OSGText(nullptr),
		m_Font("arial.ttf"),
		m_Offset(0,0,0),
		m_Color(1,1,1,1)
	{

	}

	OSGTextComponent::~OSGTextComponent()
	{

	}

	void OSGTextComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OSGTextComponent>("TextComponent");
		ADD_DEPENDENCY("OSGLocationComponent")
		RegisterGetSet("Font", &OSGTextComponent::GetFont, &OSGTextComponent::SetFont);
		RegisterGetSet("CharacterSize", &OSGTextComponent::GetCharacterSize, &OSGTextComponent::SetCharacterSize);
		RegisterGetSet("ScaleByDistance", &OSGTextComponent::GetScaleByDistance, &OSGTextComponent::SetScaleByDistance);
		RegisterGetSet("Offset", &OSGTextComponent::GetOffset, &OSGTextComponent::SetOffset);
		RegisterGetSet("Color", &OSGTextComponent::GetColor, &OSGTextComponent::SetColor);
		RegisterGetSet("DropShadow", &OSGTextComponent::GetDropShadow, &OSGTextComponent::SetDropShadow);
	}

	void OSGTextComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGTextComponent::OnLocationLoaded,LocationLoadedEvent,1));
	}

	void OSGTextComponent::SetCaption(const std::string& caption)
	{
		if (m_OSGText)
			m_OSGText->setText(caption.c_str());
	}

	std::string OSGTextComponent::GetCaption() const
	{
		std::string caption;
		if (m_OSGText)
			caption = m_OSGText->getText().createUTF8EncodedString();
		return caption;
	}

	void OSGTextComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		m_OSGText = new osgText::Text;
		SetFont(m_Font);
		m_OSGText->setCharacterSize(m_CharSize);
		m_OSGText->setAxisAlignment(osgText::Text::SCREEN);

		if(m_ScaleByDistance)
			m_OSGText->setCharacterSizeMode(osgText::Text::OBJECT_COORDS_WITH_MAXIMUM_SCREEN_SIZE_CAPPED_BY_FONT_HEIGHT);
			//m_OSGText->setCharacterSizeMode(osgText::Text::OBJECT_COORDS);
		else
			m_OSGText->setCharacterSizeMode(osgText::Text::SCREEN_COORDS);
			
		m_OSGText->setFontResolution(16, 16);

		const osg::Vec3d offset = OSGConvert::ToOSG(m_Offset);
		m_OSGText->setPosition(offset);
		m_OSGText->setColor(osg::Vec4d(m_Color.x,m_Color.y,m_Color.z,m_Color.w));

		if(m_DropShadow)
			m_OSGText->setBackdropType(osgText::Text::OUTLINE);

		m_OSGText->setAlignment(osgText::Text::CENTER_BOTTOM);
		//m_OSGText->setDrawMode(osgText::TextBase::DrawModeMask::TEXT | osgText::TextBase::DrawModeMask::FILLEDBOUNDINGBOX);
		//m_OSGText->setBoundingBoxColor(osg::Vec4d(0.2,0.2,0.3,0.7));

		m_OSGGeode = new osg::Geode;
		m_OSGGeode->addDrawable(m_OSGText.get());

		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		lc->GetOSGNode()->addChild(m_OSGGeode.get());

		m_OSGGeode->setNodeMask(~NM_RECEIVE_SHADOWS & m_OSGGeode->getNodeMask());
		m_OSGGeode->setNodeMask(~NM_CAST_SHADOWS & m_OSGGeode->getNodeMask());

		osg::ref_ptr<osg::StateSet> nodess = m_OSGGeode->getOrCreateStateSet();
		nodess->setMode(GL_LIGHTING,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);

		//skip depth test for text.
		nodess->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE);
		nodess->setRenderBinDetails(INT_MAX, "RenderBin");
		
		auto* program = new osg::Program;
        nodess->setAttribute(program);
	}

	void OSGTextComponent::SetFont(const ResourceHandle &font)
	{
		m_Font = font;
		if(m_OSGText.valid())
		{
			m_OSGText->setFont(font.GetResource()->Path().GetFullPath());
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
		float max = m_CharSize;// static_cast<float>(Math::Max(m_Width, m_Height));
		AABox box(Vec3(-max/2.0,-max/2.0,-max/2.0),Vec3(max/2.0,max/2.0,max/2.0));
		return box;
	}
	Sphere OSGTextComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		sphere.m_Pos = Vec3(0,0,0);
		sphere.m_Radius = m_CharSize;//static_cast<float>(Math::Max(m_Width,m_Height)/2.0);
		return sphere;
	}

	void OSGTextComponent::GetMeshData(GraphicsMeshPtr mesh_data)
	{

	}
}