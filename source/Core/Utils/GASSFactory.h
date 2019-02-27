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

#ifndef GASS_FACTORY_H
#define GASS_FACTORY_H

#include "Core/Common.h"
#include <map>
#include <string>
#include "Core/Utils/GASSStringUtils.h"
#include "Core/Utils/GASSLogger.h"

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
		GASS_DECLARE_CLASS_AS_INTERFACE(CreatorBase)
	public:
		typedef GASS_SHARED_PTR<Base> BasePtr;
		virtual BasePtr Create(const ConstructorParams* params = NULL) const = 0;
		virtual std::string GetClassName() const = 0;
	};

	template<class Product, class  Base>
	class Creator : public CreatorBase<Base,void>
	{
	public:
		typedef GASS_SHARED_PTR<Base> BasePtr;
		typedef GASS_SHARED_PTR<Product> ProductPtr;
		Creator()
		{
			m_ClassName = StringUtils::Demangle(typeid(Product).name());

		}
		BasePtr Create(const void* /*params*/ = nullptr) const override
		{
			const ProductPtr obj(new Product);
			return GASS_STATIC_PTR_CAST<Base>(obj);
		}
		virtual std::string GetClassName() const {return m_ClassName;}
		std::string m_ClassName;
	};


	template<class Product, class  Base, class ConstructorParams>
	class ArgumentCreator : public CreatorBase<Base,ConstructorParams>
	{
	public:
		typedef GASS_SHARED_PTR<Base> BasePtr;
		typedef GASS_SHARED_PTR<Product> ProductPtr;
		ArgumentCreator()
		{
			m_ClassName = StringUtils::Demangle(typeid(Product).name());

		}
		virtual BasePtr Create(const ConstructorParams* params = NULL) const
		{
			ProductPtr obj(new Product(params));
			return GASS_STATIC_PTR_CAST<Base>(obj);
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
		typedef GASS_SHARED_PTR<Base> BasePtr;
		BasePtr Create(ObjectType type, ConstructorParams* params= nullptr);
		bool Register(ObjectType type, CreatorBase<Base,ConstructorParams> * pCreator);
		bool Remove(ObjectType type);
		std::string GetClassName(const std::string &factory_name);
		std::string GetFactoryName(const std::string &class_name);
		std::vector<std::string> GetFactoryNames();
	private:
		typedef std::map<ObjectType, CreatorBase< Base, ConstructorParams> *> CreatorMap;
		CreatorMap m_creatorMap;
	};


	template<class Base, class ObjectType, class ConstructorParams>
	bool Factory<Base,ObjectType,ConstructorParams>::Register(ObjectType type, CreatorBase<Base, ConstructorParams> * pCreator)
	{
		const auto it = m_creatorMap.find(type);
		if (it != m_creatorMap.end())
		{
			delete pCreator;
			GASS_LOG(LWARNING) << "ObjectType "<< type << " already registred";
			return false;
		}
		m_creatorMap[type] = pCreator;
		return true;
	}

	template<class Base, class ObjectType, class ConstructorParams>
	bool Factory<Base,ObjectType,ConstructorParams>::Remove(ObjectType type)
	{
		const auto it = m_creatorMap.find(type);
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
		auto it = m_creatorMap.begin();
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
		const auto it = m_creatorMap.find(factory_name);
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
		auto it = m_creatorMap.begin();
		while(it != m_creatorMap.end())
		{
			//names.push_back((*it).second->GetClassName());
			names.push_back((*it).first);
			++it;
		}
		return names;
	}

	template<class Base, class ObjectType, class ConstructorParams>
	GASS_SHARED_PTR<Base> Factory<Base,ObjectType,ConstructorParams>::Create(ObjectType type, ConstructorParams* params)
	{
		const auto it = m_creatorMap.find(type);
		if (it == m_creatorMap.end())
		{
			GASS_SHARED_PTR<Base> ret;
			return ret;
		}

		const CreatorBase<Base,ConstructorParams> * pCreator = (*it).second;
		return pCreator->Create(params);
	}
}
#endif // #ifndef FACTORY_HH

