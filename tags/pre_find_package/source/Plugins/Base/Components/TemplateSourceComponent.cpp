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

#include "TemplateSourceComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSResourceManager.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSGraphicsMesh.h"

#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"


namespace GASS
{
	TemplateSourceComponent::TemplateSourceComponent(void) 
	{

	}

	TemplateSourceComponent::~TemplateSourceComponent(void)
	{
		
	}

	void TemplateSourceComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("TemplateSourceComponent",new GASS::Creator<TemplateSourceComponent, Component>);
		RegisterVectorProperty<std::string>("Templates", &GASS::TemplateSourceComponent::GetTemplates, &GASS::TemplateSourceComponent::SetTemplates);
	}

	void TemplateSourceComponent::OnInitialize()
	{
	
	}

}
