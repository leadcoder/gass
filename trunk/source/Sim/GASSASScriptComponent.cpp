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
		//SimEngine::Get().GetScriptManager()->GetEngine();
		
		ScriptControllerPtr  controller = SimEngine::Get().GetScriptManager()->LoadScript("c:\\temp\\test.as");
		/*asIScriptFunction *my_func = controller->GetModule()->GetFunctionByDecl("void onTick()");
		asIScriptContext *ctx = PrepareContextFromPool(my_func);

		SceneObjectPtr hej (new SceneObject());
		hej->SetName("cool");
		ctx->SetArgObject(0, hej.get());
		ExecuteCall(ctx);
		ReturnContextToPool(ctx);*/
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
