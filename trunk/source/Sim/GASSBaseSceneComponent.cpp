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
#include "Sim/GASSSceneObjectRef.h"
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
				//const std::string prop_name = prop->GetTypeName();
				//if(std::string::npos != prop_name.find("SceneObjectLink"))
				if(*prop->GetTypeID() == typeid(SceneObjectLink))
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
						boost::any any_links(links);
						prop->SetValue(this, any_links);
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
							boost::any any_link(link);
							prop->SetValue(this,any_link);
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

	void BaseSceneComponent::InitializeSceneObjectRef()
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<IProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				IProperty * prop = (*iter);
				if(*prop->GetTypeID() == typeid(SceneObjectRef))
				{
					IVectorProperty* vector_prop =  dynamic_cast<IVectorProperty*>(prop);
					if(vector_prop)
					{
						boost::any any_link;
						prop->GetValue(this,any_link);
						std::vector<SceneObjectRef> links = boost::any_cast<std::vector<SceneObjectRef> >(any_link);
						for(int i = 0 ; i < links.size(); i++)
						{
							SceneObjectRef new_ref( links[i].GetRefGUID());
							links[i] = new_ref;
						}
						boost::any any_links(links);
						prop->SetValue(this,any_links);
					}
					else
					{
						boost::any any_link;
						prop->GetValue(this,any_link);
						SceneObjectRef old_ref = boost::any_cast<SceneObjectRef>(any_link);
						SceneObjectRef new_ref( old_ref.GetRefGUID());
						boost::any any_ref(new_ref);
						prop->SetValue(this,any_ref);
					}
				}
				++iter;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
	}


	void BaseSceneComponent::ResolveTemplateReferences(SceneObjectPtr template_root)
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<IProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				IProperty * prop = (*iter);
				if(*prop->GetTypeID() == typeid(SceneObjectRef))
				{
					IVectorProperty* vector_prop =  dynamic_cast<IVectorProperty*>(prop);
					if(vector_prop)
					{
						boost::any any_link;
						prop->GetValue(this,any_link);
						std::vector<SceneObjectRef> links = boost::any_cast<std::vector<SceneObjectRef> >(any_link);
						for(int i = 0 ; i < links.size(); i++)
						{
							SceneObjectRef new_ref = links[i];
							new_ref.ResolveTemplateReferences(template_root);
							links[i] = new_ref;
						}
						boost::any any_links(links);
						prop->SetValue(this,any_links);
					}
					else
					{
						boost::any any_link;
						prop->GetValue(this,any_link);
						SceneObjectRef so_ref = boost::any_cast<SceneObjectRef>(any_link);
						so_ref.ResolveTemplateReferences(template_root);
						boost::any any_so(so_ref);
						prop->SetValue(this,any_so);
					}
				}
				++iter;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
	}



	void BaseSceneComponent::RemapReferences(const std::map<SceneObjectGUID,SceneObjectGUID> &ref_map)
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<IProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				IProperty * prop = (*iter);
				//const std::string prop_name = prop->GetTypeName();
				//if(std::string::npos != prop_name.find("SceneObjectRef"))
				if(*prop->GetTypeID() == typeid(SceneObjectRef))
				{
					IVectorProperty* vector_prop =  dynamic_cast<IVectorProperty*>(prop);
					if(vector_prop)
					{
						boost::any any_link;
						prop->GetValue(this,any_link);
						std::vector<SceneObjectRef> links = boost::any_cast<std::vector<SceneObjectRef> >(any_link);
						for(int i = 0 ; i < links.size(); i++)
						{
							std::map<SceneObjectGUID,SceneObjectGUID>::const_iterator guid_iter = ref_map.find(links[i].GetRefGUID());
							if(guid_iter != ref_map.end())
							{
								SceneObjectRef new_ref( guid_iter->second);
								links[i] = new_ref;
							}
						}
						boost::any any_links(links);
						prop->SetValue(this,any_links);
					}
					else
					{
						boost::any any_link;
						prop->GetValue(this,any_link);
						SceneObjectRef old_ref = boost::any_cast<SceneObjectRef>(any_link);
						std::map<SceneObjectGUID,SceneObjectGUID>::const_iterator guid_iter = ref_map.find(old_ref.GetRefGUID());
						if(guid_iter != ref_map.end())
						{
							SceneObjectRef new_ref(  guid_iter->second);
							boost::any any_ref(new_ref);
							prop->SetValue(this,any_ref);
						}
					}
				}
				++iter;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
	}
}
