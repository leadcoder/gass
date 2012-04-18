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
class osg::Billboard;
class osg::Image;
class osg::Drawable;
class osgText::Text;

namespace GASS
{
	


	class OSGTextComponent : public Reflection<OSGTextComponent,BaseSceneComponent> 
	{
	public:
		OSGTextComponent (void);
		~OSGTextComponent (void);
		static void RegisterReflection();
		virtual void OnCreate();
		virtual AABox GetBoundingBox()const;
		virtual Sphere GetBoundingSphere()const;
		virtual void GetMeshData(MeshDataPtr mesh_data);
	protected:
		void OnLoad(LoadGFXComponentsMessagePtr message);
		void OnTextCaptionMessage(GASS::TextCaptionMessagePtr message);


		void SetFont(const std::string &font);
		std::string GetFont()const {return m_Font;}
		float GetCharacterSize() const ;
		void SetCharacterSize(float size);
		
		std::string m_Font;
		float m_Width;
		float m_Height;
		osg::ref_ptr<osgText::Text> m_OSGText;
		osg::ref_ptr<osg::Geode> m_OSGGeode;
		float m_CharSize;
	};
}

