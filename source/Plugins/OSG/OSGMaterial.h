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
#include "Plugins/OSG/OSGCommon.h"

const std::string base_vert =
#include "Plugins/OSG/Shaders/Base.vert.glsl"
;

const std::string base_frag =
#include "Plugins/OSG/Shaders/Base.frag.glsl"
;

namespace GASS
{
	class BaseMaterial : public osg::StateSet
	{
	public:
		BaseMaterial() : osg::StateSet()
		{

		}

		osg::ref_ptr<osg::Texture2D> CreateTexture2D(osg::Image* image)
		{
			osg::ref_ptr<osg::Texture2D> texture;
			if (image)
			{
				texture = new osg::Texture2D;
				texture->setImage(image);
				texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
				texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
				texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
				texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
				texture->setMaxAnisotropy(16.0f);
			}
			return texture;
		}

		osg::ref_ptr<osg::Texture2D> CreateDummyTexture2D(osg::Image* image)
		{
			osg::ref_ptr<osg::Texture2D> texture;
			if (image)
			{
				texture = new osg::Texture2D;
				texture->setImage(image);
				texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
				texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
				texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
				texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
				texture->setMaxAnisotropy(16.0f);
			}
			return texture;
		}
	};

	class SimpleMaterial : public BaseMaterial
	{
	public:
		SimpleMaterial() : BaseMaterial()
		{
			osg::Program* program = new osg::Program;
			program->addShader(new osg::Shader(osg::Shader::VERTEX, base_vert.c_str()));
			program->addShader(new osg::Shader(osg::Shader::FRAGMENT, base_frag.c_str()));
			setAttribute(program, osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
			//setDefine("OSG_NORMAL_MAP");
			//const int ntex_unit = 1;
			//auto texture = CreateTexture2D(osgDB::readRefImageFile("D:/dev_zone/osgpbr/osgEffect/effectcompositor/data/rusted_iron/normal.png"));
			//setTextureAttribute(ntex_unit, texture, osg::StateAttribute::ON);
			//addUniform(new osg::Uniform("osg_NormalMap", ntex_unit));
		}
	};
}