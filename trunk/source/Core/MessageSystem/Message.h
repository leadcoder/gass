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
#pragma once
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <map>
#include "Core/Common.h"

namespace GASS
{
	
	class GASSCoreExport Message
	{
	public:
		Message(int type, int from);
		virtual ~Message();
		boost::any GetData(const std::string &data_name);
		void SetData(const std::string &data_name, boost::any data);
	
		//public for fast access
		int m_TypeID;
		int m_FromID;
		int m_ToID;
		float m_Timer;
		std::map<std::string,boost::any> m_Data;
	};
	typedef boost::shared_ptr<Message> MessagePtr;
	typedef boost::function<void (MessagePtr)> MessageFunc;
	

	#define MESSAGE_FUNC(X) boost::bind( &X, this, _1 )
}