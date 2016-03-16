

#ifndef LuaScriptManager_I
#define LuaScriptManager_I

%{
#include "Core/MessageSystem/IMessage.h"
#include "Plugins/Script/LuaScriptManager.h"
%}

%include "Messages.i"

class LuaScriptManager
{
public:
	LuaScriptManager();
	~LuaScriptManager();
	static LuaScriptManager* GetPtr();
	static LuaScriptManager& Get();
	void RegisterForSystemMessage(const std::string &lua_function,MessagePtr message_type);
};

#endif