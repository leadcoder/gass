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

#pragma once
#include <boost/enable_shared_from_this.hpp>
#include "Core/Common.h"
#include "Core/ComponentSystem/IComponentContainerTemplateManager.h"

#include <vector>
#include <map>
#include <string>


namespace GASS
{
	class IComponentContainer;
	class IComponentContainerTemplate;
	class BaseObjectTemplate;

	typedef boost::shared_ptr<IComponentContainer> ComponentContainerPtr;
	

	class GASSCoreExport BaseObjectTemplateManager: public boost::enable_shared_from_this<BaseObjectTemplateManager> , public IComponentContainerTemplateManager
	{
		friend class IComponentContainerTemplate;
	public:
		typedef std::map<std::string,ComponentContainerTemplatePtr> TemplateMap;
	public:
		BaseObjectTemplateManager();
		virtual ~BaseObjectTemplateManager();
		/**
			Create a new object from the template archive.
		@remarks
			This function do not initialize the object,
		@param name Template name defined in xml-file
		*/
		ComponentContainerPtr CreateFromTemplate(const std::string &name);
//		void SetNameCheck(bool value){m_NameCheck = value;}
		void AddTemplate(ComponentContainerTemplatePtr obj);
		ComponentContainerTemplatePtr GetTemplate(const std::string &name);
		bool Load(const std::string &filename);

		bool GetForceUniqueName() const{return m_ForceUniqueName;}
		void SetForceUniqueName(bool value) {m_ForceUniqueName = value;}
	protected:
		//void AddRecursive(IComponentContainer* ct);
		bool m_ForceUniqueName;
		TemplateMap m_TemplateMap;
	};
	typedef boost::shared_ptr<BaseObjectTemplateManager> BaseObjectTemplateManagerPtr;
}

