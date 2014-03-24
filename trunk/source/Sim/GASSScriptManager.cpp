/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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

#include "Sim/GASSScriptManager.h"
#include "Sim/GASSResourceGroup.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Core/Utils/GASSStringUtils.h"
#include "Core/Utils/GASSXMLUtils.h"
#include "Core/Utils/GASSException.h"
#include "Core/ComponentSystem/GASSBaseComponent.h"
#include <tinyxml.h>
#include <angelscript.h>
//addons
#include "Sim/Utils/Script/scriptstdstring.h"
#include "Sim/Utils/Script/ScriptBuilder.h"
#include "Sim/Interface/GASSILocationComponent.h"
namespace GASS
{

	void MessageCallback(const asSMessageInfo *msg, void *param)
	{
		const char *type = "ERR ";
		if( msg->type == asMSGTYPE_WARNING ) 
			type = "WARN";
		else if( msg->type == asMSGTYPE_INFORMATION ) 
			type = "INFO";
		//printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
		//LogManager::Get().stream() << msg->section << "(" << msg->row << ", " << msg->col << ") : " << type << " : " << msg->message;
		std::stringstream ss;
		ss << msg->section << "(" << msg->row << ", " << msg->col << ") : " << type << " : " << msg->message;
		SimEngine::Get().GetSimSystemManager()->PostMessage(ScriptEventPtr(new ScriptEvent(ss.str())));
	}

	// Function implementation with native calling convention
	void PrintString(std::string &str)
	{
		SimEngine::Get().GetSimSystemManager()->PostMessage(ScriptEventPtr(new ScriptEvent(str)));
		//std::cout << str;
	}

	ScriptManager::ScriptManager(void) 
	{
		m_Engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		if( m_Engine == 0 )
		{
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Failed to create script engine","ScriptManager::ScriptManager");
		}
		// The script compiler will write any compiler messages to the callback.
		m_Engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	}

	ScriptManager::~ScriptManager(void)
	{
		// Release the engine
		m_Engine->Release();
	}

	static void Vec3DefaultConstructor(Vec3 *self)
	{
		new(self) Vec3();
	}

	static void Vec3Constructor2(double x, double y, double z, Vec3 *self)
	{
		new(self) Vec3(x, y, z);
	}

	static void Vec3CopyConstruct(const Vec3 &other, Vec3 *thisPointer)
	{
		new(thisPointer) Vec3(other);
	}

	static void Vec3Destruct(Vec3 *thisPointer)
	{
		thisPointer->~Vec3();
	}

	static Vec3 &Vec3Assignment(Vec3 *other, Vec3 *self)
	{
		return *self = *other;
	}

	static void QuaternionDefaultConstructor(Quaternion *self)
	{
		new(self) Quaternion();
	}

	static void QuaternionConstructor2(double x, double y, double z, double w, Quaternion *self)
	{
		new(self) Quaternion(x, y, z, w);
	}

	static void QuaternionCopyConstruct(const Quaternion &other, Quaternion *thisPointer)
	{
		new(thisPointer) Quaternion(other);
	}

	static void QuaternionDestruct(Quaternion *thisPointer)
	{
		thisPointer->~Quaternion();
	}

	static Quaternion &QuaternionAssignment(Quaternion *other, Quaternion *self)
	{
		return *self = *other;
	}


	void ScriptManager::OnScriptEvent(ScriptEventPtr message)
	{
		LogManager::Get().stream() << message->GetMessage();
	}

	void ScriptManager::Init()
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(ScriptManager::OnScriptEvent,ScriptEvent,0));
		m_Engine->SetEngineProperty(asEP_ALLOW_UNSAFE_REFERENCES, true);
		RegisterStdString(m_Engine);
		int r = m_Engine->RegisterGlobalFunction("void Print(string &in)", asFUNCTION(PrintString), asCALL_CDECL); assert( r >= 0 );
		//r = m_Engine->RegisterGlobalFunction("uint GetSystemTime()", asFUNCTION(timeGetTime), asCALL_STDCALL); assert( r >= 0 );
		r = m_Engine->RegisterObjectType("Vec3", sizeof(Vec3), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA | asOBJ_APP_CLASS_ALLFLOATS); assert( r >= 0 );

		r = m_Engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT,  "void f()",	asFUNCTION(Vec3DefaultConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = m_Engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT,  "void f(double, double, double)",	asFUNCTION(Vec3Constructor2),	asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = m_Engine->RegisterObjectBehaviour("Vec3", asBEHAVE_DESTRUCT,   "void f()",                    asFUNCTION(Vec3Destruct),  asCALL_CDECL_OBJLAST); assert( r >= 0 );
		r = m_Engine->RegisterObjectMethod("Vec3", "Vec3 &opAssign(Vec3&in)", asFUNCTION(Vec3Assignment), asCALL_CDECL_OBJLAST); assert( r >= 0 );
		r = m_Engine->RegisterObjectProperty("Vec3", "double x", offsetof(Vec3, x)); assert( r >= 0 );
		r = m_Engine->RegisterObjectProperty("Vec3", "double y", offsetof(Vec3, y)); assert( r >= 0 );
		r = m_Engine->RegisterObjectProperty("Vec3", "double z", offsetof(Vec3, z)); assert( r >= 0 );

		r = m_Engine->RegisterObjectMethod("Vec3", "Vec3 opAdd(const Vec3 &in) const", asMETHODPR(Vec3, operator+, (const Vec3&) const,	Vec3),	asCALL_THISCALL); assert(r >= 0);
		r = m_Engine->RegisterObjectMethod("Vec3", "Vec3 opSub(const Vec3 &in) const", asMETHODPR(Vec3, operator-, (const Vec3&) const,	Vec3),	asCALL_THISCALL); assert(r >= 0);
		r = m_Engine->RegisterObjectMethod("Vec3", "Vec3 opMul(const Vec3 &in) const", asMETHODPR(Vec3, operator*, (const Vec3&) const, Vec3),	asCALL_THISCALL); assert(r >= 0);
		r = m_Engine->RegisterObjectMethod("Vec3", "Vec3 opMul(double) const", asMETHODPR(Vec3, operator*, (Float) const, Vec3),	asCALL_THISCALL); assert(r >= 0);
		r = m_Engine->RegisterObjectMethod("Vec3", "Vec3 opNeg() const", asMETHODPR(Vec3, operator-, () const, Vec3), asCALL_THISCALL); assert(r >= 0);
		
		r = m_Engine->RegisterObjectType("Quaternion", sizeof(Quaternion), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA | asOBJ_APP_CLASS_ALLFLOATS); assert( r >= 0 );
		r = m_Engine->RegisterObjectBehaviour("Quaternion", asBEHAVE_CONSTRUCT,  "void f()",	asFUNCTION(QuaternionDefaultConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = m_Engine->RegisterObjectBehaviour("Quaternion", asBEHAVE_CONSTRUCT,  "void f(double, double, double, double)",	asFUNCTION(QuaternionConstructor2),	asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = m_Engine->RegisterObjectBehaviour("Quaternion", asBEHAVE_DESTRUCT,   "void f()",                    asFUNCTION(QuaternionDestruct),  asCALL_CDECL_OBJLAST); assert( r >= 0 );
		r = m_Engine->RegisterObjectMethod("Quaternion", "Quaternion &opAssign(Quaternion&in)", asFUNCTION(QuaternionAssignment), asCALL_CDECL_OBJLAST); assert( r >= 0 );
		r = m_Engine->RegisterObjectProperty("Quaternion", "double x", offsetof(Quaternion, x)); assert( r >= 0 );
		r = m_Engine->RegisterObjectProperty("Quaternion", "double y", offsetof(Quaternion, y)); assert( r >= 0 );
		r = m_Engine->RegisterObjectProperty("Quaternion", "double z", offsetof(Quaternion, z)); assert( r >= 0 );
		r = m_Engine->RegisterObjectProperty("Quaternion", "double w", offsetof(Quaternion, w)); assert( r >= 0 );
		r = m_Engine->RegisterObjectMethod("Quaternion", "void FromEulerAngles(const Vec3&in)", asMETHOD(Quaternion, FromEulerAngles), asCALL_THISCALL);
		r = m_Engine->RegisterObjectMethod("Quaternion", "Quaternion opMul(const Quaternion &in) const", asMETHODPR(Quaternion, operator*, (const Quaternion&) const, Quaternion),	asCALL_THISCALL); assert(r >= 0);
		
		r = m_Engine->RegisterObjectType("BaseSceneComponent", 0, asOBJ_REF | asOBJ_NOCOUNT); assert( r >= 0 );
		r = m_Engine->RegisterObjectMethod("BaseSceneComponent", "string GetName() const", asMETHOD(BaseComponent, GetName), asCALL_THISCALL);

		//m_Engine->RegisterObjectType("BaseComponentContainer", 0, asOBJ_REF | asOBJ_NOCOUNT); assert( r >= 0 );
		//m_Engine->RegisterObjectMethod("BaseComponentContainer", "string GetName() const", asMETHOD(BaseComponentContainer, GetName), asCALL_THISCALL);
		r = m_Engine->RegisterObjectType("SceneObject", 0, asOBJ_REF | asOBJ_NOCOUNT); assert( r >= 0 );
		//r = m_Engine->RegisterObjectBehaviour("BaseComponentContainer", asBEHAVE_REF_CAST, "SceneObject@ f()", asFUNCTION((refCast<BaseComponentContainer,SceneObject>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );
		//r = m_Engine->RegisterObjectBehaviour("SceneObject", asBEHAVE_IMPLICIT_REF_CAST, "BaseComponentContainer@ f()", asFUNCTION((refCast<SceneObject,BaseComponentContainer>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );
		r = m_Engine->RegisterObjectMethod("SceneObject", "string GetName() const", asMETHOD(BaseComponentContainer, GetName), asCALL_THISCALL);
		r = m_Engine->RegisterObjectMethod("SceneObject", "BaseSceneComponent @ GetComponentByClassName(const string &in class_name)", asMETHOD(SceneObject, GetComponentByClassName), asCALL_THISCALL);


		//r = m_Engine->RegisterObjectType("ILocationComponent", 0, asOBJ_REF | asOBJ_NOCOUNT); assert( r >= 0 );
		//r = m_Engine->RegisterObjectBehaviour("BaseSceneComponent", asBEHAVE_REF_CAST, "ILocationComponent@ f()", asFUNCTION((refCast<BaseSceneComponent,ILocationComponent>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );
		//r = m_Engine->RegisterObjectBehaviour("LocationComponent", asBEHAVE_IMPLICIT_REF_CAST, "BaseSceneComponent@ f()", asFUNCTION((refCast<OSGLocationComponent,BaseSceneComponent>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );
		//r = m_Engine->RegisterObjectMethod("ILocationComponent", "Vec3 GetPosition() const", asMETHODPR(ILocationComponent, GetPosition,() const, Vec3), asCALL_THISCALL);assert(r >= 0);

		/*ScriptControllerPtr  controller = LoadScript("c:\\temp\\test.as");

		asIScriptFunction *my_func = controller->GetModule()->GetFunctionByDecl("void onTick(SceneObject @)");
		asIScriptContext *ctx = PrepareContextFromPool(my_func);

		SceneObjectPtr hej (new SceneObject());
		hej->SetName("cool");
		ctx->SetArgObject(0, hej.get());

		ExecuteCall(ctx);
		ReturnContextToPool(ctx);


		my_func = controller->GetModule()->GetFunctionByDecl("void onTestPos(const Vec3 &in)");
		ctx = PrepareContextFromPool(my_func);

		Vec3 pos(10,0,0);
		//hej->SetName("cool");
		ctx->SetArgAddress(0, &pos);
		//ctx->SetArgObject(0, pos);

		ExecuteCall(ctx);
		ReturnContextToPool(ctx);
		*/
	}

	int ScriptManager::ExecuteCall(asIScriptContext *ctx)
	{
		int r = ctx->Execute();
		if( r != asEXECUTION_FINISHED )
		{
			// The execution didn't finish as we had planned. Determine why.
			if( r == asEXECUTION_ABORTED )
				std::cout << "The script was aborted before it could finish. Probably it timed out." << std::endl;
			else if( r == asEXECUTION_EXCEPTION )
			{
				std::cout << "The script ended with an exception." << std::endl;

				// Write some information about the script exception
				asIScriptFunction *func = ctx->GetExceptionFunction();
				std::cout << "func: " << func->GetDeclaration() << std::endl;
				std::cout << "modl: " << func->GetModuleName() << std::endl;
				std::cout << "sect: " << func->GetScriptSectionName() << std::endl;
				std::cout << "line: " << ctx->GetExceptionLineNumber() << std::endl;
				std::cout << "desc: " << ctx->GetExceptionString() << std::endl;
			}
			else
				std::cout << "The script ended for some unforeseen reason (" << r << ")." << std::endl;
		}
		return r;
	}


	ScriptControllerPtr ScriptManager::LoadScript(const std::string &script,const std::string &init_func_arg)
	{
		int r;
		// Find the cached controller

		std::map<std::string,ScriptControllerPtr>::iterator iter = m_ScriptControllers.find(script);
		if( iter != m_ScriptControllers.end())
		{
			return iter->second;
		}

		// No controller, check if the script has already been loaded
		asIScriptModule *mod = m_Engine->GetModule(script.c_str(), asGM_ONLY_IF_EXISTS);
		if( mod )
		{
			// We've already attempted loading the script before, but there is no controller
			return 0;
		}

		// Compile the script into the module
		CScriptBuilder builder;
		r = builder.StartNewModule(m_Engine, script.c_str());
		if( r < 0 )
			return 0;

		// If the script file doesn't exist, then there is no script controller for this type
		FILE *f;
		if( (f = fopen((script).c_str(), "r")) == 0 )
			return 0;
		fclose(f);

		// Let the builder load the script, and do the necessary pre-processing (include files, etc)
		r = builder.AddSectionFromFile((script).c_str());
		if( r < 0 )
			return 0;

		r = builder.BuildModule();
		if( r < 0 )
			return 0;


		// Cache the functions and methods that will be used
		//ctrl->module = script;
		// Find the class that implements the IController interface
		mod = m_Engine->GetModule(script.c_str(), asGM_ONLY_IF_EXISTS);
		ScriptControllerPtr ctrl(new ScriptController(mod));
		m_ScriptControllers[script] = ctrl;

		//return ctrl;
		asIObjectType *type = 0;
		int tc = mod->GetObjectTypeCount();
		for( int n = 0; n < tc; n++ )
		{
			bool found = false;
			type = mod->GetObjectTypeByIndex(n);
			int ic = type->GetInterfaceCount();
			for( int i = 0; i < ic; i++ )
			{
				if( strcmp(type->GetName(), "ScriptController") == 0 )
				{
					found = true;
					break;
				}
			}
			if( found == true )
			{
				break;
			}
		}

		if( type != NULL)
		{

			// Find the factory function
			// The game engine will pass in the owning CGameObj to the controller for storage
			//string s = string(type->GetName()) + "@ " + string(type->GetName()) + "(CGameObj @)";
			//ctrl->factoryFunc = type->GetFactoryByDecl(s.c_str());
			/*if( ctrl->factoryFunc == 0 )
			{
			cout << "Couldn't find the appropriate factory for the type '" << script << "'" << endl;
			controllers.pop_back();
			delete ctrl;
			return 0;
			}*/

			
			// Get the factory function from the object type
			ctrl->m_FactoryFunction = type->GetFactoryByDecl("ScriptController @ScriptController()");

			// Find the optional event handlers
			ctrl->m_UpdateFunction     = type->GetMethodByDecl("void OnUpdate(double)");
			//ctrl->m_InitFunction     = type->GetMethodByDecl("void OnInit(SceneObject @)");
			std::string final_init_func = "void OnInit(" + init_func_arg + ")";
			ctrl->m_InitFunction     = type->GetMethodByDecl(final_init_func.c_str());
		}
		return ctrl;
	}

	asIScriptContext *ScriptManager::PrepareContextFromPool(asIScriptFunction *func)
	{
		asIScriptContext *ctx = 0;
		if( m_Contexts.size() )
		{
			ctx = *m_Contexts.rbegin();
			m_Contexts.pop_back();
		}
		else
			ctx = m_Engine->CreateContext();

		int r = ctx->Prepare(func); assert( r >= 0 );

		return ctx;
	}

	void ScriptManager::ReturnContextToPool(asIScriptContext *ctx)
	{
		m_Contexts.push_back(ctx);

		// Unprepare the context to free any objects that might be held
		// as we don't know when the context will be used again.
		ctx->Unprepare();
	}
}




