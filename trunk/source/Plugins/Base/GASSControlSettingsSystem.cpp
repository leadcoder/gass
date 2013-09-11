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

#include "GASSControlSettingsSystem.h"
#include "GASSControlSetting.h"
#include "GASSController.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSResourceHandle.h"
#include "Core/System/GASSSystemFactory.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSStringUtils.h"
#include "Core/Utils/GASSEnumLookup.h"
#include "Core/Utils/GASSException.h"

#include <tinyxml.h>


namespace GASS
{
	ControlSettingsSystem::ControlSettingsSystem()
	{
		m_InputStringTable = new EnumLookup();
		//Input
		m_InputStringTable->Add("DEVICE_KEYBOARD",DEVICE_KEYBOARD);
		m_InputStringTable->Add("DEVICE_GAME_CONTROLLER_0",DEVICE_GAME_CONTROLLER_0);
		m_InputStringTable->Add("DEVICE_GAME_CONTROLLER_1",DEVICE_GAME_CONTROLLER_1);
		m_InputStringTable->Add("DEVICE_GAME_CONTROLLER_2",DEVICE_GAME_CONTROLLER_2);
		m_InputStringTable->Add("DEVICE_GAME_CONTROLLER_3",DEVICE_GAME_CONTROLLER_3);
		m_InputStringTable->Add("DEVICE_GAME_CONTROLLER_4",DEVICE_GAME_CONTROLLER_4);
		m_InputStringTable->Add("DEVICE_MOUSE",DEVICE_MOUSE);

		m_InputStringTable->Add("INPUT_AXIS_0",INPUT_AXIS_0);
		m_InputStringTable->Add("INPUT_AXIS_1",INPUT_AXIS_1);
		m_InputStringTable->Add("INPUT_AXIS_2",INPUT_AXIS_2);
		m_InputStringTable->Add("INPUT_AXIS_3",INPUT_AXIS_3);
		m_InputStringTable->Add("INPUT_AXIS_4",INPUT_AXIS_4);
		m_InputStringTable->Add("INPUT_AXIS_5",INPUT_AXIS_5);
		m_InputStringTable->Add("INPUT_AXIS_6",INPUT_AXIS_6);
		m_InputStringTable->Add("INPUT_AXIS_7",INPUT_AXIS_7);

		m_InputStringTable->Add("INPUT_POV_0_X",INPUT_POV_0_X);
		m_InputStringTable->Add("INPUT_POV_0_Y",INPUT_POV_0_Y);
		m_InputStringTable->Add("INPUT_POV_1_X",INPUT_POV_1_X);
		m_InputStringTable->Add("INPUT_POV_1_Y",INPUT_POV_1_Y);
		m_InputStringTable->Add("INPUT_POV_2_X",INPUT_POV_2_X);
		m_InputStringTable->Add("INPUT_POV_2_Y",INPUT_POV_2_Y);
		m_InputStringTable->Add("INPUT_POV_3_X",INPUT_POV_3_X);
		m_InputStringTable->Add("INPUT_POV_3_Y",INPUT_POV_3_Y);

		m_InputStringTable->Add("INPUT_BUTTON_0",INPUT_BUTTON_0);
		m_InputStringTable->Add("INPUT_BUTTON_1",INPUT_BUTTON_1);
		m_InputStringTable->Add("INPUT_BUTTON_2",INPUT_BUTTON_2);
		m_InputStringTable->Add("INPUT_BUTTON_3",INPUT_BUTTON_3);
		m_InputStringTable->Add("INPUT_BUTTON_4",INPUT_BUTTON_4);
		m_InputStringTable->Add("INPUT_BUTTON_5",INPUT_BUTTON_5);
		m_InputStringTable->Add("INPUT_BUTTON_6",INPUT_BUTTON_6);
		m_InputStringTable->Add("INPUT_BUTTON_7",INPUT_BUTTON_7);
		m_InputStringTable->Add("INPUT_BUTTON_8",INPUT_BUTTON_8);
		m_InputStringTable->Add("INPUT_BUTTON_9",INPUT_BUTTON_9);
		m_InputStringTable->Add("INPUT_BUTTON_10",INPUT_BUTTON_10);
		m_InputStringTable->Add("INPUT_BUTTON_11",INPUT_BUTTON_11);
		m_InputStringTable->Add("INPUT_BUTTON_12",INPUT_BUTTON_12);
		m_InputStringTable->Add("INPUT_BUTTON_13",INPUT_BUTTON_13);
		m_InputStringTable->Add("INPUT_BUTTON_14",INPUT_BUTTON_14);
		m_InputStringTable->Add("INPUT_BUTTON_15",INPUT_BUTTON_15);	


		m_InputStringTable->Add("INPUT_0",KEY_0);
		m_InputStringTable->Add("INPUT_1",KEY_1);
		m_InputStringTable->Add("INPUT_2",KEY_2);
		m_InputStringTable->Add("INPUT_3",KEY_3);
		m_InputStringTable->Add("INPUT_4",KEY_4);
		m_InputStringTable->Add("INPUT_5",KEY_5);
		m_InputStringTable->Add("INPUT_6",KEY_6);
		m_InputStringTable->Add("INPUT_7",KEY_7);	
		m_InputStringTable->Add("INPUT_8",KEY_8);
		m_InputStringTable->Add("INPUT_9",KEY_9);
		m_InputStringTable->Add("INPUT_A",KEY_A);
		m_InputStringTable->Add("INPUT_ABNT_C1",KEY_ABNT_C1);
		m_InputStringTable->Add("INPUT_ABNT_C2",KEY_ABNT_C2);
		m_InputStringTable->Add("INPUT_ADD",KEY_ADD);
		m_InputStringTable->Add("INPUT_APOSTROPHE",KEY_APOSTROPHE);
		m_InputStringTable->Add("INPUT_APPS",KEY_APPS);
		m_InputStringTable->Add("INPUT_AT",KEY_AT);
		m_InputStringTable->Add("INPUT_AX",KEY_AX);
		m_InputStringTable->Add("INPUT_B",KEY_B);
		m_InputStringTable->Add("INPUT_BACK",KEY_BACK);
		m_InputStringTable->Add("INPUT_BACKSLASH",KEY_BACKSLASH);
		m_InputStringTable->Add("INPUT_BACK",KEY_BACK);
		m_InputStringTable->Add("INPUT_C",KEY_C);
		m_InputStringTable->Add("INPUT_CALCULATOR",KEY_CALCULATOR);
		m_InputStringTable->Add("INPUT_CAPITAL",KEY_CAPITAL);
		//m_InputStringTable->Add("INPUT_CAPSLOCK",KEY_CAPSLOCK);
		m_InputStringTable->Add("INPUT_PREVTRACK",KEY_PREVTRACK);
		m_InputStringTable->Add("INPUT_COLON",KEY_COLON);
		m_InputStringTable->Add("INPUT_COMMA",KEY_COMMA);
		m_InputStringTable->Add("INPUT_CONVERT",KEY_CONVERT);
		m_InputStringTable->Add("INPUT_D",KEY_D);
		m_InputStringTable->Add("INPUT_DECIMAL",KEY_DECIMAL);
		m_InputStringTable->Add("INPUT_DELETE",KEY_DELETE);
		m_InputStringTable->Add("INPUT_DIVIDE",KEY_DIVIDE);
		m_InputStringTable->Add("INPUT_DOWN",KEY_DOWN);
		//m_InputStringTable->Add("INPUT_ARROWDOWN",KEY_DOWNARROW);
		m_InputStringTable->Add("INPUT_E",KEY_E);
		m_InputStringTable->Add("INPUT_END",KEY_END);
		m_InputStringTable->Add("INPUT_EQUALS",KEY_EQUALS);
		m_InputStringTable->Add("INPUT_ESCAPE",KEY_ESCAPE);
		m_InputStringTable->Add("INPUT_F",KEY_F);
		m_InputStringTable->Add("INPUT_F1",KEY_F1);
		m_InputStringTable->Add("INPUT_F2",KEY_F2);
		m_InputStringTable->Add("INPUT_F3",KEY_F3);
		m_InputStringTable->Add("INPUT_F4",KEY_F4);
		m_InputStringTable->Add("INPUT_F5",KEY_F5);
		m_InputStringTable->Add("INPUT_F6",KEY_F6);
		m_InputStringTable->Add("INPUT_F7",KEY_F7);
		m_InputStringTable->Add("INPUT_F8",KEY_F8);
		m_InputStringTable->Add("INPUT_F9",KEY_F9);
		m_InputStringTable->Add("INPUT_F10",KEY_F10);
		m_InputStringTable->Add("INPUT_F11",KEY_F11);
		m_InputStringTable->Add("INPUT_F12",KEY_F12);
		m_InputStringTable->Add("INPUT_F13",KEY_F13);
		m_InputStringTable->Add("INPUT_F14",KEY_F14);
		m_InputStringTable->Add("INPUT_F15",KEY_F15);
		m_InputStringTable->Add("INPUT_G",KEY_G);
		m_InputStringTable->Add("INPUT_GRAVE",KEY_GRAVE);
		m_InputStringTable->Add("INPUT_H",KEY_H);
		m_InputStringTable->Add("INPUT_HOME",KEY_HOME);
		m_InputStringTable->Add("INPUT_I",KEY_I);
		m_InputStringTable->Add("INPUT_INSERT",KEY_INSERT);
		m_InputStringTable->Add("INPUT_J",KEY_J);
		m_InputStringTable->Add("INPUT_K",KEY_K);
		m_InputStringTable->Add("INPUT_KANA",KEY_KANA);
		m_InputStringTable->Add("INPUT_KANJI",KEY_KANJI);
		m_InputStringTable->Add("INPUT_L",KEY_L);
		m_InputStringTable->Add("INPUT_LMENU",KEY_LMENU);
		m_InputStringTable->Add("INPUT_LBRACKET",KEY_LBRACKET);
		//m_InputStringTable->Add("INPUT_LCONTROL",KEY_LCONTROL,
		m_InputStringTable->Add("INPUT_LEFTCTRL",KEY_LCONTROL);
		m_InputStringTable->Add("INPUT_LEFT",KEY_LEFT);
		//m_InputStringTable->Add("INPUT_ARROWLEFT",KEY_LEFTARROW);
		m_InputStringTable->Add("INPUT_LMENU",KEY_LMENU);
		m_InputStringTable->Add("INPUT_LSHIFT",KEY_LSHIFT);
		m_InputStringTable->Add("INPUT_LWIN",KEY_LWIN);
		m_InputStringTable->Add("INPUT_M",KEY_M);
		m_InputStringTable->Add("INPUT_MAIL",KEY_MAIL);
		m_InputStringTable->Add("INPUT_MEDIASELECT",KEY_MEDIASELECT);
		m_InputStringTable->Add("INPUT_MEDIASTOP",KEY_MEDIASTOP);
		m_InputStringTable->Add("INPUT_MINUS",KEY_MINUS);
		m_InputStringTable->Add("INPUT_MULTIPLY",KEY_MULTIPLY);
		m_InputStringTable->Add("INPUT_MUTE",KEY_MUTE);
		m_InputStringTable->Add("INPUT_MYCOMPUTER",KEY_MYCOMPUTER);
		m_InputStringTable->Add("INPUT_N",KEY_N);
		//m_InputStringTable->Add("INPUT_NEXT",KEY_NEXT);
		m_InputStringTable->Add("INPUT_NEXTTRACK",KEY_NEXTTRACK);
		m_InputStringTable->Add("INPUT_NOCONVERT",KEY_NOCONVERT);
		m_InputStringTable->Add("INPUT_NUMLOCK",KEY_NUMLOCK);
		m_InputStringTable->Add("INPUT_NUMPAD0",KEY_NUMPAD0);
		m_InputStringTable->Add("INPUT_NUMPAD1",KEY_NUMPAD1);
		m_InputStringTable->Add("INPUT_NUMPAD2",KEY_NUMPAD2);
		m_InputStringTable->Add("INPUT_NUMPAD3",KEY_NUMPAD3);
		m_InputStringTable->Add("INPUT_NUMPAD4",KEY_NUMPAD4);
		m_InputStringTable->Add("INPUT_NUMPAD5",KEY_NUMPAD5);
		m_InputStringTable->Add("INPUT_NUMPAD6",KEY_NUMPAD6);
		m_InputStringTable->Add("INPUT_NUMPAD7",KEY_NUMPAD7);
		m_InputStringTable->Add("INPUT_NUMPAD8",KEY_NUMPAD8);
		m_InputStringTable->Add("INPUT_NUMPAD9",KEY_NUMPAD9);
		m_InputStringTable->Add("INPUT_DECIMAL",KEY_DECIMAL);
		m_InputStringTable->Add("INPUT_NUMPADENTER",KEY_NUMPADENTER);
		m_InputStringTable->Add("INPUT_NUMPADEQUALS",KEY_NUMPADEQUALS);
		m_InputStringTable->Add("INPUT_SUBTRACT",KEY_SUBTRACT);
		//m_InputStringTable->Add("INPUT_NUMPADPERIOD",KEY_NUMPADPERIOD);
		m_InputStringTable->Add("INPUT_ADD",KEY_ADD);
		m_InputStringTable->Add("INPUT_DIVIDE",KEY_DIVIDE);
		m_InputStringTable->Add("INPUT_MULTIPLY",KEY_MULTIPLY);
		m_InputStringTable->Add("INPUT_O",KEY_O);
		m_InputStringTable->Add("INPUT_OEM_102",KEY_OEM_102);
		m_InputStringTable->Add("INPUT_P",KEY_P);
		m_InputStringTable->Add("INPUT_PAUSE",KEY_PAUSE);
		m_InputStringTable->Add("INPUT_PERIOD",KEY_PERIOD);
		m_InputStringTable->Add("INPUT_PGDOWN",KEY_PGDOWN);
		m_InputStringTable->Add("INPUT_PGUP",KEY_PGUP);
		m_InputStringTable->Add("INPUT_PLAYPAUSE",KEY_PLAYPAUSE);
		m_InputStringTable->Add("INPUT_POWER",KEY_POWER);
		m_InputStringTable->Add("INPUT_PREVTRACK",KEY_PREVTRACK);
		//m_InputStringTable->Add("INPUT_PRIOR",KEY_PRIOR);
		m_InputStringTable->Add("INPUT_Q",KEY_Q);
		m_InputStringTable->Add("INPUT_R",KEY_R);
		//m_InputStringTable->Add("INPUT_RALT",KEY_RALT);
		m_InputStringTable->Add("INPUT_RMENU",KEY_RMENU);
		m_InputStringTable->Add("INPUT_RBRACKET",KEY_RBRACKET);
		m_InputStringTable->Add("INPUT_RIGHTCTRL",KEY_RCONTROL);
		m_InputStringTable->Add("INPUT_RETURN",KEY_RETURN);
		m_InputStringTable->Add("INPUT_RIGHT",KEY_RIGHT);
		m_InputStringTable->Add("INPUT_RIGHT",KEY_RIGHT);
		m_InputStringTable->Add("INPUT_RMENU",KEY_RMENU);
		m_InputStringTable->Add("INPUT_RSHIFT",KEY_RSHIFT);
		m_InputStringTable->Add("INPUT_RWIN",KEY_RWIN);
		m_InputStringTable->Add("INPUT_S",KEY_S);
		m_InputStringTable->Add("INPUT_SCROLL",KEY_SCROLL);
		m_InputStringTable->Add("INPUT_SEMICOLON",KEY_SEMICOLON);
		m_InputStringTable->Add("INPUT_SLASH",KEY_SLASH);
		m_InputStringTable->Add("INPUT_SLEEP",KEY_SLEEP);
		m_InputStringTable->Add("INPUT_SPACE",KEY_SPACE);
		m_InputStringTable->Add("INPUT_STOP",KEY_STOP);
		m_InputStringTable->Add("INPUT_SUBTRACT",KEY_SUBTRACT);
		m_InputStringTable->Add("INPUT_SYSRQ",KEY_SYSRQ);
		m_InputStringTable->Add("INPUT_T",KEY_T);
		m_InputStringTable->Add("INPUT_TAB",KEY_TAB);
		m_InputStringTable->Add("INPUT_U",KEY_U);
		m_InputStringTable->Add("INPUT_UNDERLINE",KEY_UNDERLINE);
		m_InputStringTable->Add("INPUT_UNLABELED",KEY_UNLABELED);
		m_InputStringTable->Add("INPUT_UP",KEY_UP);
		//m_InputStringTable->Add("INPUT_UP",KEY_UP);
		m_InputStringTable->Add("INPUT_V",KEY_V);
		m_InputStringTable->Add("INPUT_VOLUMEDOWN",KEY_VOLUMEDOWN);
		m_InputStringTable->Add("INPUT_VOLUMEUP",KEY_VOLUMEUP);
		m_InputStringTable->Add("INPUT_W",KEY_W);
		m_InputStringTable->Add("INPUT_WAKE",KEY_WAKE);
		m_InputStringTable->Add("INPUT_WEBBACK",KEY_WEBBACK);
		m_InputStringTable->Add("INPUT_WEBFAVORITES",KEY_WEBFAVORITES);
		m_InputStringTable->Add("INPUT_WEBFORWARD",KEY_WEBFORWARD);
		m_InputStringTable->Add("INPUT_WEBHOME",KEY_WEBHOME);
		m_InputStringTable->Add("INPUT_WEBREFRESH",KEY_WEBREFRESH);
		m_InputStringTable->Add("INPUT_WEBSEARCH",KEY_WEBSEARCH);
		m_InputStringTable->Add("INPUT_WEBSTOP",KEY_WEBSTOP);
		m_InputStringTable->Add("INPUT_X",KEY_X);
		m_InputStringTable->Add("INPUT_Y",KEY_Y);
		m_InputStringTable->Add("INPUT_YEN",KEY_YEN);
		m_InputStringTable->Add("INPUT_Z",KEY_Z);
	}

	ControlSettingsSystem::~ControlSettingsSystem()
	{
		delete m_InputStringTable;
		Free();
	}

	void ControlSettingsSystem::Free()
	{
		ControlSettingMap::const_iterator iter = m_ControlSettingMap.begin();
		while(iter != m_ControlSettingMap.end())
		{
			delete iter->second;
			iter++;
		}
		m_ControlSettingMap.clear();
	}
	

	

	void ControlSettingsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("ControlSettingsSystem",new GASS::Creator<ControlSettingsSystem, ISystem>);
	}

	void ControlSettingsSystem::Init() 
	{
		//Register at rtc
		SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);

		//load default settings
		ResourceHandle res("default_control_settings.xml");
		Load(res.GetResource()->Path().GetFullPath());
	}

	ControlSetting* ControlSettingsSystem::GetControlSetting(const std::string &name) const
	{
		ControlSettingMap::const_iterator pos;
		//	name = StringUtils::ToLower(name);
		pos = m_ControlSettingMap.find(name);

		if (pos != m_ControlSettingMap.end()) // not in map.
		{
			return (*pos).second;
		}
		else
		{
			
			return NULL;
		}
	}

	void ControlSettingsSystem::Load(const std::string &filename)
	{
		if(filename =="") 
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS,"No File name provided", "ControlSettingsSystem::Load");
		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
		if (!xmlDoc->LoadFile())
		{
			// Fatal error, cannot load
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load:" + filename, "ControlSettingsSystem::Load");
		}
		TiXmlElement *control_settings = xmlDoc->FirstChildElement("ControlSettings");	
		int nInputCount = 0;
		control_settings = control_settings->FirstChildElement();
	
		//remove previous settings
		Free();

		InputSystemPtr input_system = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<IInputSystem>();

		if(!input_system)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No input system found!", "ControlSettingsSystem::Load");		
		// Loop through each template
		while(control_settings)
		{
			const std::string name = control_settings->Value();
			ControlSetting* cs = new ControlSetting(name,this,input_system.get());
			Add(name,cs);
			TiXmlElement *control_map = control_settings->FirstChildElement();
			while(control_map)
			{
				if(!control_map->Attribute("Controller")) 
					GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No Controller parameter", "ControlSettingsSystem::Load");
				const std::string controller_name = control_map->Attribute("Controller");
				const int action = m_InputStringTable->Get(controller_name);
				if(!control_map->Attribute("InputDevice")) 
					GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No InputDevice parameter", "ControlSettingsSystem::Load");
				const std::string inputdevicename = control_map->Attribute("InputDevice");
				const int inputdevice = m_InputStringTable->Get(inputdevicename);
				if(inputdevice == DEVICE_KEYBOARD)
				{
					if(control_map->Attribute("Key"))
					{
						const std::string input = control_map->Attribute("Key");
						KeyTriggerController* controller = new KeyTriggerController(controller_name,cs);
						controller->m_Device = inputdevice;
						controller->m_Key = m_InputStringTable->Get(input);
						std::string repete;
						if(control_map->Attribute("Repeat")) repete = control_map->Attribute("Repeat");
						if(repete == "false")
						{
							controller->m_NonRepeating = true;
						}else
							controller->m_NonRepeating = false;
						cs->AddController(controller,controller_name,action);
					}
					else if(control_map->Attribute("PosKey") && control_map->Attribute("NegKey"))
					{
						const std::string pos_input = control_map->Attribute("PosKey");
						const std::string neg_input = control_map->Attribute("NegKey");
						KeyAxisController* controller = new KeyAxisController(controller_name,cs);
						controller->m_NonRepeating = false;
						controller->m_Device = inputdevice;
						controller->m_PosKey = m_InputStringTable->Get(pos_input);
						controller->m_NegKey = m_InputStringTable->Get(neg_input);
						cs->AddController(controller,controller_name,action);
					}

				}
				else if((inputdevice >= DEVICE_GAME_CONTROLLER_0 && inputdevice <= DEVICE_GAME_CONTROLLER_4) ||
					inputdevice == DEVICE_MOUSE)
				{

					if(control_map->Attribute("Axis"))//axis
					{
						const std::string input = control_map->Attribute("Axis");
						AxisAxisController* controller = new AxisAxisController(controller_name,cs);
						controller->m_Device = inputdevice;
						controller->m_Axis = m_InputStringTable->Get(input);
						controller->m_NonRepeating = false;

						std::string invert;
						if(control_map->Attribute("Invert")) invert = control_map->Attribute("Invert");
						if(invert =="true") controller->m_Invert = -1;
						else controller->m_Invert = 1;
						cs->AddController(controller,controller_name,action);
					}
					else
					{
						if(control_map->Attribute("Button")) //button
						{
							const std::string input = control_map->Attribute("Button");
							ButtonTriggerController* controller = new ButtonTriggerController(controller_name,cs);
							controller->m_Device = inputdevice;
							controller->m_Button = m_InputStringTable->Get(input);
							std::string repete;
							if(control_map->Attribute("Repeat")) repete = control_map->Attribute("Repeat");
							if(repete == "false")
							{
								controller->m_NonRepeating = true;
							}else
								controller->m_NonRepeating = false;
							cs->AddController(controller,controller_name,action);


						}
						else if(control_map->Attribute("PosButton") && control_map->Attribute("NegButton"))
						{
							const std::string posinput = control_map->Attribute("PosButton");
							const std::string neginput = control_map->Attribute("NegButton");
							ButtonAxisController* controller = new ButtonAxisController(controller_name,cs);
							controller->m_Device = inputdevice;
							controller->m_PosKey = m_InputStringTable->Get(posinput);
							controller->m_NegKey = m_InputStringTable->Get(neginput);
							controller->m_NonRepeating = false;
							cs->AddController(controller,controller_name,action);
						}
					}

				}
				control_map = control_map->NextSiblingElement();
			}
			control_settings = control_settings->NextSiblingElement();
		}
		xmlDoc->Clear();
		// Delete our allocated document and return success ;)
		delete xmlDoc;
	}

	
	ControlSetting* ControlSettingsSystem::NewRemoteControlSetting(const std::string &name)
	{
		ControlSetting* local = GetControlSetting(name);
		ControlSetting* remote = new ControlSetting(name,this,NULL);
		if(local)
		{
			remote->m_IndexToName = local->m_IndexToName;
			remote->m_NameToIndex = local->m_NameToIndex;
			ControlSetting::ControllerMap::iterator iter;
			for(iter = local->m_ControllerMap.begin(); iter != local->m_ControllerMap.end(); ++iter)
			{
				const std::string name = (std::string) iter->first;
				Controller* remote_controller  = new RemoteController("remotecontroller",local);
				remote->m_ControllerMap[name] = remote_controller;
				Controller* controller = (Controller*) iter->second;
				*remote_controller  = *controller;
			}
			/*for(int i = 0;  i< MAX_ACTIONS; i++)
			{
				if(local->m_ActionControllers[i])
				{
					remote->m_ActionControllers[i] = new RemoteController();
					remote->m_ActionControllers[i]->SetValue(0);
				}
			}*/

		}
		return remote;
	}

	void ControlSettingsSystem::Add(const std::string &name,ControlSetting* cs)
	{
		assert(cs); 
		m_ControlSettingMap[name]=cs;
	}

	std::string ControlSettingsSystem::GetNameFromIndex(const std::string &settings, int index)
	{
		ControlSetting* cs = GetControlSetting(settings);
		if(cs)
			return cs->m_IndexToName[index];
		return "";
	}

	int ControlSettingsSystem::GetIndexFromName(const std::string &settings, const std::string &name)
	{
		ControlSetting* cs = GetControlSetting(settings);
		if(cs)
			return cs->m_NameToIndex[name];
		return -1;
	}
	

}
