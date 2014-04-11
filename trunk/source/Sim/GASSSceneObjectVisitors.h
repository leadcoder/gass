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

#pragma once

#include "Sim/GASSCommon.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/ComponentSystem/GASSComponentContainer.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplate.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSBaseSceneComponent.h"

namespace GASS
{
	class GASSExport SceneObjectVisitor
	{
	public:
		SceneObjectVisitor() {};
		virtual ~SceneObjectVisitor() {}
		virtual bool Visit(SceneObjectPtr sceneobject) = 0;
	};
	typedef SPTR<SceneObjectVisitor> SceneObjectVisitorPtr;

	class GASSExport ClassComponentsVisitor : public SceneObjectVisitor
	{
	public:
		ClassComponentsVisitor(const std::string &component_class_name) : m_ComponentClassName(component_class_name) {}
		virtual ~ClassComponentsVisitor() {}
		virtual bool Visit(SceneObjectPtr scene_object)
		{
			ComponentContainer::ComponentIterator comp_iter = scene_object->GetComponents();
			while(comp_iter.hasMoreElements())
			{
				BaseSceneComponentPtr comp = STATIC_PTR_CAST<BaseSceneComponent>(comp_iter.getNext());
				if(comp->GetRTTI()->IsDerivedFrom(m_ComponentClassName))
				{
					m_Components.push_back(comp);
				}
			}
			return true;
		}
		ComponentContainer::ComponentVector m_Components;
		std::string m_ComponentClassName;
	};

	template <class T>
	class GASSExport TypedClassComponentsVisitor : SceneObjectVisitor
	{
	public:
		TypedClassComponentsVisitor() {}
		virtual ~TypedClassComponentsVisitor() {}
		virtual bool Visit(SceneObjectPtr scene_object)
		{
			ComponentContainer::ComponentIterator comp_iter = scene_object->GetComponents();
			while(comp_iter.hasMoreElements())
			{
			    ComponentPtr comp = comp_iter.getNext();
				SPTR<T> ret = DYNAMIC_PTR_CAST<T>(comp);
				if(ret)
				{
					m_Components.push_back(comp);
				}
			}
			return true;
		}
		ComponentContainer::ComponentVector m_Components;
	};
}
