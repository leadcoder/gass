
#ifndef SIM_SYSTEM_MANAGER_I
#define SIM_SYSTEM_MANAGER_I

%{
#include "Sim/Systems/SimSystemManager.h"
#include "Plugins/CEGUI/LuaScriptManager.h"
#include <boost/shared_ptr.hpp>
%}
%include "Common.i"
%include "Messages.i"

class SimSystemManager
{
public:
	void PostMessage(MessagePtr message);
	
	%extend {
	void RegisterForSystemMessage(const std::string &lua_function,MessagePtr message_type)
	{
		LuaScriptManager::Get().RegisterForSystemMessage(lua_function,message_type);
	}
	}

};



class SimSystemManagerPtr
{
public:
	SimSystemManager* get();
};

//%template(SimSystemManagerPtr) boost::shared_ptr<SimSystemManager>;

#endif