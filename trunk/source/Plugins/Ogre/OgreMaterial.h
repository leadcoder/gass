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

#include <string>
#include <OgreMaterialManager.h>

namespace GASS
{
	class OgreMaterial
	{
	public:
		OgreMaterial(const std::string name) : m_Name(name){}
		OgreMaterial(){}
		virtual ~OgreMaterial(){}
		std::string GetName() const {return m_Name;}
		void SetName(const std::string &name) {m_Name =name;}
		friend std::ostream& operator << (std::ostream& os, const OgreMaterial& mat)
		{
			os << mat.GetName(); 
			return os;
		}
		friend std::istream& operator >> (std::istream& os, OgreMaterial& mat)
		{
			std::string name;
			os >> name;
			mat.SetName(name);
			return os;
		}
	protected:
		std::string m_Name;
	};

	class OgreMaterialPropertyMetaData : public EnumerationPropertyMetaData
	{
	public:
		OgreMaterialPropertyMetaData(const std::string &annotation, PropertyFlags flags, std::string res_group = ""): EnumerationPropertyMetaData(annotation,flags,false),
			m_ResourceGroup(res_group)
		{

		}
		virtual std::vector<std::string> GetEnumeration(BaseReflectionObjectPtr object) const 
		{
			std::vector<std::string> content;
			Ogre::MaterialManager::ResourceMapIterator iter = Ogre::MaterialManager::getSingleton().getResourceIterator();
			while(iter.hasMoreElements())
			{
				Ogre::MaterialPtr ptr = iter.getNext();
				if(m_ResourceGroup == "" ||  ptr->getGroup() == m_ResourceGroup)
				{
					content.push_back(ptr->getName());
				}
			}
			return content;
		}
	private:
		std::string m_ResourceGroup;
	};
	typedef SPTR<OgreMaterialPropertyMetaData> OgreMaterialPropertyMetaDataPtr;
}
