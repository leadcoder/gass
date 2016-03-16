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
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Core/Utils/GASSFileUtils.h"

namespace GASS
{
	OSGSkyboxComponent::OSGSkyboxComponent() : m_Size(200), 
		m_Node(NULL)
	{

	}

	OSGSkyboxComponent::~OSGSkyboxComponent()
	{

	}

	void OSGSkyboxComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("SkyboxComponent",new GASS::Creator<OSGSkyboxComponent, Component>);
		RegisterProperty<std::string>("Material", &OSGSkyboxComponent::GetMaterial, &OSGSkyboxComponent::SetMaterial);
		RegisterProperty<Float>("Size", &OSGSkyboxComponent::GetSize, &OSGSkyboxComponent::SetSize);
	}

	void OSGSkyboxComponent::OnInitialize()
	{
		OSGGraphicsSceneManagerPtr  scene_man = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>();
		osg::ref_ptr<osg::Group> root_node = scene_man->GetOSGRootNode();
		m_Node = _CreateSkyBox();
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

	std::string OSGSkyboxComponent::_GetTexturePath(const std::string &side) const
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

	osg::TextureCubeMap* OSGSkyboxComponent::_ReadCubeMap()
	{
		osg::TextureCubeMap* cubemap = new osg::TextureCubeMap();

		osgDB::ReaderWriter::Options* options = new osgDB::ReaderWriter::Options("dds_flip");


		osg::Image* imagePosX = osgDB::readImageFile(_GetTexturePath("east"),options);
		if(!imagePosX)
			imagePosX = osgDB::readImageFile(_GetTexturePath("rt"),options);

		osg::Image* imageNegX = osgDB::readImageFile(_GetTexturePath("west"),options);
		if(!imageNegX)
			imageNegX = osgDB::readImageFile(_GetTexturePath("lf"),options);

		osg::Image* imageNegY = osgDB::readImageFile(_GetTexturePath("up"),options);
		if(!imageNegY)
			imageNegY = osgDB::readImageFile(_GetTexturePath("up"),options);

		osg::Image* imagePosY = osgDB::readImageFile(_GetTexturePath("down"),options);
		if(!imagePosY)
			imagePosY = osgDB::readImageFile(_GetTexturePath("dn"),options);

		osg::Image* imagePosZ = osgDB::readImageFile(_GetTexturePath("north"),options);
		if(!imagePosZ)
			imagePosZ = osgDB::readImageFile(_GetTexturePath("fr"),options);

		osg::Image* imageNegZ = osgDB::readImageFile(_GetTexturePath("south"),options);
		if(!imageNegZ)
			imageNegZ= osgDB::readImageFile(_GetTexturePath("bk"),options);


		if (imagePosX && imageNegX && imagePosY && imageNegY && imagePosZ && imageNegZ)
		{
			cubemap->setImage(osg::TextureCubeMap::POSITIVE_X, imagePosX);
			cubemap->setImage(osg::TextureCubeMap::NEGATIVE_X, imageNegX);
			cubemap->setImage(osg::TextureCubeMap::POSITIVE_Y, imagePosY);
			cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Y, imageNegY);
			cubemap->setImage(osg::TextureCubeMap::POSITIVE_Z, imagePosZ);
			cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Z, imageNegZ);

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

		  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
		  {
			  osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
			  if (cv)
			  {
				  const osg::Matrixd& MV = *(cv->getModelViewMatrix());
				  const osg::Matrixd R = osg::Matrixd::rotate( osg::DegreesToRadians(112.0f), 0.0f,0.0f,1.0f)*
					  osg::Matrixd::rotate( osg::DegreesToRadians(90.0f), 1.0f,0.0f,0.0f);

				  osg::Quat q = MV.getRotate();
				  osg::Matrixd C = osg::Matrixd::rotate( q.inverse() );
				  //C.setTrans(osg::Vec3d(0,0,0));

				  _texMat.setMatrix( C*R );
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
		virtual bool computeLocalToWorldMatrix(osg::Matrixd& matrix,osg::NodeVisitor* nv) const
		{
			osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
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
		virtual bool computeWorldToLocalMatrix(osg::Matrixd& matrix,osg::NodeVisitor* nv) const
		{
			osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
			if (cv)
			{
				const osg::Vec3d eyePointLocal = cv->getEyeLocal();
				matrix.postMult(osg::Matrixd::translate(-eyePointLocal.x(),-eyePointLocal.y(),-eyePointLocal.z()));
			}
			return true;
		}
	};

	osg::Node* OSGSkyboxComponent::_CreateSkyBox()
	{
		osg::StateSet* stateset = new osg::StateSet();

		osg::TexEnv* te = new osg::TexEnv;
		te->setMode(osg::TexEnv::REPLACE);
		stateset->setTextureAttributeAndModes(0, te, osg::StateAttribute::ON);

		osg::TexGen *tg = new osg::TexGen;
		tg->setMode(osg::TexGen::NORMAL_MAP);
		stateset->setTextureAttributeAndModes(0, tg, osg::StateAttribute::ON);

		osg::TexMat *tm = new osg::TexMat;
		stateset->setTextureAttribute(0, tm);

		osg::TextureCubeMap* skymap = _ReadCubeMap();
		stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON);

		stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
		stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );

		// clear the depth to the far plane.
		osg::Depth* depth = new osg::Depth;
		depth->setFunction(osg::Depth::ALWAYS);
		depth->setRange(1.0,1.0);
		stateset->setAttributeAndModes(depth, osg::StateAttribute::ON );

		osg::ref_ptr<osg::Material> material = new osg::Material;
		material->setColorMode(osg::Material::DIFFUSE);
        material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
        material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
        material->setShininess(osg::Material::FRONT_AND_BACK, 64.0f);
        stateset->setAttributeAndModes(material.get(), osg::StateAttribute::ON);

		stateset->setRenderBinDetails(-1,"RenderBin");

		osg::Drawable* drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f), static_cast<float>(m_Size)));

		osg::Geode* geode = new osg::Geode;
		geode->setCullingActive(false);
		geode->setStateSet( stateset );
		geode->addDrawable(drawable);


		MyMoveEarthySkyWithEyePointTransform* transform = new MyMoveEarthySkyWithEyePointTransform;
		transform->m_Skybox = this;
		transform->setCullingActive(false);
		transform->addChild(geode);

		osg::ClearNode* clearNode = new osg::ClearNode;
		clearNode->setRequiresClear(false);
		clearNode->setCullCallback(new TexMatCallback(*tm));
		clearNode->addChild(transform);

		geode->setInitialBound(osg::BoundingSphere());
		geode->setNodeMask(~NM_RECEIVE_SHADOWS & geode->getNodeMask());
		geode->setNodeMask(~NM_CAST_SHADOWS & geode->getNodeMask());
		clearNode->setNodeMask(~NM_RECEIVE_SHADOWS & clearNode->getNodeMask());
		clearNode->setNodeMask(~NM_CAST_SHADOWS & clearNode->getNodeMask());
		transform->setNodeMask(~NM_RECEIVE_SHADOWS & transform->getNodeMask());
		transform->setNodeMask(~NM_CAST_SHADOWS & transform->getNodeMask());
		return clearNode;
	}
}
