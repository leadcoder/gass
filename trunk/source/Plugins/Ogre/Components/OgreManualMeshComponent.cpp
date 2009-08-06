/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
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

//#include "Plugins/Ogre/Components/OgreManualMeshComponent.h"
#include <boost/bind.hpp>
#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreTextureUnitState.h>
#include <OgreMaterialManager.h>
#include <OgreManualObject.h>
#include <OgreCommon.h>

#include "OgreManualMeshComponent.h"

#include "Core/Utils/Log.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"

#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"

#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/OgreConvert.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"

namespace GASS
{
	OgreManualMeshComponent::OgreManualMeshComponent(): m_MeshObject (NULL)
	{

	}

	OgreManualMeshComponent::~OgreManualMeshComponent()
	{

	}

	void OgreManualMeshComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("ManualMeshComponent",new Creator<OgreManualMeshComponent, IComponent>);
	}

	void OgreManualMeshComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_LOAD_GFX_COMPONENTS,  MESSAGE_FUNC(OgreManualMeshComponent::OnLoad),1);
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_MANUAL_MESH_PARAMETER,  MESSAGE_FUNC(OgreManualMeshComponent::OnParameterMessage),1);
	}

	void OgreManualMeshComponent::OnLoad(MessagePtr message)
	{
		OgreGraphicsSceneManager* ogsm = boost::any_cast<OgreGraphicsSceneManager*>(message->GetData("GraphicsSceneManager"));
		assert(ogsm);
		Ogre::SceneManager* sm = ogsm->GetSceneManger();

		static unsigned int obj_id = 0;
		obj_id++;
		std::stringstream ss;
		std::string name;
		ss << GetName() << obj_id;
		ss >> name;

		m_MeshObject = sm->createManualObject(name);
		m_MeshObject->setDynamic(true);
		m_MeshObject->setCastShadows(false);
		OgreLocationComponent * lc = GetSceneObject()->GetFirstComponent<OgreLocationComponent>().get();
		lc->GetOgreNode()->attachObject(m_MeshObject);
		Log::Print("created");
	}

	void OgreManualMeshComponent::OnParameterMessage(MessagePtr message)
	{
		SceneObject::ManualMeshParameterType type = boost::any_cast<SceneObject::ManualMeshParameterType>(message->GetData("Parameter"));
		switch(type)
		{
		case SceneObject::MESH_DATA:
			{
				ManualMeshDataPtr data = boost::any_cast<ManualMeshDataPtr>(message->GetData("Data"));
				CreateMesh(data);
			}
			break;
		case SceneObject::CLEAR:
			Clear();
			break;
		}
	}

	void OgreManualMeshComponent::Clear()
	{
		if(m_MeshObject)
			m_MeshObject->clear();
	}

	void OgreManualMeshComponent::CreateMesh(ManualMeshDataPtr data)
	{
		if(m_MeshObject)
		{
			m_MeshObject->clear();
			Ogre::RenderOperation::OperationType op = Ogre::RenderOperation::OT_LINE_LIST;
			switch(data->Type)
			{
			case LINE_LIST:
				op = Ogre::RenderOperation::OT_LINE_LIST;
				break;
			case POINT_LIST:
				op = Ogre::RenderOperation::OT_POINT_LIST;
				break;
			case LINE_STRIP:
				op = Ogre::RenderOperation::OT_LINE_STRIP;
				break;
			case TRIANGLE_FAN:
				op = Ogre::RenderOperation::OT_TRIANGLE_FAN;
				break;
			case TRIANGLE_LIST:
				op = Ogre::RenderOperation::OT_TRIANGLE_LIST;
				break;
			case TRIANGLE_STRIP:
				op = Ogre::RenderOperation::OT_TRIANGLE_STRIP;
				break;
			}

			m_MeshObject->begin(data->Material, op);
			for(int i = 0; i < data->VertexVector.size(); i++)
			{
				Vec3 pos = data->VertexVector[i].Pos;
				Vec2 tex_coord  = data->VertexVector[i].TexCoord;
				Vec4 color  = data->VertexVector[i].Color;
				m_MeshObject->position(pos.x, pos.y, pos.z);
				m_MeshObject->textureCoord(tex_coord.x,tex_coord.y);
				Ogre::ColourValue col;
				col.r = color.x;
				col.g = color.y;
				col.b = color.z;
				col.a = color.w;
				m_MeshObject->colour(col);
			}
			for(int i = 0; i < data->IndexVector.size();i++)
			{
				m_MeshObject->index(data->IndexVector[i]);
			}
			m_MeshObject->end();
		}
	}


	AABox OgreManualMeshComponent::GetBoundingBox() const
	{
		//AABox box;
		//return box;
		assert(m_MeshObject);
		return Convert::ToGASS(m_MeshObject->getBoundingBox());
	}

	Sphere OgreManualMeshComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		assert(m_MeshObject);
		sphere.m_Pos = Vec3(0,0,0);
		sphere.m_Radius = m_MeshObject->getBoundingRadius();
		return sphere;
	}

}
