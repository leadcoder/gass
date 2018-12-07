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

#include "Sim/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/RTC/GASSTBBManager.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Utils/GASSSimpleProfile.h"
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{
	SimSystemManager::SimSystemManager() : m_SystemStepper(this),
		m_SystemMessageManager(new MessageManager())
	{
		
	}

	SimSystemManager::~SimSystemManager()
	{
	}

	void SimSystemManager::Init()
	{
		m_SystemStepper.OnInit();
		GASS_LOG(LINFO) << "SimSystemManager Initialization Started";
		for(size_t i = 0 ; i < m_Systems.size(); i++)
		{
			m_Systems[i]->Init();
		}
		GASS_LOG(LINFO) << "SimSystemManager Initialization Completed";

		RegisterForMessage(REG_TMESS(SimSystemManager::OnSimulationStepRequest, TimeStepRequest, 0));
	}

	void SimSystemManager::OnSimulationStepRequest(TimeStepRequestPtr message)
	{
		m_SystemStepper.OnTimeStepRequest(message->GetTimeStep());
	}

	void SimSystemManager::OnUpdate(double delta_time)
	{
		m_SystemStepper.OnUpdate(delta_time);
	}

	void SimSystemManager::_UpdateSystems(double delta_time, UpdateGroupID group)
	{
		for (size_t i = 0; i < m_Systems.size(); i++)
		{
			if (group == m_Systems[i]->GetUpdateGroup().GetValue())
			{
				m_Systems[i]->_Update(delta_time);
			}
		}
	}

	void SimSystemManager::SyncMessages(double delta_time)
	{
		m_SystemMessageManager->Update(delta_time);
	}

	size_t SimSystemManager::GetQueuedMessages() const
	{
		return m_SystemMessageManager->GetQueuedMessages();
	}

	int SimSystemManager::RegisterForMessage(const MessageType &type, MessageFuncPtr callback, int priority)
	{
		return m_SystemMessageManager->RegisterForMessage(type, callback, priority); 
	}

	void SimSystemManager::UnregisterForMessage(const MessageType &type,  MessageFuncPtr callback)
	{
		m_SystemMessageManager->UnregisterForMessage(type,  callback);
	}

	void SimSystemManager::PostMessage( SystemMessagePtr message )
	{
		m_SystemMessageManager->PostMessage(message);
	}

	void SimSystemManager::SendImmediate( SystemMessagePtr message )
	{
		m_SystemMessageManager->SendImmediate(message);
	}

	void SimSystemManager::ClearMessages()
	{
		m_SystemMessageManager->Clear();
	}

	SimSystemPtr SimSystemManager::GetSystemByName(const std::string &system_name) const
	{
		for(size_t i = 0 ; i < m_Systems.size(); i++)
		{
			if(system_name ==  m_Systems[i]->GetSystemName())
			{
				return GASS_DYNAMIC_PTR_CAST<SimSystem>(m_Systems[i]);
			}
		}
		return SimSystemPtr();
	}

	void SimSystemManager::Load(const std::string &filename)
	{
		if(filename =="")
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS,"No File name provided", "SimSystemManager::Load");
		
		tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
		if (xmlDoc->LoadFile(filename.c_str()) != tinyxml2::XML_NO_ERROR)
		{
			delete xmlDoc;
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE, "Failed to load:" + filename,"SimSystemManager::Load");
		}
		
		if (tinyxml2::XMLElement *gass_elem = xmlDoc->FirstChildElement("GASS"))
		{
			if (tinyxml2::XMLElement *systems_elem = gass_elem->FirstChildElement("Systems"))
			{

				if (systems_elem->Attribute("MaxUpdateFrequency"))
				{
					double max_update_freq = 0;
					systems_elem->QueryDoubleAttribute("MaxUpdateFrequency", &max_update_freq);
					m_SystemStepper.SetMaxUpdateFrequency(max_update_freq);
				}

				if (systems_elem->Attribute("SimulationUpdateFrequency"))
				{
					double update_freq = 0;
					systems_elem->QueryDoubleAttribute("SimulationUpdateFrequency", &update_freq);
					m_SystemStepper.SetSimulationUpdateFrequency(update_freq);
				}

				if (systems_elem->Attribute("MaxSimulationSteps"))
				{
					int max_steps = 0;
					systems_elem->QueryIntAttribute("MaxSimulationSteps", &max_steps);
					m_SystemStepper.SetMaxSimulationSteps(max_steps);
				}

				tinyxml2::XMLElement *system_elem = systems_elem->FirstChildElement();
				//Load all systems tags
				while (system_elem)
				{
					SimSystemPtr system = LoadSystem(system_elem);
					if (system)
					{
						system->OnCreate(shared_from_this());
						GASS_LOG(LINFO) << system->GetSystemName() << " created";
						m_Systems.push_back(system);
					}
					system_elem = system_elem->NextSiblingElement();
				}
			}
		}
		xmlDoc->Clear();
		// Delete our allocated document and return success
		delete xmlDoc;
	}

	void SimSystemManager::AddSystem(SimSystemPtr system)
	{
		m_Systems.push_back(system);
	}


	SimSystemPtr SimSystemManager::AddSystem(const std::string &system_name)
	{
		SimSystemPtr system = SystemFactory::Get().Create(system_name);
		system->OnCreate(shared_from_this());
		m_Systems.push_back(system);
		return system;
	}

	SimSystemPtr SimSystemManager::LoadSystem(tinyxml2::XMLElement *system_elem)
	{
		const std::string system_type = system_elem->Value();
		SimSystemPtr system = SystemFactory::Get().Create(system_type);
		if(system)
		{
			XMLSerializePtr  serialize = GASS_DYNAMIC_PTR_CAST<IXMLSerialize> (system);
			if(serialize)
				serialize->LoadXML(system_elem);
		}
		return system;
	}
}
