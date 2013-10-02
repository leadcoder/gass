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

#pragma once

#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSColorRGB.h"
#include "Core/Reflection/GASSPropertyMetaData.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSIGraphicsSystem.h"
#include "Sim/GASSSimSystemManager.h"

namespace GASS
{
	class GraphicsMaterial
	{
	public:
		GraphicsMaterial() : m_Diffuse(1,1,1,1),
			m_Ambient(1,1,1),
			m_Specular(0,0,0),
			m_SelfIllumination(0,0,0),
			m_Shininess(0), 
			m_DepthTest(false)
		{}
		GraphicsMaterial(const ColorRGBA &diffuse,const ColorRGB &ambient,const ColorRGB &specular = ColorRGB(-1,-1,-1), const ColorRGB &selfIllumination = ColorRGB(-1,-1,-1), float shininess = -1,bool depth_test_on = true) : m_Diffuse(diffuse),
			m_Ambient(ambient),
			m_Specular(specular),
			m_SelfIllumination(selfIllumination ),
			m_Shininess(shininess), 
			m_DepthTest(depth_test_on)
		{

		}
		  ColorRGBA GetDiffuse()const {return m_Diffuse;}
		  ColorRGB GetAmbient()const {return m_Ambient;}
		  ColorRGB GetSpecular()const {return m_Specular;}
		  ColorRGB GetSelfIllumination()const {return m_SelfIllumination;}
		  float GetShininess()const {return m_Shininess;}
		  bool GetDepthTest()const {return m_DepthTest;}
		  std::vector<std::string> Textures;
	private:
		ColorRGBA m_Diffuse;
		ColorRGB m_Ambient;
		ColorRGB m_Specular;
		ColorRGB m_SelfIllumination;
		
		float m_Shininess;
		bool m_DepthTest;
	};
	typedef SPTR<GraphicsMaterial> GraphicsMaterialPtr;


	class GraphicsMaterialPropertyMetaData : public EnumerationPropertyMetaData
	{
	public:
		GraphicsMaterialPropertyMetaData(const std::string &annotation, PropertyFlags flags, std::string res_group = ""): EnumerationPropertyMetaData(annotation,flags,false),
			m_ResourceGroup(res_group)
		{

		}
		virtual std::vector<std::string> GetEnumeration(BaseReflectionObjectPtr object) const 
		{
			GASS::GraphicsSystemPtr gfx_system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IGraphicsSystem>();
			std::vector<std::string> content = gfx_system->GetMaterialNames(m_ResourceGroup);
			return content;
		}
	private:
		std::string m_ResourceGroup;
	};
	typedef SPTR<GraphicsMaterialPropertyMetaData> GraphicsMaterialPropertyMetaDataPtr;



}