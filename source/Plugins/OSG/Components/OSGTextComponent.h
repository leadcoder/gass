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
#include "Plugins/OSG/OSGCommon.h"

namespace osg
{
	class Billboard;
	class Image;
	class Drawable;
	class Text;
}

namespace GASS
{
	class OSGTextComponent : public Reflection<OSGTextComponent,Component> 
	{
	public:
		OSGTextComponent (void);
		~OSGTextComponent (void) override;
		static void RegisterReflection();
		void OnInitialize() override;
		virtual AABox GetBoundingBox()const;
		virtual Sphere GetBoundingSphere()const;
		virtual void GetMeshData(GraphicsMeshPtr mesh_data);
	protected:
		void OnLocationLoaded(LocationLoadedEventPtr message);
		void OnTextCaptionMessage(TextCaptionRequestPtr message);
		void SetFont(const ResourceHandle &font);
		ResourceHandle GetFont()const {return m_Font;}
		float GetCharacterSize() const ;
		void SetCharacterSize(float size);
		void SetScaleByDistance(bool value) {m_ScaleByDistance = value;}
		bool GetScaleByDistance() const {return m_ScaleByDistance;}
		void SetOffset(const Vec3 &value) {m_Offset = value;}
		Vec3 GetOffset() const {return m_Offset;}
		void SetColor(const Vec4 &value) {m_Color = value;}
		Vec4 GetColor() const {return m_Color;}
		void SetDropShadow(bool value) {m_DropShadow = value;}
		bool GetDropShadow() const {return m_DropShadow;}

		ResourceHandle m_Font;
		//float m_Width;
		//float m_Height;
		osg::ref_ptr<osgText::Text> m_OSGText;
		osg::ref_ptr<osg::Geode> m_OSGGeode;
		float m_CharSize{16.0f};
		bool m_ScaleByDistance{false};
		Vec3 m_Offset;
		bool m_DropShadow{true};
		Vec4 m_Color;
	};
}