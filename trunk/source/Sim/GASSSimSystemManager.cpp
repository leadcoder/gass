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

#include "Core/System/GASSISystem.h"
#include "Core/System/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSScene.h"
#include "Sim/Utils/GASSSimpleProfile.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "tinyxml.h"
#include "tbb/parallel_for_each.h"


namespace GASS
{
	SimSystemManager::SimSystemManager() // :m_SimulationPaused(true), 
		//m_SimulationUpdateInterval(1.0/60.0),
		//m_SimulationTimeToProcess(0),
		//m_MaxSimSteps(4),
		//m_SimulateRealTime(true),
		//m_LastNumSimulationSteps(0),
		//m_StepSimulationRequest(false),
		//m_RequestDeltaTime(0)
	{
		m_SystemMessageManager = MessageManagerPtr(new MessageManager());
		//m_SimStats = new SimpleProfileDataMap;
	}

	SimSystemManager::~SimSystemManager()
	{
		//delete m_SimStats;
	}

	void SimSystemManager::Init()
	{
		LogManager::getSingleton().stream() << "SimSystemManager Initialization Started";

		//support asyncron request
		//SPTR<SimSystemManager> shared_this = shared_from_this();
		//MessageFuncPtr func_ptr(new GASS::MessageFunc<TimeStepRequest>(boost::bind( &SimSystemManager::OnSimulationStepRequest, this, _1 ),shared_this));
		//RegisterForMessage(typeid(TimeStepRequest),func_ptr,0);

		for(size_t i = 0 ; i < m_Systems.size(); i++)
		{
			m_Systems[i]->Init();
		}
		LogManager::getSingleton().stream() << "SimSystemManager Initialization Completed";
	}	



	void SimSystemManager::SyncMessages(double delta_time)
	{
		m_SystemMessageManager->Update(delta_time);
	}

	size_t SimSystemManager::GetQueuedMessages() const
	{
		int num = (int) m_SystemMessageManager->GetQueuedMessages();
		return (size_t) num;
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
				return DYNAMIC_CAST<SimSystem>(m_Systems[i]);
			}
		}
		return SimSystemPtr();
	}


	void SimSystemManager::Load(const std::string &filename)
	{
		if(filename =="")
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS,"No File name provided", "SimSystemManager::Load");
		
		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
		if (!xmlDoc->LoadFile())
		{
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE, "Failed to load:" + filename,"SimSystemManager::Load");
		}
		
		TiXmlElement *systems = xmlDoc->FirstChildElement("Systems");

		if(systems)
		{
			systems= systems->FirstChildElement();
			//Loop through each template
			while(systems)
			{
				SystemPtr system = LoadSystem(systems);
				if(system)
				{
					system->OnCreate(shared_from_this());
					LogManager::getSingleton().stream() << system->GetSystemName() << " created";
					
					m_Systems.push_back(system);
					
				}
				systems  = systems->NextSiblingElement();
			}
		}
		xmlDoc->Clear();
		// Delete our allocated document and return success ;)
		delete xmlDoc;
	}

	SystemPtr SimSystemManager::LoadSystem(TiXmlElement *system_elem)
	{
		const std::string system_type = system_elem->Value();
		SystemPtr system = SystemFactory::Get().Create(system_type);
		if(system)
		{
			XMLSerializePtr  serialize = DYNAMIC_CAST<IXMLSerialize> (system);
			if(serialize)
				serialize->LoadXML(system_elem);
		}
		return system;
	}
}
