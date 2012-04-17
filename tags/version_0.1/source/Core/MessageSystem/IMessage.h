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

#ifndef I_MESSAGE_HH
#define I_MESSAGE_HH

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <map>
#include "Core/Common.h"
#include "Core/Utils/lokitypeinfo.h"

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
	public:
		virtual ~IMessage(){};


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
	typedef boost::shared_ptr<IMessageFunc> MessageFuncPtr;
	typedef boost::shared_ptr<IMessage> MessagePtr;

	class IMessageListener
	{
	public:
		virtual ~IMessageListener(){};
	};
	typedef boost::shared_ptr<IMessageListener> MessageListenerPtr;
	typedef boost::weak_ptr<IMessageListener> MessageListenerWeakPtr;


	
	/**
		Message function interface that is used by the message manager.
	*/
	class IMessageFunc
	{
	public:
		virtual ~IMessageFunc(){};
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
		virtual void* GetFuncPtr() const  = 0;
	};



	/**
	Template based implementation of the message function interface.
	This template class is used to create message function objects for
	specific message types. The template paramerer  MESSAGE_TYPE,
	specify what kind of message the callback takes as argument.
	*/
	template <class MESSAGE_TYPE>
	class MessageFunc : public IMessageFunc
	{
	public:
		MessageFunc() 
		{

		}

		MessageFunc(boost::function<void (boost::shared_ptr<MESSAGE_TYPE>)> func, MessageListenerPtr object) : m_Func(func), m_Object(object)
		{

		}

		virtual ~MessageFunc()
		{
		}

		/*
		Implements the Fire function of the IMessageFunc interface.
		In this implementation the message is casted to the message type
		specified by the template argument MESSAGE_TYPE
		*/
		void Fire(MessagePtr message)
		{
			//cast to this message type
			//TODO: should we use dynamic cast instead so messages of incorrect type can be spotted?
			boost::shared_ptr<MESSAGE_TYPE> typed_mess = boost::shared_static_cast<MESSAGE_TYPE>(message);
			m_Func(typed_mess);
		}

		bool operator== (const IMessageFunc &func) const
		{
			return (func.GetObjectPtr() == GetObjectPtr()) &&
				(m_Func.functor.func_ptr == func.GetFuncPtr());
		}

		MessageListenerPtr GetObjectPtr() const
		{
			return MessageListenerPtr(m_Object, boost::detail::sp_nothrow_tag());
		}

		void* GetFuncPtr() const
		{
			return (void*) m_Func.functor.func_ptr;
		}

		MessageListenerWeakPtr m_Object;
		boost::function<void (boost::shared_ptr<MESSAGE_TYPE>)> m_Func;
	};


	
}

#define MESSAGE_FUNC(FUNCTION) GASS::MessageFuncPtr(new GASS::MessageFunc<GASS::IMessage>(boost::bind( &FUNCTION, this, _1 ),shared_from_this()))
#define TYPED_MESSAGE_FUNC(FUNCTION,TYPED_MESSAGE) GASS::MessageFuncPtr(new GASS::MessageFunc<TYPED_MESSAGE>(boost::bind( &FUNCTION, this, _1 ),shared_from_this()))
//#define TYPED_MESSAGE_FUNC_CUSTOM_LISTENER(FUNCTION,TYPED_MESSAGE, LISTENER) GASS::MessageFuncPtr(new GASS::MessageFunc<TYPED_MESSAGE>(boost::bind( &FUNCTION, this, _1 ),LISTENER))



#endif // #ifndef MESSAGE_HH
