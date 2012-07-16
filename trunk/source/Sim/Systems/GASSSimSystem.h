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

#include "Sim/GASSCommon.h"
#include "Core/System/GASSISystem.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include <string>

namespace GASS
{
	class ISystemManager;
	typedef boost::weak_ptr<ISystemManager> SystemManagerWeakPtr;

	class SimSystemManager;
	typedef boost::shared_ptr<SimSystemManager> SimSystemManagerPtr;

	/** \addtogroup GASSSim
	*  @{
	*/
	/** \addtogroup System
	*  @{
	*/

	/**
		Base class that GASSSim systems should be derived from 
	*/
	class GASSExport SimSystem : public Reflection<SimSystem, BaseReflectionObject>, public ISystem, public boost::enable_shared_from_this<SimSystem>,  public IMessageListener, public IXMLSerialize
	{
	public:
		SimSystem();
		virtual ~SimSystem();
		static void RegisterReflection();

		virtual void OnCreate();
		virtual std::string GetName() const {return m_Name;}
		virtual void SetName(const std::string &name) {m_Name = name;}
		virtual SystemManagerPtr GetOwner() const {return SystemManagerPtr(m_Owner);}
		virtual void SetOwner(SystemManagerPtr owner){m_Owner = owner;}
		virtual void Register(SystemListenerPtr listener);
		virtual void Unregister(SystemListenerPtr listener);
		virtual void Update(double delta);
		virtual SystemType GetSystemType()  const {return "SimSystem";}
		virtual int GetUpdatePriority() const;
		virtual void SetUpdatePriority(int priority);
	
		//IXMLSerialize interface
		virtual void LoadXML(TiXmlElement *xml_elem);
		virtual void SaveXML(TiXmlElement *xml_elem);

		/**
		Convenience function to get SimSystemManager without casting from ISystemManager
		*/
		SimSystemManagerPtr GetSimSystemManager() const;
		
	protected:
		std::vector<SystemListenerPtr> m_Listeners;
		std::string m_Name;
		SystemManagerWeakPtr m_Owner;
		int m_UpdatePriority;
	};
}
