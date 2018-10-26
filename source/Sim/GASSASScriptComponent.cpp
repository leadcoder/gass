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

#include "GASSASScriptComponent.h"
#include "GASSScriptManager.h"
#include <angelscript.h>

namespace GASS
{
	ASScriptComponent::ASScriptComponent() : m_ScriptObject(NULL)
	{
	}

	ASScriptComponent::~ASScriptComponent()
	{
		
	}

	void ASScriptComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("ASScriptComponent",new Creator<ASScriptComponent, Component>);
		RegisterProperty<std::string>("Script", &ASScriptComponent::GetScriptFile, &ASScriptComponent::SetScriptFile);
	}

	void ASScriptComponent::OnInitialize()
	{
		RegisterForPostUpdate<BaseSceneManager>();
		
		m_Controller = SimEngine::Get().GetScriptManager()->LoadScript(m_Script, "SceneObject @");
		//asIScriptFunction *init_func = controller->GetModule()->GetFunctionByDecl("void onInit(SceneObject @)");

		if(m_Controller->m_FactoryFunction)
		{
			asIScriptContext *ctx = SimEngine::Get().GetScriptManager()->PrepareContextFromPool(m_Controller->m_FactoryFunction);
			
			SimEngine::Get().GetScriptManager()->ExecuteCall(ctx);
			
			// Get the object that was created
			m_ScriptObject = *(asIScriptObject**)ctx->GetAddressOfReturnValue();
			// If you're going to store the object you must increase the reference,
			// otherwise it will be destroyed when the context is reused or destroyed.
			m_ScriptObject->AddRef();
			if(m_Controller->m_InitFunction)
			{
				ctx->Prepare(m_Controller->m_InitFunction);
				//Set the object pointer
				ctx->SetObject(m_ScriptObject);
				ctx->SetArgObject(0, GetSceneObject().get());
				SimEngine::Get().GetScriptManager()->ExecuteCall(ctx);
			}
			SimEngine::Get().GetScriptManager()->ReturnContextToPool(ctx);
		}
	}

	void ASScriptComponent::SceneManagerTick(double delta_time)
	{
		if(m_ScriptObject)
		{
			asIScriptContext *ctx = SimEngine::Get().GetScriptManager()->PrepareContextFromPool(m_Controller->m_UpdateFunction);
			//Set the object pointer
			ctx->SetObject(m_ScriptObject);
			ctx->SetArgDouble(0, delta_time);
			SimEngine::Get().GetScriptManager()->ExecuteCall(ctx);
			SimEngine::Get().GetScriptManager()->ReturnContextToPool(ctx);
		}
	}

	void ASScriptComponent::SetScriptFile(const std::string &script_file)
	{
		m_Script = script_file;
	}

	std::string ASScriptComponent::GetScriptFile() const
	{
		return m_Script;
	}
}
