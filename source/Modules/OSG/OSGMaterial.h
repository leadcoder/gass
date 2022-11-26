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
#include "Modules/OSG/OSGCommon.h"
#include "Modules/OSG/OSGConvert.h"

const std::string base_vert =
#include "Modules/OSG/Shaders/Base.vert.glsl"
;

const std::string base_frag =
#include "Modules/OSG/Shaders/Base.frag.glsl"
;

namespace GASS
{
	namespace Material
	{

		enum TexUnits
		{
			ALBEDO_UNIT = 0,
			NORMAL_UNIT = 1,
		};

		inline void SetLighting(osg::StateSet* ss, osg::StateAttribute::Values value)
		{
			ss->setMode(GL_LIGHTING, osg::StateAttribute::ON);
			ss->setDefine("OSG_LIGHTING", value);
			ss->setDefine("OE_LIGHTING", value);
		}

		inline void SetCastShadows(osg::StateSet* ss, osg::StateAttribute::Values value)
		{
			ss->setDefine("OSG_CASTSHADOWS", value);
		}

		inline void SetReceiveShadows(osg::StateSet* ss, osg::StateAttribute::Values value)
		{
			ss->setDefine("OSG_RECEIVESHADOWS", value);
		}

		inline osg::Vec4ub ToOsgUB(ColorRGBA color)
		{
			return osg::Vec4ub(static_cast<osg::Vec4ub::value_type>(color.r * 255),
				static_cast<osg::Vec4ub::value_type>(color.g * 255),
				static_cast<osg::Vec4ub::value_type>(color.b * 255),
				static_cast<osg::Vec4ub::value_type>(color.a * 255));
		}

		static osg::Texture2D* CreateColorTexture2D(ColorRGBA color)
		{
			osg::ref_ptr<osg::Image> image = new osg::Image;
			image->allocateImage(1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE);
			*(osg::Vec4ub*)image->data() = ToOsgUB(color);
			auto texture = new osg::Texture2D(image.get());
			texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
			texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
			texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
			texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);
			return texture;
		}
		
	}
	
	class BaseMaterial : public osg::StateSet
	{
	public:
		BaseMaterial() : osg::StateSet()
		{

		}

		static osg::Texture2D* CreateTexture2D(osg::Image* image)
		{
			osg::Texture2D* texture = nullptr;
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
			setAlbedoMap(Material::CreateColorTexture2D(ColorRGBA(1,1,1,1)));
		}

		void setAlbedoMap(const std::string& filename)
		{
			auto texture = CreateTexture2D(osgDB::readRefImageFile(filename));
			if (texture)
				setAlbedoMap(texture);
		}

		void setAlbedoMap(osg::Texture2D* texture)
		{
			setDefine("OSG_ALBEDO_MAP");
			setTextureAttribute(Material::ALBEDO_UNIT, texture, osg::StateAttribute::ON);
			addUniform(new osg::Uniform("osg_AlbedoMap", Material::ALBEDO_UNIT));
		}

		void setNormalMap(const std::string &filename)
		{
			setDefine("OSG_NORMAL_MAP");
			auto texture = CreateTexture2D(osgDB::readRefImageFile(filename));
			setTextureAttribute(Material::NORMAL_UNIT, texture, osg::StateAttribute::ON);
			addUniform(new osg::Uniform("osg_NormalMap", Material::NORMAL_UNIT));
		}
	};


	class GenericMaterial : public osg::StateSet
	{
	public:
		virtual ~GenericMaterial() {}
		GenericMaterial()
		{

		}
		
	};

	class UnlitMaterial : public GenericMaterial
	{
	public:
		UnlitMaterial(const UnlitMaterialConfig& config)
		{
			Material::SetLighting(this, osg::StateAttribute::OFF);
			setDefine("OSG_ALBEDO_MAP");
			auto tex = Material::CreateColorTexture2D(config.Color);
			setTextureAttribute(Material::ALBEDO_UNIT, tex, osg::StateAttribute::ON);
			addUniform(new osg::Uniform("osg_AlbedoMap", Material::ALBEDO_UNIT));

			if (config.DepthTest)
				setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
			else
			{
				setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
				setRenderBinDetails(INT_MAX, "RenderBin");
			}

			if (config.AlphaBlend)
			{
				setAttributeAndModes(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA));
				setRenderBinDetails(11, "RenderBin");
			}
		}
	};

	class PhongMaterial : public GenericMaterial
	{
	public:
		PhongMaterial(const PhongMaterialConfig& config)
		{
			osg::ref_ptr<osg::Material> mat(new osg::Material);
			mat->setDiffuse(osg::Material::FRONT_AND_BACK, OSGConvert::ToOSG(config.Diffuse));
			mat->setAmbient(osg::Material::FRONT_AND_BACK, OSGConvert::ToOSG(config.Ambient));
			mat->setSpecular(osg::Material::FRONT_AND_BACK, OSGConvert::ToOSG(config.Specular));
			mat->setShininess(osg::Material::FRONT_AND_BACK, config.Shininess);
			mat->setEmission(osg::Material::FRONT_AND_BACK, OSGConvert::ToOSG(config.Emissive));
			setAttribute(mat);
			setAttributeAndModes(mat, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			Material::SetLighting(this, osg::StateAttribute::ON);
		}
	};
#if 0
	class FFPMaterial : public osg::StateSet
	{
	public:
		FFPMaterial(const GraphicsMaterial& config)
		{
			ColorRGBA diffuse = config.Diffuse;
			ColorRGB ambient = config.Ambient;
			ColorRGB specular = config.Specular;
			ColorRGB si = config.SelfIllumination;
			if (config.TrackVertexColor) //Use vertex color
			{
				Material::SetLighting(this, osg::StateAttribute::OFF);
				if (config.Transparent > 0)
				{
					setAttributeAndModes(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA));
					setRenderBinDetails(11, "RenderBin");
				}
			}
			else
			{
				osg::ref_ptr<osg::Material> mat(new osg::Material);
				mat->setDiffuse(osg::Material::FRONT_AND_BACK, OSGConvert::ToOSG(diffuse));
				mat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(static_cast<float>(ambient.r), static_cast<float>(ambient.g), static_cast<float>(ambient.b), 1));
				mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(static_cast<float>(specular.r), static_cast<float>(specular.g), static_cast<float>(specular.b), 1));
				mat->setShininess(osg::Material::FRONT_AND_BACK, config.Shininess);
				mat->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(static_cast<float>(si.r), static_cast<float>(si.g), static_cast<float>(si.b), 1));
				setAttribute(mat);
				setAttributeAndModes(mat, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
				Material::SetLighting(this, osg::StateAttribute::ON);
			}
			//mat->setColorMode(osg::Material::ColorMode::DIFFUSE); //Track vertex color

			if (config.DepthTest)
				setMode(GL_DEPTH_TEST, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			else
			{
				setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE);
				setRenderBinDetails(INT_MAX, "RenderBin");
			}

			//disabling depth write will effect GL_DEPTH_TEST some how,
			//osg::ref_ptr<osg::Depth> depth (new osg::Depth(Depth::LEQUAL,0.0,1.0,material.DepthWrite));
			/*	osg::ref_ptr<osg::Depth> depth (new osg::Depth());
				depth->setWriteMask( material.DepthWrite );
				state_set->setAttributeAndModes( depth, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			*/

			// Turn on blending
			if (!config.TrackVertexColor && diffuse.a < 1.0) //special handling if we have transparent a material,
			{
				//TODO: provide blending mode in material!
				osg::ref_ptr<osg::BlendFunc> bf(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA));
				setAttributeAndModes(bf);

				// Enable blending, select transparent bin.
				setMode(GL_BLEND, osg::StateAttribute::ON);
				setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

				// Enable depth test so that an opaque polygon will occlude a transparent one behind it.
				setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

				// Conversely, disable writing to depth buffer so that
				// a transparent polygon will allow polygons behind it to shine through.
				// OSG renders transparent polygons after opaque ones.
				osg::ref_ptr<osg::Depth> depth(new osg::Depth);
				depth->setWriteMask(false);
				setAttributeAndModes(depth, osg::StateAttribute::ON);
			}
		}
	};
#endif
	namespace Material
	{
		inline osg::StateSet* CreateFromConfig(IGfxMaterialConfig* config)
		{
			if (auto unlit = dynamic_cast<UnlitMaterialConfig*>(config))
			{
				return new UnlitMaterial(*unlit);
			}
			else if (auto phong = dynamic_cast<PhongMaterialConfig*>(config))
			{
				return new PhongMaterial(*phong);
			}
			return nullptr;
		}
	}
}