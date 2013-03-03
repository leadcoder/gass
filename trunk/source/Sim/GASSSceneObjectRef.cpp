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

#include "Core/Common.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"

namespace GASS
{

	 

	SceneObjectRef::SceneObjectRef() : m_RefObjectGUID(boost::uuids::nil_uuid())
	{

	}

	SceneObjectRef::SceneObjectRef(SceneObjectPtr obj) : m_RefObject(obj)
	{
		m_RefObjectGUID = obj->GetGUID();
	}

	SceneObjectRef::SceneObjectRef(SceneObjectGUID guid) : m_RefObjectGUID(guid)
	{
		UpdateRefPtr();
	}


	SceneObjectRef::~SceneObjectRef(void)
	{
		
	}

	void SceneObjectRef::SetRefGUID(const SceneObjectGUID &guid)
	{
		m_RefObjectGUID  = guid;
		UpdateRefPtr();
	}

	void SceneObjectRef::UpdateRefPtr()
	{
		if(!m_RefObjectGUID.is_nil())
		{
			SimEngine::SceneIterator iter = SimEngine::Get().GetScenes();
			while(iter.hasMoreElements())
			{
				ScenePtr scene = iter.getNext();
				SceneObjectPtr obj = scene->GetRootSceneObject()->GetChildByGUID(m_RefObjectGUID);
				if(obj)
				{
					m_RefObject = obj;
					return;
				}
			}
		}
		else
			m_RefObject.reset();
	}

	void SceneObjectRef::ResolveTemplateReferences(SceneObjectPtr root)
	{
		if(m_LocalID != "")
		{
			SceneObjectPtr obj;
			if(root->GetID()  == m_LocalID)
				obj = root;
			else 
				obj = root->GetChildByID(m_LocalID);
			if(obj)
			{
				//generate id if not present?
				m_RefObjectGUID = obj->GetGUID();
				m_RefObject = obj;
				m_LocalID = "";
			}
		}
	}
}