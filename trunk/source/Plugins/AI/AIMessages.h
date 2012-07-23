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

#ifndef AI_MESSAGES_H
#define AI_MESSAGES_H
#include "Sim/GASS.h"

namespace GASS
{
	class DoorMessage : public BaseMessage
	{
	public:
		DoorMessage(bool open, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay) , m_Open(open)
		  {

		  }
		  bool GetOpen() const {return m_Open;}
	private:
		bool m_Open;
	};
	typedef boost::shared_ptr<DoorMessage> DoorMessagePtr;

	/*class NavigationMeshMaterialMessage : public BaseMessage
	{
	public:
		NavigationMeshMaterialMessage(int material_flag, const Vec3 &pos, SenderID sender_id = -1, double delay= 0) : 
		BaseMessage(sender_id , delay) , m_Flag(material_flag), m_Pos(pos)
		{

		}
		int m_Flag;
		Vec3 m_Pos;
	};
	typedef boost::shared_ptr<NavigationMeshMaterialMessage> NavigationMeshMaterialMessagePtr;*/
}
#endif
