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
#include "PGIncludes.h"



namespace GASS
{
	class GrassMaterial
	{
	public:
		GrassMaterial(const std::string name) : m_Name(name){}
		GrassMaterial(){}
		virtual ~GrassMaterial(){}
		std::string GetName() const {return m_Name;}
		void SetName(const std::string &name) {m_Name =name;}
		friend std::ostream& operator << (std::ostream& os, const GrassMaterial& mat)
		{
			os << mat.GetName(); 
			return os;
		}
		friend std::istream& operator >> (std::istream& os, GrassMaterial& mat)
		{
			std::string name;
			os >> name;
			mat.SetName(name);
			return os;
		}

		static std::vector<std::string> GetStringEnumeration()
		{
			std::vector<std::string> content;
			Ogre::MaterialManager::ResourceMapIterator iter = Ogre::MaterialManager::getSingleton().getResourceIterator();
			while(iter.hasMoreElements())
			{
#if (OGRE_19_RC1)
				Ogre::MaterialPtr ptr = iter.getNext();
#else
				Ogre::MaterialPtr ptr = iter.getNext().staticCast<Ogre::Material>();
				//Ogre::MaterialPtr ptr = iter.getNext();
#endif
				if(ptr->getGroup() == "GASS_VEGETATION_MATERIALS")
				{
					content.push_back(ptr->getName());
				}
			}
			return content;
		}

		bool static IsMultiValue() {return false;}

	protected:
		std::string m_Name;
	};
}
