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
#include "Core/Utils/GASSColorRGB.h"
#include "Core/Utils/GASSColorRGBA.h"
#include "Core/Reflection/GASSPropertyMetaData.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSIGraphicsSystem.h"
#include "Sim/GASSSimSystemManager.h"

namespace GASS
{
#if 0
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
			TrackVertexColor(false),
			Transparent(0)
		{}
		GraphicsMaterial(const std::string &name, const ColorRGBA &diffuse,const ColorRGB &ambient,const ColorRGB &specular = ColorRGB(-1,-1,-1), const ColorRGB &selfIllumination = ColorRGB(-1,-1,-1), float shininess = -1,bool depth_test_on = true, bool depth_write_on = true) : Diffuse(diffuse),
			Ambient(ambient),
			Specular(specular),
			SelfIllumination(selfIllumination ),
			Shininess(shininess), 
			DepthTest(depth_test_on),
			DepthWrite(depth_write_on),
			Name(name),
			TrackVertexColor(false),
			Transparent(0)
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
		  int Transparent;
	};
	typedef GASS_SHARED_PTR<GraphicsMaterial> GraphicsMaterialPtr;
#endif

	class IGfxMaterialConfig
	{
	public:
		virtual ~IGfxMaterialConfig()	{}
		bool DepthTest = true;
	};

	class UnlitMaterialConfig : public IGfxMaterialConfig
	{
	public:
		UnlitMaterialConfig() : Color(1,1,1,1) {}
		UnlitMaterialConfig(const ColorRGBA &color) : Color(color){}
		virtual ~UnlitMaterialConfig() {}
		ColorRGBA Color;
		bool AlphaBlend = false;
	};

	class UnlitNoDTMaterialConfig : public UnlitMaterialConfig
	{
	public:
		UnlitNoDTMaterialConfig() {}
		UnlitNoDTMaterialConfig(const ColorRGBA& color) :UnlitMaterialConfig(color)
		{ DepthTest = false; }
		virtual ~UnlitNoDTMaterialConfig() {}
	};

	class PhongMaterialConfig : public IGfxMaterialConfig
	{
	public:
		PhongMaterialConfig() {}
		PhongMaterialConfig(const ColorRGBA& color) : Diffuse (color), Ambient(color){}
		virtual ~PhongMaterialConfig() {}
		ColorRGBA Diffuse = {1,1,1,1};
		ColorRGBA Ambient = {1,1,1,1};
		ColorRGBA Specular = {1,1,1,1};
		ColorRGBA Emissive = {0,0,0,1 };
		float Shininess = 20;
	};

}