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

#ifndef MESSAGE_HH
#define MESSAGE_HH

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <map>
#include "Core/Common.h"

namespace GASS
{
	/**
		Base class for all messages in the message system.
		If you want to make custom messages you should derive 
		from this class. Note however by using boost::any this 
		class support custom data, see setData for details.  
	*/
	class MessageManager;

	class GASSCoreExport Message
	{
		friend class MessageManager;
	public:
		/**
		Constructor
			type unique id for each Message type.
			
		*/
		Message(int type);

		/**
		Constructor
			type unique id for each Message type.
			sender_id to identify sender, usefull if you want to filter messages by sender
			@remark 
		*/
		Message(int type, int sender_id);
		virtual ~Message();

		/**
		Get data by name
		To get the actual data you have to know what kind
		of data is connected to the name, and then use boost::any_cast
		get hold of it.
		*/
		boost::any GetData(const std::string &data_name);
		
		/**
		Set data by name, name has to be unique for each new data
		this message should contain. The boost::any field mean
		that you can provide any data type you want. 
		*/
		void SetData(const std::string &data_name, boost::any data);


		/**
		Get the sender id, -1 is returned if no sender id was provided in message constructor.
		*/
		int GetSenderID()const;
		
		protected: //give fast access to message manager
			int m_TypeID;
			float m_Timer;
		private:
			int m_SenderID;
			std::map<std::string,boost::any> m_Data;
	};
	typedef boost::shared_ptr<Message> MessagePtr;
	typedef boost::function<void (MessagePtr)> MessageFunc;


	#define MESSAGE_FUNC(X) boost::bind( &X, this, _1 )
}
#endif // #ifndef MESSAGE_HH
