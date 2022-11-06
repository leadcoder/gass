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


#include "Modules/OSG/Components/OSGSkyboxComponent.h"
#include "Modules/OSG/Components/OSGLocationComponent.h"
#include "Modules/OSG/OSGGraphicsSystem.h"
#include "Modules/OSG/OSGGraphicsSceneManager.h"
#include "Modules/OSG/OSGNodeMasks.h"
#include "Core/Utils/GASSFileUtils.h"

namespace GASS
{
	OSGSkyboxComponent::OSGSkyboxComponent()  
	{

	}

	OSGSkyboxComponent::~OSGSkyboxComponent()
	{

	}

	void OSGSkyboxComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OSGSkyboxComponent>("SkyboxComponent");
		RegisterGetSet("Material", &OSGSkyboxComponent::GetMaterial, &OSGSkyboxComponent::SetMaterial);
		RegisterMember("Size", &OSGSkyboxComponent::m_Size);
	}

	void OSGSkyboxComponent::OnInitialize()
	{
		OSGGraphicsSceneManagerPtr  scene_man = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>();
		osg::ref_ptr<osg::Group> root_node = scene_man->GetOSGRootNode();
		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		m_Node = CreateSkyBox();
		root_node->addChild(m_Node);
	}

	void OSGSkyboxComponent::OnDelete()
	{
		OSGGraphicsSceneManagerPtr  scene_man = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>();
		osg::ref_ptr<osg::Group> root_node = scene_man->GetOSGRootNode();
		if (m_Node)
		{
			root_node->removeChild(m_Node);
		}
	}

	std::string OSGSkyboxComponent::GetTexturePath(const std::string &side) const
	{
		std::string extension = FileUtils::GetExtension(m_Material);
		std::string name = FileUtils::RemoveExtension(m_Material);

		std::string full_path = name + side;
		full_path += ".";
		full_path += extension;

		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		if(rm->HasResource(full_path))
			full_path = rm->GetFirstResourceByName(full_path)->Path().GetFullPath();
		return full_path;
	}

	osg::TextureCubeMap* OSGSkyboxComponent::ReadCubeMap()
	{
		auto* cubemap = new osg::TextureCubeMap();

		auto* options = new osgDB::ReaderWriter::Options("dds_flip");

		osg::Image* image_pos_x = osgDB::readImageFile(GetTexturePath("east"),options);
		if(!image_pos_x)
			image_pos_x = osgDB::readImageFile(GetTexturePath("rt"),options);

		osg::Image* image_neg_x = osgDB::readImageFile(GetTexturePath("west"),options);
		if(!image_neg_x)
			image_neg_x = osgDB::readImageFile(GetTexturePath("lf"),options);

		osg::Image* image_neg_y = osgDB::readImageFile(GetTexturePath("up"),options);
		if(!image_neg_y)
			image_neg_y = osgDB::readImageFile(GetTexturePath("up"),options);

		osg::Image* image_pos_y = osgDB::readImageFile(GetTexturePath("down"),options);
		if(!image_pos_y)
			image_pos_y = osgDB::readImageFile(GetTexturePath("dn"),options);

		osg::Image* image_pos_z = osgDB::readImageFile(GetTexturePath("north"),options);
		if(!image_pos_z)
			image_pos_z = osgDB::readImageFile(GetTexturePath("fr"),options);

		osg::Image* image_neg_z = osgDB::readImageFile(GetTexturePath("south"),options);
		if(!image_neg_z)
			image_neg_z= osgDB::readImageFile(GetTexturePath("bk"),options);


		if (image_pos_x && image_neg_x && image_pos_y && image_neg_y && image_pos_z && image_neg_z)
		{
			cubemap->setImage(osg::TextureCubeMap::POSITIVE_X, image_pos_x);
			cubemap->setImage(osg::TextureCubeMap::NEGATIVE_X, image_neg_x);
			cubemap->setImage(osg::TextureCubeMap::POSITIVE_Y, image_pos_y);
			cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Y, image_neg_y);
			cubemap->setImage(osg::TextureCubeMap::POSITIVE_Z, image_pos_z);
			cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Z, image_neg_z);

			cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
			cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
			cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

			cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
			cubemap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
		}
		return cubemap;
	}

	class MoveEarthySkyWithEyePointTransform : public osg::PositionAttitudeTransform
	{
	public:
		OSGSkyboxComponent *m_Skybox;
		/** Get the transformation matrix which moves from local coordinates to world coordinates.*/
		bool computeLocalToWorldMatrix(osg::Matrixd& matrix,osg::NodeVisitor* nv) const override
		{
			auto* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
			if (cv)
			{
			
				//const osg::Vec3d eyePointLocal = cv->getEyePoint();
				//matrix.preMult(osg::Matrixd::translate(eyePointLocal.x(),eyePointLocal.y(),eyePointLocal.z()));
				//replaced from code above to avoid jitter at large camera coordinates, always put skybox at 0,0,0?
				matrix.setTrans(0,0,0);
			}
			return true;
		}

		/** Get the transformation matrix which moves from world coordinates to local coordinates.*/
		bool computeWorldToLocalMatrix(osg::Matrixd& matrix,osg::NodeVisitor* nv) const override
		{
			auto* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
			if (cv)
			{
				const osg::Vec3d eye_point_local = cv->getEyeLocal();
				matrix.postMult(osg::Matrixd::translate(-eye_point_local.x(),-eye_point_local.y(),-eye_point_local.z()));
			}
			return true;
		}
	};

	osg::ref_ptr<osg::Program> createShader(void)
	{
		osg::ref_ptr<osg::Program> program = new osg::Program;

		// Do not use shaders if they were globally disabled.
		if (true)
		{
			const char vertexSource[] = R"(
				varying vec3 vTexCoord;
				
				void main(void)
				{
				    gl_Position = ftransform();
				    vTexCoord = gl_Vertex.xyz;
				}
			)";

			const char fragmentSource[] = R"(
				uniform samplerCube uEnvironmentMap;
				varying vec3 vTexCoord;
				void main(void)
				{
				    vec3 tex = vec3(vTexCoord.x, vTexCoord.y, -vTexCoord.z);
				    gl_FragColor = textureCube( uEnvironmentMap, tex.xzy );\
				    gl_FragColor.a = 0.0;
				}
			)";

			program->setName("sky_dome_shader");
			program->addShader(new osg::Shader(osg::Shader::VERTEX, vertexSource));
			program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragmentSource));
		}

		return program;
	}

	osg::Node* OSGSkyboxComponent::CreateSkyBox()
	{
		auto* stateset = new osg::StateSet();

		osg::TextureCubeMap* skymap = ReadCubeMap();
		stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON);

		stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
		stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );

		// clear the depth to the far plane.
		auto* depth = new osg::Depth;
		depth->setFunction(osg::Depth::ALWAYS);
		depth->setRange(1.0,1.0);
		stateset->setAttributeAndModes(depth, osg::StateAttribute::ON );
		stateset->setRenderBinDetails(-1,"RenderBin");
		stateset->setAttributeAndModes(createShader(), osg::StateAttribute::ON);
		
		auto* geode = new osg::Geode;
		geode->setStateSet(stateset);
		geode->setCullingActive(false);
		geode->setNodeMask(~NM_RECEIVE_SHADOWS & geode->getNodeMask());
		geode->setNodeMask(~NM_CAST_SHADOWS & geode->getNodeMask());
		//osg::Drawable* drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), static_cast<float>(m_Size)));
		osg::Drawable* drawable = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f), static_cast<float>(m_Size)));

		drawable->setInitialBound(osg::BoundingBox());

		geode->addDrawable(drawable);
		geode->setInitialBound(osg::BoundingSphere());

		auto* sky_transform = new MoveEarthySkyWithEyePointTransform;
		sky_transform->m_Skybox = this;
		sky_transform->addChild(geode);
		sky_transform->setNodeMask(~NM_RECEIVE_SHADOWS & sky_transform->getNodeMask());
		sky_transform->setNodeMask(~NM_CAST_SHADOWS & sky_transform->getNodeMask());
		sky_transform->setCullingActive(false);
		sky_transform->setInitialBound(osg::BoundingBox());

		auto* offset_t = new osg::PositionAttitudeTransform();
		//move below ground to avoid camera intersection
		offset_t->setPosition(osg::Vec3(0, 0, -1000));
		offset_t->addChild(sky_transform);

		auto* clear_node = new osg::ClearNode;
		clear_node->setRequiresClear(false);
		
		clear_node->addChild(offset_t);
		clear_node->setNodeMask(~NM_RECEIVE_SHADOWS & clear_node->getNodeMask());
		clear_node->setNodeMask(~NM_CAST_SHADOWS & clear_node->getNodeMask());
		clear_node->setCullingActive(false);
		clear_node->setInitialBound(osg::BoundingBox());

		///return clearNode;
		// A nested camera isolates the projection matrix calculations so the node won't 
		// affect the clip planes in the rest of the scene.
		auto* cam = new osg::Camera();
		cam->getOrCreateStateSet()->setRenderBinDetails(-100000, "RenderBin");
		cam->setRenderOrder(osg::Camera::NESTED_RENDER);
		cam->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
		cam->addChild(clear_node);
		return cam;
	}
}
