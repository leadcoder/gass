#pragma once
#include "Core/Utils/GASSException.h"
#include <unordered_map>
#include <utility>

namespace GASS
{
	template <class KeyType, class BasePointerType, typename...ARGs>
	class GenericFactory {
	public:
		typedef BasePointerType result_type;
		virtual ~GenericFactory() {}

		//Registers a derived type against a particular key.
		template <class DerivedType>
		void Register(const KeyType& key)
		{
			m_CreatorMap[key] = BindFunction<DerivedType, ARGs...>();
		}

		//Add template specializations to support std::bind,
		//Workaround due to fact that std::bind don't support ARGs... as placeholders
		template <class DerivedType>
		std::function<BasePointerType(ARGs...)> BindFunction()
		{
			return std::bind(&GenericFactory::CreateImpl<DerivedType>, this);
		}

		template <class DerivedType, typename P0>
		std::function<BasePointerType(ARGs...)> BindFunction()
		{
			return std::bind(&GenericFactory::CreateImpl<DerivedType>, this, std::placeholders::_1);
		}

		template <class DerivedType, typename P0, typename P1>
		std::function<BasePointerType(ARGs...)> BindFunction()
		{
			return std::bind(&GenericFactory::CreateImpl<DerivedType>, this, std::placeholders::_1, std::placeholders::_2);
		}

		template <class DerivedType, typename P0, typename P1, typename P2>
		std::function<BasePointerType(ARGs...)> BindFunction()
		{
			return std::bind(&GenericFactory::CreateImpl<DerivedType>, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		}

		template <class DerivedType, typename P0, typename P1, typename P2, typename P3>
		std::function<BasePointerType(ARGs...)> BindFunction()
		{
			return std::bind(&GenericFactory::CreateImpl<DerivedType>, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		}

		//This method performs the creation of the derived type object on the heap.
		template <class DerivedType>
		BasePointerType CreateImpl(ARGs...args)
		{
			BasePointerType new_object(new DerivedType(std::forward<ARGs>(args)...));
			return new_object;
		}

		//Deregisters an existing registration.
		bool Deregister(const KeyType& key)
		{
			return (m_CreatorMap.erase(key) == 1);
		}

		//Returns true if the key is registered in this factory, false otherwise.
		bool IsCreatable(const KeyType& key) const
		{
			return (m_CreatorMap.count(key) != 0);
		}

		//Creates the derived type associated with key. Throws ::out_of_range if key not found.
		BasePointerType Create(const KeyType& key, ARGs...args) const
		{
			BasePointerType ret;

			try
			{
				ret = m_CreatorMap.at(key)(std::forward<ARGs>(args)...);
			}
			catch (...) //std::out_of_range, cast custom exception...
			{
				GASS_EXCEPT(GASS::Exception::ERR_ITEM_NOT_FOUND, "Failed to create object:" + key, "GenericFactory_::Create");
			}
			return  ret;
		}

		std::vector<KeyType> GetAllKeys() const
		{
			std::vector<KeyType> keys;
			typename CreatorMapType::const_iterator it = m_CreatorMap.begin();
			while (it != m_CreatorMap.end())
			{
				//names.push_back((*it).second->GetClassName());
				keys.push_back((*it).first);
				++it;
			}
			return keys;
		}

	private:
		typedef std::function<BasePointerType(ARGs...)> CreatorFuncType;
		typedef std::unordered_map<KeyType, CreatorFuncType> CreatorMapType;
		CreatorMapType m_CreatorMap;
	};
}
