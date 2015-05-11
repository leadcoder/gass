/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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
#include "Core/Utils/GASSColorRGBA.h"
#include "Core/Reflection/GASSPropertyMetaData.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSIGraphicsSystem.h"
#include "Sim/GASSSimSystemManager.h"

namespace GASS
{
	/**
		Class holding material data used by graphic system
	*/
	class GraphicsMaterial
	{
	public:
		GraphicsMaterial() : Diffuse(1,1,1,1),
			Ambient(1,1,1),
			Specular(0,0,0),
			SelfIllumination(0,0,0),
			Shininess(0), 
			DepthTest(true),
			DepthWrite(true),
			TrackVertexColor(false)
		{}
		GraphicsMaterial(const std::string &name, const ColorRGBA &diffuse,const ColorRGB &ambient,const ColorRGB &specular = ColorRGB(-1,-1,-1), const ColorRGB &selfIllumination = ColorRGB(-1,-1,-1), float shininess = -1,bool depth_test_on = true, bool depth_write_on = true) : Diffuse(diffuse),
			Ambient(ambient),
			Specular(specular),
			SelfIllumination(selfIllumination ),
			Shininess(shininess), 
			DepthTest(depth_test_on),
			DepthWrite(depth_write_on),
			Name(name),
			TrackVertexColor(false)
		{

		}
		  std::vector<std::string> Textures;
		  ColorRGBA Diffuse;
		  ColorRGB Ambient;
		  ColorRGB Specular;
		  ColorRGB SelfIllumination;
		  std::string Name;
		  float Shininess;
		  bool DepthTest;
		  bool DepthWrite;
		  bool TrackVertexColor;
	};
	typedef SPTR<GraphicsMaterial> GraphicsMaterialPtr;


	/**
		Property meta data used for material property binding.
		
	*/
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