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

#ifndef BASESYSTEMMANAGER_HH
#define BASESYSTEMMANAGER_HH

#include <list>
#include <map>
#include <vector>

#include "Core/Common.h"
#include "Core/MessageSystem/MessageType.h"
#include "Core/System/ISystem.h"
#include "Core/System/ISystemManager.h"

class TiXmlElement;
namespace GASS
{
	class MessageManager;
	class GASSCoreExport BaseSystemManager : public ISystemManager
	{
	public:
		BaseSystemManager();
		virtual ~BaseSystemManager();
		bool Load(const std::string &filename);

		//ISystemInterface
		virtual void Init();
		virtual void Update(double delta_time);

		template <class T>
		boost::shared_ptr<T> GetFirstSystem()
		{
			boost::shared_ptr<T> sys;
			for(int i = 0 ; i < m_Systems.size(); i++)
			{
				sys = boost::shared_dynamic_cast<T>(m_Systems[i]);
				if(sys)
					break;
			}
			return sys;
		}
	private:
		SystemPtr LoadSystem(TiXmlElement *system_elem);
	private:
		std::vector<SystemPtr> m_Systems;
	};
}
#endif // #ifndef BASESYSTEMMANAGER_HH
