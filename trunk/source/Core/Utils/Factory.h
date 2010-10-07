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

#ifndef FACTORY_HH
#define FACTORY_HH

#include "Core/Common.h"
#include <map>
#include <string>
#include "Core/Utils/Misc.h"
#include "Core/Utils/Log.h"

namespace GASS
{

    /**

    */

	template<class Base>
	class CreatorBase
	{
	public:
		typedef boost::shared_ptr<Base> BasePtr;
		virtual ~CreatorBase() {}
		virtual BasePtr Create() const = 0;
		virtual std::string GetClassName() const = 0;
	};

	template<class Product, class  Base>
	class Creator : public CreatorBase<Base>
	{


	public:
		typedef boost::shared_ptr<Base> BasePtr;
		typedef boost::shared_ptr<Product> ProductPtr;
		Creator()
		{
			m_ClassName = Misc::Demangle(typeid(Product).name());

		}
		virtual BasePtr Create() const
		{
			ProductPtr obj(new Product);
			return boost::shared_static_cast<Base>(obj);
		}
		virtual std::string GetClassName() const {return m_ClassName;}
		std::string m_ClassName;
	};




	template<class Base, class ObjectType>
	class Factory
	{
	public:
		typedef boost::shared_ptr<Base> BasePtr;
		BasePtr Create(ObjectType type);
		bool Register(ObjectType type, CreatorBase<Base> * pCreator);
		std::string GetFactoryName(const std::string &class_name);
		std::vector<std::string> GetFactoryNames();
	private:
		typedef std::map<ObjectType, CreatorBase<Base> *> CreatorMap;
		CreatorMap m_creatorMap;
	};


	template<class Base, class ObjectType>
	bool Factory<Base,ObjectType>::Register(ObjectType type, CreatorBase<Base> * pCreator)
	{
		typename CreatorMap::iterator it = m_creatorMap.find(type);
		if (it != m_creatorMap.end())
		{
			delete pCreator;
			Log::Warning("ObjectType already registred");
			return false;
		}
		m_creatorMap[type] = pCreator;
		return true;
	}


	template<class Base, class ObjectType>
	std::string Factory<Base,ObjectType>::GetFactoryName(const std::string &class_name)
	{
		typename CreatorMap::iterator it = m_creatorMap.begin();
		while(it != m_creatorMap.end())
		{
			if((*it).second->GetClassName() == class_name)
			{
				return (*it).first;
			}
			it++;
		}
		return std::string("");
	}

	template<class Base, class ObjectType>
	std::vector<std::string> Factory<Base,ObjectType>::GetFactoryNames()
	{
		std::vector<std::string> names;
		typename CreatorMap::iterator it = m_creatorMap.begin();
		while(it != m_creatorMap.end())
		{
			//names.push_back((*it).second->GetClassName());
			names.push_back((*it).first);
			it++;
		}
		return names;
	}

	template<class Base, class ObjectType>
	boost::shared_ptr<Base> Factory<Base,ObjectType>::Create(ObjectType type)
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
#endif // #ifndef FACTORY_HH

