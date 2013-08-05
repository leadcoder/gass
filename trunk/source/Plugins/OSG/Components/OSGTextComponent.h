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
#include <osg/ref_ptr>
//#include "Sim/GASSResourceHandle.h"

#include <osgText/Text>
namespace osg
{
	class Billboard;
	class Image;
	class Drawable;
	class Text;
}

namespace GASS
{
	class OSGTextComponent : public Reflection<OSGTextComponent,BaseSceneComponent> 
	{
	public:
		OSGTextComponent (void);
		~OSGTextComponent (void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual AABox GetBoundingBox()const;
		virtual Sphere GetBoundingSphere()const;
		virtual void GetMeshData(MeshDataPtr mesh_data);
	protected:
		void OnLocationLoaded(LocationLoadedMessagePtr message);
		void OnTextCaptionMessage(GASS::TextCaptionMessagePtr message);
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
		float m_Width;
		float m_Height;
		osg::ref_ptr<osgText::Text> m_OSGText;
		osg::ref_ptr<osg::Geode> m_OSGGeode;
		float m_CharSize;
		bool m_ScaleByDistance;
		Vec3 m_Offset;
		bool m_DropShadow;
		Vec4 m_Color;
	};
}

