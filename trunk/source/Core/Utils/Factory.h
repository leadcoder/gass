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

#ifndef FACTORY_H_
#define FACTORY_H_
#ifdef WIN32
#pragma warning (disable : 4786)  
#endif
#include <map>
#include <string>
#include "Core/Utils/Misc.h"


namespace GASS
{
	typedef std::string ObjectType;


	template<class Base>
	class CreatorBase
	{
	public:
		typedef boost::shared_ptr<Base> BasePtr;
		virtual ~CreatorBase() {}
		virtual BasePtr Create() const = 0;
	};



	template<class Product, class  Base>
	class Creator : public CreatorBase<Base>
	{

		
	public:
		typedef boost::shared_ptr<Base> BasePtr;
		typedef boost::shared_ptr<Product> ProductPtr;
		virtual BasePtr Create() const 
		{ 
			ProductPtr obj(new Product);
			return boost::shared_static_cast<Base>(obj);
		}
	};




	template<class Base>
	class Factory
	{
	public:
		typedef boost::shared_ptr<Base> BasePtr;
		BasePtr Create(ObjectType type);
		bool Register(ObjectType type, CreatorBase<Base> * pCreator);
	private:
		typedef std::map<ObjectType, CreatorBase<Base> *> CreatorMap;
		CreatorMap m_creatorMap;
	};


	template<class Base>
		bool Factory<Base>::Register(ObjectType type, CreatorBase<Base> * pCreator)
	{
		//type = Misc::ToLower(type);
		typename CreatorMap::iterator it = m_creatorMap.find(type);
		if (it != m_creatorMap.end()) 
		{
			delete pCreator;
			return false;
		}
		m_creatorMap[type] = pCreator;
		return true;
	}



	template<class Base>
		boost::shared_ptr<Base> Factory<Base>::Create(ObjectType type)
	{
		typename CreatorMap::iterator it = m_creatorMap.find(type);
		if (it == m_creatorMap.end()) 
		{
			boost::shared_ptr<Base> ret;
			return ret;
		}

		CreatorBase<Base> * pCreator = (*it).second;
		return pCreator->Create();
	}
}
#endif
