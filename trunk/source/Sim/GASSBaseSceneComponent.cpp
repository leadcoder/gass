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


#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneObjectLink.h"
#include "Core/Utils/GASSException.h"

namespace GASS
{
	BaseSceneComponent::BaseSceneComponent()
	{

	}

	BaseSceneComponent::~BaseSceneComponent()
	{

	}

	SceneObjectPtr BaseSceneComponent::GetSceneObject() const
	{
		return STATIC_PTR_CAST<SceneObject>(GetOwner());
	}

	void BaseSceneComponent::OnInitialize()
	{
		//InitializePointers();
	}

	void BaseSceneComponent::InitializePointers()
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<IProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				IProperty * prop = (*iter);
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
								{
									GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, 
										"Component:" + GetName() + " in object:" + GetSceneObject()->GetName() + "failed to initilize scene object link:" + prop->GetName() + " with id:" + links[i].GetLinkObjectID(),
										"BaseSceneComponent::InitializePointers()");
								}
							}
							else
								LogManager::getSingleton().stream() << "WARNING:Component:" << GetName() <<  " in object:" << GetSceneObject()->GetName() << " has no link id for:" << prop->GetName();
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
							{
								GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, 
										"Component:" + GetName() + " in object:" + GetSceneObject()->GetName() + "failed to initilize scene object link:" + prop->GetName() + " with id:" + link.GetLinkObjectID(),
										"BaseSceneComponent::InitializePointers()");

							}
							prop->SetValue(this,boost::any(link));
						}
						else
						{
							GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, 
										"Component:" + GetName() + " in object:" + GetSceneObject()->GetName() + " has no link id for" + prop->GetName(),
										"BaseSceneComponent::InitializePointers()");

						}
					}
				}
				++iter;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
	}
}
