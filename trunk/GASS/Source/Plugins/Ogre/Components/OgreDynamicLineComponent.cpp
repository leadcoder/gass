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
#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreTextureUnitState.h>
#include <OgreMaterialManager.h>
#include <OgreManualObject.h>
#include <OgreCommon.h>

#include "OgreDynamicLineComponent.h"
#include "Plugins/Ogre/Helpers/DynamicLines.h"



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
	OgreDynamicLineComponent::OgreDynamicLineComponent():
		m_DynLines (NULL),
		m_TexScale(0.1,1),
		m_HeightOffset (0),
		m_MaterialName("BaseWhiteNoLighting"),
		m_Type("line_list")
	{

	}

	OgreDynamicLineComponent::~OgreDynamicLineComponent()
	{

	}

	void OgreDynamicLineComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("DynamicLineComponent",new Creator<OgreDynamicLineComponent, IComponent>);
		RegisterProperty<std::string>("MaterialName", &OgreDynamicLineComponent::GetMaterialName, &OgreDynamicLineComponent::SetMaterialName);
		RegisterProperty<std::string>("Type", &OgreDynamicLineComponent::GetType, &OgreDynamicLineComponent::SetType);
	}

	void OgreDynamicLineComponent::OnCreate()
	{
		int obj_id = (int) this;
		MessageManager * mm = GetMessageManager();
		mm->RegisterForMessage(ScenarioScene::SM_MESSAGE_LOAD_GFX_COMPONENTS, obj_id,  boost::bind( &OgreDynamicLineComponent::OnLoad, this, _1 ),1);
	}

	void OgreDynamicLineComponent::OnLoad(MessagePtr message)
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

		Ogre::RenderOperation::OperationType op = Ogre::RenderOperation::OT_LINE_LIST;

		if(m_Type == "line_list")
		{
			op = Ogre::RenderOperation::OT_LINE_LIST;
		}
		if(m_Type == "point_list")
		{
			op = Ogre::RenderOperation::OT_POINT_LIST;
		}
		else if(m_Type == "line_strip")
		{
			op = Ogre::RenderOperation::OT_LINE_STRIP;
		}

		m_DynLines = new DynamicLines(op);

		m_DynLines->setMaterial(m_MaterialName);
		
		OgreLocationComponent * lc = GetSceneObject()->GetFirstComponent<OgreLocationComponent>().get();

		lc->GetOgreNode()->attachObject(m_DynLines);
		
		if(m_ControlPointList != "")
		{
			Vec3 new_pos;
			std::string cpl  = m_ControlPointList;
			int pos = cpl.find("#");
			while(pos != -1)
			{
				std::string pos_post = cpl.substr(0,pos);
				Misc::GetVector(pos_post.c_str(),new_pos);
				m_ControlPoints.push_back(new_pos);
				cpl = cpl.substr(pos+1);
				pos = cpl.find("#");

			}
			Misc::GetVector(cpl.c_str(),new_pos);
			m_ControlPoints.push_back(new_pos);
			BuildLineFromControlPoints();
		}
	}

	/*void OgreDynamicLineComponent::GetControlPoints()
	{
		m_ControlPoints.clear();
		ISceneNode::ISceneNodeList::iterator iter;
		for(iter = m_SceneNode->GetChildren()->begin(); iter != m_SceneNode->GetChildren()->end(); iter++)
		{
			BaseObject* obj = DYNAMIC_CAST(BaseObject,(*iter));
			if(obj)
			{
				Vec3 pos = obj->GetPosition();
				m_ControlPoints.push_back(pos);
			}
		}
	}*/



	void OgreDynamicLineComponent::UpdateLineFromControlPoints()
	{
		m_DynLines->clear();
		if(m_ControlPoints.size() > 0)
		{
			float tex_coord = 0;
			for(int i = 0; i < m_ControlPoints.size(); i++)
			{
				Vec3 pos = m_ControlPoints[i];
				tex_coord  = pos.FastLength();
				m_DynLines->addPoint(pos.x, pos.y+m_HeightOffset, pos.z);
			}
			m_DynLines->update();
		}
	}

	void OgreDynamicLineComponent::Clear()
	{
		m_ControlPoints.clear();
		if(m_DynLines)
				m_DynLines->clear();

	}

	void OgreDynamicLineComponent::BuildLineFromControlPoints()
	{
		if(m_DynLines)
		{
			float tex_coord = 0;
			m_DynLines->clear();
			for(int i = 0; i < m_ControlPoints.size(); i++)
			{
				Vec3 pos = m_ControlPoints[i];
				tex_coord  = pos.FastLength();
				m_DynLines->addPoint(pos.x, pos.y+ m_HeightOffset, pos.z);
			}
			m_DynLines->update();
		}
	}

	void OgreDynamicLineComponent::Add(const std::vector<LineData> &line_seg)
	{
		for(unsigned int i = 0 ; i < line_seg.size();i++)
		{
			m_ControlPoints.push_back(line_seg[i].pos);
		}
		BuildLineFromControlPoints();
	}


	AABox OgreDynamicLineComponent::GetBoundingBox() const
	{
		AABox box;
		return box;
	}

	Sphere OgreDynamicLineComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		return sphere;
	}

}
