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

#ifndef ANY_MESSAGE_HH
#define ANY_MESSAGE_HH

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <map>
#include "Core/Common.h"
#include "Core/MessageSystem/IMessage.h"

namespace GASS
{
	/**
		Implementaion of the IMessage interface that use
		boost::any to support any kind of data, see setData for details.  
	*/

	class GASSCoreExport AnyMessage : public IMessage
	{
	public:
		/**
		Constructor
			type unique id for each AnyMessage type.
			sender_id to identify sender, usefull if you want to filter AnyMessages by sender
			@remark 
		*/
		AnyMessage(MessageType type, SenderID sender_id = -1, double delay= 0);

		virtual ~AnyMessage();

		/**
		Get data by name
		To get the actual data you have to know what kind
		of data is connected to the name, and then use boost::any_cast
		get hold of it.
		Note: If you try to get data that not exist a 
		error is logged and the program will exit.
		*/
		boost::any GetData(const std::string &data_name);
		
		/**
		Set data by name, name has to be unique for each new data
		this AnyMessage should contain. The boost::any field mean
		that you can provide any data type you want. 
		*/
		void SetData(const std::string &data_name, boost::any data);

		/**	
		Set delay (in seconds) from current frame time until this AnyMessage should be delivered.
		Note: This time is ignored when using the SendImmediate method of the AnyMessageManager
		*/

		void SetDeliverDelay(double delay);


		/**	
		Get delay (in seconds) util message is delivered.
		*/
		double  GetDeliverDelay() const;


		/**
		Get the sender id, -1 is returned if no sender id was provided in AnyMessage constructor.
		*/
		SenderID GetSenderID() const;


		MessageType GetType() const;
		
		protected: //give fast access to AnyMessage manager
			MessageType m_TypeID;
			double m_Delay;
		private:
			SenderID m_SenderID;
			std::map<std::string,boost::any> m_Data;
	};

	typedef boost::shared_ptr<AnyMessage> AnyMessagePtr;
}
#endif // #ifndef AnyMessage_HH
