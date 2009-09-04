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
#include <boost/any.hpp>
#include "Core/Utils/Log.h"
#include "Core/MessageSystem/Message.h"
namespace GASS
{
	Message::Message(int type) :
			m_TypeID(type),
			m_SenderID(-1),
			m_Delay(0)
	{

	}

	Message::Message(int type, int sender_id) :
			m_TypeID(type),
			m_SenderID(sender_id),
			m_Delay(0)
	{

	}


	Message::~Message()
	{

	}

	boost::any Message::GetData(const std::string &data_name)
	{
		if(m_Data.end() == m_Data.find(data_name))
		{
			Log::Error("Message data %s doesnt exist",data_name.c_str());
			return boost::any();
		}
		return m_Data[data_name];
	}

	void Message::SetData(const std::string &data_name, boost::any data)
	{
		m_Data[data_name] = data;
	}

	int Message::GetSenderID() const
	{
		return m_SenderID;
	}

	void Message::SetDeliverDelay(double delay)
	{
		m_Delay = delay;
	}




}
