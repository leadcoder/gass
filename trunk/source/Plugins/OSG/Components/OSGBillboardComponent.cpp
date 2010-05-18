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

		Vec3 up = GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->GetSceneUp()*m_Height;
		Vec3 east = GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->GetSceneEast()*m_Width;
		
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
		//m_OSGBillboard->setNodeMask();
	}

	AABox OSGBillboardComponent::GetBoundingBox() const
	{
		Vec3 up = GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->GetSceneUp()*m_Height;
		Vec3 east = GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->GetSceneEast()*m_Width;
		Vec3 north = GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->GetSceneNorth()*m_Width;
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
		(*tcoords)[0].set(0.0f,1.0f);
		(*tcoords)[1].set(1.0f,1.0f);
		(*tcoords)[2].set(1.0f,0.0f);
		(*tcoords)[3].set(0.0f,0.0f);
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
