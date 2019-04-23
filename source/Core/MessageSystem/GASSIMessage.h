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

#ifndef GASS_I_MESSAGE_H
#define GASS_I_MESSAGE_H

#include "Core/Common.h"
#include "Core/Utils/GASSLokiTypeInfo.h"

namespace GASS
{
	class MessageManager;

	typedef Loki::TypeInfo MessageType;
	typedef int SenderID;


	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Message
	*  @{
	*/

	/**
	Message interface that all messages should implement.
	Most of the methods of this interface is a product of what message manager
	needs to know about a message during message proccessing.
	*/

	class GASSCoreExport IMessage
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(IMessage)
	public:
	
		/**
		Set delay (in seconds) from current frame time until this message should be delivered.
		Note: This time is ignored when using the SendImmediate method of the MessageManager
		*/

		virtual void SetDeliverDelay(double delay) = 0;
		virtual double GetDeliverDelay() const = 0;

		/**
		Get the sender id, -1 is returned if no sender id was provided in message constructor.
		*/
		virtual SenderID GetSenderID() const = 0;
		virtual MessageType GetType() const = 0;

	};
	class IMessageFunc;
	typedef GASS_SHARED_PTR<IMessageFunc> MessageFuncPtr;
	typedef GASS_SHARED_PTR<IMessage> MessagePtr;

	class IMessageListener
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(IMessageListener)
	};
	typedef GASS_SHARED_PTR<IMessageListener> MessageListenerPtr;
	typedef GASS_WEAK_PTR<IMessageListener> MessageListenerWeakPtr;



	/**
		Message function interface that is used by the message manager.
	*/
	class IMessageFunc
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(IMessageFunc)
	public:
		
		/*
		Fire is called by the message manager when a message is being delivered
		*/
		virtual void Fire(MessagePtr message) = 0;

		/*
		This operator is used by the message manager to identify message functions
		*/
		virtual bool operator== (const IMessageFunc &func) const = 0;

		/*
		This function should return a pointer to the object the class callback belongs to
		*/
		virtual MessageListenerPtr GetObjectPtr() const = 0;

		/*
		This function should return a pointer to the actual callback function
		*/
		virtual size_t GetFuncHash() const  = 0;

		virtual const std::type_info& GetTypeID() const = 0;

	};

	/*template <typename T, typename U>
	constexpr int moffset(T const& t, U T::* a) {
		return (char const*)&t - (char const*)&(t.*a);
	}*/

	/**
	Template based implementation of the message function interface.
	This template class is used to create message function objects for
	specific message types. The template parameter  MESSAGE_TYPE,
	specify what kind of message the callback takes as argument.
	*/
	template <class MESSAGE_TYPE>
	class MessageFunc : public IMessageFunc
	{
	public:
		MessageFunc()
		{

		}

		MessageFunc(GASS_FUNCTION<void (GASS_SHARED_PTR<MESSAGE_TYPE>)> func, size_t func_hash, MessageListenerPtr object) : m_Func(func),
			m_Object(object),
			m_FunctionHash(func_hash)
		{

		}
		

		/*
		Implements the Fire function of the IMessageFunc interface.
		In this implementation the message is casted to the message type
		specified by the template argument MESSAGE_TYPE
		*/
		void Fire(MessagePtr message) override
		{
			//cast to this message type
			//TODO: should we use dynamic cast instead so messages of incorrect type can be spotted?
			GASS_SHARED_PTR<MESSAGE_TYPE> typed_mess = GASS_STATIC_PTR_CAST<MESSAGE_TYPE>(message);
			m_Func(typed_mess);
		}

		bool operator== (const IMessageFunc &func) const override
		{
			return (func.GetObjectPtr() == GetObjectPtr() &&
				GetFuncHash() == func.GetFuncHash());
		}

		const std::type_info& GetTypeID() const override
		{
			return m_Func.target_type();
		}

		MessageListenerPtr GetObjectPtr() const override
		{
			return m_Object.lock();
		}

		size_t GetFuncHash() const override
		{
			//return (size_t) m_Func.functor.func_ptr;
			return m_FunctionHash;
		}

		MessageListenerWeakPtr m_Object;
		GASS_FUNCTION<void (GASS_SHARED_PTR<MESSAGE_TYPE>)> m_Func;
		size_t m_FunctionHash;
	};
}

#ifdef GASS_USE_BOOST 
	#include <boost/functional/hash.hpp>
	#ifdef _MSC_VER
		static boost::hash<std::string> GASSMessageHasher;
	#else //to avoid gcc build warnings: "__attribute__((unused))", TODO: investigate this further...
		static boost::hash<std::string> GASSMessageHasher __attribute__((unused));
	#endif
#else
	#ifdef _MSC_VER
		static std::hash<std::string> GASSMessageHasher;
	#else //to avoid gcc build warnings: "__attribute__((unused))", TODO: investigate this further...
		static std::hash<std::string> GASSMessageHasher __attribute__((unused));
	#endif
	
#endif

#define MESSAGE_FUNC(FUNCTION) GASS::MessageFuncPtr(new GASS::MessageFunc<GASS::IMessage>(GASS_BIND( &FUNCTION, this, GASS_PLACEHOLDERS::_1 ), GASSMessageHasher(#FUNCTION), shared_from_this()))
#define TYPED_MESSAGE_FUNC(FUNCTION,TYPED_MESSAGE) GASS::MessageFuncPtr(new GASS::MessageFunc<TYPED_MESSAGE>(GASS_BIND( &FUNCTION, this, GASS_PLACEHOLDERS::_1 ), GASSMessageHasher(#FUNCTION) , shared_from_this()))

#endif // #ifndef MESSAGE_HH
