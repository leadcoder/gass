/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSBaseSceneComponent.h"

namespace GASS
{
	class GASSExport SceneObjectVisitor
	{
	public:
		SceneObjectVisitor() {};
		virtual bool Visit(SceneObjectPtr sceneobject) = 0;
	};
	using SceneObjectVisitorPtr = std::shared_ptr<SceneObjectVisitor>;

	class GASSExport ClassComponentsVisitor : public SceneObjectVisitor
	{
	public:
		ClassComponentsVisitor(const std::string &component_class_name) : m_ComponentClassName(component_class_name) {}
		bool Visit(SceneObjectPtr scene_object) override
		{
			SceneObject::ComponentIterator comp_iter = scene_object->GetComponents();
			while(comp_iter.hasMoreElements())
			{
				ComponentPtr comp = comp_iter.getNext();
				if(comp->GetRTTI()->IsDerivedFrom(m_ComponentClassName))
				{
					m_Components.push_back(comp);
				}
			}
			return true;
		}
		SceneObject::ComponentVector m_Components;
		std::string m_ComponentClassName;
	};

	template <class T>
	class GASSExport TypedClassComponentsVisitor : SceneObjectVisitor
	{
	public:
		TypedClassComponentsVisitor() {}
		bool Visit(SceneObjectPtr scene_object) override
		{
			SceneObject::ComponentIterator comp_iter = scene_object->GetComponents();
			while(comp_iter.hasMoreElements())
			{
			    ComponentPtr comp = comp_iter.getNext();
				GASS_SHARED_PTR<T> ret = GASS_DYNAMIC_PTR_CAST<T>(comp);
				if(ret)
				{
					m_Components.push_back(comp);
				}
			}
			return true;
		}
		SceneObject::ComponentVector m_Components;
	};
}
