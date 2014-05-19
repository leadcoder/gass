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

#ifndef GASS_FACTORY_H
#define GASS_FACTORY_H

#include "Core/Common.h"
#include <map>
#include <string>
#include "Core/Utils/GASSStringUtils.h"
#include "Core/Utils/GASSLogManager.h"

namespace GASS
{

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Utility
	*  @{
	*/

    

	template<class Base, class ConstructorParams>
	class CreatorBase
	{
	public:
		typedef SPTR<Base> BasePtr;
		virtual ~CreatorBase() {}
		virtual BasePtr Create(ConstructorParams* params = NULL) const = 0;
		virtual std::string GetClassName() const = 0;
	};

	template<class Product, class  Base>
	class Creator : public CreatorBase<Base,void>
	{
	public:
		typedef SPTR<Base> BasePtr;
		typedef SPTR<Product> ProductPtr;
		Creator()
		{
			m_ClassName = StringUtils::Demangle(typeid(Product).name());

		}
		virtual BasePtr Create(void* params = NULL) const
		{
			ProductPtr obj(new Product);
			return STATIC_PTR_CAST<Base>(obj);
		}
		virtual std::string GetClassName() const {return m_ClassName;}
		std::string m_ClassName;
	};


	template<class Product, class  Base, class ConstructorParams>
	class ArgumentCreator : public CreatorBase<Base,ConstructorParams>
	{
	public:
		typedef SPTR<Base> BasePtr;
		typedef SPTR<Product> ProductPtr;
		ArgumentCreator()
		{
			m_ClassName = StringUtils::Demangle(typeid(Product).name());

		}
		virtual BasePtr Create(ConstructorParams* params = NULL) const
		{
			ProductPtr obj(new Product(params));
			return STATIC_PTR_CAST<Base>(obj);
		}
		virtual std::string GetClassName() const {return m_ClassName;}
		std::string m_ClassName;
	};



	/**
		Template class that implement the factory pattern
    */

	template<class Base, class ObjectType, class ConstructorParams>
	class Factory
	{
	public:
		typedef SPTR<Base> BasePtr;
		BasePtr Create(ObjectType type, ConstructorParams* params= NULL);
		bool Register(ObjectType type, CreatorBase<Base,ConstructorParams> * pCreator);
		bool Remove(ObjectType type);
		std::string GetClassName(const std::string &factory_name);
		std::string GetFactoryName(const std::string &class_name);
		std::vector<std::string> GetFactoryNames();
	private:
		typedef std::map<ObjectType, CreatorBase<Base, ConstructorParams> *> CreatorMap;
		CreatorMap m_creatorMap;
	};


	template<class Base, class ObjectType, class ConstructorParams>
	bool Factory<Base,ObjectType,ConstructorParams>::Register(ObjectType type, CreatorBase<Base, ConstructorParams> * pCreator)
	{
		typename CreatorMap::iterator it = m_creatorMap.find(type);
		if (it != m_creatorMap.end())
		{
			delete pCreator;
			LogManager::getSingleton().stream() << "WARNING:ObjectType "<< type << " already registred";
			return false;
		}
		m_creatorMap[type] = pCreator;
		return true;
	}

	template<class Base, class ObjectType, class ConstructorParams>
	bool Factory<Base,ObjectType,ConstructorParams>::Remove(ObjectType type)
	{
		typename CreatorMap::iterator it = m_creatorMap.find(type);
		if (it != m_creatorMap.end())
		{
			delete m_creatorMap[type];
			m_creatorMap.erase(it);
			return true;
		}
		return false;
	}
	

	template<class Base, class ObjectType, class ConstructorParams>
	std::string Factory<Base,ObjectType,ConstructorParams>::GetFactoryName(const std::string &class_name)
	{
		typename CreatorMap::iterator it = m_creatorMap.begin();
		while(it != m_creatorMap.end())
		{
			if((*it).second->GetClassName() == class_name)
			{
				return (*it).first;
			}
			++it;
		}
		return std::string("");
	}


	template<class Base, class ObjectType, class ConstructorParams>
	std::string Factory<Base,ObjectType,ConstructorParams>::GetClassName(const std::string &factory_name)
	{
		typename CreatorMap::iterator it = m_creatorMap.find(factory_name);
		if (it != m_creatorMap.end())
		{
			return (*it).second->GetClassName();
		}
		return std::string("");
	}

	template<class Base, class ObjectType, class ConstructorParams>
	std::vector<std::string> Factory<Base,ObjectType,ConstructorParams>::GetFactoryNames()
	{
		std::vector<std::string> names;
		typename CreatorMap::iterator it = m_creatorMap.begin();
		while(it != m_creatorMap.end())
		{
			//names.push_back((*it).second->GetClassName());
			names.push_back((*it).first);
			++it;
		}
		return names;
	}

	template<class Base, class ObjectType, class ConstructorParams>
	SPTR<Base> Factory<Base,ObjectType,ConstructorParams>::Create(ObjectType type, ConstructorParams* params= NULL)
	{
		typename CreatorMap::iterator it = m_creatorMap.find(type);
		if (it == m_creatorMap.end())
		{
			SPTR<Base> ret;
			return ret;
		}

		CreatorBase<Base,ConstructorParams> * pCreator = (*it).second;
		return pCreator->Create(params);
	}
}
#endif // #ifndef FACTORY_HH

