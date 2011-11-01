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
				if(std::string::npos != prop_name.find("SceneObjectLink"))
				{
					IVectorProperty* vector_prop =  dynamic_cast<IVectorProperty*>(prop);
					if(vector_prop)
					{

						boost::any any_link;
						prop->GetValue(this,any_link);
						std::vector<SceneObjectLink> links = boost::any_cast<std::vector<SceneObjectLink> >(any_link);
						for(int i = 0 ; i < links.size(); i++)
						{
							if(links[i].GetLinkObjectID() != UNKOWN_LINK_ID)
							{

								if(!links[i].Initlize(GetSceneObject()))
									Log::Error("Component:%s in object:%s failed to initilize scene object link:%s with id:%s",
									GetName().c_str(),
									GetSceneObject()->GetName().c_str(),
									prop->GetName().c_str(),
									links[i].GetLinkObjectID().c_str());
							}
							else
								Log::Warning("Component:%s in object:%s has no link id for:%s",
								GetName().c_str(),
								GetSceneObject()->GetName().c_str(),
								prop->GetName().c_str());
						}
						prop->SetValue(this,boost::any(links));
					}
					else
					{
						boost::any any_link;
						prop->GetValue(this,any_link);
						SceneObjectLink link = boost::any_cast<SceneObjectLink>(any_link);
						if(link.GetLinkObjectID() != UNKOWN_LINK_ID)
						{

							if(!link.Initlize(GetSceneObject()))
								Log::Error("Component:%s in object:%s failed to initilize scene object link:%s with id:%s",
								GetName().c_str(),
								GetSceneObject()->GetName().c_str(),
								prop->GetName().c_str(),
								link.GetLinkObjectID().c_str());
							prop->SetValue(this,boost::any(link));
						}
						else
							Log::Error("Component:%s in object:%s has no link id for:%s",
							GetName().c_str(),
							GetSceneObject()->GetName().c_str(),
							prop->GetName().c_str());
					}
				}
				++iter;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
	}
}
