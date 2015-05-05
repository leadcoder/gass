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

#include "Sim/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Core/RTC/GASSRuntimeController2.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSScene.h"
#include "Sim/Utils/GASSSimpleProfile.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "tinyxml2.h"
#include <tbb/parallel_for_each.h>


namespace GASS
{
	SimSystemManager::SimSystemManager() 
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

		//support asynchron request
		//SPTR<SimSystemManager> shared_this = shared_from_this();
		//MessageFuncPtr func_ptr(new GASS::MessageFunc<TimeStepRequest>(boost::bind( &SimSystemManager::OnSimulationStepRequest, this, _1 ),shared_this));
		//RegisterForMessage(typeid(TimeStepRequest),func_ptr,0);

		for(size_t i = 0 ; i < m_Systems.size(); i++)
		{
			m_Systems[i]->Init();
			//auto register for updates

			UpdateGroupID ugid = m_Systems[i]->GetUpdateGroup().GetValue();
			if(ugid != UGID_NO_UPDATE)
			{
				TaskNode2* node = SimEngine::Get().GetRootTaskNode()->GetChildByID(ugid);
				node->Register(m_Systems[i]);
			}
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
				return DYNAMIC_PTR_CAST<SimSystem>(m_Systems[i]);
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
		
		tinyxml2::XMLElement *systems = xmlDoc->FirstChildElement("GASS");
		systems = systems->FirstChildElement("Systems");

		if(systems)
		{
			systems= systems->FirstChildElement();
			//Loop through each template
			while(systems)
			{
				SimSystemPtr system = LoadSystem(systems);
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

	SimSystemPtr SimSystemManager::LoadSystem(tinyxml2::XMLElement *system_elem)
	{
		const std::string system_type = system_elem->Value();
		SimSystemPtr system = SystemFactory::Get().Create(system_type);
		if(system)
		{
			XMLSerializePtr  serialize = DYNAMIC_PTR_CAST<IXMLSerialize> (system);
			if(serialize)
				serialize->LoadXML(system_elem);
		}
		return system;
	}
}
