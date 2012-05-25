#ifndef GASS_I
#define GASS_I

%module GASS
%include "std_string.i"  
%apply const std::string& {std::string* foo}; 
%{
//#include "Sim/GASS.h"
//typedef double Float;
//using namespace GASS;
%}

//%include <std_string.i>
//%include <std_vector.i>
//%include "Common.i"
//%include "Vector.i"
%include "LuaScriptSceneObject.i"

#endif