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

#include "Core/Reflection/BaseReflectionObject.h"
#include "tinyxml.h"

namespace GASS
{

	BaseReflectionObject::BaseReflectionObject()
	{

	}

	BaseReflectionObject::~BaseReflectionObject()
	{

	}


	void BaseReflectionObject::LoadProperties(TiXmlElement *elem)
	{
		TiXmlElement *attrib = elem->FirstChildElement();
		while(attrib)
		{
			const std::string attrib_name = attrib->Value();
			const std::string attrib_val = attrib->FirstAttribute()->Value();
			if (!SetPropertyByString(attrib_name,attrib_val))
				Log::Warning("BaseReflectionObject::LoadProperties() - Filename: %s\tproperty not found: %s", attrib->GetDocument()->Value(), attrib_name.c_str());

			attrib  = attrib->NextSiblingElement();
		}
	}

	void BaseReflectionObject::SaveProperties(TiXmlElement *parent)
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<AbstractProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				AbstractProperty * prop = (*iter);
				
				TiXmlElement *prop_elem = new TiXmlElement( prop->GetName().c_str());
				prop_elem->SetAttribute("value", prop->GetValueAsString(this).c_str());
				parent->LinkEndChild( prop_elem);
				iter++;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
	}

	bool BaseReflectionObject::SerializeProperties(ISerializer* serializer)
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<AbstractProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				AbstractProperty * prop = (*iter);
				prop->Serialize(this,serializer);
				iter++;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
		return true;
	}

	bool BaseReflectionObject::SetPropertyByString(const std::string &property_name,const std::string &value)
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<AbstractProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				AbstractProperty *prop = (*iter);
				const std::string prop_name = prop->GetName();
				if(prop_name == property_name)
				{
					prop->SetValueByString(this,value);

					return true;
				}
				iter++;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}

		return false;
	}

	bool BaseReflectionObject::SetPropertyByType(const std::string &property_name, boost::any value)
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<AbstractProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				AbstractProperty * prop = (*iter);
				const std::string prop_name = prop->GetName();
				if(prop_name == property_name)
				{
					prop->SetValue(this,value);
					return true;
				}
				iter++;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}

		return false;
	}

	bool BaseReflectionObject::GetPropertyByType(const std::string &property_name, boost::any &value)
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<AbstractProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				AbstractProperty * prop = (*iter);
				const std::string prop_name = prop->GetName();
				if(prop_name == property_name)
				{
					prop->GetValue(this,value);
					return true;
				}
				iter++;
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
			//std::list<AbstractProperty*>::const_iterator	iter = pRTTI->GetFirstProperty();
			std::list<AbstractProperty*>::iterator iter = pRTTI->GetFirstProperty();

			while(iter != pRTTI->GetProperties()->end())
			{
				AbstractProperty * prop = (*iter);
				const std::string prop_name = prop->GetName();
				if(prop_name == property_name)
				{
					value = prop->GetValueAsString(this);
					return true;
				}
				iter++;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
		return false;
	}

	void BaseReflectionObject::SetProperties(BaseReflectionObjectPtr dest) const
	{
		RTTI* pRTTI = GetRTTI();
		RTTI* pdestRTTI = dest->GetRTTI();

		if(pRTTI == pdestRTTI)
		{
			while(pRTTI)
			{
				std::list<AbstractProperty*>::iterator	iter = pRTTI->GetFirstProperty();
				while(iter != pRTTI->GetProperties()->end())
				{
					AbstractProperty * prop = (*iter);
					prop->SetValue(dest.get(),this);
					iter++;
				}
				pRTTI = pRTTI->GetAncestorRTTI();
			}
		}
		else
		{
			while(pRTTI)
			{
				std::list<AbstractProperty*>::iterator	iter = pRTTI->GetFirstProperty();
				while(iter != pRTTI->GetProperties()->end())
				{
					AbstractProperty * prop = (*iter);
					bool ret = dest->SetPropertyByString(prop->GetName(),prop->GetValueAsString(this));
					//Here we want to copy all common properties from one object to another 
					//(typically from template to instance), so ignore if some properties don't exist in destination object
					//if (!ret)
					//	Log::Warning("BaseReflectionObject::SetProperties() - Property not found: %s", prop->GetName().c_str());
					iter++;
				}
				pRTTI = pRTTI->GetAncestorRTTI();
			}
		}
	}

}
