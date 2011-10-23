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


#include "Sim/Common.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectLink.h"

namespace GASS
{
	BaseSceneComponent::BaseSceneComponent()
	{

	}

	BaseSceneComponent::~BaseSceneComponent()
	{

	}

	/*MessageManager* BaseSceneComponent::GetMessageManager() const
	{
		return boost::shared_static_cast<SceneObject>(GetOwner())->GetMessageManager();
	}*/

	SceneObjectPtr BaseSceneComponent::GetSceneObject() const
	{
		return boost::shared_static_cast<SceneObject>(GetOwner());
	}


	void BaseSceneComponent::OnCreate()
	{
		BaseComponent::OnCreate();
		InitializePointers();
	}

	void BaseSceneComponent::InitializePointers()
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<AbstractProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				AbstractProperty * prop = (*iter);
				const std::string prop_name = prop->GetTypeName();
				if(prop_name == "SceneObjectLink")
				{
					boost::any any_link;
					prop->GetValue(this,any_link);
					SceneObjectLink link = boost::any_cast<SceneObjectLink>(any_link);
					link.Initlize(GetSceneObject());
					prop->SetValue(this,boost::any(link));
					
				}
				++iter;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
	}
}
