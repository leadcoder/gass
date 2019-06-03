/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSystemStepper.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Reflection/GASSBaseReflectionObject.h"

namespace GASS
{

	class ISystemListener
	{
		
	public:
		virtual ~ISystemListener(){}
		virtual void OnPreSystemUpdate(double delta_time) = 0;
		virtual void OnPostSystemUpdate(double delta_time) = 0;
	};
	typedef GASS_SHARED_PTR<ISystemListener> SystemListenerPtr;
	typedef GASS_WEAK_PTR<ISystemListener> SystemListenerWeakPtr;
	
	class SimSystemManager;
	typedef GASS_SHARED_PTR<SimSystemManager> SimSystemManagerPtr;
	typedef GASS_WEAK_PTR<SimSystemManager> SimSystemManagerWeakPtr;

	/** \addtogroup GASSSim
	*  @{
	*/
	/** \addtogroup System
	*  @{
	*/

	/**
		Base class that GASSSim systems should be derived from 
	*/
	class GASSExport SimSystem : public Reflection<SimSystem, BaseReflectionObject>, public GASS_ENABLE_SHARED_FROM_THIS<SimSystem>,  public IMessageListener, public IXMLSerialize
	{
	public:
		static void RegisterReflection();

		SimSystem(SimSystemManagerWeakPtr manager);
		~SimSystem() override;
		virtual void OnPostConstruction() {};
		virtual void OnSystemInit() = 0;
		virtual void OnSystemShutdown() {};
		virtual std::string  GetSystemName() const = 0;
		virtual std::string GetName() const {return m_Name;}
		virtual void RegisterListener(SystemListenerPtr listener);
		virtual void UnregisterListener(SystemListenerPtr listener);
		virtual void OnSystemUpdate(double delta_time);

		//IXMLSerialize interface
		void LoadXML(tinyxml2::XMLElement *xml_elem) override;
		void SaveXML(tinyxml2::XMLElement *xml_elem) override;

		SimSystemManagerPtr GetSimSystemManager() const;
		ADD_PROPERTY(UpdateGroupIDBinder, UpdateGroup)
	
			//internal
		void _Update(double delta_time);
	protected:
		void _PreUpdate(double delta_time);
		void _PostUpdate(double delta_time);
		
		std::vector<SystemListenerWeakPtr> m_Listeners;
		std::string m_Name;
		SimSystemManagerWeakPtr m_Owner;
	};
	typedef GASS_SHARED_PTR<SimSystem> SimSystemPtr;
}
