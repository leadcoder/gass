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
			std::string attrib_name = attrib->Value();
			std::string attrib_val = attrib->FirstAttribute()->Value();
			SetPropertyByString(attrib_name,attrib_val);
			attrib  = attrib->NextSiblingElement();
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

	bool BaseReflectionObject::SetPropertyByString(const std::string &attrib_name,const std::string &attrib_val)
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<AbstractProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				AbstractProperty * prop = (*iter);
				std::string prop_name = prop->GetName();
				if(prop_name == attrib_name)
				{
					prop->SetValueByString(this,attrib_val);
					
					return true;
				}
				iter++;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
		return false;
	}

	bool BaseReflectionObject::SetPropertyByType(const std::string &attrib_name, boost::any attribute)
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<AbstractProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				AbstractProperty * prop = (*iter);
				std::string prop_name = prop->GetName();
				if(prop_name == attrib_name)
				{
					prop->SetValue(this,attribute);
					return true;
				}
				iter++;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
		return false;
	}

	bool BaseReflectionObject::GetPropertyByType(const std::string &attrib_name, boost::any &attribute)
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<AbstractProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				AbstractProperty * prop = (*iter);
				std::string prop_name = prop->GetName();
				if(prop_name == attrib_name)
				{
					prop->GetValue(this,attribute);
					return true;
				}
				iter++;
			}
			pRTTI = pRTTI->GetAncestorRTTI();
		}
		return false;
	}


	bool BaseReflectionObject::GetPropertyByString(const std::string &attrib_name, std::string &value)
	{
		RTTI* pRTTI = GetRTTI();
		while(pRTTI)
		{
			std::list<AbstractProperty*>::iterator	iter = pRTTI->GetFirstProperty();
			while(iter != pRTTI->GetProperties()->end())
			{
				AbstractProperty * prop = (*iter);
				std::string prop_name = prop->GetName();
				if(prop_name == attrib_name)
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



	void BaseReflectionObject::SetProperties(BaseReflectionObjectPtr dest)
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
					dest->SetPropertyByString(prop->GetName(),prop->GetValueAsString(this));
					iter++;
				}
				pRTTI = pRTTI->GetAncestorRTTI();
			}
		}
	}

}