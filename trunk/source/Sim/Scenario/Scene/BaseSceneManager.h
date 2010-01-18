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

#include <boost/enable_shared_from_this.hpp>
#include <string>
#include "Sim/Common.h"
#include "Sim/Scenario/Scene/ISceneManager.h"
#include "Core/Reflection/BaseReflectionObject.h"
#include "Core/Serialize/IXMLSerialize.h"



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

	class GASSExport BaseSceneManager : public Reflection<BaseSceneManager, BaseReflectionObject> , public boost::enable_shared_from_this<BaseSceneManager>, public ISceneManager, public IXMLSerialize
	{
	public:
		BaseSceneManager();
		virtual ~BaseSceneManager();

		static void RegisterReflection();

		//ISceneManager
		virtual std::string GetName() const {return m_Name;}
		virtual void SetName(const std::string &name) {m_Name = name;}
		virtual ScenarioScenePtr GetScenarioScene() const {return ScenarioScenePtr(m_Scene,boost::detail::sp_nothrow_tag());}//allow null pointer}
		virtual void SetScenarioScene(ScenarioScenePtr owner){m_Scene = owner;}
		virtual void OnCreate();
		virtual void Update(double delta_time);
		//IXMLSerialize
		virtual void LoadXML(TiXmlElement *xml_elem);
		virtual void SaveXML(TiXmlElement *xml_elem);
	protected:
		std::string m_Name;
		ScenarioSceneWeakPtr m_Scene;
	};
}
