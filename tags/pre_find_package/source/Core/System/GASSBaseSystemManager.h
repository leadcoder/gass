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

#ifndef GASS_BASE_SYSTEM_MANAGER_H
#define GASS_BASE_SYSTEM_MANAGER_H

#include "Core/Common.h"
#include "Core/System/GASSISystem.h"
#include "Core/System/GASSISystemManager.h"

namespace tinyxml2
{
	class XMLElement;
}

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
	
	class GASSCoreExport BaseSystemManager : public ISystemManager, public SHARE_CLASS<BaseSystemManager>
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
		void Load(tinyxml2::XMLElement *systems_elem);

		//ISystemInterface

		/**
			Initliize all systems
		*/
		virtual void Init();

		/**
			Get hold of system by class type. If more then one system
			of the class type exist tbe first one loaded will be returned
		*/
		template <class T>
		SPTR<T> GetFirstSystem()
		{
			SPTR<T> sys;
			for(size_t i = 0 ; i < m_Systems.size(); i++)
			{
				sys = DYNAMIC_PTR_CAST<T>(m_Systems[i]);
				if(sys)
					break;
			}
			return sys;
		}
	private:
		SystemPtr LoadSystem(tinyxml2::XMLElement *system_elem);
	protected:
		typedef std::vector<SystemPtr> SystemVector;
		SystemVector m_Systems;
		typedef std::map<int,SystemVector> UpdateMap;
		UpdateMap m_UpdateBuckets;
	};
}
#endif 