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
#include <boost/enable_shared_from_this.hpp>

#include "Core/Common.h"
#include "Core/System/ISystem.h"
#include "Core/System/ISystemManager.h"


class TiXmlElement;
namespace GASS
{

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup System
	*  @{
	*/

	/**
		Convinience system manager that load system from xml file
	*/
	
	class GASSCoreExport BaseSystemManager : public ISystemManager, public boost::enable_shared_from_this<BaseSystemManager>
	{
	public:
		BaseSystemManager();
		virtual ~BaseSystemManager();
		/**
			Loading systems from xml-file, syntax example:

			<?xml version="1.0" encoding="utf-8"?>
			<Systems>
				<GraphicsSystem type="OgreGraphicsSystem">
					... gfx system params
	  		    </GraphicsSystem>
				<InputSystem type="OISInputSystem">
					... input system params
				</InputSystem>
				...
			<System>
			A system is specified by a name tag and att type attribute that 
			specify the class implementing the system.
		*/
		void Load(const std::string &filename);

		//ISystemInterface

		/**
			Initliize all systems
		*/
		virtual void Init();

		/**
			Update all systems
		*/
		virtual void Update(double delta_time);

		/**
			Get hold of system by class type. If more then one system
			of the class type exist tbe first one loaded will be returned
		*/
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
	protected:
		std::vector<SystemPtr> m_Systems;
	};
}
#endif // #ifndef BASESYSTEMMANAGER_HH
