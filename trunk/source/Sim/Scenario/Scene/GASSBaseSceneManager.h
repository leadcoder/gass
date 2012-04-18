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

#include <string>
#include "Sim/GASSCommon.h"
#include "Sim/Scenario/Scene/GASSISceneManager.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/System/GASSISystem.h"


namespace GASS
{

	/**
		This is a convenience class that implements the ISceneManager interface. 
		It also implements the IXMLSerialize interface to enable the 
		derived scene manager to support xml serialization.
		New scene managers can derive from this class to reduce some implementation. 
		To avoid this class to be instantiated the constructors could be declared 
		private, however because we derived from the reflection class this is
		impossible, so we keep it public for now.
	*/

	class GASSExport BaseSceneManager : public Reflection<BaseSceneManager, BaseReflectionObject> , public boost::enable_shared_from_this<BaseSceneManager>, public ISceneManager, public IXMLSerialize, public IMessageListener, public ISystemListener
	{
	public:
		BaseSceneManager();
		virtual ~BaseSceneManager();

		static void RegisterReflection();

		//ISceneManager
		virtual std::string GetName() const {return m_Name;}
		virtual void SetName(const std::string &name) {m_Name = name;}
		virtual ScenarioPtr GetScenario() const {return ScenarioPtr(m_Scenario,boost::detail::sp_nothrow_tag());}//allow null pointer}
		virtual void SetScenario(ScenarioPtr owner){m_Scenario = owner;}
		virtual void OnCreate();
		virtual void SystemTick(double delta_time);
		virtual void Register(SceneManagerListenerPtr listener);
		virtual void Unregister(SceneManagerListenerPtr listener);

		virtual void PreUpdate(){};
		virtual void PostUpdate(){};
	
		//IXMLSerialize
		virtual void LoadXML(TiXmlElement *xml_elem);
		virtual void SaveXML(TiXmlElement *xml_elem);
	protected:
		std::string m_Name;
		ScenarioWeakPtr m_Scenario;
		std::vector<SceneManagerListenerPtr> m_Listeners;
	};
}