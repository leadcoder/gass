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

//#include "Plugins/Ogre/Components/OgreLineComponent.h"
#include <boost/bind.hpp>
#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreTextureUnitState.h>
#include <OgreMaterialManager.h>
#include <OgreManualObject.h>
#include <OgreCommon.h>

#include "OgreLineComponent.h"

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
	OgreLineComponent::OgreLineComponent(): m_LineObject (NULL),
		m_TexScale(0.1,1),
		m_HeightOffset (0),
		m_MaterialName("BaseWhiteNoLighting"),
		m_Type("line_list")
	{

	}

	OgreLineComponent::~OgreLineComponent()
	{

	}

	void OgreLineComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("LineComponent",new Creator<OgreLineComponent, IComponent>);
		RegisterProperty<std::string>("MaterialName", &OgreLineComponent::GetMaterialName, &OgreLineComponent::SetMaterialName);
		RegisterProperty<std::string>("Type", &OgreLineComponent::GetType, &OgreLineComponent::SetType);
	}

	void OgreLineComponent::OnCreate()
	{
		
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_MESSAGE_LOAD_GFX_COMPONENTS,  MESSAGE_FUNC(OgreLineComponent::OnLoad),1);
	}

	void OgreLineComponent::OnLoad(MessagePtr message)
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

		m_LineObject = sm->createManualObject(name);
		m_LineObject->setDynamic(true);
		m_LineObject->setCastShadows(false);

		OgreLocationComponent * lc = GetSceneObject()->GetFirstComponent<OgreLocationComponent>().get();

		lc->GetOgreNode()->attachObject(m_LineObject);

		if(m_ControlPointList != "")
		{
			Vec3 new_pos;
			std::string cpl  = m_ControlPointList;
			int pos = cpl.find("#");
			while(pos != -1)
			{
				std::string pos_post = cpl.substr(0,pos);
				Misc::GetVector(pos_post.c_str(),new_pos);
				LineData data;
				data.pos = new_pos;
				data.color = Vec3(1,1,1);
				m_ControlPoints.push_back(data);
				cpl = cpl.substr(pos+1);
				pos = cpl.find("#");

			}
			Misc::GetVector(cpl.c_str(),new_pos);
			LineData data;
			data.pos = new_pos;
			data.color = Vec3(1,1,1);
			m_ControlPoints.push_back(data);
				
			BuildLineFromControlPoints();
		}
	}

	void OgreLineComponent::Clear()
	{
		m_ControlPoints.clear();
			if(m_LineObject)
				m_LineObject->clear();

	}


	void OgreLineComponent::UpdateLineFromControlPoints()
	{
		if(m_ControlPoints.size() > 0)
		{
			m_LineObject->beginUpdate(0);
			float tex_coord = 0;
			for(int i = 0; i < m_ControlPoints.size(); i++)
			{
				Vec3 pos = m_ControlPoints[i].pos;
				
				
				tex_coord  = pos.FastLength();
				m_LineObject->position(pos.x, pos.y+m_HeightOffset, pos.z);
				m_LineObject->textureCoord(0,tex_coord);

				Ogre::ColourValue col;
				col.r = m_ControlPoints[i].color.x;
				col.g = m_ControlPoints[i].color.y;
				col.b = m_ControlPoints[i].color.z;
				col.a = 1;
				m_LineObject->colour(col);
			}

			for(int i = 0; i < m_ControlPoints.size();i++)
			{
				m_LineObject->index(i);
			}
			if(m_Closed && m_ControlPoints.size() > 0)
			{
				m_LineObject->index(0);
			}
			m_LineObject->end();
		}
	}

	void OgreLineComponent::BuildLineFromControlPoints()
	{
		if(m_LineObject)
		{
			float tex_coord = 0;
			m_LineObject->clear();
			Ogre::RenderOperation::OperationType op = Ogre::RenderOperation::OT_LINE_LIST;;

			if(m_Type == "line_list")
			{
				op = Ogre::RenderOperation::OT_LINE_LIST;
			}
			if(m_Type == "point_list")
			{
				op = Ogre::RenderOperation::OT_POINT_LIST;
			}
			else
			{
				op = Ogre::RenderOperation::OT_LINE_STRIP;
			}

			m_LineObject->begin(m_MaterialName, op);
			for(int i = 0; i < m_ControlPoints.size(); i++)
			{
				Vec3 pos = m_ControlPoints[i].pos;
				tex_coord  = pos.FastLength();
				m_LineObject->position(pos.x, pos.y+m_HeightOffset, pos.z);
				m_LineObject->textureCoord(0,tex_coord);
				Ogre::ColourValue col;
				col.r = m_ControlPoints[i].color.x;
				col.g = m_ControlPoints[i].color.y;
				col.b = m_ControlPoints[i].color.z;
				col.a = 1;
				m_LineObject->colour(col);


			}
			for(int i = 0; i < m_ControlPoints.size();i++)
			{
				m_LineObject->index(i);
			}
			if(m_Closed && m_ControlPoints.size() > 0)
			{
				m_LineObject->index(0);
			}
			m_LineObject->end();
		}
	}

	void OgreLineComponent::Add(const std::vector<LineData> &line_seg)
	{
		for(unsigned int i = 0 ; i < line_seg.size();i++)
		{
			m_ControlPoints.push_back(line_seg[i]);
		}
		BuildLineFromControlPoints();
	}


	AABox OgreLineComponent::GetBoundingBox() const
	{
		AABox box;
		return box;
	}

	Sphere OgreLineComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		return sphere;
	}

}
