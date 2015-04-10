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

#include "Plugins/OSG/Components/OSGSkyboxComponent.h"
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/AlphaFunc>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/TextureCubeMap>
#include <osg/TexEnv>
#include <osg/Depth>
#include <osg/StateSet>
#include <osg/TexMat>
#include <osg/TexGen>
#include <osg/Material>
#include <osg/TexEnvCombine>
#include <osg/ShapeDrawable>
#include <osgUtil/CullVisitor>
#include <osgShadow/ShadowTechnique>
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"

#include "Plugins/OSG/Components/OSGLocationComponent.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/OSGNodeMasks.h"
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
		GASS::ComponentFactory::GetPtr()->Register("SkyboxComponent",new GASS::Creator<OSGSkyboxComponent, Component>);
		RegisterProperty<std::string>("Material", &GetMaterial, &SetMaterial);
	}

	void OSGSkyboxComponent::OnInitialize()
	{
		OSGGraphicsSceneManagerPtr  scene_man = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>();
		osg::ref_ptr<osg::Group> root_node = scene_man->GetOSGRootNode();
		root_node->addChild(CreateSkyBox());
	}

	void OSGSkyboxComponent::OnDelete()
	{
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
		osg::TextureCubeMap* cubemap = new osg::TextureCubeMap();

		osgDB::ReaderWriter::Options* options = new osgDB::ReaderWriter::Options("dds_flip");

		
		osg::Image* imagePosX = osgDB::readImageFile(GetTexturePath("east"),options);
		if(!imagePosX)
			imagePosX = osgDB::readImageFile(GetTexturePath("rt"),options);
		
		osg::Image* imageNegX = osgDB::readImageFile(GetTexturePath("west"),options);
		if(!imageNegX)
			imageNegX = osgDB::readImageFile(GetTexturePath("lf"),options);
		
		osg::Image* imageNegY = osgDB::readImageFile(GetTexturePath("up"),options);
		if(!imageNegY)
			imageNegY = osgDB::readImageFile(GetTexturePath("up"),options);

		osg::Image* imagePosY = osgDB::readImageFile(GetTexturePath("down"),options);
		if(!imagePosY)
			imagePosY = osgDB::readImageFile(GetTexturePath("dn"),options);

		osg::Image* imagePosZ = osgDB::readImageFile(GetTexturePath("north"),options);
		if(!imagePosZ)
			imagePosZ = osgDB::readImageFile(GetTexturePath("fr"),options);

		osg::Image* imageNegZ = osgDB::readImageFile(GetTexturePath("south"),options);
		if(!imageNegZ)
			imageNegZ= osgDB::readImageFile(GetTexturePath("bk"),options);


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
				  const osg::Matrix& MV = *(cv->getModelViewMatrix());
				  const osg::Matrix R = osg::Matrix::rotate( osg::DegreesToRadians(112.0f), 0.0f,0.0f,1.0f)*
					  osg::Matrix::rotate( osg::DegreesToRadians(90.0f), 1.0f,0.0f,0.0f);

				  osg::Quat q = MV.getRotate();
				  const osg::Matrix C = osg::Matrix::rotate( q.inverse() );

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
		/** Get the transformation matrix which moves from local coords to world coords.*/
		virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const 
		{
			osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
			if (cv)
			{
				
				//Vec3 pos = m_Skybox->GetEyePosition();
				//osg::Vec3 eyePointLocal = OSGConvert::Get().ToOSG(pos);
				osg::Vec3 eyePointLocal = cv->getEyePoint();
				
				matrix.preMult(osg::Matrix::translate(eyePointLocal.x(),eyePointLocal.y(),eyePointLocal.z()));
				//matrix.preMultTranslate(eyePointLocal);
			}
			return true;
		}

		/** Get the transformation matrix which moves from world coords to local coords.*/
		virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
		{
			osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
			if (cv)
			{
				//osg::Vec3 eyePointLocal(1.52206e+006,6.49688e+006,10);
				osg::Vec3 eyePointLocal = cv->getEyeLocal();
				//Vec3 pos = m_Skybox->GetEyePosition();
				//osg::Vec3 eyePointLocal(pos.x,pos.y,pos.z);// =
				matrix.postMult(osg::Matrixd::translate(-eyePointLocal.x(),-eyePointLocal.y(),-eyePointLocal.z()));
				//matrix.postMultTranslate(-eyePointLocal);
			}
			return true;
		}
	};

	/*Vec3 OSGSkyboxComponent::GetEyePosition()
	{
		SceneObjectPtr cam_obj (m_ActiveCameraObject,NO_THROW);
		Vec3 pos(0,0,0);
		if(cam_obj)
		{
			LocationComponentPtr lc = cam_obj->GetFirstComponentByClass<ILocationComponent>();
			if(lc)
			{
				pos = lc->GetWorldPosition();

			}
		}
		return pos;
	}*/
	


	osg::Node* OSGSkyboxComponent::CreateSkyBox()
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

		osg::TextureCubeMap* skymap = ReadCubeMap();
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

		osg::Drawable* drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),300));

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

		geode->setNodeMask(~NM_RECEIVE_SHADOWS & geode->getNodeMask());
		geode->setNodeMask(~NM_CAST_SHADOWS & geode->getNodeMask());
		clearNode->setNodeMask(~NM_RECEIVE_SHADOWS & clearNode->getNodeMask());
		clearNode->setNodeMask(~NM_CAST_SHADOWS & clearNode->getNodeMask());
		transform->setNodeMask(~NM_RECEIVE_SHADOWS & transform->getNodeMask());
		transform->setNodeMask(~NM_CAST_SHADOWS & transform->getNodeMask());

		return clearNode;
	}
}
