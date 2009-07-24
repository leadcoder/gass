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
#include <osg/Billboard>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/AlphaFunc>

#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Core/Utils/Log.h"
#include "Sim/SimEngine.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/Components/OSGBillboardComponent.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"



using namespace Ogre;

namespace GASS
{

	OSGBillboardComponent::OSGBillboardComponent() : m_CastShadow(true),
		m_OSGBillboard (NULL),
		m_Width(1.0f),
		m_Height(1.0f)
	{

	}	

	OSGBillboardComponent::~OSGBillboardComponent()
	{
		m_OSGBillboard.release();
	}

	void OSGBillboardComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("BillboardComponent",new GASS::Creator<OSGBillboardComponent, IComponent>);
		RegisterProperty<std::string>("Material", &GetMaterial, &SetMaterial);
		RegisterProperty<bool>("CastShadow", &GetCastShadow, &SetCastShadow);
	}

	void OSGBillboardComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_LOAD_GFX_COMPONENTS, MESSAGE_FUNC( OSGBillboardComponent::OnLoad),1);
		//mm.RegisterForMessage(MESSAGE_UPDATE, address,  boost::bind( &LocationComponent::OnUpdate, this, _1 ),m_InitPriority);
	}

	void OSGBillboardComponent::OnLoad(MessagePtr message)
	{
		OSGGraphicsSceneManager* ogsm = boost::any_cast<OSGGraphicsSceneManager*>(message->GetData("GraphicsSceneManager"));
		assert(ogsm);

		std::string full_path;
		ResourceSystemPtr rs = SimEngine::GetPtr()->GetSystemManager()->GetFirstSystem<IResourceSystem>();
		if(!rs->GetFullPath(m_Material,full_path))
		{
			Log::Error("Failed to find texture:%s",full_path.c_str());
		}


		m_OSGBillboard = new osg::Billboard();
		m_OSGBillboard->setMode(osg::Billboard::POINT_ROT_EYE);
		m_OSGBillboard->addDrawable(
			CreateSquare(osg::Vec3(-0.5f,0.0f,0.0f),
			osg::Vec3(1.0f,0.0f,0.0f)*m_Width,
			osg::Vec3(0.0f,0.0f,1.0f)*m_Height,
			//osgDB::readImageFile("Images/reflect.rgb")),
			osgDB::readImageFile(full_path)).get(),
			osg::Vec3(0.0f,0.0f,0.0f));

		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponent<OSGLocationComponent>();
		lc->GetOSGNode()->addChild(m_OSGBillboard.get());


		


		

		/*if(m_Material != "")
		{
		std::string material_name = m_Material;
		Ogre::MaterialPtr material;
		if(Ogre::MaterialManager::getSingleton().resourceExists(material_name)) material = Ogre::MaterialManager::getSingleton().getByName(material_name);
		else 
		{
		material = Ogre::MaterialManager::getSingleton().create(material_name, "GASS",false, 0); // Manual, loader
		// Remove pre-created technique from defaults
		material->removeAllTechniques();
		// Create a techinique and a pass and a texture unit
		Ogre::Technique * technique = material->createTechnique();
		Ogre::Pass* pass = technique->createPass();
		material->setLightingEnabled(false);
		material->setDepthWriteEnabled(true);
		material->setCullingMode(Ogre::CULL_NONE);
		std::string fullpath;
		IResourceSystem* rs = SimEngine::GetPtr()->GetSystemManager()->GetFirstSystem<IResourceSystem>().get();

		if(rs->GetFullPath(m_Material,fullpath))
		{
		Ogre::TextureUnitState * textureUnit = pass->createTextureUnitState(m_Material,0);
		//pass->setSceneBlending(Ogre::SBF_SOURCE_ALPHA,Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
		//pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
		pass->setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL, 128);
		}
		}


		m_BillboardSet = ogsm->GetSceneManger()->createBillboardSet(name);
		m_BillboardSet->setMaterialName(material_name);

		Vec3 pos = Vec3(0,m_Height/2.0,0);
		Ogre::ColourValue color = Ogre::ColourValue::White;
		Ogre::Billboard* billboard = m_BillboardSet->createBillboard(Convert::ToOgre(pos),color);
		billboard->mPosition = Convert::ToOgre(pos);
		billboard->setColour(color);
		billboard->setDimensions(m_Width,m_Height);
		billboard->setTexcoordRect(0, 0,1, 1);
		float bbsize = m_Height;
		if(m_Width > m_Height) bbsize = m_Width;
		bbsize *=  0.5f;
		m_BillboardSet->setBounds(Ogre::AxisAlignedBox(Ogre::Vector3(-bbsize,-bbsize + pos.y,-bbsize),Ogre::Vector3(bbsize,bbsize+ pos.y,bbsize)),bbsize*2);
		lc->GetOgreNode()->attachObject((Ogre::MovableObject*) m_BillboardSet);
		}*/
	}

	AABox OSGBillboardComponent::GetBoundingBox() const
	{
		float max = Math::Max(m_Width,m_Height);
		AABox box(Vec3(-max/2.0,-max/2.0,-max/2.0),Vec3(max/2.0,max/2.0,max/2.0));
		return box;

	}
	Sphere OSGBillboardComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		sphere.m_Pos = Vec3(0,0,0);
		sphere.m_Radius = Math::Max(m_Width,m_Height)/2.0;
		return sphere;
	}

	void OSGBillboardComponent::GetMeshData(MeshDataPtr mesh_data)
	{

	}

	//From OSG billboard example
	osg::ref_ptr<osg::Geometry> OSGBillboardComponent::CreateSquare(const osg::Vec3& corner,const osg::Vec3& width,const osg::Vec3& height, osg::Image* image)
	{
		// set up the Geometry.
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array(4);
		(*coords)[0] = corner;
		(*coords)[1] = corner+width;
		(*coords)[2] = corner+width+height;
		(*coords)[3] = corner+height;
		geom->setVertexArray(coords.get());

		osg::ref_ptr<osg::Vec3Array> norms = new osg::Vec3Array(1);
		(*norms)[0] = width^height;
		(*norms)[0].normalize();

		geom->setNormalArray(norms.get());
		geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

		osg::ref_ptr<osg::Vec2Array> tcoords = new osg::Vec2Array(4);
		(*tcoords)[0].set(0.0f,0.0f);
		(*tcoords)[1].set(1.0f,0.0f);
			(*tcoords)[2].set(1.0f,1.0f);
		(*tcoords)[3].set(0.0f,1.0f);
		geom->setTexCoordArray(0,tcoords.get());
		osg::ref_ptr<osg::DrawArrays> arrays = new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4);
		geom->addPrimitiveSet(arrays.get());
		if (image)
		{
			osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
			osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
			texture->setImage(image);

			osg::ref_ptr<osg::AlphaFunc> alphaFunc = new osg::AlphaFunc;
			alphaFunc->setFunction(osg::AlphaFunc::GEQUAL,0.05f);
			stateset->setAttributeAndModes( alphaFunc.get(), osg::StateAttribute::ON );
			
			stateset->setTextureAttributeAndModes(0,texture.get(),osg::StateAttribute::ON);
			geom->setStateSet(stateset.get());

			
		}

		return geom;
	}

}
