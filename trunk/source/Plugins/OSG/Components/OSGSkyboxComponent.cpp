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

#include <boost/bind.hpp>
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
#include "Plugins/OSG/Components/OSGSkyboxComponent.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/OSGNodeMasks.h"




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
		GASS::ComponentFactory::GetPtr()->Register("SkyboxComponent",new GASS::Creator<OSGSkyboxComponent, IComponent>);
		RegisterProperty<std::string>("Material", &GetMaterial, &SetMaterial);
	}


	void OSGSkyboxComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGSkyboxComponent::OnLoad,LoadComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGSkyboxComponent::OnUnload,UnloadComponentsMessage,1));
	}
	
	void OSGSkyboxComponent::OnChangeCamera(CameraChangedNotifyMessagePtr message)
	{
		SceneObjectPtr cam_obj = message->GetCamera();
		m_ActiveCameraObject = cam_obj;
	}

	void OSGSkyboxComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		GetSceneObject()->GetScene()->UnregisterForMessage(UNREG_TMESS( OSGSkyboxComponent::OnChangeCamera,CameraChangedNotifyMessage));
	}

	void OSGSkyboxComponent::OnLoad(LoadComponentsMessagePtr message)
	{
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS( OSGSkyboxComponent::OnChangeCamera,CameraChangedNotifyMessage,0));

		OSGGraphicsSceneManagerPtr  scene_man = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>();
		osg::ref_ptr<osg::Group> root_node = scene_man->GetOSGRootNode();
		root_node->addChild(CreateSkyBox());


		//check if dds, then flip texcoords

	/*	m_OSGBillboard = new osg::Billboard();
		m_OSGBillboard->setMode(osg::Billboard::POINT_ROT_EYE);
		m_OSGBillboard->addDrawable(
			CreateSquare(osg::Vec3(-0.5f,0.0f,0.0f),
			osg::Vec3(1.0f,0.0f,0.0f)*m_Width,
			osg::Vec3(0.0f,0.0f,1.0f)*m_Height,
			//osgDB::readImageFile("Images/reflect.rgb")),
			osgDB::readImageFile(full_path)).get(),
			osg::Vec3(0.0f,0.0f,0.0f));

		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		lc->GetOSGNode()->addChild(m_OSGBillboard.get());*/

	}


	std::string OSGSkyboxComponent::GetTexturePath(const std::string &side) const
	{
		std::string extension = Misc::GetExtension(m_Material);
		std::string name = Misc::RemoveExtension(m_Material);
		
		std::string full_path = name + side;
		full_path += ".";
		full_path += extension;
		
		
		ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>();
		if(!rs->GetFullPath(full_path,full_path))
		{
			GASS_EXCEPT(Exception::ERR_FILE_NOT_FOUND,"Failed to find texture: " + full_path,"OSGSkyboxComponent::GetTexturePath");
		}
		return full_path;
	}

	osg::TextureCubeMap* OSGSkyboxComponent::ReadCubeMap()
	{
		osg::TextureCubeMap* cubemap = new osg::TextureCubeMap();
		
		osg::Image* imagePosX = osgDB::readImageFile(GetTexturePath("east"));
		osg::Image* imageNegX = osgDB::readImageFile(GetTexturePath("west"));
		osg::Image* imagePosY = osgDB::readImageFile(GetTexturePath("up"));
		osg::Image* imageNegY = osgDB::readImageFile(GetTexturePath("down"));
		osg::Image* imagePosZ = osgDB::readImageFile(GetTexturePath("north"));
		osg::Image* imageNegZ = osgDB::readImageFile(GetTexturePath("south"));

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

	Vec3 OSGSkyboxComponent::GetEyePosition()
	{
		SceneObjectPtr cam_obj (m_ActiveCameraObject,boost::detail::sp_nothrow_tag());
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
	}
	


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

		/*osg::ref_ptr<osg::Material> matirial = new osg::Material;
		matirial->setColorMode(osg::Material::DIFFUSE);
        matirial->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
        matirial->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
        matirial->setShininess(osg::Material::FRONT_AND_BACK, 64.0f);
        stateset->setAttributeAndModes(matirial.get(), osg::StateAttribute::ON);*/


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
