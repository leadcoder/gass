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
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/SimEngine.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Systems/Resource/IResourceSystem.h"

#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/Components/OSGBillboardComponent.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"
#include "Plugins/OSG/OSGConvert.h"
#include <osg/Material>
#include <osg/BlendFunc>


namespace GASS
{

	OSGBillboardComponent::OSGBillboardComponent() : m_CastShadow(false),
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
		RegisterProperty<float>("Height", &GASS::OSGBillboardComponent::GetHeight, &GASS::OSGBillboardComponent::SetHeight);
		RegisterProperty<float>("Width", &GASS::OSGBillboardComponent::GetWidth, &GASS::OSGBillboardComponent::SetWidth);
	}

	float OSGBillboardComponent::GetWidth() const 
	{
		return m_Width;
	}

	void OSGBillboardComponent::SetWidth(float width)
	{
		m_Width = width;

		//TODO::support run time change
		
	}
	float OSGBillboardComponent::GetHeight() const
	{
		return m_Height;
	}
	void OSGBillboardComponent::SetHeight(float height)
	{
		m_Height = height;
		//TODO::support run-time change
	}


	void OSGBillboardComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGBillboardComponent::OnLoad,LoadGFXComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGBillboardComponent::OnMaterialMessage,MaterialMessage,1));
	}

	void OSGBillboardComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		//OSGGraphicsSceneManager* osgsm = static_cast<OSGGraphicsSceneManager*>(message->GetGFXSceneManager());
		//assert(osgsm);

		std::string full_path;
		ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>();
		if(!rs->GetFullPath(m_Material,full_path))
		{
			Log::Error("Failed to find texture:%s",full_path.c_str());
		}

		Vec3 up(0,0,m_Height);
		Vec3 east(m_Width,0,0);
		
		//make offset
		Vec3 corner = -east*0.5;

		m_OSGBillboard = new osg::Billboard();
		m_OSGBillboard->setMode(osg::Billboard::POINT_ROT_EYE);
		m_OSGBillboard->addDrawable(
			CreateSquare(osg::Vec3(corner.x,corner.y,corner.z),
			osg::Vec3(east.x,east.y,east.z),
			osg::Vec3(up.x,up.y,up.z),
			//osgDB::readImageFile("Images/reflect.rgb")),
			osgDB::readImageFile(full_path)).get(),
			osg::Vec3(0.0f,0.0f,0.0f));

		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponent<OSGLocationComponent>();
		lc->GetOSGNode()->addChild(m_OSGBillboard.get());
		GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(shared_from_this())));
		//m_OSGBillboard->setNodeMask();
	}

	AABox OSGBillboardComponent::GetBoundingBox() const
	{
		
		Vec3 up = OSGConvert::Get().ToGASS(osg::Vec3(0,0,m_Height));
		Vec3 east = OSGConvert::Get().ToGASS(osg::Vec3(m_Width,0,0));
		Vec3 north = OSGConvert::Get().ToGASS(osg::Vec3(0,m_Width,0));
		Vec3 corner = -east*0.5 - north*0.5;
		AABox box(corner,corner+east+up+north);
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

	void OSGBillboardComponent::OnMaterialMessage(MaterialMessagePtr message)
	{
		Vec4 diffuse = message->GetDiffuse();
		Vec3 ambient = message->GetAmbient();
		Vec3 specular = message->GetSpecular();
		Vec3 si = message->GetSelfIllumination();

		osg::ref_ptr<osg::Material> mat (new osg::Material);
		//Specifying the yellow colour of the object
		mat->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(diffuse.x,diffuse.y,diffuse.z,diffuse.w));
		mat->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(ambient.x,ambient.y,ambient.z,1));
		mat->setSpecular(osg::Material::FRONT_AND_BACK,osg::Vec4(specular.x,specular.y,specular.z,1));
		mat->setShininess(osg::Material::FRONT_AND_BACK,message->GetShininess());
		mat->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(si.x,si.y,si.z,1));

		//mat->setAmbient(osg::Material::FRONT,osg::Vec4(color.x,color.y,color.z,color.w));
		//Attaching the newly defined state set object to the node state set
		osg::ref_ptr<osg::StateSet> nodess (m_OSGBillboard->getOrCreateStateSet());
		nodess->setAttribute(mat.get());

		
		nodess->setAttributeAndModes( mat.get() , osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        // Turn on blending
		if(diffuse.w < 1.0)
		{
			osg::ref_ptr<osg::BlendFunc> bf (new   osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,  osg::BlendFunc::ONE_MINUS_SRC_ALPHA ));
			nodess->setAttributeAndModes(bf);
		}
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


	void OSGBillboardComponent::SetCastShadow(bool value)
	{
		m_CastShadow = value;
		if(m_CastShadow && m_OSGBillboard.valid())
			m_OSGBillboard->setNodeMask(OSGGraphicsSystem::m_CastsShadowTraversalMask | m_OSGBillboard->getNodeMask());
		else if(m_OSGBillboard.valid())
		{
			m_OSGBillboard->setNodeMask(~OSGGraphicsSystem::m_CastsShadowTraversalMask & m_OSGBillboard->getNodeMask());
		}
	}
}
