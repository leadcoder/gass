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

#include "GASSASScriptComponent.h"
#include "GASSScriptManager.h"
#include <angelscript.h>

namespace GASS
{
	ASScriptComponent::ASScriptComponent() 
	{
	}

	ASScriptComponent::~ASScriptComponent()
	{
		
	}

	void ASScriptComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("ASScriptComponent",new Creator<ASScriptComponent, IComponent>);
		RegisterProperty<std::string>("Script", &ASScriptComponent::GetScriptFile, &ASScriptComponent::SetScriptFile);
	}

	void ASScriptComponent::OnInitialize()
	{
		ScriptControllerPtr  controller = SimEngine::Get().GetScriptManager()->LoadScript("c:\\temp\\test.as");
		asIScriptFunction *init_func = controller->GetModule()->GetFunctionByDecl("void onInit(SceneObject @)");
		asIScriptContext *ctx = SimEngine::Get().GetScriptManager()->PrepareContextFromPool(init_func);
		ctx->SetArgObject(0, GetSceneObject().get());
		SimEngine::Get().GetScriptManager()->ExecuteCall(ctx);
		SimEngine::Get().GetScriptManager()->ReturnContextToPool(ctx);
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
