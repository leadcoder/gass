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


#include "Plugins/OSG/Components/OSGSkyboxComponent.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Core/Utils/GASSFileUtils.h"

namespace GASS
{

	class NoAlias : public osg::Drawable::DrawCallback
	{
	public:
		void drawImplementation(osg::RenderInfo& renderInfo, const osg::Drawable* drawable) const override
		{
			//renderInfo.getState()->getAttributeDispatchers().setUseVertexAttribAlias(false);
			drawable->drawImplementation(renderInfo);
			//renderInfo.getState()->getAttributeDispatchers().setUseVertexAttribAlias(true);
			///renderInfo.getState()->setUseVertexAttributeAliasing(true);
		}


	};


	inline unsigned int calc_idx(unsigned int r, unsigned int c, unsigned int row_len)
	{
		return c + r * row_len;
	}

	osg::ref_ptr<osg::Geometry>  CreateSphere(float radius,
		unsigned int longitudeSteps,
		unsigned int lattitudeSteps,
		float longStart,
		float longEnd,
		float latStart,
		float latEnd)

	{
		osg::Vec3Array* vertices = new osg::Vec3Array();
		osg::Vec2Array* texcoords = new osg::Vec2Array();

		double x, y, z, t, p, sin_t, cos_t;

		double longInc = (longEnd - longStart) / (double)longitudeSteps;
		double latInc = (latEnd - latStart) / (double)lattitudeSteps;

		double theta = longStart, phi = latStart;

		float uScale = 1.f / longitudeSteps;
		float vScale = 1.f / lattitudeSteps;

		for (unsigned int i = 0; i <= longitudeSteps; ++i)
		{
			t = osg::DegreesToRadians(theta);
			sin_t = sin(t);
			cos_t = cos(t);

			for (unsigned int j = 0; j <= lattitudeSteps; ++j)
			{
				p = osg::DegreesToRadians(phi);

				x = radius * sin_t * cos(p);
				y = radius * sin_t * sin(p);
				z = radius * cos_t;

				vertices->push_back(osg::Vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)));
				texcoords->push_back(osg::Vec2(j * vScale, i * uScale));

				phi += latInc;
			}

			theta -= longInc;
			phi = latStart;
		}

		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();

		for (unsigned int r = 0; r <= longitudeSteps - 1; r += 1)
		{
			osg::DrawElementsUInt* indices =
				new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP, 0);

			for (unsigned int c = 0; c <= lattitudeSteps; c += 1)
			{
				indices->push_back(calc_idx(r, c, lattitudeSteps + 1));
				indices->push_back(calc_idx(r + 1, c, lattitudeSteps + 1));
			}

			geom->addPrimitiveSet(indices);
		}

		osg::Vec4Array* colors = new osg::Vec4Array();
		colors->push_back(osg::Vec4(1.f, 1.f, 1.f, 1.f));

		geom->setVertexArray(vertices);
		geom->setTexCoordArray(0, texcoords);
		geom->setColorArray(colors);
		geom->setColorBinding(osg::Geometry::BIND_OVERALL);

		return geom;
	}



	osg::ref_ptr<osg::Geometry>  CreateBox()
	{
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
		osg::ref_ptr<osg::DrawElementsUInt> de = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
		osg::ref_ptr<osg::Vec3Array> positions = new osg::Vec3Array();
		osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec2Array> tex_coords = new osg::Vec2Array;
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		
		colors->push_back(osg::Vec4(1, 1, 1, 1));
		geom->setColorArray(colors, osg::Array::BIND_OVERALL);

		GASS::Vec3f size(10,10,10);
		positions->push_back(osg::Vec3(-size.x, -size.y, -size.z));
		positions->push_back(osg::Vec3(-size.x, -size.y, -size.z));
		positions->push_back(osg::Vec3(-size.x, -size.y, -size.z));
		tex_coords->push_back(osg::Vec2(0, 0));
		tex_coords->push_back(osg::Vec2(0, 0));
		tex_coords->push_back(osg::Vec2(0, 0));
		positions->push_back(osg::Vec3(size.x, -size.y, -size.z));
		positions->push_back(osg::Vec3(size.x, -size.y, -size.z));
		positions->push_back(osg::Vec3(size.x, -size.y, -size.z));
		tex_coords->push_back(osg::Vec2(1, 0));
		tex_coords->push_back(osg::Vec2(1, 0));
		tex_coords->push_back(osg::Vec2(1, 0));

		positions->push_back(osg::Vec3(size.x, size.y, -size.z));
		positions->push_back(osg::Vec3(size.x, size.y, -size.z));
		positions->push_back(osg::Vec3(size.x, size.y, -size.z));
		tex_coords->push_back(osg::Vec2(1, 1));
		tex_coords->push_back(osg::Vec2(1, 1));
		tex_coords->push_back(osg::Vec2(1, 1));

		positions->push_back(osg::Vec3(-size.x, size.y, -size.z));
		positions->push_back(osg::Vec3(-size.x, size.y, -size.z));
		positions->push_back(osg::Vec3(-size.x, size.y, -size.z));
		tex_coords->push_back(osg::Vec2(0, 1));
		tex_coords->push_back(osg::Vec2(0, 1));
		tex_coords->push_back(osg::Vec2(0, 1));


		positions->push_back(osg::Vec3(-size.x, -size.y, size.z));
		positions->push_back(osg::Vec3(-size.x, -size.y, size.z));
		positions->push_back(osg::Vec3(-size.x, -size.y, size.z));
		tex_coords->push_back(osg::Vec2(1, 0));
		tex_coords->push_back(osg::Vec2(1, 0));
		tex_coords->push_back(osg::Vec2(1, 0));


		positions->push_back(osg::Vec3(size.x, -size.y, size.z));
		positions->push_back(osg::Vec3(size.x, -size.y, size.z));
		positions->push_back(osg::Vec3(size.x, -size.y, size.z));
		tex_coords->push_back(osg::Vec2(0, 0));
		tex_coords->push_back(osg::Vec2(0, 0));
		tex_coords->push_back(osg::Vec2(0, 0));

		positions->push_back(osg::Vec3(size.x, size.y, size.z));
		positions->push_back(osg::Vec3(size.x, size.y, size.z));
		positions->push_back(osg::Vec3(size.x, size.y, size.z));
		tex_coords->push_back(osg::Vec2(0, 1));
		tex_coords->push_back(osg::Vec2(0, 1));
		tex_coords->push_back(osg::Vec2(0, 1));


		positions->push_back(osg::Vec3(-size.x, size.y, size.z));
		positions->push_back(osg::Vec3(-size.x, size.y, size.z));
		positions->push_back(osg::Vec3(-size.x, size.y, size.z));
		tex_coords->push_back(osg::Vec2(1, 0));
		tex_coords->push_back(osg::Vec2(1, 0));
		tex_coords->push_back(osg::Vec2(1, 0));

		normals->push_back(osg::Vec3(0, 0, -1));
		normals->push_back(osg::Vec3(-1, 0, 0));
		normals->push_back(osg::Vec3(0, -1, 0));
		normals->push_back(osg::Vec3(0, 0, -1));
		normals->push_back(osg::Vec3(1, 0, 0));
		normals->push_back(osg::Vec3(0, -1, 0));
		normals->push_back(osg::Vec3(0, 0, -1));
		normals->push_back(osg::Vec3(1, 0, 0));
		normals->push_back(osg::Vec3(0, 1, 0));
		normals->push_back(osg::Vec3(0, 0, -1));
		normals->push_back(osg::Vec3(-1, 0, 0));
		normals->push_back(osg::Vec3(0, 1, 0));
		normals->push_back(osg::Vec3(0, 0, 1));
		normals->push_back(osg::Vec3(-1, 0, 0));
		normals->push_back(osg::Vec3(0, -1, 0));
		normals->push_back(osg::Vec3(0, 0, 1));
		normals->push_back(osg::Vec3(1, -0, 0));
		normals->push_back(osg::Vec3(0, -1, 0));
		normals->push_back(osg::Vec3(0, 0, 1));
		normals->push_back(osg::Vec3(1, 0, 0));
		normals->push_back(osg::Vec3(0, 1, 0));
		normals->push_back(osg::Vec3(-0, 0, 1));
		normals->push_back(osg::Vec3(-1, -0, -0));
		normals->push_back(osg::Vec3(0, 1, 0));

		//bottom
		de->push_back(0);
		de->push_back(9);
		de->push_back(3);
		de->push_back(9);
		de->push_back(6);
		de->push_back(3);

		// top
		de->push_back(21);  //face 3
		de->push_back(12);
		de->push_back(18);
		de->push_back(12);  //face 4
		de->push_back(15);
		de->push_back(18);
		// left
		de->push_back(22);  //face 5
		de->push_back(10);
		de->push_back(13);
		de->push_back(10);  //face 6
		de->push_back(1);
		de->push_back(13);
		// right
		de->push_back(16);  //face 7
		de->push_back(4);
		de->push_back(19);
		de->push_back(4);  //face 8
		de->push_back(7);
		de->push_back(19);
		// front
		de->push_back(14);  //face 9
		de->push_back(2);
		de->push_back(17);
		de->push_back(2);   //face 10
		de->push_back(5);
		de->push_back(17);
		// back
		de->push_back(20);  //face 11
		de->push_back(8);
		de->push_back(23);
		de->push_back(8);   //face 12
		de->push_back(11);
		de->push_back(23);

		geom->setVertexArray(positions);
		geom->setNormalArray(normals,osg::Array::BIND_PER_VERTEX);
		geom->setTexCoordArray(0, tex_coords, osg::Array::BIND_PER_VERTEX);
		geom->addPrimitiveSet(de);
		return geom;
	}


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

		
		//lc->GetOSGNode()->addChild(m_Node);
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



	// Update texture matrix for cubemaps
	struct TexMatCallback : public osg::NodeCallback
	{
	public:

		TexMatCallback(osg::TexMat& tm) :
		  _texMat(tm)
		  {
		  }

		  void operator()(osg::Node* node, osg::NodeVisitor* nv) override
		  {
			  auto* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
			  if (cv)
			  {
				  const osg::Matrixd& mv = *(cv->getModelViewMatrix());
				  const osg::Matrixd r = osg::Matrixd::rotate( osg::DegreesToRadians(0.0f), 0.0f,0.0f,1.0f)*
					  osg::Matrixd::rotate( osg::DegreesToRadians(90.0f), 1.0f,0.0f,0.0f);

				  osg::Quat q = mv.getRotate();
				  osg::Matrixd c = osg::Matrixd::rotate( q.inverse() );
				  //C.setTrans(osg::Vec3d(0,0,0));

				  _texMat.setMatrix( c*r );
			  }

			  traverse(node,nv);
		  }

		  osg::TexMat& _texMat;
	};

	class MyMoveEarthySkyWithEyePointTransform : public osg::PositionAttitudeTransform
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

		auto* te = new osg::TexEnv;
		te->setMode(osg::TexEnv::REPLACE);
		stateset->setTextureAttributeAndModes(0, te, osg::StateAttribute::ON);

		auto *tg = new osg::TexGen;
		tg->setMode(osg::TexGen::NORMAL_MAP);
		stateset->setTextureAttributeAndModes(0, tg, osg::StateAttribute::ON);

		auto *tm = new osg::TexMat;
		stateset->setTextureAttribute(0, tm);

		osg::TextureCubeMap* skymap = ReadCubeMap();
		stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON);

		stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
		stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );

		// clear the depth to the far plane.
		auto* depth = new osg::Depth;
		depth->setFunction(osg::Depth::ALWAYS);
		depth->setRange(1.0,1.0);
		stateset->setAttributeAndModes(depth, osg::StateAttribute::ON );

		/*osg::ref_ptr<osg::Material> material = new osg::Material;
		material->setColorMode(osg::Material::DIFFUSE);
        material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
        material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
        material->setShininess(osg::Material::FRONT_AND_BACK, 64.0f);
        stateset->setAttributeAndModes(material.get(), osg::StateAttribute::ON);
		*/
		stateset->setRenderBinDetails(-1,"RenderBin");
		//stateset->setAttribute(new osg::Program(), osg::StateAttribute::PROTECTED);
		stateset->setAttributeAndModes(createShader(), osg::StateAttribute::ON);
		
		auto* geode = new osg::Geode;
		geode->setStateSet(stateset);
		
		geode->setCullingActive(false);
		geode->setNodeMask(~NM_RECEIVE_SHADOWS & geode->getNodeMask());
		geode->setNodeMask(~NM_CAST_SHADOWS & geode->getNodeMask());
#if 0
		//osg::Drawable* drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), static_cast<float>(m_Size)));
		//osg::Drawable* drawable = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f), static_cast<float>(m_Size)));
		//drawable->setDrawCallback(new NoAlias());
#else	
		auto drawable = CreateBox();
		//auto drawable = CreateSphere(static_cast<float>(m_Size), 10, 10, 0.f, 180.f, 0.f, 360.f);
#endif
		//drawable->setCullingActive(false);
		drawable->setInitialBound(osg::BoundingBox());

		geode->addDrawable(drawable);
		geode->setInitialBound(osg::BoundingSphere());

		auto* sky_transform = new MyMoveEarthySkyWithEyePointTransform;
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
		
		clear_node->setCullCallback(new TexMatCallback(*tm));
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
