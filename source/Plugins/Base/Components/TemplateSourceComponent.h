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
#ifndef I_TEMPLATE_SOURCE_COMPONENT_H
#define I_TEMPLATE_SOURCE_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Interface/GASSITemplateSourceComponent.h"

namespace GASS
{

	/**
		Component that hold a template list of scene objects that can be added as child to owner scene object
	*/

	class TemplateSourceComponent : public Reflection<TemplateSourceComponent,BaseSceneComponent>, public ITemplateSourceComponent
	{
	public:
		TemplateSourceComponent(void);
		~TemplateSourceComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		void SetTemplates(const std::vector<std::string> &template_vec) {m_Templates = template_vec;}
		std::vector<std::string>  GetTemplates() const {return m_Templates;}
		Vec3 GetOffset() const {return Vec3(0,0,0);}
	protected:
		std::vector<std::string> m_Templates;
	};
	//typedef GASS_SHARED_PTR<ITemplateSourceComponent> TemplateSourceComponentPtr; 
}
#endif
