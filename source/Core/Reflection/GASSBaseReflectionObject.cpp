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

#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "tinyxml2.h"

namespace GASS
{

	BaseReflectionObject::BaseReflectionObject()
	{

	}

	BaseReflectionObject::~BaseReflectionObject()
	{

	}


	void BaseReflectionObject::_LoadProperties(tinyxml2::XMLElement *elem)
	{
		tinyxml2::XMLElement *prop_elem = elem->FirstChildElement();
		while(prop_elem)
		{
			const std::string prop_name = prop_elem->Value();
			if(prop_elem->FirstAttribute())
			{
				const std::string attrib_name =  StringUtils::ToLower(prop_elem->FirstAttribute()->Name());

				if(attrib_name == "value") 
				{
					const std::string prop_val = prop_elem->FirstAttribute()->Value();
					try
					{
						SetPropertyByString(prop_name,prop_val);
					}
					catch(...)
					{
						GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed parsing:" + prop_name +" With attribute:"+ prop_val+  " in:" + std::string(elem->GetDocument()->GetFileName()),"BaseReflectionObject::LoadXML");
					}
				}
				else
				{
					GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Unkown property syntax for:" + prop_name + " in:" + std::string(elem->GetDocument()->GetFileName()),"BaseReflectionObject::_LoadProperties");
				}
			}
			else
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "No attribute found for property for:" + prop_name + " in:" + std::string(elem->GetDocument()->GetFileName()),"BaseReflectionObject::_LoadProperties");
			}
			prop_elem  = prop_elem->NextSiblingElement();
		}
	}

	void BaseReflectionObject::_SaveProperties(tinyxml2::XMLElement *parent) const
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<IProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				IProperty * prop = (*iter);
				
				tinyxml2::XMLElement *prop_elem = parent->GetDocument()->NewElement(prop->GetName().c_str());
				prop_elem->SetAttribute("value", prop->GetValueAsString(this).c_str());
				parent->LinkEndChild( prop_elem);
				++iter;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
	}

	bool BaseReflectionObject::_SerializeProperties(ISerializer* serializer)
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<IProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				IProperty * prop = (*iter);
				prop->Serialize(this,serializer);
				++iter;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
		return true;
	}

	void BaseReflectionObject::SetPropertyByString(const std::string &property_name,const std::string &value)
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<IProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				IProperty *prop = (*iter);
				const std::string prop_name = prop->GetName();
				if(prop_name == property_name)
				{
					prop->SetValueByString(this,value);
					return;
				}
				++iter;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
		GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed find property:" + property_name + " With value:" + value,"BaseReflectionObject::SetPropertyByString");
	}


	bool BaseReflectionObject::HasProperty(const std::string &property_name) const
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<IProperty*>::const_iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				IProperty * prop = (*iter);
				const std::string prop_name = prop->GetName();
				if(prop_name == property_name)
				{
					
					return true;
				}
				++iter;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
		return false;
	}

	void BaseReflectionObject::SetPropertyByType(const std::string &property_name, GASS_ANY value)
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<IProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				IProperty * prop = (*iter);
				const std::string prop_name = prop->GetName();
				if(prop_name == property_name)
				{
					prop->SetValue(this,value);
					return;
				}
				++iter;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
		GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed find property:" + property_name,"BaseReflectionObject::SetPropertyByType");
	}

	bool BaseReflectionObject::GetPropertyByType(const std::string &property_name, GASS_ANY &value) const
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<IProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				IProperty * prop = (*iter);
				const std::string prop_name = prop->GetName();
				if(prop_name == property_name)
				{
					prop->GetValue(this,value);
					return true;
				}
				++iter;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
		return false;
	}


	bool BaseReflectionObject::GetPropertyByString(const std::string &property_name, std::string &value) const
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			//std::list<IProperty*>::const_iterator	iter = pRTTI->GetFirstProperty();
			std::list<IProperty*>::iterator iter = pRTTI->GetFirstProperty();

			while(iter != pRTTI->GetProperties()->end())
			{
				IProperty * prop = (*iter);
				const std::string prop_name = prop->GetName();
				if(prop_name == property_name)
				{
					value = prop->GetValueAsString(this);
					return true;
				}
				++iter;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
		return false;
	}

	void BaseReflectionObject::CopyPropertiesTo(BaseReflectionObjectPtr dest) const
	{
		RTTI* pRTTI = GetRTTI();
		RTTI* pdestRTTI = dest->GetRTTI();

		if(pRTTI == pdestRTTI)
		{
			while(pRTTI)
			{
				std::list<IProperty*>::iterator	iter = pRTTI->GetFirstProperty();
				while(iter != pRTTI->GetProperties()->end())
				{
					IProperty * prop = (*iter);
					prop->SetValue(dest.get(),this);
					++iter;
				}
				pRTTI = pRTTI->GetAncestorRTTI();
			}
		}
		else
		{
			while(pRTTI)
			{
				std::list<IProperty*>::iterator	iter = pRTTI->GetFirstProperty();
				while(iter != pRTTI->GetProperties()->end())
				{
					IProperty * prop = (*iter);
					if(dest->HasProperty(prop->GetName()))
						dest->SetPropertyByString(prop->GetName(), prop->GetValueAsString(this));
					//Here we want to copy all common properties from one object to another 
					//(typically from template to instance), so ignore if some properties don't exist in destination object
					//if (!ret)
					//	LogManager::getSingleton().stream() << "WARNING:BaseReflectionObject::CopyPropertiesTo() - Property not found: %s", prop->GetName().c_str());
					++iter;
				}
				pRTTI = pRTTI->GetAncestorRTTI();
			}
		}
	}


	PropertyVector BaseReflectionObject::GetProperties() const
	{
		PropertyVector props;
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<IProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				IProperty * prop = (*iter);
				props.push_back(prop);
				++iter;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
		return props;
	}


	bool BaseReflectionObject::HasMetaData() const
	{
		RTTI* pRTTI = GetRTTI();
		return pRTTI->HasMetaData();
	}

	ClassMetaDataPtr BaseReflectionObject::GetMetaData() const
	{
		RTTI* pRTTI = GetRTTI();
		return pRTTI->GetMetaData();
	}
}
