#ifndef GASS_I
#define GASS_I

%module GASS
%{
//#include "SWIGTest.h"
#include "Sim/SimEngine.h"
//typedef double Float;
using namespace GASS;
%}

%include <std_string.i>
%include "Common.i"
%include "Vector.i"
%include "Messages.i"
%include "SimEngine.i"
%include "SimSystemManager.i"
%include "LuaScriptManager.i"


%wrapper %{
   SWIGEXPORT void swig_new_message_ptr(lua_State* L, MessagePtr* message)
		{
		SWIG_NewPointerObj(L,message,SWIGTYPE_p_MessagePtr,0);
		}
%}

#endif