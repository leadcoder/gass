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
#include "Sim/GASSTaskNode.h"
#include "Sim/GASSRunTimeController.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include <string>

namespace GASS
{

	class ISystemListener
	{

	public:
		virtual void SystemTick(double delta_time) = 0;
	};
	typedef SPTR<ISystemListener> SystemListenerPtr;
	typedef WPTR<ISystemListener> SystemListenerWeakPtr;
	
	class SimSystemManager;
	typedef SPTR<SimSystemManager> SimSystemManagerPtr;
	typedef WPTR<SimSystemManager> SimSystemManagerWeakPtr;

	/** \addtogroup GASSSim
	*  @{
	*/
	/** \addtogroup System
	*  @{
	*/

	/**
		Base class that GASSSim systems should be derived from 
	*/
	class GASSExport SimSystem : public Reflection<SimSystem, BaseReflectionObject>, public SHARE_CLASS<SimSystem>,  public IMessageListener, public IXMLSerialize, public ITaskNodeListener
	{
	public:
		SimSystem();
		virtual ~SimSystem();
		static void RegisterReflection();
		
		virtual void OnCreate(SimSystemManagerPtr owner) {m_Owner=owner;}
		virtual void Init() = 0;
		virtual std::string  GetSystemName() const = 0;
		virtual void Update(double delta);
		virtual std::string GetName() const {return m_Name;}
		virtual void Register(SystemListenerPtr listener);
		virtual void Unregister(SystemListenerPtr listener);
		
		std::string GetTaskNode() const {return m_TaskNodeName;}
		void SetTaskNode(const std::string name) {m_TaskNodeName = name;}
	
		//IXMLSerialize interface
		virtual void LoadXML(tinyxml2::XMLElement *xml_elem);
		virtual void SaveXML(tinyxml2::XMLElement *xml_elem);
		SimSystemManagerPtr GetSimSystemManager() const;
		
	protected:
		std::vector<SystemListenerWeakPtr> m_Listeners;
		std::string m_Name;
		SimSystemManagerWeakPtr m_Owner;
		std::string m_TaskNodeName;
	};
	typedef SPTR<SimSystem> SimSystemPtr;
}
